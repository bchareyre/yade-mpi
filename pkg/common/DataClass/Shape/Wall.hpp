// © 2009 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/Shape.hpp>
#include<yade/pkg-common/BoundFunctor.hpp>


/*! Object representing infinite plane aligned with the coordinate system (axis-aligned wall). */
class Wall: public Shape{
	public:
		virtual ~Wall(); // vtable
	YADE_CLASS_BASE_DOC_ATTRDECL_CTOR_PY(Wall,Shape,"Object representing infinite plane aligned with the coordinate system (axis-aligned wall).",
		((int,sense,0,"Which side of the wall interacts: -1 for negative only, 0 for both, +1 for positive only"))
		((int,axis,0,"Axis of the normal; can be 0,1,2 for +x, +y, +z respectively (Body's orientation is disregarded for walls)")),
		/*ctor*/createIndex();,
		/*py*/
	);
	REGISTER_CLASS_INDEX(Wall,Shape);
};	
REGISTER_SERIALIZABLE(Wall);

/*! Functor for computing axis-aligned bounding box
    from axis-aligned wall. Has no parameters. */
class Bo1_Wall_Aabb: public BoundFunctor{
	public:
		virtual void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body*);
	FUNCTOR2D(Wall,Aabb);
	YADE_CLASS_BASE_DOC_ATTRDECL_CTOR_PY(Bo1_Wall_Aabb,BoundFunctor,"Creates/updates an Aabb of a wall",,,);
};
REGISTER_SERIALIZABLE(Bo1_Wall_Aabb);
#ifdef YADE_OPENGL
	#include<yade/pkg-common/GLDrawFunctors.hpp>
	class Gl1_Wall: public GlShapeFunctor{	
		//! Number of divisions
		static int div;
		public:
			virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
		RENDERS(Wall);
		YADE_CLASS_BASE_DOC_ATTRS(Gl1_Wall,GlShapeFunctor,"Renders :yref:`Wall` object",
			((div,"|ystatic| Number of divisions of the wall inside visible scene part. :ydefault:`20`"))
		);
	};
	REGISTER_SERIALIZABLE(Gl1_Wall);
#endif

