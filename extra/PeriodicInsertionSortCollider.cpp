// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#include"PeriodicInsertionSortCollider.hpp"
#include<yade/pkg-common/InsertionSortCollider.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/InteractionContainer.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/VelocityBins.hpp>
#include<yade/pkg-dem/NewtonsDampedLaw.hpp>

#include<algorithm>
#include<vector>
#include<boost/static_assert.hpp>
#include<stdexcept>

using namespace std;

YADE_PLUGIN((PeriodicInsertionSortCollider)(PeriIsoCompressor))
CREATE_LOGGER(PeriodicInsertionSortCollider);

// return floating value wrapped between x0 and x1 and saving period number to period
Real PeriodicInsertionSortCollider::cellWrap(const Real x, const Real x0, const Real x1, int& period){
	Real xNorm=(x-x0)/(x1-x0);
	period=(int)floor(xNorm); // some people say this is very slow; probably optimized by gcc, however (google suggests)
	return x0+(xNorm-period)*(x1-x0);
}

// return coordinate wrapped to x0…x1, relative to x0; don't care about period
Real PeriodicInsertionSortCollider::cellWrapRel(const Real x, const Real x0, const Real x1){
	Real xNorm=(x-x0)/(x1-x0);
	return (xNorm-floor(xNorm))*(x1-x0);
}


/* Performance hint
	================

	Since this function is called (most the time) from insertionSort,
	we actually already know what is the overlap status in that one dimension, including
	periodicity information; both values could be passed down as a parameters, avoiding 1 of 3 loops here.
	We do some floats math here, so the speedup could noticeable; doesn't concertn the non-periodic variant,
	where it is only plain comparisons taking place.

	In the same way, handleBoundInversion is passed only id1 and id2, but again insertionSort already knows in which sense
	the inversion happens; if the boundaries get apart (min getting up over max), it wouldn't have to check for overlap
	at all, for instance.
*/
//! return true if bodies bb overlap in all 3 dimensions
bool PeriodicInsertionSortCollider::spatialOverlap(body_id_t id1, body_id_t id2,MetaBody* rb, Vector3<int>& periods) const {
	assert(id1!=id2); // programming error, or weird bodies (too large?)
	for(int axis=0; axis<3; axis++){
		Real dim=rb->cellMax[axis]-rb->cellMin[axis];
		// too big bodies in interaction
		assert(maxima[3*id1+axis]-minima[3*id1+axis]<.99*dim); assert(maxima[3*id2+axis]-minima[3*id2+axis]<.99*dim);
		// find body of which when taken as period start will make the gap smaller
		Real m1=minima[3*id1+axis],m2=minima[3*id2+axis];
		Real wMn=(cellWrapRel(m1,m2,m2+dim)<cellWrapRel(m2,m1,m1+dim)) ? m2 : m1;
		#ifdef PISC_DEBUG
		if(watchIds(id1,id2)){
			TRVAR3(id1,id2,axis);
			TRVAR4(minima[3*id1+axis],maxima[3*id1+axis],minima[3*id2+axis],maxima[3*id2+axis]);
			TRVAR2(cellWrapRel(m1,m2,m2+dim),cellWrapRel(m2,m1,m1+dim));
			TRVAR3(m1,m2,wMn);
		}
		#endif
		int pmn1,pmx1,pmn2,pmx2;
		Real mn1=cellWrap(minima[3*id1+axis],wMn,wMn+dim,pmn1), mx1=cellWrap(maxima[3*id1+axis],wMn,wMn+dim,pmx1);
		Real mn2=cellWrap(minima[3*id2+axis],wMn,wMn+dim,pmn2), mx2=cellWrap(maxima[3*id2+axis],wMn,wMn+dim,pmx2);
		#ifdef PISC_DEBUG
			if(watchIds(id1,id2)){
				TRVAR4(mn1,mx1,mn2,mx2);
				TRVAR4(pmn1,pmx1,pmn2,pmx2);
			}
		#endif
		if((pmn1!=pmx1) || (pmn2!=pmx2)){
			LOG_FATAL("Body #"<<(pmn1!=pmx1?id1:id2)<<" spans over half of the cell size "<<dim<<" (axis="<<axis<<", min="<<(pmn1!=pmx1?mn1:mn2)<<", max="<<(pmn1!=mn1?mx1:mx2)<<")");
			throw runtime_error(__FILE__ ": Body larger than half of the cell size encountered.");
		}
		periods[axis]=(int)(pmn1-pmn2);
		if(!(mn1<=mx2 && mx1 >= mn2)) return false;
	}
	#ifdef PISC_DEBUG
		if(watchIds(id1,id2)) LOG_INFO("Overlap #"<<id1<<"+#"<<id2<<", periods "<<periods);
	#endif
	return true;
}

