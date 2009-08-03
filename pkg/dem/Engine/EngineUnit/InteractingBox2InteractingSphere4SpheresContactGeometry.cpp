/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"InteractingBox2InteractingSphere4SpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingBox.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>



bool InteractingBox2InteractingSphere4SpheresContactGeometry::go(
		const shared_ptr<InteractingGeometry>& cm1,
		const shared_ptr<InteractingGeometry>& cm2,
		const Se3r& se31,
		const Se3r& se32,
		const shared_ptr<Interaction>& c)
{
	bool inside=true;
	Vector3r pt1,pt2,normal;
	Real depth;

	InteractingBox* obb = static_cast<InteractingBox*>(cm1.get());
	InteractingSphere* s = static_cast<InteractingSphere*>(cm2.get());
	
	Vector3r extents = obb->extents;

	// FIXME: do we need rotation matrix? Can't quaternion do just fine?
	Matrix3r boxAxisT; se31.orientation.ToRotationMatrix(boxAxisT); 
	Matrix3r boxAxis = boxAxisT.Transpose();
	
	Vector3r relPos21 = se32.position-se31.position; // relative position of centroids
	
	// cOnBox_boxLocal is the sphere centroid (in box-local coordinates), but projected onto box if it is outside.
	// _boxLocal means that ROTATION is local and origin is in box's origin
	Vector3r cOnBox_boxLocal=boxAxis*relPos21; 

	if (cOnBox_boxLocal[0]<-extents[0]){cOnBox_boxLocal[0]=-extents[0]; inside=false; }
	if (cOnBox_boxLocal[0]> extents[0]){cOnBox_boxLocal[0]= extents[0]; inside=false; }
	if (cOnBox_boxLocal[1]<-extents[1]){cOnBox_boxLocal[1]=-extents[1]; inside=false; }
	if (cOnBox_boxLocal[1]> extents[1]){cOnBox_boxLocal[1]= extents[1]; inside=false; }
	if (cOnBox_boxLocal[2]<-extents[2]){cOnBox_boxLocal[2]=-extents[2]; inside=false; }
	if (cOnBox_boxLocal[2]> extents[2]){cOnBox_boxLocal[2]= extents[2]; inside=false; }
	
	if (inside){
		// sphere center inside box. find largest `cOnBox_boxLocal' value:
		// minCBoxDist_index is the coordinate index that minimizes extents[minCBoxDist_index]-abs(cOnBox_boxLocal[minCBoxDist_index] (sphere center closest to box boundary)
		// where cOnBox_boxLocal is minimal (i.e. where sphere center is closest perpendicularly to the box)
		Real minCBoxDist=extents[0]-fabs(cOnBox_boxLocal[0]); int minCBoxDist_index=0;
		for (int i=1; i<3; i++){Real tt=extents[i]-fabs(cOnBox_boxLocal[i]); if (tt<minCBoxDist){minCBoxDist=tt; minCBoxDist_index=i;}}
		
		// contact normal aligned with box edge along largest `cOnBox_boxLocal' value
		Vector3r normal_boxLocal = Vector3r(0,0,0);
		normal_boxLocal[minCBoxDist_index]=(cOnBox_boxLocal[minCBoxDist_index]>0)?1.0:-1.0;
		
		normal = boxAxisT*normal_boxLocal;
		normal.Normalize();
		
		// se32 is sphere's se3
		/*
		 *
		 *           _--(pt1)-_         BOX
		 *  +------~-----x-----~----------------+
		 *  |    /       ^      \               |
		 *  |   /        | (normal)*minCBoxDist |
		 *  |   |        x        |             |
		 *  |   \        | c ≡ se32->position   |
		 *  |    \       |       /              |
		 *  |      ~     |     /  SPHERE        |
		 *  |        ^~~ x ~~^                  |
		 *  |              (pt2)                |
		 *  +-----------------------------------+
		 *
		 *
		 */
		pt1 = se32.position+normal*minCBoxDist;
		pt2 = se32.position-normal*s->radius;

		shared_ptr<SpheresContactGeometry> scm;
		bool isNew=!c->interactionGeometry;
		if (isNew) scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
		else scm = YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);

		#ifdef SCG_SHEAR
			if(isNew) { /* same as below */ scm->prevNormal=pt1-pt2; scm->prevNormal.Normalize(); }
			else {scm->prevNormal=scm->normal;}
		#endif
			
		// contact point is in the middle of overlapping volumes
		//(in the direction of penetration, which is normal to the box surface closest to sphere center) of overlapping volumes
		scm->contactPoint = 0.5*(pt1+pt2);
		scm->normal = pt1-pt2; scm->normal.Normalize();
		scm->penetrationDepth = (pt1-pt2).Length();
		scm->radius1 = s->radius*2;
		scm->radius2 = s->radius;
		c->interactionGeometry = scm;
	} else { // outside
		Vector3r cOnBox_box = boxAxisT*cOnBox_boxLocal; // projection of sphere's center on closest box surface - relative to box's origin, but GLOBAL orientation!
		Vector3r cOnBox_sphere = cOnBox_box-relPos21; // same, but origin in sphere's center
		depth=s->radius-cOnBox_sphere.Length();
		if (depth<0) return false;

		/*
		 *  +-----------------------------------+
		 *  |                                   |
		 *  |          se31->position           |
		 *  |         pt2   ×                   |
		 *  |            × / cOnBox_box         |
		 *  |         pt1 /                     |
		 *  +------~-----×-----~----------------+
		 *       /       ^       \
		 *      /        | cOnBox_sphere
		 *      |        ×        |
		 *      \           c ≡ se32->position
		 *       \               /              
		 *         ~           /
		 *           ^~~ ~ ~~^
		 *                 
		 */

		pt1=cOnBox_box+se31.position;

		cOnBox_sphere.Normalize(); // we want only direction in the following

		pt2=se32.position+cOnBox_sphere*s->radius;
		
		shared_ptr<SpheresContactGeometry> scm;
		bool isNew=!c->interactionGeometry;
		if (isNew) scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
		else scm = YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);	
		#ifdef SCG_SHEAR
			if(isNew) { /* same as below */ scm->prevNormal=-cOnBox_sphere; }
			else {scm->prevNormal=scm->normal;}
		#endif
		scm->contactPoint = 0.5*(pt1+pt2);
		//scm->normal = pt1-pt2; scm->normal.Normalize();
		//scm->penetrationDepth = (pt1-pt2).Length();
		scm->normal = -cOnBox_sphere;
		scm->penetrationDepth = depth;
		
		
		scm->radius1 = s->radius*2;
		scm->radius2 = s->radius;
		c->interactionGeometry = scm;
	}
	return true;
}


bool InteractingBox2InteractingSphere4SpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	c->swapOrder();
	return go(cm2,cm1,se32,se31,c);
}

YADE_PLUGIN((InteractingBox2InteractingSphere4SpheresContactGeometry));