/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron.hpp"
#include<yade/pkg-dem/InteractionOfMyTetrahedron.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-dem/InteractingMyTetrahedron.hpp>
#include<yade/pkg-dem/InteractingBox2InteractingSphere4SpheresContactGeometry.hpp>
#include <boost/shared_ptr.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>

void InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron::calcSphereAndBox(
		Vector3r c1,
		Real r1,
		Vector3r& extents,
		const Se3r& se3Box,
		Real& penetrationDepth,
		Vector3r& normal,
		Vector3r& contactPoint)
{
	// I will call here a function InteractingBox2InteractingSphere4SpheresContactGeometry::go()
	// and extract results from it, because it's a lengty calculation.
	//
	// to do this I must link with this class inside .pro file, otherwise it will compile, but linker will fail
	
	InteractingBox2InteractingSphere4SpheresContactGeometry worker;

	// Box
	boost::shared_ptr<InteractingGeometry>	box(new InteractingBox);
	static_cast<InteractingBox*>(box.get())->extents = extents;

	// Sphere
	boost::shared_ptr<InteractingGeometry>	sphere(new InteractingSphere);
	static_cast<InteractingSphere*>(sphere.get())->radius = r1;
	Se3r seSphere;
	seSphere.position = c1;
	//seSphere.orientation = ; // left uninitalized with random numbers, because is not used

	// empty interaction
	boost::shared_ptr<Interaction>		interaction(new Interaction);
	
	// do the work. Note that box is before sphere (**), just like the lenghty class name indicates.
	bool isInteracting = worker.go(box,sphere,se3Box,seSphere,interaction);

	SpheresContactGeometry* result = dynamic_cast<SpheresContactGeometry*>(interaction->interactionGeometry.get());

	if(isInteracting)
	{
		penetrationDepth	= result->penetrationDepth;
		normal			= -1.0*(result->normal); // -1.0 because of different order: tetrahedron before box (**)
		contactPoint		= result->contactPoint;
	}
	else
		penetrationDepth	= -1.0;

}

bool InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron::go(	
		const shared_ptr<InteractingGeometry>& cm1,
		const shared_ptr<InteractingGeometry>& cm2,
		const Se3r& se31,
		const Se3r& se32,
		const shared_ptr<Interaction>& c)
{
	InteractingMyTetrahedron* t = static_cast<InteractingMyTetrahedron*>(cm1.get());
	InteractingBox*	b = static_cast<InteractingBox*>(cm2.get());

	shared_ptr<InteractionOfMyTetrahedron> imt;
	// depending whether it's a new interaction: create new one, or use the existing one.
	if (c->isNew)
		imt = shared_ptr<InteractionOfMyTetrahedron>(new InteractionOfMyTetrahedron());
	else
		imt = YADE_PTR_CAST<InteractionOfMyTetrahedron>(c->interactionGeometry);	



	bool isInteracting = false;
	for(int i=0 ; i<4 ; ++i )
	{
		// both sphere and a box must be calculated in global coordinate system
		calcSphereAndBox(	
			se31.orientation*(&(t->c1))[i]+se31.position,
			(&(t->r1))[i],

			b->extents,
			se32,

			imt->penetrationDepths[i][0], imt->normals[i][0], imt->contactPoints[i][0] );

		if( imt->penetrationDepths[i][0] > 0 )
			isInteracting = true;

		imt->penetrationDepths[i][0] /= 4.0; // scatter this penetration along four emulated spheres.
		for(int j=1 ; j<4 ; ++j )
		{
			imt->penetrationDepths[i][j]	= imt->penetrationDepths[i][0];
			imt->normals[i][j]		= imt->normals[i][0];
			imt->contactPoints[i][j]	= imt->contactPoints[i][0];
		}
	}

	c->interactionGeometry = imt;
	return isInteracting;

}

bool InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron::goReverse(
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

