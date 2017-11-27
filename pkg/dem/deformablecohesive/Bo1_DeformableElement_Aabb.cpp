/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include <pkg/dem/deformablecohesive/Bo1_DeformableElement_Aabb.hpp>
#include <pkg/dem/deformablecohesive/DeformableElement.hpp>
#include <pkg/common/Aabb.hpp>

void Bo1_DeformableElement_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){

	DeformableElement* deformableElement = static_cast<DeformableElement*>(cm.get());

	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }

	Aabb* aabb=static_cast<Aabb*>(bv.get());
	Real inf=std::numeric_limits<Real>::infinity();
	Vector3r min,max;
	min=Vector3r(inf,inf,inf); max=Vector3r(-inf,-inf,-inf);
	//Not sure this is the best of all possible algorithms, therefore, I think it can be improved
	for (DeformableElement::NodeMap::iterator it = deformableElement->localmap.begin(); it != deformableElement->localmap.end(); it++)
	{
		if(it->first->state->pos(0)<min(0))
		{
			min(0)=it->first->state->pos(0);
		}
		if(it->first->state->pos(1)<min(1))
		{
			min(1)=it->first->state->pos(1);
		}
		if(it->first->state->pos(0)<min(2))
		{
			min(2)=it->first->state->pos(2);
		}

		if(it->first->state->pos(0)>max(0))
		{
			max(0)=it->first->state->pos(0);
		}
		if(it->first->state->pos(1)>max(1))
		{
			max(1)=it->first->state->pos(1);
		}
		if(it->first->state->pos(2)>max(2))
		{
			max(2)=it->first->state->pos(2);
		}

	}


	aabb->min=min;
	aabb->max=max;

//	if(!scene->isPeriodic){
//		aabb->min=se3.position-halfSize; aabb->max=se3.position+halfSize;
//		return;
//	}
//	// adjust box size along axes so that DeformableElement doesn't stick out of the box even if sheared (i.e. parallelepiped)
//	if(scene->cell->hasShear()) {
//		Vector3r refHalfSize(halfSize);
//		const Vector3r& cos=scene->cell->getCos();
//		for(int i=0; i<3; i++){
//			//cerr<<"cos["<<i<<"]"<<cos[i]<<" ";
//			int i1=(i+1)%3,i2=(i+2)%3;
//			halfSize[i1]+=.5*refHalfSize[i1]*(1/cos[i]-1);
//			halfSize[i2]+=.5*refHalfSize[i2]*(1/cos[i]-1);
//		}
//	}
//	//cerr<<" || "<<halfSize<<endl;
//	aabb->min = scene->cell->unshearPt(se3.position)-halfSize;
//	aabb->max = scene->cell->unshearPt(se3.position)+halfSize;
}
	
YADE_PLUGIN((Bo1_DeformableElement_Aabb));
