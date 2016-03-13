// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL

#include "Polyhedra.hpp"

//***************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Polyhedras. */
class Ig2_Polyhedra_Polyhedra_PolyhedraGeom: public IGeomFunctor
{
	public:
		virtual ~Ig2_Polyhedra_Polyhedra_PolyhedraGeom(){};
		virtual bool go(const shared_ptr<Shape>& shape1, const shared_ptr<Shape>& shape2, const State& state1,
			const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(const shared_ptr<Shape>& shape1, const shared_ptr<Shape>& shape2, const State& state1,
			const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		FUNCTOR2D(Polyhedra,Polyhedra);
		DEFINE_FUNCTOR_ORDER_2D(Polyhedra,Polyhedra);
		YADE_CLASS_BASE_DOC(Ig2_Polyhedra_Polyhedra_PolyhedraGeom,IGeomFunctor,"Create/update geometry of collision between 2 Polyhedras");	
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Polyhedra_Polyhedra_PolyhedraGeom);

//***************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Wall & Polyhedra. */
class Ig2_Wall_Polyhedra_PolyhedraGeom: public IGeomFunctor
{
	public:
		virtual ~Ig2_Wall_Polyhedra_PolyhedraGeom(){};
		virtual bool go(const shared_ptr<Shape>& shape1, const shared_ptr<Shape>& shape2, const State& state1,
			const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		FUNCTOR2D(Wall,Polyhedra);
		DEFINE_FUNCTOR_ORDER_2D(Wall,Polyhedra);
		YADE_CLASS_BASE_DOC(Ig2_Wall_Polyhedra_PolyhedraGeom,IGeomFunctor,"Create/update geometry of collision between Wall and Polyhedra");	
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Wall_Polyhedra_PolyhedraGeom);

//***************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Facet & Polyhedra. */
class Ig2_Facet_Polyhedra_PolyhedraGeom: public IGeomFunctor
{
	public:
		virtual ~Ig2_Facet_Polyhedra_PolyhedraGeom(){};
		virtual bool go(const shared_ptr<Shape>& shape1, const shared_ptr<Shape>& shape2, const State& state1,
			const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		FUNCTOR2D(Facet,Polyhedra);
		DEFINE_FUNCTOR_ORDER_2D(Facet,Polyhedra);
		YADE_CLASS_BASE_DOC(Ig2_Facet_Polyhedra_PolyhedraGeom,IGeomFunctor,"Create/update geometry of collision between Facet and Polyhedra");	
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Facet_Polyhedra_PolyhedraGeom);

//***************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Sphere & Polyhedra. */
class Ig2_Sphere_Polyhedra_ScGeom: public IGeomFunctor
{
	public:
		enum PointTriangleRelation { inside, edge, vertex, none };
		virtual ~Ig2_Sphere_Polyhedra_ScGeom(){};
		virtual bool go(const shared_ptr<Shape>& shape1, const shared_ptr<Shape>& shape2, const State& state1,
			const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		FUNCTOR2D(Sphere,Polyhedra);
		DEFINE_FUNCTOR_ORDER_2D(Sphere,Polyhedra);
		YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_Polyhedra_ScGeom,IGeomFunctor,"Create/update geometry of collision between Sphere and Polyhedra",
			((Real,edgeCoeff,1.0,,"multiplier of penetrationDepth when sphere contacts edge (simulating smaller volume of actual intersection or when several polyhedrons has common edge)"))
			((Real,vertexCoeff,1.0,,"multiplier of penetrationDepth when sphere contacts vertex (simulating smaller volume of actual intersection or when several polyhedrons has common vertex)"))
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Sphere_Polyhedra_ScGeom);

//***************************************************************************
#endif // YADE_CGAL
