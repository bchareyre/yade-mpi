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

#pragma once
#include<yade/pkg/common/Dispatching.hpp>

class Ig2_Box_Sphere_ScGeom : public IGeomFunctor
{
	public :
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);

		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);

	YADE_CLASS_BASE_DOC(Ig2_Box_Sphere_ScGeom,IGeomFunctor,"Create an interaction geometry :yref:`ScGeom` from :yref:`Box` and :yref:`Sphere`, representing the box with a projected virtual sphere of same radius.")
	FUNCTOR2D(Box,Sphere);
	DEFINE_FUNCTOR_ORDER_2D(Box,Sphere);
};
REGISTER_SERIALIZABLE(Ig2_Box_Sphere_ScGeom);

class Ig2_Box_Sphere_ScGeom6D : public Ig2_Box_Sphere_ScGeom
{
	public :
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);

		virtual bool goReverse(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);

	YADE_CLASS_BASE_DOC(Ig2_Box_Sphere_ScGeom6D,Ig2_Box_Sphere_ScGeom,"Create an interaction geometry :yref:`ScGeom6D` from :yref:`Box` and :yref:`Sphere`, representing the box with a projected virtual sphere of same radius.")
	FUNCTOR2D(Box,Sphere);
	DEFINE_FUNCTOR_ORDER_2D(Box,Sphere);
};
REGISTER_SERIALIZABLE(Ig2_Box_Sphere_ScGeom6D);

