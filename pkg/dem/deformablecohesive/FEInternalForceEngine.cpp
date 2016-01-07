/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <pkg/dem/deformablecohesive/DeformableElement.hpp>
#include <pkg/dem/deformablecohesive/FEInternalForceDispatchers.hpp>
#include <pkg/dem/deformablecohesive/FEInternalForceEngine.hpp>

YADE_PLUGIN((FEInternalForceEngine));
CREATE_LOGGER(FEInternalForceEngine);

void FEInternalForceEngine::pyHandleCustomCtorArgs(boost::python::tuple& t, boost::python::dict& d){
	cout<<"Entered the initialization function";
	if(boost::python::len(t)==0) return; // nothing to do

	if(boost::python::len(t)!=1) throw invalid_argument("At least 1 functors must be given");



	boost::python::list vv=boost::python::extract<boost::python::list>(t[0])();

//	FOREACH(shared_ptr<InternalForceFunctor> iff, vv) this->internalforcedispatcher->add(iff);

	for( int i=0;i<len(vv);i++) this->internalforcedispatcher->add(boost::python::extract<shared_ptr<InternalForceFunctor>>(vv[i]));
	

	t=boost::python::tuple(); // empty the args; not sure if this is OK, as there is some refcounting in raw_constructor code
	cout<<"Added to the list";
}


void FEInternalForceEngine::action(){

	// update Scene* of the dispatcher
	internalforcedispatcher->scene=scene;
	// ask dispatcher to update Scene* of their functors
	internalforcedispatcher->updateScenePtr();

	// call InternalForceFunctor::preStep
	//FOREACH(const shared_ptr<InternalForceFunctor>& iff, internalforcedispatcher->functors) iff->preStep();

	/*
		initialize callbacks; they return pointer (used only in this timestep) to the function to be called
		returning NULL deactivates the callback in this timestep
	*/
//	// pair of callback object and pointer to the function to be called
//	vector<IntrCallback::FuncPtr> callbackPtrs;
//	FOREACH(const shared_ptr<IntrCallback> cb, callbacks){
//		cb->scene=scene;
//		callbackPtrs.push_back(cb->stepInit());
//	}
//	assert(callbackPtrs.size()==callbacks.size());
//	size_t callbacksSize=callbacks.size();

	// cache transformed cell size
	//	Matrix3r cellHsize; if(scene->isPeriodic) cellHsize=scene->cell->hSize;


	#ifdef YADE_OPENMP
	const long size=scene->bodies->size();
	#pragma omp parallel for schedule(guided) num_threads(ompThreads>0 ? ompThreads : omp_get_max_threads())
	for(long i=0; i<size; i++){
		shared_ptr<Body>& bdy=(*scene->bodies)[i];
	#else
	FOREACH(const shared_ptr<Body>& bdy, *scene->bodies){
	#endif

			//Try to get internal functor
			if(bdy->shape->internalforcefunctor.get()==0) {
				//If there isn't any internalforcefunctor; try to get one
				bdy->shape->internalforcefunctor=internalforcedispatcher->getFunctor(bdy->shape,bdy->material);
				//İf we found any of them that is suitable to this element???
				if(bdy->shape->internalforcefunctor!=0){
					//If yes try to apply internal force functor
					shared_ptr<DeformableElement> deformableElement=YADE_PTR_CAST<DeformableElement>(bdy->shape);
					bdy->shape->internalforcefunctor->go(deformableElement,bdy->material,bdy);
				}
			}
			else{
				//Take functor from cache and call it's go functor
				shared_ptr<DeformableElement> deformableElement=YADE_PTR_CAST<DeformableElement>(bdy->shape);
				bdy->shape->internalforcefunctor->go(deformableElement,bdy->material,bdy);
			}

	}
}
