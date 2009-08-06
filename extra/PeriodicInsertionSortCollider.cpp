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

using namespace std;

YADE_PLUGIN((PeriodicInsertionSortCollider))
CREATE_LOGGER(PeriodicInsertionSortCollider);

Real PeriodicInsertionSortCollider::cellWrap(const Real x, const Real x0, const Real x1, long& period){
	Real xNorm=(x-x0)/(x1-x0);
	period=(long)floor(xNorm); // FIXME: some people say this is very slow
	return x0+(xNorm-period)*(x1-x0);
}


// return true if bodies bb overlap in all 3 dimensions
bool PeriodicInsertionSortCollider::spatialOverlap(body_id_t id1, body_id_t id2,MetaBody* rb, Vector3<int>& periods) const {
	assert(id1!=id2) // programming error, or weird bodies (too large?)
	for(int axis=0; axis<3; axis++){
		Real dim=rb->cellMax[axis]-rb->cellMin[axis];
		// wrap all 4 numbers to the period starting and the most minimal number
		#if 0
			Real mn=min(minima[3*id1+axis],minima[3*id2+axis])-0.001*dim; // avoid rounding issues
			Real mx=max(maxima[3*id1+axis],maxima[3*id2+axis]);
			TRVAR2(mn,mx);
		#endif
		// too big bodies in interaction
		assert(maxima[3*id1+axis]-minima[3*id1+axis]<.99*dim); assert(maxima[3*id2+axis]-minima[3*id2+axis]<.99*dim);
		// different way: find body of which when taken as period start will make the gap smaller
		long p;
		Real mn1w=cellWrap(minima[3*id1+axis],0,dim,p), mn2w=cellWrap(minima[3*id2+axis],0,dim,p);
		Real wMn=(abs(mn2w-mn1w)<dim/2 ? mn1w : mn2w) -/*avoid rounding issues*/1e-4*dim; /* selected wrap base */
		//TRVAR3(id1,id2,axis);
		//TRVAR4(minima[3*id1+axis],maxima[3*id1+axis],minima[3*id2+axis],maxima[3*id2+axis]);
		//TRVAR3(mn1w,mn2w,wMn);
		long pmn1,pmx1,pmn2,pmx2;
		Real mn1=cellWrap(minima[3*id1+axis],wMn,wMn+dim,pmn1), mx1=cellWrap(maxima[3*id1+axis],wMn,wMn+dim,pmx1);
		Real mn2=cellWrap(minima[3*id2+axis],wMn,wMn+dim,pmn2), mx2=cellWrap(maxima[3*id2+axis],wMn,wMn+dim,pmx2);
		//TRVAR4(mn1,mx1,mn2,mx2);
		//TRVAR4(pmn1,pmx1,pmn2,pmx2);
		assert(pmn1==pmx1); assert(pmn2==pmx2);
		periods[axis]=(int)(pmn1-pmn2);
		if(!(mn1<=mx2 && mx1 >= mn2)) return false;
	}
	//LOG_TRACE("Returning true for #"<<id1<<"+#"<<id2<<", periods "<<periods);
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
	// interaction doesn't exist and shouldn't, or it exists and should
	if(!overlap && !hasInter) return;
	if(overlap && hasInter){  return; }
	// create interaction if not yet existing
	if(overlap && !hasInter){ // second condition only for readability
		if(!Collider::mayCollide(Body::byId(id1,rb).get(),Body::byId(id2,rb).get())) return;
		// LOG_TRACE("Creating new interaction #"<<id1<<"+#"<<id2);
		shared_ptr<Interaction> newI=shared_ptr<Interaction>(new Interaction(id1,id2));
		newI->cellDist=periods;
		interactions->insert(newI);
		return;
	}
	if(!overlap && hasInter){ if(!I->isReal()) interactions->erase(id1,id2); return; }
	assert(false); // unreachable
}