// called by the insertion sort if 2 bodies swapped their bounds
void PeriodicInsertionSortCollider::handleBoundInversion(body_id_t id1, body_id_t id2, InteractionContainer* interactions, MetaBody* rb){
	// do bboxes overlap in all 3 dimensions?
	Vector3<int> periods;
	bool overlap=spatialOverlap(id1,id2,rb,periods);
	// existing interaction?
	const shared_ptr<Interaction>& I=interactions->find(id1,id2);
	bool hasInter=(bool)I;
	#ifdef PISC_DEBUG
		if(watchIds(id1,id2)) LOG_INFO("Inversion #"<<id1<<"+#"<<id2<<", overlap=="<<overlap<<", hasInter=="<<hasInter);
	#endif
	// interaction doesn't exist and shouldn't, or it exists and should
	if(!overlap && !hasInter) return;
	if(overlap && hasInter){  return; }
	// create interaction if not yet existing
	if(overlap && !hasInter){ // second condition only for readability
		#ifdef PISC_DEBUG
			if(watchIds(id1,id2)) LOG_INFO("Attemtping collision of #"<<id1<<"+#"<<id2);
		#endif
		if(!Collider::mayCollide(Body::byId(id1,rb).get(),Body::byId(id2,rb).get())) return;
		// LOG_TRACE("Creating new interaction #"<<id1<<"+#"<<id2);
		shared_ptr<Interaction> newI=shared_ptr<Interaction>(new Interaction(id1,id2));
		newI->cellDist=periods;
		#ifdef PISC_DEBUG
			if(watchIds(id1,id2)) LOG_INFO("Created intr #"<<id1<<"+#"<<id2<<", periods="<<periods);
		#endif
		interactions->insert(newI);
		return;
	}
	if(!overlap && hasInter){ if(!I->isReal()) interactions->erase(id1,id2); return; }
	assert(false); // unreachable
}

void PeriodicInsertionSortCollider::insertionSort(VecBounds& v, InteractionContainer* interactions, MetaBody*rb, bool doCollide){
	long &loIdx=v.loIdx; const long &size=v.size;
	for(long _i=0; _i<size; _i++){
		const long i=v.norm(_i);
		const long i_1=v.norm(i-1);
		//switch period of (i) if the coord is below the lower edge cooridnate-wise and just above the split
		if(i==loIdx && v[i].coord<v.cellMin){ v[i].period-=1; v[i].coord+=v.cellDim; loIdx=v.norm(loIdx+1); }
		// coordinate of v[i] used to check inversions
		// if crossing the split, adjust by cellDim;
		// if we get below the loIdx however, the v[i].coord will have been adjusted already, no need to do that here
		const Real iCmpCoord=v[i].coord+(i==loIdx ? v.cellDim : 0); 
		// no inversion
		if(v[i_1].coord<=iCmpCoord) continue;
		// vi is the copy that will travel down the list, while other elts go up
		// if will be placed in the list only at the end, to avoid extra copying
		int j=i_1; Bound vi=v[i];  const bool viHasBB=vi.flags.hasBB;
		while(v[j].coord>vi.coord + /* wrap for elt just below split */ (v.norm(j+1)==loIdx ? v.cellDim : 0)){
			long j1=v.norm(j+1);
			// OK, now if many bodies move at the same pace through the cell and at one point, there is inversion,
			// this can happen without any side-effects
			if (false && v[j].coord>v.cellMax+v.cellDim){
				// this condition is not strictly necessary, but the loop of insertionSort would have to run more times.
				// Since size of particle is required to be < .5*cellDim, this would mean simulation explosion anyway
				LOG_FATAL("Body #"<<v[j].id<<" going faster than 1 cell in one step? Not handled.");
				throw runtime_error(__FILE__ ": body mmoving too fast (skipped 1 cell).");
			}
			Bound& vNew(v[j1]); // elt at j+1 being overwritten by the one at j and adjusted
			vNew=v[j];
			// inversions close the the split need special care
			if(j==loIdx && vi.coord<v.cellMin) { vi.period-=1; vi.coord+=v.cellDim; loIdx=v.norm(loIdx+1); }
			else if(j1==loIdx) { vNew.period+=1; vNew.coord-=v.cellDim; loIdx=v.norm(loIdx-1); }
			if(doCollide && viHasBB && v[j].flags.hasBB){
				if(vi.id==vNew.id){ // BUG!!
					LOG_FATAL("Inversion of body's #"<<vi.id<<" boundary with its other boundary.");
					throw runtime_error(__FILE__ "Body's boundary metting its opposite boundary.");
				}
				handleBoundInversion(vi.id,vNew.id,interactions,rb);
			}
			j=v.norm(j-1);
		}
		v[v.norm(j+1)]=vi;
	}
}

