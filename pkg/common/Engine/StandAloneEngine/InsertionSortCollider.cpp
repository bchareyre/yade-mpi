// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#include"InsertionSortCollider.hpp"
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

YADE_PLUGIN("InsertionSortCollider")

CREATE_LOGGER(InsertionSortCollider);

// return true if bodies bb overlap in all 3 dimensions
bool InsertionSortCollider::spatialOverlap(body_id_t id1, body_id_t id2) const {
	return
		(minima[3*id1+0]<=maxima[3*id2+0]) && (maxima[3*id1+0]>=minima[3*id2+0]) &&
		(minima[3*id1+1]<=maxima[3*id2+1]) && (maxima[3*id1+1]>=minima[3*id2+1]) &&
		(minima[3*id1+2]<=maxima[3*id2+2]) && (maxima[3*id1+2]>=minima[3*id2+2]);
}

// called by the insertion sort if 2 bodies swapped their bounds
void InsertionSortCollider::handleBoundInversion(body_id_t id1, body_id_t id2, InteractionContainer* interactions, MetaBody* rb){
	// do bboxes overlap in all 3 dimensions?
	bool overlap=spatialOverlap(id1,id2);
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
		interactions->insert(newI);
		return;
	}
	if(!overlap && hasInter){ if(!I->isReal()) interactions->erase(id1,id2); return; }
	assert(false); // unreachable
}

void InsertionSortCollider::insertionSort(vector<Bound>& v, InteractionContainer* interactions, MetaBody* rb, bool doCollide){
	long size=v.size();
	for(long i=0; i<size; i++){
		const Bound viInit=v[i]; long j=i-1; /* cache hasBB; otherwise 1% overall performance hit */ const bool viInitBB=viInit.flags.hasBB;
		while(j>=0 && v[j]>viInit){
			v[j+1]=v[j];
			// no collisions without bounding boxes
			if(doCollide && viInitBB && v[j].flags.hasBB) handleBoundInversion(viInit.id,v[j].id,interactions,rb);
			j--;
		}
		v[j+1]=viInit;
	}
}

#ifdef COLLIDE_STRIDED
	bool InsertionSortCollider::isActivated(MetaBody* rb){
		// activated if number of bodies changes (hence need to refresh collision information)
		// or the time of scheduled run already came, or we were never scheduled yet
		if(!strideActive) return true;
		if(!newton || (nBins>=1 && !newton->velocityBins)) return true;
		if(nBins>=1 && newton->velocityBins->incrementDists_shouldCollide(rb->dt)) return true;
		if(nBins<=0){
			if(fastestBodyMaxDist<0){fastestBodyMaxDist=0; return true;}
			fastestBodyMaxDist+=sqrt(newton->maxVelocitySq)*rb->dt;
			if(fastestBodyMaxDist>=sweepLength) return true;
		}
		if(XX.size()!=2*rb->bodies->size()) return true;
		// we wouldn't run in this step; in that case, just delete pending interactions
		// this is done in ::action normally, but it would make the call counters not reflect the stride
		rb->interactions->erasePending(*this);
		return false;
	}
#endif

