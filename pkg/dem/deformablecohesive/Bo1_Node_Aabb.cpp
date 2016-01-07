/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include <pkg/dem/deformablecohesive/Bo1_Node_Aabb.hpp>
#include <pkg/dem/deformablecohesive/Node.hpp>
#include <pkg/common/Aabb.hpp>

void Bo1_Node_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Node* node = static_cast<Node*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	Vector3r halfSize = (aabbEnlargeFactor>0?aabbEnlargeFactor:1.)*Vector3r(node->radius,node->radius,node->radius);
	if(!scene->isPeriodic){
		aabb->min=se3.position-halfSize; aabb->max=se3.position+halfSize;
		return;
	}
	// adjust box size along axes so that Node doesn't stick out of the box even if sheared (i.e. parallelepiped)
	if(scene->cell->hasShear()) {
		Vector3r refHalfSize(halfSize);
		const Vector3r& cos=scene->cell->getCos();
		for(int i=0; i<3; i++){
			//cerr<<"cos["<<i<<"]"<<cos[i]<<" ";
			int i1=(i+1)%3,i2=(i+2)%3;
			halfSize[i1]+=.5*refHalfSize[i1]*(1/cos[i]-1);
			halfSize[i2]+=.5*refHalfSize[i2]*(1/cos[i]-1);
		}
	}
	//cerr<<" || "<<halfSize<<endl;
	aabb->min = scene->cell->unshearPt(se3.position)-halfSize;
	aabb->max = scene->cell->unshearPt(se3.position)+halfSize;
}
	
YADE_PLUGIN((Bo1_Node_Aabb));