// old code, can be removed later
#if 0
void PeriodicInsertionSortCollider::insertionSort(VecBounds& v, InteractionContainer* interactions, MetaBody* rb, bool doCollide){
	// It seems that due to wrapping, it is not predetermined, how many times should we traverse the container
	// We therefore add one blank traversal at the end that finds no inversions; then stop
	long &loIdx=v.loIdx, &size=v.size; // shorthands
	for(bool hadInversion=true; hadInversion; ){
		hadInversion=false;
		long cnt=0; //loIdx
		for(long i=v.norm(loIdx-1); cnt++<size; i=v.norm(i-1)){
			long i_1=v.norm(i-1);
			// fast test, if the first pair is inverted
			if(v[i].coord<v[i_1].coord-(i_1==v.norm(loIdx-1) ? v.cellDim : 0) ){
				//v.dump(cerr);
				if(i==loIdx && v[i].coord<v.cellMin){ v[i].period-=1; v[i].coord+=v.cellDim; loIdx=v.norm(loIdx+1); }
				hadInversion=true; Bound vi=v[i]; int j; const bool viBB=vi.flags.hasBB;
				for(j=i_1; vi.coord<v[j].coord-(j==v.norm(loIdx-1) ? v.cellDim : 0); j=v.norm(j-1)) {
					//{ Bound vj1=v[v.norm(j+1)]; v[v.norm(j+1)]=vi;
					//v[v.norm(j+1)]=vj1; }
					long j1=v.norm(j+1);
					//LOG_TRACE("Inversion of i="<<i<<"(#"<<vi.id<<" @ "<<vi.coord<<") j="<<j<<"(#"<<v[j].id<<" @ "<<v[j].coord<<"); j1="<<j1); v.dump(cerr);
					v[j1]=v[j];
					//if(v[j1].coord>v.cellMax && j2==loIdx){ v[j1].period+=1; v[j1].coord-=v.cellDim; loIdx=v.norm(loIdx-1); }
					if(j1==loIdx) { assert(v[j1].coord>=v.cellMax); v[j1].period+=1; v[j1].coord-=v.cellDim; loIdx=v.norm(loIdx-1); }
					else if (vi.coord<v.cellMin && j==loIdx){ vi.period-=1; vi.coord+=v.cellDim; loIdx=v.norm(loIdx+1); }
					if(doCollide && viBB && v[j].flags.hasBB){
						if(vi.id==v[j].id){ // BUG!!
							ofstream of("/tmp/dump");
							Omega::instance().saveSimulation("/tmp/dump.xml");
							v.dump(of);
							LOG_FATAL("Inversion of a body's boundary with itself, id="<<vi.id);
							body_id_t id=vi.id;
							TRVAR4(vi.coord,vi.id,vi.period,vi.flags.isMin);
							TRVAR4(v[j].coord,v[j].id,v[j].period,v[j].flags.isMin);
							TRVAR2(*(Vector3r*)(&minima[3*id]),*(Vector3r*)(&maxima[3*id]));
							TRVAR3(i,j,v.loIdx);
							abort();
						}
						handleBoundInversion(vi.id,v[j].id,interactions,rb);
					}
					//v.dump(cerr);
				}
				v[v.norm(j+1)]=vi;
				//LOG_TRACE("DONE:"); v.dump(cerr);
			}
		}
	}
}
#endif

