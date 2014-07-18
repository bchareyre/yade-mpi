/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include <yade/pkg/common/Bo1_Aabb.hpp>

YADE_PLUGIN((Bo1_Sphere_Aabb)(Bo1_Facet_Aabb)(Bo1_Box_Aabb));

void Bo1_Sphere_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Sphere* sphere = static_cast<Sphere*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	Vector3r halfSize = (aabbEnlargeFactor>0?aabbEnlargeFactor:1.)*Vector3r(sphere->radius,sphere->radius,sphere->radius);
	if(!scene->isPeriodic){
		aabb->min=se3.position-halfSize; aabb->max=se3.position+halfSize;
		return;
	}
	// adjust box size along axes so that sphere doesn't stick out of the box even if sheared (i.e. parallelepiped)
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

void Bo1_Facet_Aabb::go(	  const shared_ptr<Shape>& cm
				, shared_ptr<Bound>& bv
				, const Se3r& se3
				, const Body* b)
{
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	Facet* facet = static_cast<Facet*>(cm.get());
	const Vector3r& O = se3.position;
	Matrix3r facetAxisT=se3.orientation.toRotationMatrix();
	const vector<Vector3r>& vertices=facet->vertices;
	if(!scene->isPeriodic){
		aabb->min=aabb->max = O + facetAxisT * vertices[0];
		for (int i=1;i<3;++i)
		{
			Vector3r v = O + facetAxisT * vertices[i];
			aabb->min = aabb->min.cwiseMin(v);
			aabb->max = aabb->max.cwiseMax(v);
		}
	} else {
		Real inf=std::numeric_limits<Real>::infinity();
		aabb->min=Vector3r(inf,inf,inf); aabb->max=Vector3r(-inf,-inf,-inf);
		for(int i=0; i<3; i++){
			Vector3r v=scene->cell->unshearPt(O+facetAxisT*vertices[i]);
			aabb->min=aabb->min.cwiseMin(v);
			aabb->max=aabb->max.cwiseMax(v);
		}
	}
}

void Bo1_Box_Aabb::go(	const shared_ptr<Shape>& cm,
				shared_ptr<Bound>& bv,
				const Se3r& se3,
				const Body*	b)
{
	Box* box = static_cast<Box*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());

	if(scene->isPeriodic && scene->cell->hasShear()) throw logic_error(__FILE__ "Boxes not (yet?) supported in sheared cell.");
	
	Matrix3r r=se3.orientation.toRotationMatrix();
	Vector3r halfSize(Vector3r::Zero());
	for( int i=0; i<3; ++i )
		for( int j=0; j<3; ++j )
			halfSize[i] += std::abs( r(i,j) * box->extents[j] );
	
	aabb->min = se3.position-halfSize;
	aabb->max = se3.position+halfSize;
}
