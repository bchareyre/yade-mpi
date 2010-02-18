/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionGeometryFunctor.hpp>

class Ig2_Sphere_Sphere_ScGeom: public InteractionGeometryFunctor{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& se32, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& se32, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_Sphere_ScGeom,InteractionGeometryFunctor,"Create/update a :yref:`ScGeom` instance representing intersection of two :yref:`Spheres<Sphere>`.",
		((Real,interactionDetectionFactor,1,"Enlarge both radii by this factor (if >1), to permit creation of distant interactions.\n\nInteractionGeometry will be computed when interactionDetectionFactor*(rad1+rad2) > distance.\n\n.. note::\n\t This parameter is functionally coupled with :yref:`Bo1_Sphere_Aabb::aabbEnlargeFactor`, which will create larger bounding boxes and should be of the same value.\n\n.. warning::\n\tFunctionally equal class :yref:`Ig2_Sphere_Sphere_Dem3DofGeom` (which creates :yref:`Dem3DofGeom` rather than :yref:`ScGeom`) calls this parameter :yref:`distFactor<Ig2_Sphere_Sphere_Dem3DofGeom::distFactor>`, but its semantics is *different* in some aspects."))
	);
	FUNCTOR2D(Sphere,Sphere);
	// needed for the dispatcher, even if it is symmetric
	DEFINE_FUNCTOR_ORDER_2D(Sphere,Sphere);
};
REGISTER_SERIALIZABLE(Ig2_Sphere_Sphere_ScGeom);

