#pragma once
#include "Sphere.hpp"
#include <yade/pkg/dem/ScGeom.hpp>
#include <yade/core/Body.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/dem/Ig2_Sphere_Sphere_ScGeom.hpp>
#ifdef YADE_OPENGL
	#include<yade/pkg/common/GLDrawFunctors.hpp>
#endif



//!##################	SHAPES   #####################

class GridConnection: public Sphere{
public:
		virtual ~GridConnection();
		float getLength();
		Vector3r getSegment();
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(GridConnection,Sphere,"Cylinder geometry of a connection between two :yref:`Node`.",
		((shared_ptr<Body> , node1 , ,,))
		((shared_ptr<Body> , node2 , ,,))
  		//((Quaternionr,chainedOrientation,Quaternionr::Identity(),,"Deviation of node1 orientation from node2 vector"))
		,createIndex();/*ctor*/
		);
	REGISTER_CLASS_INDEX(GridConnection,Sphere);
};
REGISTER_SERIALIZABLE(GridConnection);


class GridNode: public Sphere{
	public:
		virtual ~GridNode();
		void addConnection(shared_ptr<Body> GC);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(GridNode,Sphere,"GridNode component of a grid",
		((vector<shared_ptr<Body> >,ConnList,,,"List of :yref:'GridConnection' the GridNode is connected to.")),
		/*ctor*/
		createIndex();,
		/*py*/
		.def("addConnection",&GridNode::addConnection,(python::arg("Body")),"Add a GridConnection to the GridNode")
	);
	REGISTER_CLASS_INDEX(GridNode,Sphere);
};

REGISTER_SERIALIZABLE(GridNode);


//!##################	Contact Geometry   #####################

class GridNodeGeom6D: public ScGeom6D {
	public:
		virtual ~GridNodeGeom6D();
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(GridNodeGeom6D,ScGeom6D,"Geometry of a GridNode-GridNode contact. Inherits almost everything from :yref:'ScGeom6D'.",
		((shared_ptr<Body>, connectionBody,,,":yref:'GridNode' yref:'Body' who is linking the two :yref:'GridNodes'."))
		,
		/* extra initializers */,
		/* ctor */ createIndex();,
		/* py */
	);
	REGISTER_CLASS_INDEX(GridNodeGeom6D,ScGeom6D);
};
REGISTER_SERIALIZABLE(GridNodeGeom6D);

//!##################	IGeom Functors   #####################

class Ig2_GridNode_GridNode_GridNodeGeom6D: public Ig2_Sphere_Sphere_ScGeom{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);

		YADE_CLASS_BASE_DOC_ATTRS(Ig2_GridNode_GridNode_GridNodeGeom6D,Ig2_Sphere_Sphere_ScGeom,"Create/update a :yref:`ScGeom6D` instance representing the geometry of a contact point between two :yref:`Sphere` s, including relative rotations.",
		((bool,updateRotations,true,,"Precompute relative rotations. Turning this false can speed up simulations when rotations are not needed in constitutive laws (e.g. when spheres are compressed without cohesion and moment in early stage of a triaxial test), but is not foolproof. Change this value only if you know what you are doing."))
		((bool,creep,false,,"Substract rotational creep from relative rotation. The rotational creep :yref:`ScGeom6D::twistCreep` is a quaternion and has to be updated inside a constitutive law, see for instance :yref:`Law2_ScGeom6D_CohFrictPhys_CohesionMoment`."
		))
	);
	FUNCTOR2D(GridNode,GridNode);
	// needed for the dispatcher, even if it is symmetric
	DEFINE_FUNCTOR_ORDER_2D(GridNode,GridNode);
};
REGISTER_SERIALIZABLE(Ig2_GridNode_GridNode_GridNodeGeom6D);


//!##################	Bounds   #####################

class Bo1_GridConnection_Aabb : public BoundFunctor
{
	public :
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
	FUNCTOR1D(GridConnection);
	YADE_CLASS_BASE_DOC_ATTRS(Bo1_GridConnection_Aabb,BoundFunctor,"Functor creating :yref:`Aabb` from :yref:'GridConnection'.",
		((Real,aabbEnlargeFactor,((void)"deactivated",-1),,"Relative enlargement of the bounding box; deactivated if negative.\n\n.. note::\n\tThis attribute is used to create distant interaction, but is only meaningful with an :yref:`IGeomFunctor` which will not simply discard such interactions:  :yref:`Ig2_Cylinder_Cylinder_ScGeom::interactionDetectionFactor` should have the same value as :yref:`aabbEnlargeFactor<Bo1_Cylinder_Aabb::aabbEnlargeFactor>`."))
	);
};
REGISTER_SERIALIZABLE(Bo1_GridConnection_Aabb);

//!##################	Rendering   #####################
#ifdef YADE_OPENGL
class Gl1_GridConnection : public GlShapeFunctor{
	private:
		//static int glCylinderList;
		//void subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth);
		void drawCylinder(bool wire, Real radius, Real length, const Quaternionr& shift=Quaternionr::Identity()) const;
		//void initGlLists(void);
	public:
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
		void out( Quaternionr q );
	YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_GridConnection,GlShapeFunctor,"Renders :yref:`Cylinder` object",
		((bool,wire,false,,"Only show wireframe (controlled by ``glutSlices`` and ``glutStacks``."))
		((bool,glutNormalize,true,,"Fix normals for non-wire rendering"))
		((int,glutSlices,8,,"Number of sphere slices."))
		((int,glutStacks,4,,"Number of sphere stacks."))
	);
	RENDERS(GridConnection);
};
REGISTER_SERIALIZABLE(Gl1_GridConnection);
#endif