void PeriodicInsertionSortCollider::VecBounds::update(MetaBody* rb, int axis){
	assert(axis>=0 && axis<3);
	if(!rb->isPeriodic){
		// maybe just set cell from -inf to +inf and go ahead?
		LOG_FATAL("MetaBody::isPeriodic is false, unable to continue!"); throw runtime_error("Non-periodic MetaBody for periodic collider.");
	}
	cellMin=rb->cellMin[axis]; cellMax=rb->cellMax[axis]; cellDim=cellMax-cellMin;
	size=vec.size();
}

void PeriodicInsertionSortCollider::action(MetaBody* rb){
	long nBodies=(long)rb->bodies->size();
	InteractionContainer* interactions=rb->interactions.get();

	// pre-conditions
		// adjust storage size
		bool doInitSort=false;
		if(XX.size!=2*nBodies){
			LOG_DEBUG("Resize bounds containers from "<<XX.size<<" to "<<nBodies*2<<", will std::sort.");
			// bodies deleted; clear the container completely, and do as if all bodies were added (rather slow…)
			// future possibility: insertion sort with such operator that deleted bodies would all go to the end, then just trim bounds
			if(2*nBodies<XX.size){ XX.vec.clear(); YY.vec.clear(); ZZ.vec.clear(); }
			// more than 100 bodies was added, do initial sort again
			// maybe: should rather depend on ratio of added bodies to those already present...?
			if(2*nBodies-XX.size>200 || XX.size==0) doInitSort=true;
			XX.vec.reserve(2*nBodies); YY.vec.reserve(2*nBodies); ZZ.vec.reserve(2*nBodies);
			assert((XX.vec.size()%2)==0);
			for(size_t id=XX.vec.size()/2; id<(size_t)nBodies; id++){
				// add lower and upper bounds; coord is not important, will be updated from bb shortly
				XX.vec.push_back(Bound(0,id,/*isMin=*/true)); XX.vec.push_back(Bound(0,id,/*isMin=*/false));
				YY.vec.push_back(Bound(0,id,          true)); YY.vec.push_back(Bound(0,id,          false));
				ZZ.vec.push_back(Bound(0,id,          true)); ZZ.vec.push_back(Bound(0,id,          false));
			}
			// XX.dump(cerr); YY.dump(cerr); ZZ.dump(cerr);
		}
		if(minima.size()!=3*(size_t)nBodies){ minima.resize(3*nBodies); maxima.resize(3*nBodies); }
		assert(XX.vec.size()==2*rb->bodies->size());
		//PERI: copy periodicity information
		XX.update(rb,0); YY.update(rb,1); ZZ.update(rb,2);

	// copy bounds along given axis into our arrays
		for(size_t i=0; i<2*(size_t)nBodies; i++){
			const body_id_t& idXX=XX[i].id; const body_id_t& idYY=YY[i].id; const body_id_t& idZZ=ZZ[i].id;
			const shared_ptr<BoundingVolume>& bvXX=Body::byId(idXX,rb)->boundingVolume; const shared_ptr<BoundingVolume>& bvYY=Body::byId(idYY,rb)->boundingVolume; const shared_ptr<BoundingVolume>& bvZZ=Body::byId(idZZ,rb)->boundingVolume;
			// copy bounds from boundingVolume if there is one, otherwise use position; store what was used in the flags.hasBB bit
			// PERI: add current period number to the coordinate
			XX[i].coord=((XX[i].flags.hasBB=(bool)bvXX) ? (XX[i].flags.isMin ? bvXX->min[0] : bvXX->max[0]) : (Body::byId(idXX,rb)->physicalParameters->se3.position[0])) - XX.cellDim*XX[i].period;
			YY[i].coord=((YY[i].flags.hasBB=(bool)bvYY) ? (YY[i].flags.isMin ? bvYY->min[1] : bvYY->max[1]) : (Body::byId(idYY,rb)->physicalParameters->se3.position[1])) - YY.cellDim*YY[i].period;
			ZZ[i].coord=((ZZ[i].flags.hasBB=(bool)bvZZ) ? (ZZ[i].flags.isMin ? bvZZ->min[2] : bvZZ->max[2]) : (Body::byId(idZZ,rb)->physicalParameters->se3.position[2])) - ZZ.cellDim*ZZ[i].period;
			// PERI: at the initial step, fix periods of bodies
			// doInitSort is also called when bodies are just added; changing the period should not have influence here, though.
			if(doInitSort){
				if(XX[i].coord<XX.cellMin || XX[i].coord>=XX.cellMax) XX[i].coord=cellWrap(XX[i].coord,XX.cellMin,XX.cellMax,XX[i].period);
				if(YY[i].coord<XX.cellMin || YY[i].coord>=YY.cellMax) YY[i].coord=cellWrap(YY[i].coord,YY.cellMin,YY.cellMax,YY[i].period);
				if(ZZ[i].coord<ZZ.cellMin || ZZ[i].coord>=ZZ.cellMax) ZZ[i].coord=cellWrap(ZZ[i].coord,ZZ.cellMin,ZZ.cellMax,ZZ[i].period);
			}
			// and for each body, copy its minima and maxima arrays as well
			if(XX[i].flags.isMin){
				BOOST_STATIC_ASSERT(sizeof(Vector3r)==3*sizeof(Real));
				if(bvXX) {
					memcpy(&minima[3*idXX],&bvXX->min,3*sizeof(Real)); memcpy(&maxima[3*idXX],&bvXX->max,3*sizeof(Real)); // ⇐ faster than 6 assignments
				}  
				else{ const Vector3r& pos=Body::byId(idXX,rb)->physicalParameters->se3.position; memcpy(&minima[3*idXX],pos,3*sizeof(Real)); memcpy(&maxima[3*idXX],pos,3*sizeof(Real)); }
			}
		}

	// process interactions that the constitutive law asked to be erased
		interactions->erasePending(*this,rb);

	// sort
		if(!doInitSort){
			/* each inversion in insertionSort calls handleBoundInversion, which in turns may add/remove interaction */
			insertionSort(XX,interactions,rb); insertionSort(YY,interactions,rb); insertionSort(ZZ,interactions,rb);
		}
		else {
			// the initial sort is in independent in 3 dimensions, may be run in parallel
			// it seems that there is no time gain running this in parallel, though
			 std::sort(XX.vec.begin(),XX.vec.end()); std::sort(YY.vec.begin(),YY.vec.end()); std::sort(ZZ.vec.begin(),ZZ.vec.end());

			// traverse the container along requested axis
			assert(sortAxis==0 || sortAxis==1 || sortAxis==2);
			VecBounds& V=(sortAxis==0?XX:(sortAxis==1?YY:ZZ));
			// go through potential aabb collisions, create interactions as necessary
			for(size_t i=0; i<2*(size_t)nBodies; i++){
				// start from the lower bound
				// skip bodies without bbox since we would possibly never meet the upper bound again (std::sort may not be stable) and we don't want to collide those anyway
				if(!(V[i].flags.isMin && V[i].flags.hasBB)) continue;
				const body_id_t& iid=V[i].id;
				/* If std::sort swaps equal min/max bounds, there are 2 cases distinct cases to handle:

					1. i is inside V, j gets to the end of V (since it loops till the maxbound is found)
						here, we swap min/max to get the in the right order and continue with the loop over i;
						next time this j-bound is handled (with a different i, for sure), it will be OK.
					2. i is at the end of V, therefore (j=i+1)==2*nBodies, therefore V[j] doesn't exist (past the end)
						here, we can just check for that and break the loop if it happens.
						It is the last i that we process, nothing will come after.

					NOTE: XX,YY,ZZ containers don't guarantee that i_min<i_max. This is needed only here and is
						handled only for the sortAxis. Functionality-wise, this has no impact on further collision
						detection, though.
				*/
				// TRVAR3(i,iid,V[i].coord);
				// go up until we meet the upper bound
				size_t cnt=0;
				for(size_t j=V.norm(i+1); V[j].id!=iid; j=V.norm(j+1)){
					const body_id_t& jid=V[j].id;
					/// Not sure why this doesn't work. If this condition is commented out, we have exact same interactions as from SpatialQuickSort. Otherwise some interactions are missing!
					// skip bodies with smaller (arbitrary, could be greater as well) id, since they will detect us when their turn comes
					//if(jid<iid) { /* LOG_TRACE("Skip #"<<V[j].id<<(V[j].flags.isMin?"(min)":"(max)")<<" with "<<iid<<" (smaller id)"); */ continue; }
					// take 2 of the same condition (only handle collision [min_i..max_i]+min_j, not [min_i..max_i]+min_i (symmetric)
					if(!V[j].flags.isMin) continue;
					/* abuse the same function here; since it does spatial overlap check first, it is OK to use it */
					handleBoundInversion(iid,jid,interactions,rb);
					// now we are at the last element, but we still have not met the upper bound of V[i].id
					// that means that the upper bound is before the upper one; that can only happen if they
					// are equal and the unstable std::sort has swapped them. In that case, we need to go reverse
					// from V[i] until we meet the upper bound and swap the isMin flag

					#if 0
					if(j==2*(size_t)nBodies-1){ /* handle case 1. of swapped min/max */
						size_t k=i-1;
						while(V[k].id!=iid && k>0) k--;
						assert(V[k].id==iid); // if this fails, we didn't meet the other bound in the downwards sense either; that should never happen
						assert(!V[k].flags.isMin); // the lower bound should be maximum in this (exceptional) case; we will fix that now
						V[k].flags.isMin=true; V[i].flags.isMin=false;
						LOG_DEBUG("Swapping coincident min/max of #"<<iid<<" at positions "<<k<<" and "<<i<<" (coords: "<<V[k].coord<<" and "<<V[i].coord<<")");
						break; // would happen anyways
					}
					#endif
					if(cnt++>2*(size_t)nBodies){ LOG_FATAL("Uninterrupted loop in the initial sort?"); throw std::logic_error("loop??"); }
				}
			}
		}
}





