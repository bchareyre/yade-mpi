/*****************************************************************************
*  Copyright (C) 2015 by Anna Effeindzourou   anna.effeindzourou@gmail.com   *
*  Copyright (C) 2015 by Bruno Chareyre       bruno.chareyre@hmg.inpg.fr     *
*  Copyright (C) 2015 by Klaus Thoeni         klaus.thoeni@gmail.com         *
*  This program is free software; it is licensed under the terms of the      *
*  GNU General Public License v2 or later. See file LICENSE for details.     *
******************************************************************************/
#pragma once
#include<core/Shape.hpp>
#include <pkg/dem/ScGeom.hpp>
#include<pkg/common/Grid.hpp>
#include <core/Body.hpp>
#include<pkg/common/Sphere.hpp>
#include <pkg/common/Dispatching.hpp>
#include <pkg/common/Grid.hpp>
#include <pkg/dem/FrictPhys.hpp>
#include<lib/base/Math.hpp>
#include<pkg/common/InteractionLoop.hpp>
#include <pkg/dem/ElasticContactLaw.hpp>
#include <pkg/dem/Ig2_Facet_Sphere_ScGeom.hpp>
#ifdef YADE_OPENGL
	#include<pkg/common/GLDrawFunctors.hpp>
#endif


//!##################	IGeom Functors   ##################
//!			O/
class Ig2_Sphere_PFacet_ScGridCoGeom: public Ig2_Sphere_GridConnection_ScGridCoGeom{
	public :
		
		boost::tuple<Vector3r,bool, double, double,double> projection(
					const shared_ptr<Shape>& cm2,
					const State& state1);
		virtual bool go(const shared_ptr<Shape>& cm1,
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
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_PFacet_ScGridCoGeom,Ig2_Sphere_GridConnection_ScGridCoGeom,"Create/update a :yref:`ScGridCoGeom` instance representing intersection of :yref:`PFacet` and :yref:`Sphere`.",
		((Real,shrinkFactor,((void)"no shrinking",0),,"The radius of the inscribed circle of the facet is decreased by the value of the sphere's radius multipled by *shrinkFactor*. From the definition of contact point on the surface made of facets, the given surface is not continuous and becomes in effect surface covered with triangular tiles, with gap between the separate tiles equal to the sphere's radius multiplied by 2×*shrinkFactor*. If zero, no shrinking is done."))
	);
	DECLARE_LOGGER;
	FUNCTOR2D(Sphere,PFacet);
	DEFINE_FUNCTOR_ORDER_2D(Sphere,PFacet);
};

REGISTER_SERIALIZABLE(Ig2_Sphere_PFacet_ScGridCoGeom);




class Ig2_GridConnection_PFacet_ScGeom: public Ig2_Sphere_GridConnection_ScGridCoGeom{
	public :
		virtual bool go(const shared_ptr<Shape>& cm1,
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
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_GridConnection_PFacet_ScGeom,Ig2_Sphere_GridConnection_ScGridCoGeom,"Create/update a :yref:`ScGeom` instance representing intersection of :yref:`Facet` and :yref:`GridConnection`.",
		((Real,shrinkFactor,((void)"no shrinking",0),,"The radius of the inscribed circle of the facet is decreased by the value of the sphere's radius multipled by *shrinkFactor*. From the definition of contact point on the surface made of facets, the given surface is not continuous and becomes in effect surface covered with triangular tiles, with gap between the separate tiles equal to the sphere's radius multiplied by 2×*shrinkFactor*. If zero, no shrinking is done."))
	);
	DECLARE_LOGGER;
	FUNCTOR2D(GridConnection,PFacet);
	DEFINE_FUNCTOR_ORDER_2D(GridConnection,PFacet);
};

REGISTER_SERIALIZABLE(Ig2_GridConnection_PFacet_ScGeom);

//!			O/
class Ig2_PFacet_PFacet_ScGeom: public Ig2_Sphere_PFacet_ScGridCoGeom{
	public :
		virtual bool go(const shared_ptr<Shape>& cm1,
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
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_PFacet_PFacet_ScGeom,Ig2_Sphere_PFacet_ScGridCoGeom,"Create/update a :yref:`ScGridCoGeom` instance representing intersection of :yref:`Facet` and :yref:`Sphere`.",
		((Real,shrinkFactor,((void)"no shrinking",0),,"The radius of the inscribed circle of the facet is decreased by the value of the sphere's radius multipled by *shrinkFactor*. From the definition of contact point on the surface made of facets, the given surface is not continuous and becomes in effect surface covered with triangular tiles, with gap between the separate tiles equal to the sphere's radius multiplied by 2×*shrinkFactor*. If zero, no shrinking is done."))
	);
	DECLARE_LOGGER;
	FUNCTOR2D(PFacet,PFacet);
	DEFINE_FUNCTOR_ORDER_2D(PFacet,PFacet);
};

REGISTER_SERIALIZABLE(Ig2_PFacet_PFacet_ScGeom);


/********* Wall + Sphere **********/

class Ig2_Wall_PFacet_ScGeom: public Ig2_Wall_Sphere_ScGeom{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Wall_PFacet_ScGeom,Ig2_Wall_Sphere_ScGeom,"Create/update a :yref:`ScGeom` instance representing intersection of :yref:`Wall` and :yref:`PFacet`.",
	);
	FUNCTOR2D(Wall,PFacet);
	DEFINE_FUNCTOR_ORDER_2D(Wall,PFacet);
};
REGISTER_SERIALIZABLE(Ig2_Wall_PFacet_ScGeom);


//!##################	Bounds   #####################

class Bo1_PFacet_Aabb : public BoundFunctor
{
	public :
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
	FUNCTOR1D(PFacet);
	YADE_CLASS_BASE_DOC_ATTRS(Bo1_PFacet_Aabb,BoundFunctor,"Functor creating :yref:`Aabb` from a :yref:`PFacet`.",
		((Real,aabbEnlargeFactor,((void)"deactivated",-1),,"Relative enlargement of the bounding box; deactivated if negative."))
	);
};
REGISTER_SERIALIZABLE(Bo1_PFacet_Aabb);

