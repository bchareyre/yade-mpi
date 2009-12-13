/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/pkg-common/InteractionGeometryFunctor.hpp>

class Ig2_Facet_Sphere_ScGeom : public InteractionGeometryFunctor
{
	public :
		virtual bool go(	const shared_ptr<Shape>& cm1,
					const shared_ptr<Shape>& cm2,
					const State& state1,
					const State& state2,
					const Vector3r& shift2,
					const bool& force,
					const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1,
					const shared_ptr<Shape>& cm2,
					const State& state1,
					const State& state2,
					const Vector3r& shift2,
					const bool& force,
					const shared_ptr<Interaction>& c);
					
		Ig2_Facet_Sphere_ScGeom();		
					
	REGISTER_CLASS_NAME(Ig2_Facet_Sphere_ScGeom);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryFunctor);
	REGISTER_ATTRIBUTES(InteractionGeometryFunctor,(shrinkFactor));

	DECLARE_LOGGER;

	FUNCTOR2D(InteractingFacet,InteractingSphere);

	DEFINE_FUNCTOR_ORDER_2D(InteractingFacet,InteractingSphere);

	// The radius of the inscribed circle of the facet is decreases by the
	// value of the sphere's radius multipled by shrinkFactor. So, at
	// definition of a contact point on the surface made of facets, the given
	// surface is not continuous and is similar to a surface covered with a
	// tile, with a gap between the separate tiles equal to the sphere's radius
	// multiplied by 2*shrinkFactor. By default shrinkFactor=0.
	Real shrinkFactor; 
};

REGISTER_SERIALIZABLE(Ig2_Facet_Sphere_ScGeom);


