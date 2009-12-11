/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BoundDispatcher.hpp"


#include<yade/core/Scene.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/VelocityBins.hpp>

CREATE_LOGGER(BoundDispatcher);

void BoundDispatcher::action(Scene*)
{
	updateScenePtr();
	shared_ptr<BodyContainer>& bodies = scene->bodies;
	const long numBodies=(long)bodies->size();
	bool haveBins=(bool)velocityBins;
	if(sweepDist!=0 && haveBins){ LOG_FATAL("Only one of sweepDist or velocityBins can used!"); abort(); }
	//#pragma omp parallel for
	for(int id=0; id<numBodies; id++){
		if(!bodies->exists(id)) continue; // don't delete this check  - Janek
		const shared_ptr<Body>& b=(*bodies)[id];
		shared_ptr<Shape>& ig=b->shape;
		if(!ig || !b->bound) continue;

		#ifdef BV_FUNCTOR_CACHE
			if(!ig->boundFunctor){ bool swap=false; ig->boundFunctor=this->getFunctor2D(ig,b->bound,swap); /* no sense, different types: */ assert(!swap); if(!ig->boundFunctor) continue; }
			// LOG_DEBUG("ig->boundFunctor.get()=="<<ig->boundFunctor.get()<<" for "<<b->shape->getClassName()<<", #"<<id);
			//if(!ig->boundFunctor) throw runtime_error("boundFunctor not found for #"+lexical_cast<string>(id)); assert(ig->boundFunctor);
			ig->boundFunctor->go(ig,b->bound,b->state->se3,b.get());
		#else
			operator()(ig,b->bound,b->state->se3,b.get());
		#endif
		if(sweepDist>0){
			AABB* aabb=YADE_CAST<AABB*>(b->bound.get());
			aabb->halfSize+=Vector3r(sweepDist,sweepDist,sweepDist);
			aabb->min=aabb->center-aabb->halfSize; aabb->max=aabb->center+aabb->halfSize;
		}
		if(haveBins){
			AABB* aabb=YADE_CAST<AABB*>(b->bound.get());
			Real sweep=velocityBins->bins[velocityBins->bodyBins[b->getId()]].maxDist;
			aabb->halfSize+=Vector3r(sweep,sweep,sweep);
			aabb->min=aabb->center-aabb->halfSize; aabb->max=aabb->center+aabb->halfSize;
		}
	}
	scene->updateBound();
}



YADE_PLUGIN((BoundDispatcher));

