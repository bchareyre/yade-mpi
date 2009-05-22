// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#include"InsertionSortCollider.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/InteractionContainer.hpp>

#include<algorithm>
#include<vector>
#include<boost/static_assert.hpp>

using namespace std;

YADE_PLUGIN("InsertionSortCollider")

CREATE_LOGGER(InsertionSortCollider);

// return true if bodies bb overlap in all 3 dimensions
bool InsertionSortCollider::spatialOverlap(body_id_t id1, body_id_t id2){
	return
		(minima[3*id1+0]<maxima[3*id2+0]) && (maxima[3*id1+0]>minima[3*id2+0]) &&
		(minima[3*id1+1]<maxima[3*id2+1]) && (maxima[3*id1+1]>minima[3*id2+1]) &&
		(minima[3*id1+2]<maxima[3*id2+2]) && (maxima[3*id1+2]>minima[3*id2+2]);
}

// called by the insertion sort if 2 bodies swapped their bounds
void InsertionSortCollider::handleBoundInversion(body_id_t id1, body_id_t id2, InteractionContainer* interactions, MetaBody* rb){
	// do bboxes overlap in all 3 dimensions?
	bool overlap=spatialOverlap(id1,id2);
	// existing interaction?
	const shared_ptr<Interaction>& I=interactions->find(id1,id2);
	bool hasInter=(bool)I;
	// interaction doesn't exist and shouldn't, or it exists and should
	if((!overlap && !hasInter) || (overlap && hasInter)) return;
	// create interaction if not yet existing
	if(overlap && !hasInter){ // second condition only for readability
		// FIXME: if(!Collider::mayCollide(bi.get(),bj.get())) return;
		if(!Collider::mayCollide(Body::byId(id1,rb).get(),Body::byId(id2,rb).get())) return;
		LOG_TRACE("Creating new interaction #"<<id1<<"+#"<<id2);
		shared_ptr<Interaction> newI=shared_ptr<Interaction>(new Interaction(id1,id2));
		interactions->insert(newI);
		return;
	}
	/* Note: this doesn't cover all disappearing interactions, only those that broke in the sortAxis direction;
	 	it is only a minor optimization (to be verified) to have it here.
		The rest of interaction will be deleted at the end of action. */
	if(!overlap && hasInter){ if(!I->isReal) interactions->erase(id1,id2); return; }
	assert(false); // unreachable
}

void InsertionSortCollider::insertionSort(vector<Bound>& v, InteractionContainer* interactions, MetaBody* rb){
	long size=v.size();
	for(long i=0; i<size; i++){
		Bound viInit=v[i]; long j=i-1;
		while(j>=0 && v[j]>viInit){
			v[j+1]=v[j];
			handleBoundInversion(viInit.id,v[j].id,interactions,rb);
			j--;
		}
		v[j+1]=viInit;
	}
}

