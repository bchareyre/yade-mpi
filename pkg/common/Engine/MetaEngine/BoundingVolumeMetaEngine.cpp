/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BoundingVolumeMetaEngine.hpp"


#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>

CREATE_LOGGER(BoundingVolumeMetaEngine);

void BoundingVolumeMetaEngine::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	const long numBodies=(long)bodies->size();
	//#pragma omp parallel for
	for(int id=0; id<numBodies; id++){
		if(!bodies->exists(id)) continue; // don't delete this check  - Janek
		const shared_ptr<Body>& b=(*bodies)[id];
		shared_ptr<InteractingGeometry>& ig=b->interactingGeometry;
		if(!ig || !b->boundingVolume) continue;

		#ifdef BV_FUNCTOR_CACHE
			if(!ig->boundFunctor){ bool swap=false; ig->boundFunctor=this->getFunctor2D(ig,b->boundingVolume,swap); /* no sense, different types: */ assert(!swap); if(!ig->boundFunctor) continue; }
			// LOG_DEBUG("ig->boundFunctor.get()=="<<ig->boundFunctor.get()<<" for "<<b->interactingGeometry->getClassName()<<", #"<<id);
			//if(!ig->boundFunctor) throw runtime_error("boundFunctor not found for #"+lexical_cast<string>(id)); assert(ig->boundFunctor);
			ig->boundFunctor->go(ig,b->boundingVolume,b->physicalParameters->se3,b.get());
		#else
			operator()(ig,b->boundingVolume,b->physicalParameters->se3,b.get());
		#endif
		if(sweepTime>0){
			/* compute se3 the body would have after given time (linear interpolation), and use the maximum bounding volume then.
			It is a pessimisation that allows us to not update the aabb during some period of time. */
			Vector3r min0=b->boundingVolume->min, max0=b->boundingVolume->max;
			const Se3r& se3=b->physicalParameters->se3;
			const RigidBodyParameters* rbp=YADE_CAST<RigidBodyParameters*>(b->physicalParameters.get());
			/* For position change, Δr=vΔt+½aΔt²; similar for orientation. */
			Se3r sweptSe3; sweptSe3.position=se3.position+(rbp->velocity+.5*rbp->acceleration*sweepTime)*sweepTime;
			Vector3r axis=rbp->angularVelocity+.5*rbp->angularAcceleration*sweepTime; Real angle=axis.Normalize(); Quaternionr q(axis,angle*sweepTime); sweptSe3.orientation=q*se3.orientation;
			#ifdef BV_FUNCTOR_CACHE
				ig->boundFunctor->go(ig,b->boundingVolume,sweptSe3,b.get());
			#else
				operator()(ig,b->boundingVolume,sweptSe3,b.get());
			#endif
			AABB* aabb=YADE_CAST<AABB*>(b->boundingVolume.get()); // to update halfSize and center as well
			aabb->min=componentMinVector(min0,aabb->min); aabb->max=componentMaxVector(max0,aabb->max);
			aabb->center=.5*(aabb->min+aabb->max); aabb->halfSize=.5*(aabb->max-aabb->min);
		}
		if(sweepDist!=0){
			AABB* aabb=YADE_CAST<AABB*>(b->boundingVolume.get());
			if(sweepDist<0) aabb->halfSize=-sweepDist*aabb->halfSize; // relative scaling by abs(sweepDist)
			else aabb->halfSize+=Vector3r(sweepDist,sweepDist,sweepDist); // absolute distance increment
			aabb->min=aabb->center-aabb->halfSize; aabb->max=aabb->center+aabb->halfSize;
		}
	}
	operator()(ncb->interactingGeometry,ncb->boundingVolume,ncb->physicalParameters->se3,ncb);
}



YADE_PLUGIN();
