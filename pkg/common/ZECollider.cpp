// 2011 © Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>

#ifdef YADE_CGAL

#include"ZECollider.hpp"
#include<core/Scene.hpp>
#include<core/Interaction.hpp>
#include<core/InteractionContainer.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/dem/NewtonIntegrator.hpp>
#include<pkg/common/Sphere.hpp>

#include<boost/static_assert.hpp>

YADE_PLUGIN((ZECollider))
CREATE_LOGGER(ZECollider);

InteractionContainer* ZECollider::interactions = NULL;
Scene* ZECollider::sscene = NULL;

void ZECollider::handleOverlap(const CGBox& a, const CGBox& b){
	const Body::id_t& id1=a.handle()->id;
	const Body::id_t& id2=b.handle()->id;
	//existing interaction?
	if (interactions->found(id1,id2)) return;
	//if it doesn't exist and bounds overlap, create a virtual interaction
	else if (Collider::mayCollide(Body::byId(id1,sscene).get(),Body::byId(id2,sscene).get(),scene->subdomain))
		interactions->insert(shared_ptr<Interaction>(new Interaction(id1,id2)));
}


// STRIDE
	
	bool ZECollider::isActivated(){
		// activated if number of bodies changes (hence need to refresh collision information)
		// or the time of scheduled run already came, or we were never scheduled yet
		if(!strideActive) return true;
		if(!newton) return true;
		if(fastestBodyMaxDist<0){fastestBodyMaxDist=0; return true;}
		fastestBodyMaxDist=newton->maxVelocitySq;
		if(fastestBodyMaxDist>=1 || fastestBodyMaxDist==0) return true;
		if(scene->interactions->dirty) return true;
		return false;
	}

void ZECollider::action(){
	#ifdef ISC_TIMING
		timingDeltas->start();
	#endif
	interactions=scene->interactions.get();
	sscene = scene;
	scene->interactions->iterColliderLastRun=-1;

	// periodicity changed, force reinit
	if(scene->isPeriodic != periodic){
// 		for(int i=0; i<3; i++) BB[i].vec.clear();
		periodic=scene->isPeriodic;
	}

		if(verletDist<0){
			Real minR=std::numeric_limits<Real>::infinity();
			FOREACH(const shared_ptr<Body>& b, *scene->bodies){
				if(!b || !b->shape) continue;
				Sphere* s=dynamic_cast<Sphere*>(b->shape.get());
				if(!s) continue;
				minR=min(s->radius,minR);
			}
			// if no spheres, disable stride
			verletDist=std::isinf(minR) ? 0 : std::abs(verletDist)*minR;
		}
		
		// update bounds via boundDispatcher
		boundDispatcher->scene=scene;
		boundDispatcher->sweepDist=verletDist;
		boundDispatcher->targetInterv=targetInterv;
		boundDispatcher->updatingDispFactor=updatingDispFactor;
		boundDispatcher->action();

		// if interactions are dirty, force reinitialization
		if(scene->interactions->dirty){
// 			doInitSort=true;
			scene->interactions->dirty=false;
		}
		
		// STRIDE
		if(verletDist>0){
			// get NewtonIntegrator, to ask for the maximum velocity value
			if(!newton){
				FOREACH(shared_ptr<Engine>& e, scene->engines){ newton=YADE_PTR_DYN_CAST<NewtonIntegrator>(e); if(newton) break; }
				if(!newton){ throw runtime_error("ZECollider.verletDist>0, but unable to locate NewtonIntegrator within O.engines."); }
			}
		}
	ISC_CHECKPOINT("init");

		// STRIDE
			// get us ready for strides, if they were deactivated
			if(!strideActive && verletDist>0 && newton->maxVelocitySq>=0){ // maxVelocitySq is a really computed value
				strideActive=true;
			}
			if(strideActive){
				assert(verletDist>0);
				assert(strideActive); assert(newton->maxVelocitySq>=0);
				newton->updatingDispFactor=updatingDispFactor;
			} else { /* !strideActive */
				boundDispatcher->sweepDist=0;
			}

	ISC_CHECKPOINT("bound");
	
// 	// erase virtuals the lazy way
// 	// FIXME: suboptimal since some of them will re-created just below, needs smarter erase by checking overlaps
// 	interactions->eraseNonReal();
// 	ISC_CHECKPOINT("eraseNonReal");
	
	// copy bounds into our arrays
	
// 	std::vector<int> boxesIdx;
// 	boxesIdx.resize(scene->bodies.size());
	boxes.clear();
	FOREACH(shared_ptr<Body>& b, *scene->bodies){
		if(b){
			shared_ptr<Bound>& bv=b->bound;
			if(bv) {
				boxes.push_back(CGBox(CGBbox(bv->min[0],bv->min[1],bv->min[2],bv->max[0],bv->max[1],bv->max[2]),b));
// 				boxesIdx[b->id]
			}
		}
	}
	ISC_CHECKPOINT("copy");
	
	// erase virtuals the lazy way
	// FIXME: suboptimal since some of them will re-created just below, needs smarter erase by checking overlaps
// 	interactions->conditionalyEraseNonReal(*this,scene);
	interactions->eraseNonReal();
	ISC_CHECKPOINT("conditionalyEraseNonReal");
	
	// collide
	CGAL::box_self_intersection_d( boxes.begin(), boxes.end(), handleOverlap);
	ISC_CHECKPOINT("collide");
}

#endif
