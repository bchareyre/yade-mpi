/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron.hpp"
#include "InteractionOfMyTetrahedron.hpp"
#include "InteractingMyTetrahedron.hpp"

void InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron::calcTwoSpheres(
		Vector3r c1, Real r1, Vector3r c2, Real r2, Real& penetrationDepth, Vector3r& normal, Vector3r& contactPoint)
{
	// I could call here a function in InteractingSphere2InteractingSphere4SpheresContactGeometry::go()
	// and extract results from it, but those three lines are much shorter :)
	normal = c2 - c1;
	contactPoint = (c1 + c2)*0.5;
	Real length = normal.Normalize();
	penetrationDepth = ( r2 + r1 ) - length; // positive: penetration exists
}

bool InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron::go(	
		const shared_ptr<InteractingGeometry>& cm1,
		const shared_ptr<InteractingGeometry>& cm2,
		const Se3r& se31,
		const Se3r& se32,
		const shared_ptr<Interaction>& c)
{
	InteractingMyTetrahedron* t1 = static_cast<InteractingMyTetrahedron*>(cm1.get());
	InteractingMyTetrahedron* t2 = static_cast<InteractingMyTetrahedron*>(cm2.get());
	
	shared_ptr<InteractionOfMyTetrahedron> imt;
	// depending whether it's a new interaction: create new one, or use the existing one.
	if (c->isNew)
		imt = shared_ptr<InteractionOfMyTetrahedron>(new InteractionOfMyTetrahedron());
	else
		imt = dynamic_pointer_cast<InteractionOfMyTetrahedron>(c->interactionGeometry);	

	bool isInteracting = false;
	for(int i=0 ; i<4 ; ++i )
		for(int j=0 ; j<4 ; ++j)
		{
			// both spheres must be calculated in global coordinate system
			//
			// c1,c2,c3,c4 are sphere centers in local coordinate system of tetrahedron,
			//             they must by rotated, so that they have the rotation of tetrahedron
			//             they must be translated, so that they have global coordinates of tetrahedron
			calcTwoSpheres(	
				// (Quaternionr)orientation * (Vector3r)(c1)     : rotates the sphere center from local into global coordinate system.
				//                             + se31.position   : translates it, to stay in the x,y,z in global coordinate system
				se31.orientation*(&(t1->c1))[i]+se31.position,

				(&(t1->r1))[i], // radius of sphere from first InteractingMyTetrahedron

				se32.orientation*(&(t2->c1))[j]+se32.position,
				(&(t2->r1))[j],

				imt->penetrationDepths[i][j], imt->normals[i][j], imt->contactPoints[i][j] );

			if( imt->penetrationDepths[i][j] > 0 )
				isInteracting = true;
		}


	c->interactionGeometry = imt;
	return isInteracting;
}


bool InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron::goReverse(
		const shared_ptr<InteractingGeometry>& cm1,
		const shared_ptr<InteractingGeometry>& cm2,
		const Se3r& se31,
		const Se3r& se32,
		const shared_ptr<Interaction>& c)
{
	bool isInteracting = go(cm2,cm1,se32,se31,c);
	if (isInteracting)
	{
		InteractionOfMyTetrahedron* itm = static_cast<InteractionOfMyTetrahedron*>(c->interactionGeometry.get());
		// reverse direction of normals
		for(int i=0 ; i<4 ; ++i )
			for(int j=0 ; j<4 ; ++j)
				itm->normals[i][j] =  -1.0*( itm->normals[i][j] );
	}
	return isInteracting;
}