/***********************************************************************************************************
******************************* PeriIsoCompressor *********************************************************/
#include<yade/pkg-dem/Shop.hpp>


CREATE_LOGGER(PeriIsoCompressor);
void PeriIsoCompressor::action(MetaBody* rb){
	if(!rb->isPeriodic){ LOG_FATAL("Being used on non-periodic simulation!"); throw; }
	if(state>=stresses.size()) return;
	// initialize values
	if(charLen<=0){
		BoundingVolume* bv=Body::byId(0,rb)->boundingVolume.get();
		if(!bv){ LOG_FATAL("No charLen defined and body #0 has no boundingVolume"); throw; }
		const Vector3r sz=bv->max-bv->min;
		charLen=(sz[0]+sz[1]+sz[2])/3.;
		LOG_INFO("No charLen defined, taking avg bbox size of body #0 = "<<charLen);
	}
	if(maxSpan<=0){
		FOREACH(const shared_ptr<Body>& b, *rb->bodies){
			if(!b->boundingVolume) continue;
			for(int i=0; i<3; i++) maxSpan=max(maxSpan,b->boundingVolume->max[i]-b->boundingVolume->min[i]);
		}
		
	}
	if(maxDisplPerStep<0) maxDisplPerStep=1e-2*charLen; // this should be tuned somehow…
	const long& step=rb->currentIteration;
	Vector3r cellSize=rb->cellMax-rb->cellMin; //unused: Real cellVolume=cellSize[0]*cellSize[1]*cellSize[2];
	Vector3r cellArea=Vector3r(cellSize[1]*cellSize[2],cellSize[0]*cellSize[2],cellSize[0]*cellSize[1]);
	Real minSize=min(cellSize[0],min(cellSize[1],cellSize[2]));
	if(minSize<2.1*maxSpan){ throw runtime_error("Minimum cell size is smaller than 2.1*span_of_the_biggest_body! (periodic collider requirement)"); }
	if(((step%globalUpdateInt)==0) || avgStiffness<0 || sigma[0]<0 || sigma[1]<0 || sigma[2]<0){
		Vector3r sumForces=Shop::totalForceInVolume(avgStiffness,rb);
		sigma=Vector3r(sumForces[0]/cellArea[0],sumForces[1]/cellArea[1],sumForces[2]/cellArea[2]);
		LOG_TRACE("Updated sigma="<<sigma<<", avgStiffness="<<avgStiffness);
	}
	Real sigmaGoal=stresses[state]; assert(sigmaGoal>0);
	// expansion of cell in this step (absolute length)
	Vector3r cellGrow(Vector3r::ZERO);
	// is the stress condition satisfied in all directions?
	bool allStressesOK=true;
	if(keepProportions){ // the same algo as below, but operating on quantitites averaged over all dimensions
		Real sigAvg=(sigma[0]+sigma[1]+sigma[2])/3., avgArea=(cellArea[0]+cellArea[1]+cellArea[2])/3.;
		Real grow=1e-4*(sigAvg-sigmaGoal)*avgArea/(avgStiffness>0?avgStiffness:1);
		if(abs(grow)>maxDisplPerStep) grow=Mathr::Sign(grow)*maxDisplPerStep;
		grow=max(grow,-(minSize-2.1*maxSpan));
		if(avgStiffness>0) { sigma-=(grow*avgStiffness)*Vector3r::ONE; sigAvg-=grow*avgStiffness; }
		if(abs((sigAvg-sigmaGoal)/sigmaGoal)>5e-3) allStressesOK=false;
		cellGrow=Vector3r(grow,grow,grow);
	}
	else{ // handle each dimension separately
		for(int axis=0; axis<3; axis++){
			// Δσ=ΔεE=(Δl/l)×(l×K/A) ↔ Δl=Δσ×A/K
			// FIXME: either NormalShearInteraction::{kn,ks} is computed wrong or we have dimensionality problem here
			// FIXME: that is why the fixup 1e-4 is needed here
			// FIXME: or perhaps maxDisplaPerStep=1e-2*charLen is too big??
			cellGrow[axis]=1e-4*(sigma[axis]-sigmaGoal)*cellArea[axis]/(avgStiffness>0?avgStiffness:1);
			if(abs(cellGrow[axis])>maxDisplPerStep) cellGrow[axis]=Mathr::Sign(cellGrow[axis])*maxDisplPerStep;
			cellGrow[axis]=max(cellGrow[axis],-(cellSize[axis]-2.1*maxSpan));
			// crude way of predicting sigma, for steps when it is not computed from intrs
			if(avgStiffness>0) sigma[axis]-=cellGrow[axis]*avgStiffness;
			if(abs((sigma[axis]-sigmaGoal)/sigmaGoal)>5e-3) allStressesOK=false;
		}
	}
	TRVAR4(cellGrow,sigma,sigmaGoal,avgStiffness);
	rb->cellMin-=.5*cellGrow; rb->cellMax+=.5*cellGrow;
	// handle state transitions
	if(allStressesOK){
		if((step%globalUpdateInt)==0) currUnbalanced=Shop::unbalancedForce(/*useMaxForce=*/false,rb);
		if(currUnbalanced<maxUnbalanced){
			state+=1;
			// sigmaGoal reached and packing stable
			if(state==stresses.size()){ // no next stress to go for
				LOG_INFO("Finished");
				#ifdef YADE_PYTHON
					if(!doneHook.empty()){ LOG_DEBUG("Running doneHook: "<<doneHook); PyGILState_STATE gstate; gstate=PyGILState_Ensure(); PyRun_SimpleString(doneHook.c_str()); PyGILState_Release(gstate); }
				#endif
			} else { LOG_INFO("Loaded to "<<sigmaGoal<<" done, going to "<<stresses[state]<<" now"); }
		} else {
			if((step%globalUpdateInt)==0) LOG_DEBUG("Stress="<<sigma<<", goal="<<sigmaGoal<<", unbalanced="<<currUnbalanced);
		}
	}
}