void PeriodicInsertionSortCollider::insertionSort(VecBounds& v, InteractionContainer* interactions, MetaBody* rb, bool doCollide){
	// It seems that due to wrapping, it is not predetermined, how many times should we traverse the container
	// We therefore add one blank traversal at the end that finds no inversions; then stop
	long &loIdx=v.loIdx, &size=v.size; // shorthands
	for(bool hadInversion=true; hadInversion; ){
		hadInversion=false;
		long cnt=0; //loIdx
		for(long i=v.norm(loIdx-1); cnt++<size; i=v.norm(i-1)){
			long i_1=v.norm(i-1), loIdx_1=v.norm(loIdx-1);
			// fast test, if the first pair is inverted
			if(v[i].coord<v[i_1].coord-(i_1==loIdx_1 ? v.cellDim : 0) ){
				// v.dump(cerr);
				hadInversion=true; Bound vi=v[i]; int j; const bool viBB=vi.flags.hasBB;
				for(j=i_1; vi.coord<v[j].coord-(j==v.norm(loIdx-1) ? v.cellDim : 0); j=v.norm(j-1)) {
					//{ Bound vj1=v[v.norm(j+1)]; v[v.norm(j+1)]=vi;
					//v[v.norm(j+1)]=vj1; }
					long j1=v.norm(j+1); // j2=v.norm(j+2);
					//LOG_TRACE("Inversion of i="<<i<<"(#"<<vi.id<<" @ "<<vi.coord<<") j="<<j<<"(#"<<v[j].id<<" @ "<<v[j].coord<<"); j1="<<j1<<", j2="<<j2); v.dump(cerr);
					v[j1]=v[j];
					//if(v[j1].coord>v.cellMax && j2==loIdx){ v[j1].period+=1; v[j1].coord-=v.cellDim; loIdx=v.norm(loIdx-1); }
					if(j1==loIdx) { assert(v[j1].coord>v.cellMax); v[j1].period+=1; v[j1].coord-=v.cellDim; loIdx=v.norm(loIdx-1); }
					else if (vi.coord<v.cellMin && j==loIdx){ vi.period-=1; vi.coord+=v.cellDim; loIdx=v.norm(loIdx+1); }
					if(doCollide && viBB && v[j].flags.hasBB) handleBoundInversion(vi.id,v[j].id,interactions,rb);
					//v.dump(cerr);
				}
				v[v.norm(j+1)]=vi;
				//LOG_TRACE("DONE:"); v.dump(cerr);
			}
		}
	}
}

void PeriodicInsertionSortCollider::VecBounds::update(MetaBody* rb, int axis){
	assert(axis>=0 && axis<3);
	if(!rb->isPeriodic){
		// maybe just set cell from -inf to +inf and go ahead?
		LOG_FATAL("MetaBody::isPeriodic is false, unable to continue!"); throw;
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
		//PERI: copy periodiciy information
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
			// and for each body, copy its minima and maxima arrays as well
			if(XX[i].flags.isMin){
				BOOST_STATIC_ASSERT(sizeof(Vector3r)==3*sizeof(Real));
				if(bvXX) {
					memcpy(&minima[3*idXX],&bvXX->min,3*sizeof(Real)); memcpy(&maxima[3*idXX],&bvXX->max,3*sizeof(Real)); // ⇐ faster than 6 assignments
				}  
				else{ const Vector3r& pos=Body::byId(idXX,rb)->physicalParameters->se3.position; memcpy(&minima[3*idXX],pos,3*sizeof(Real)); memcpy(&maxima[3*idXX],pos,3*sizeof(Real)); }
				// PERI: add periods, but such that both minimum and maximum is within the cell!
				Vector3r period(XX[i].period*XX.cellDim,YY[i].period*YY.cellDim,ZZ[i].period*ZZ.cellDim);
				*(Vector3r*)(&minima[3*idXX])+=period; *(Vector3r*)(&maxima[3*idXX])+=period; //ugh
			}
		}

	// process interactions that the constitutive law asked to be erased
		interactions->erasePending(*this,rb);
	LOG_DEBUG("Step "<<Omega::instance().getCurrentIteration());
	ZZ.dump(cerr);
	// XX.dump(cerr); YY.dump(cerr); ZZ.dump(cerr);

	// sort
		if(!doInitSort){
			/* each inversion in insertionSort calls handleBoundInversion, which in turns may add/remove interaction */
			insertionSort(XX,interactions,rb); insertionSort(YY,interactions,rb); insertionSort(ZZ,interactions,rb);
		}
		else {
			if(doInitSort){
				// the initial sort is in independent in 3 dimensions, may be run in parallel
				// it seems that there is no time gain running this in parallel, though
				 std::sort(XX.vec.begin(),XX.vec.end()); std::sort(YY.vec.begin(),YY.vec.end()); std::sort(ZZ.vec.begin(),ZZ.vec.end());
			} else { // sortThenCollide
				insertionSort(XX,interactions,rb,false); insertionSort(YY,interactions,rb,false); insertionSort(ZZ,interactions,rb,false);
			}
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
				for(size_t j=i+1; V[j].id!=iid && /* handle case 2. of swapped min/max */ j<2*(size_t)nBodies; j++){
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
					if(j==2*(size_t)nBodies-1){ /* handle case 1. of swapped min/max */
						size_t k=i-1;
						while(V[k].id!=iid && k>0) k--;
						assert(V[k].id==iid); // if this fails, we didn't meet the other bound in the downwards sense either; that should never happen
						assert(!V[k].flags.isMin); // the lower bound should be maximum in this (exceptional) case; we will fix that now
						V[k].flags.isMin=true; V[i].flags.isMin=false;
						LOG_DEBUG("Swapping coincident min/max of #"<<iid<<" at positions "<<k<<" and "<<i<<" (coords: "<<V[k].coord<<" and "<<V[i].coord<<")");
						break; // would happen anyways
					}
				}
			}
		}
}
