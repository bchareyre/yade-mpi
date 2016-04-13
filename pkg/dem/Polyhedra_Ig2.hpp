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
		YADE_CLASS_BASE_DOC_ATTRS(Ig2_Polyhedra_Polyhedra_PolyhedraGeom,IGeomFunctor,"Create/update geometry of collision between 2 Polyhedras",
			((Real,interactionDetectionFactor,1,,"see :yref:`Ig2_Sphere_Sphere_ScGeom.interactionDetectionFactor`"))
		);
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
/*! Plyhedra -> ScGeom. */
class Ig2_Polyhedra_Polyhedra_ScGeom: public IGeomFunctor
{
	public:
		virtual ~Ig2_Polyhedra_Polyhedra_ScGeom(){};
		virtual bool go(const shared_ptr<Shape>& shape1, const shared_ptr<Shape>& shape2, const State& state1,
			const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(const shared_ptr<Shape>& shape1, const shared_ptr<Shape>& shape2, const State& state1,
			const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		FUNCTOR2D(Polyhedra,Polyhedra);
		DEFINE_FUNCTOR_ORDER_2D(Polyhedra,Polyhedra);
		YADE_CLASS_BASE_DOC_ATTRS(Ig2_Polyhedra_Polyhedra_ScGeom,IGeomFunctor,"EXPERIMENTAL. Ig2 functor creating ScGeom from two Polyhedra shapes. The radii are computed as a distance of contact point (computed using Ig2_Polyhedra_Polyhedra_PolyhedraGeom) and center of particle. Tested only for face-face contacts (like brick wall).",
			((Real,interactionDetectionFactor,1,,"see Ig2_Sphere_Sphere_ScGeom.interactionDetectionFactor"))
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Polyhedra_Polyhedra_ScGeom);

class Ig2_Polyhedra_Polyhedra_PolyhedraGeomOrScGeom: public IGeomFunctor
{
	public:
		virtual ~Ig2_Polyhedra_Polyhedra_PolyhedraGeomOrScGeom(){};
		virtual bool go(const shared_ptr<Shape>& shape1, const shared_ptr<Shape>& shape2, const State& state1,
			const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(const shared_ptr<Shape>& shape1, const shared_ptr<Shape>& shape2, const State& state1,
			const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		FUNCTOR2D(Polyhedra,Polyhedra);
		DEFINE_FUNCTOR_ORDER_2D(Polyhedra,Polyhedra);
		YADE_CLASS_BASE_DOC_ATTRS(Ig2_Polyhedra_Polyhedra_PolyhedraGeomOrScGeom,IGeomFunctor,"EXPERIMENTAL. A hacky helper Ig2 functor combining two Polyhedra shapes and creating, according to the settings, either ScGeom or PolyhedraGeom.",
			((bool,createScGeom,true,,"If true, creates ScGeom on new contacts. Creates PolyhedraGeom otherwise. On existing contacts Ig2_Polyhedra_Polyhedra_PolyhedraGeom or Ig2_Polyhedra_Polyhedra_ScGeom is used according to present IGeom isntance."))
			((shared_ptr<Ig2_Polyhedra_Polyhedra_PolyhedraGeom>,ig2polyhedraGeom,new Ig2_Polyhedra_Polyhedra_PolyhedraGeom,,"Helper Ig2 functor for PolyhedraGeom (to be able to modify its settings)"))
			((shared_ptr<Ig2_Polyhedra_Polyhedra_ScGeom>,ig2scGeom,new Ig2_Polyhedra_Polyhedra_ScGeom,,"Helper Ig2 functor for ScGeom (to be able to modify its settings)"))
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Polyhedra_Polyhedra_PolyhedraGeomOrScGeom);

//***************************************************************************
#endif // YADE_CGAL