void InsertionSortCollider::action(MetaBody* rb){
	//timingDeltas->start();

	size_t nBodies=rb->bodies->size();
	// int axis1=(sortAxis+1)%3, axis2=(sortAxis+2)%3, axis0=sortAxis;
	long iter=rb->currentIteration;
	InteractionContainer* interactions=rb->interactions.get();


	// pre-conditions
		// adjust storage size
		bool doInitSort=false;
		if(XX.size()!=2*nBodies){
			LOG_DEBUG("Resize bounds containers from "<<XX.size()<<" to "<<nBodies*2<<", will std::sort.");
			// bodies deleted; clear the container completely, and do as if all bodies were added (rather slow…)
			if(2*nBodies<XX.size()){ XX.clear(); YY.clear(); ZZ.clear(); }
			// more than 100 bodies was added, do initial sort again
			// maybe: should rather depend on ratio of added bodies to those already present...?
			if(2*nBodies-XX.size()>200 || XX.size()==0) doInitSort=true;
			XX.reserve(2*nBodies); YY.reserve(2*nBodies); ZZ.reserve(2*nBodies);
			assert((XX.size()%2)==0);
			for(size_t id=XX.size()/2; id<nBodies; id++){
				// add lower and upper bounds; coord is not important, will be updated from bb shortly
				XX.push_back(Bound(0,id,true)); XX.push_back(Bound(0,id,false));
				YY.push_back(Bound(0,id,true)); YY.push_back(Bound(0,id,false));
				ZZ.push_back(Bound(0,id,true)); ZZ.push_back(Bound(0,id,false));
			}
		}
		if(minima.size()!=3*nBodies){ minima.resize(3*nBodies); maxima.resize(3*nBodies); }
		assert(XX.size()==2*rb->bodies->size());

	//timingDeltas->checkpoint("setup");

	// copy bounds along given axis into our arrays
		for(size_t i=0; i<2*nBodies; i++){
			const body_id_t& idXX=XX[i].id; const body_id_t& idYY=YY[i].id; const body_id_t& idZZ=ZZ[i].id;
			const shared_ptr<BoundingVolume>& bvXX=Body::byId(idXX,rb)->boundingVolume; const shared_ptr<BoundingVolume>& bvYY=Body::byId(idYY,rb)->boundingVolume; const shared_ptr<BoundingVolume>& bvZZ=Body::byId(idZZ,rb)->boundingVolume;
			// if(!bvXX){ LOG_FATAL("InsertionSortCollider doesn't handle boundingVolume-less bodies."); throw runtime_error("InsertionSortCollider encountered boundingVolume-less body."); }
			XX[i].coord=bvXX ? (XX[i].isMin ? bvXX->min[0] : bvXX->max[0]) : Body::byId(idXX,rb)->physicalParameters->se3.position[0];
			YY[i].coord=bvYY ? (YY[i].isMin ? bvYY->min[1] : bvYY->max[1]) : Body::byId(idYY,rb)->physicalParameters->se3.position[1];
			ZZ[i].coord=bvZZ ? (ZZ[i].isMin ? bvZZ->min[2] : bvZZ->max[2]) : Body::byId(idZZ,rb)->physicalParameters->se3.position[2];
			//YY[i].coord=(YY[i].isMin ? Body::byId(YY[i].id,rb)->boundingVolume->min[1] :  Body::byId(YY[i].id,rb)->boundingVolume->max[1]);
			//ZZ[i].coord=(ZZ[i].isMin ? Body::byId(ZZ[i].id,rb)->boundingVolume->min[2] :  Body::byId(ZZ[i].id,rb)->boundingVolume->max[2]);
			// and for each body, copy its minima and maxima arrays as well
			if(XX[i].isMin){
				BOOST_STATIC_ASSERT(sizeof(Vector3r)==3*sizeof(Real));
				//minima[3*id]=bvXX->min[0]; minima[3*id+1]=bvXX->min[1]; minima[3*id+2]=bvXX->min[2]; maxima[3*id]=bvXX->max[0]; maxima[3*id+1]=bvXX->max[1]; maxima[3*id+2]=bvXX->max[2];
				if(bvXX) { memcpy(&minima[3*idXX],&bvXX->min,3*sizeof(Real)); memcpy(&maxima[3*idXX],&bvXX->max,3*sizeof(Real)); } // ⇐ faster than 6 assignments 
				else{ const Vector3r& pos=Body::byId(idXX,rb)->physicalParameters->se3.position; memcpy(&minima[3*idXX],pos,3*sizeof(Real)); memcpy(&maxima[3*idXX],pos,3*sizeof(Real)); }
			}
		}

	//timingDeltas->checkpoint("copy");
	

	// sort
		if(!doInitSort){
			insertionSort(XX,interactions,rb);
			insertionSort(YY,interactions,rb);
			insertionSort(ZZ,interactions,rb);
		}
		else {
			std::sort(XX.begin(),XX.end());
			std::sort(YY.begin(),YY.end());
			std::sort(ZZ.begin(),ZZ.end());
			// traverse the container along requested axis
			assert(sortAxis==0 || sortAxis==1 || sortAxis==2);
			vector<Bound>& V=(sortAxis==0?XX:(sortAxis==1?YY:ZZ));
			// go through potential aabb collisions, create interactions as necessary
			for(size_t i=0; i<2*nBodies; i++){
				// start from the lower bound
				if(!V[i].isMin) continue;
				const body_id_t& iid=V[i].id;
				// TRVAR3(i,iid,V[i].coord);
				// go up until we meet the upper bound
				for(size_t j=i+1; V[j].id!=iid; j++){
					// skip bodies with smaller (arbitrary, could be greater as well) id,
					// since they will detect us when their turn comes
					const body_id_t& jid=V[j].id;
					if(jid<iid) { /* LOG_TRACE("Skip #"<<V[j].id<<(V[j].isMin?"(min)":"(max)")<<" with "<<iid<<" (smaller id)"); */ continue; }
					handleBoundInversion(iid,jid,interactions,rb);
				}
			}
		}
	//timingDeltas->checkpoint("sort&collide");

	// garbage collection once in a while: for interactions that were still real when the bounding boxes separated
	// the collider would never get to see them again otherwise
	if(iter%1000==0){
		typedef pair<body_id_t,body_id_t> bodyIdPair;
		list<bodyIdPair> toBeDeleted;
		FOREACH(const shared_ptr<Interaction>& I,*interactions){
			if(!I->isReal && (!I->isNew || !spatialOverlap(I->getId1(),I->getId2()))) toBeDeleted.push_back(bodyIdPair(I->getId1(),I->getId2()));
		}
		FOREACH(const bodyIdPair& p, toBeDeleted){ interactions->erase(p.first,p.second); LOG_TRACE("Deleted interaction #"<<p.first<<"+#"<<p.second); }
	}
	//timingDeltas->checkpoint("stale");
}