void InsertionSortCollider::action(MetaBody* rb){
	#ifdef ISC_TIMING
		timingDeltas->start();
	#endif

	size_t nBodies=rb->bodies->size();
	InteractionContainer* interactions=rb->interactions.get();
	
	// bite: conditions that make it necessary to run collider even if not scheduled are in isActivated

	// pre-conditions
		// adjust storage size
		bool doInitSort=false;
		if(XX.size()!=2*nBodies){
			LOG_DEBUG("Resize bounds containers from "<<XX.size()<<" to "<<nBodies*2<<", will std::sort.");
			// bodies deleted; clear the container completely, and do as if all bodies were added (rather slow…)
			// future possibility: insertion sort with such operator that deleted bodies would all go to the end, then just trim bounds
			if(2*nBodies<XX.size()){ XX.clear(); YY.clear(); ZZ.clear(); }
			// more than 100 bodies was added, do initial sort again
			// maybe: should rather depend on ratio of added bodies to those already present...?
			if(2*nBodies-XX.size()>200 || XX.size()==0) doInitSort=true;
			XX.reserve(2*nBodies); YY.reserve(2*nBodies); ZZ.reserve(2*nBodies);
			assert((XX.size()%2)==0);
			for(size_t id=XX.size()/2; id<nBodies; id++){
				// add lower and upper bounds; coord is not important, will be updated from bb shortly
				XX.push_back(Bound(0,id,/*isMin=*/true)); XX.push_back(Bound(0,id,/*isMin=*/false));
				YY.push_back(Bound(0,id,          true)); YY.push_back(Bound(0,id,          false));
				ZZ.push_back(Bound(0,id,          true)); ZZ.push_back(Bound(0,id,          false));
			}
		}
		if(minima.size()!=3*nBodies){ minima.resize(3*nBodies); maxima.resize(3*nBodies); }
		assert(XX.size()==2*rb->bodies->size());
		#ifdef COLLIDE_STRIDED
			// get the BoundingVolumeMetaEngine and turn it off; we will call it ourselves
			if(!boundDispatcher){
				FOREACH(shared_ptr<Engine>& e, rb->engines){ boundDispatcher=dynamic_pointer_cast<BoundingVolumeMetaEngine>(e); if(boundDispatcher) break; }
				if(!boundDispatcher){ LOG_FATAL("Unable to locate BoundingVolumeMetaEngine within engines, aborting."); abort(); }
				boundDispatcher->activated=false; // deactive the engine, we will call it ourselves from now (just when needed)
			}
			if(sweepLength>0){
				// get NewtonsDampedLaw, to ask for the maximum velocity value
				if(!newton){
					FOREACH(shared_ptr<Engine>& e, rb->engines){ newton=dynamic_pointer_cast<NewtonsDampedLaw>(e); if(newton) break; }
					if(!newton){ LOG_FATAL("Unable to locate NewtonsDampedLaw within engines, aborting."); abort(); }
				}
			}
		#endif
	ISC_CHECKPOINT("init");

		#ifdef COLLIDE_STRIDED
			// get us ready for strides, if they were deactivated
			if(!strideActive && sweepLength>0 && newton->maxVelocitySq>=0){ // maxVelocitySq is a really computed value
				strideActive=true;
			}
			if(strideActive){
				assert(sweepLength>0);
				if(nBins<=0){
					// reset bins, in case they were active but are not anymore
					if(newton->velocityBins) newton->velocityBins=shared_ptr<VelocityBins>(); if(boundDispatcher->velocityBins) boundDispatcher->velocityBins=shared_ptr<VelocityBins>();
					assert(strideActive); assert(newton->maxVelocitySq>=0); assert(sweepFactor>1.);
					Real sweepVelocity=sqrt(newton->maxVelocitySq)*sweepFactor; int stride=-1;
					if(sweepVelocity>0) {
						stride=max(1,int((sweepLength/sweepVelocity)/rb->dt));
						boundDispatcher->sweepDist=rb->dt*(stride-1)*sweepVelocity;
					} else { // no motion
						boundDispatcher->sweepDist=0; // nothing moves, no need to make bboxes larger
					}
					LOG_DEBUG(rb->simulationTime<<"s: stride ≈"<<stride<<"; maxVelocity="<<sqrt(newton->maxVelocitySq)<<", sweepDist="<<boundDispatcher->sweepDist);
					fastestBodyMaxDist=0; // reset
				} else { // nBins>=1
					if(!newton->velocityBins){ newton->velocityBins=shared_ptr<VelocityBins>(new VelocityBins(nBins,newton->maxVelocitySq,binCoeff,binOverlap)); }
					if(!boundDispatcher->velocityBins) boundDispatcher->velocityBins=newton->velocityBins;
					newton->velocityBins->nBins=nBins; newton->velocityBins->binCoeff=binCoeff; newton->velocityBins->binOverlap=binOverlap; // update things 
					boundDispatcher->sweepDist=0;
					// re-bin bodies
					newton->velocityBins->setBins(rb,newton->maxVelocitySq,sweepLength);
				}
			}
			boundDispatcher->action(rb);
		#endif

	ISC_CHECKPOINT("bound");

	// copy bounds along given axis into our arrays
		for(size_t i=0; i<2*nBodies; i++){
			const body_id_t& idXX=XX[i].id; const body_id_t& idYY=YY[i].id; const body_id_t& idZZ=ZZ[i].id;
			const shared_ptr<BoundingVolume>& bvXX=Body::byId(idXX,rb)->boundingVolume; const shared_ptr<BoundingVolume>& bvYY=Body::byId(idYY,rb)->boundingVolume; const shared_ptr<BoundingVolume>& bvZZ=Body::byId(idZZ,rb)->boundingVolume;
			// copy bounds from boundingVolume if there is one, otherwise use position; store what was used in the flags.hasBB bit
			XX[i].coord=(XX[i].flags.hasBB=(bool)bvXX) ? (XX[i].flags.isMin ? bvXX->min[0] : bvXX->max[0]) : (Body::byId(idXX,rb)->physicalParameters->se3.position[0]);
			YY[i].coord=(YY[i].flags.hasBB=(bool)bvYY) ? (YY[i].flags.isMin ? bvYY->min[1] : bvYY->max[1]) : (Body::byId(idYY,rb)->physicalParameters->se3.position[1]);
			ZZ[i].coord=(ZZ[i].flags.hasBB=(bool)bvZZ) ? (ZZ[i].flags.isMin ? bvZZ->min[2] : bvZZ->max[2]) : (Body::byId(idZZ,rb)->physicalParameters->se3.position[2]);
			// and for each body, copy its minima and maxima arrays as well
			if(XX[i].flags.isMin){
				BOOST_STATIC_ASSERT(sizeof(Vector3r)==3*sizeof(Real));
				if(bvXX) { memcpy(&minima[3*idXX],&bvXX->min,3*sizeof(Real)); memcpy(&maxima[3*idXX],&bvXX->max,3*sizeof(Real)); } // ⇐ faster than 6 assignments 
				else{ const Vector3r& pos=Body::byId(idXX,rb)->physicalParameters->se3.position; memcpy(&minima[3*idXX],pos,3*sizeof(Real)); memcpy(&maxima[3*idXX],pos,3*sizeof(Real)); }
			}
		}
	ISC_CHECKPOINT("copy");

	// process interactions that the constitutive law asked to be erased
		interactions->erasePending(*this);
	ISC_CHECKPOINT("erase");

	// sort
		if(!doInitSort && !sortThenCollide){
			/* each inversion in insertionSort calls handleBoundInversion, which in turns may add/remove interaction */
			insertionSort(XX,interactions,rb); insertionSort(YY,interactions,rb); insertionSort(ZZ,interactions,rb);
		}
		else {
			if(doInitSort){
				// the initial sort is in independent in 3 dimensions, may be run in parallel
				// it seems that there is no time gain running this in parallel, though
				#pragma omp parallel sections
				{
					#pragma omp section
						{ std::sort(XX.begin(),XX.end()); }
					#pragma omp section
						{ std::sort(YY.begin(),YY.end()); }
					#pragma omp section
						{ std::sort(ZZ.begin(),ZZ.end()); }
				}
			} else { // sortThenCollide
				insertionSort(XX,interactions,rb,false); insertionSort(YY,interactions,rb,false); insertionSort(ZZ,interactions,rb,false);
			}
			// traverse the container along requested axis
			assert(sortAxis==0 || sortAxis==1 || sortAxis==2);
			vector<Bound>& V=(sortAxis==0?XX:(sortAxis==1?YY:ZZ));
			// go through potential aabb collisions, create interactions as necessary
			for(size_t i=0; i<2*nBodies; i++){
				// start from the lower bound
				// skip bodies without bbox since we would possibly never meet the upper bound again (std::sort may not be stable) and we don't want to collide those anyway
				if(!(V[i].flags.isMin && V[i].flags.hasBB)) continue;
				const body_id_t& iid=V[i].id;
				// TRVAR3(i,iid,V[i].coord);
				// go up until we meet the upper bound
				for(size_t j=i+1; V[j].id!=iid; j++){
					const body_id_t& jid=V[j].id;
					/// Not sure why this doesn't work. If this condition is commented out, we have exact same interactions as from SpatialQuickSort. Otherwise some interactions are missing!
					// skip bodies with smaller (arbitrary, could be greater as well) id, since they will detect us when their turn comes
					//if(jid<iid) { /* LOG_TRACE("Skip #"<<V[j].id<<(V[j].flags.isMin?"(min)":"(max)")<<" with "<<iid<<" (smaller id)"); */ continue; }
					/* abuse the same function here; since it does spatial overlap check first, it is OK to use it */
					handleBoundInversion(iid,jid,interactions,rb);
					// now we are at the last element, but we still have not met the upper bound of V[i].id
					// that means that the upper bound is before the upper one; that can only happen if they
					// are equal and the unstable std::sort has swapped them. In that case, we need to go reverse
					// from V[i] until we meet the upper bound and swap the isMin flag
					if(j==2*nBodies-1){
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
	ISC_CHECKPOINT("sort&collide");
}
