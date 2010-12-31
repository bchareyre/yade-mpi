/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ig2_Box_Sphere_ScGeom.hpp"
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/Box.hpp>
#include<yade/core/Scene.hpp>
#include<yade/lib/base/Math.hpp>



bool Ig2_Box_Sphere_ScGeom::go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c)
{
	const Se3r& se31=state1.se3; const Se3r& se32=state2.se3;

	bool inside=true;
	Vector3r pt1,pt2,normal;
	Real depth;

	Box* obb = static_cast<Box*>(cm1.get());
	Sphere* s = static_cast<Sphere*>(cm2.get());

	Vector3r extents = obb->extents;

	// FIXME: do we need rotation matrix? Can't quaternion do just fine?
	Matrix3r boxAxisT=se31.orientation.toRotationMatrix();
	Matrix3r boxAxis = boxAxisT.transpose();

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

	shared_ptr<ScGeom> scm;
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
		normal.normalize();

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
		Vector3r normal = pt1-pt2; normal.normalize();
		bool isNew=!c->geom;
		if (isNew) scm = shared_ptr<ScGeom>(new ScGeom());
		else scm = YADE_PTR_CAST<ScGeom>(c->geom);

		// contact point is in the middle of overlapping volumes
		//(in the direction of penetration, which is normal to the box surface closest to sphere center) of overlapping volumes
		scm->contactPoint = 0.5*(pt1+pt2);
// 		scm->normal = normal;
		scm->penetrationDepth = (pt1-pt2).norm();
		scm->radius1 = s->radius;
		scm->radius2 = s->radius;
		c->geom = scm;
		scm->precompute(state1,state2,scene,c,normal,isNew,shift2,true);
	} else { // outside
		Vector3r cOnBox_box = boxAxisT*cOnBox_boxLocal; // projection of sphere's center on closest box surface - relative to box's origin, but GLOBAL orientation!
		Vector3r cOnBox_sphere = cOnBox_box-relPos21; // same, but origin in sphere's center
		depth=s->radius-cOnBox_sphere.norm();
		if (depth<0 && !c->isReal() && !force) return false;

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

		cOnBox_sphere.normalize(); // we want only direction in the following

		pt2=se32.position+cOnBox_sphere*s->radius;

		bool isNew=!c->geom;
		if (isNew) scm = shared_ptr<ScGeom>(new ScGeom());
		else scm = YADE_PTR_CAST<ScGeom>(c->geom);
		scm->contactPoint = 0.5*(pt1+pt2);
		scm->penetrationDepth = depth;
		scm->radius1 = s->radius;
		scm->radius2 = s->radius;
		c->geom = scm;
		//FIXME : NOT TESTED : do we precompute correctly if boxes are moving?
		scm->precompute(state1,state2,scene,c,-cOnBox_sphere,isNew,shift2,false);
	}
	return true;
}



bool Ig2_Box_Sphere_ScGeom::goReverse(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2,
  const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c)
{
	c->swapOrder();
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}

YADE_PLUGIN((Ig2_Box_Sphere_ScGeom));


bool Ig2_Box_Sphere_ScGeom6D::go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c)
{
	bool isNew = !c->geom;
	if (Ig2_Box_Sphere_ScGeom::go(cm1,cm2,state1,state2,shift2,force,c)) {
		if (isNew) {//generate a 6DOF interaction from the 3DOF one generated by Ig2_Box_Sphere_ScGeom
			shared_ptr<ScGeom6D> sc(new ScGeom6D());
			*(YADE_PTR_CAST<ScGeom>(sc)) = *(YADE_PTR_CAST<ScGeom>(c->geom));
			c->geom=sc;
		}
		YADE_PTR_CAST<ScGeom6D>(c->geom)->precomputeRotations(state1,state2,isNew,false);
		return true;
	}
	else return false;
}


bool Ig2_Box_Sphere_ScGeom6D::goReverse(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c)
{
	c->swapOrder();
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}

YADE_PLUGIN((Ig2_Box_Sphere_ScGeom6D));
