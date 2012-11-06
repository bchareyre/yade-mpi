#pragma once
#include "Sphere.hpp"
#include<yade/pkg/dem/FrictPhys.hpp>
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
		Real getLength();
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

//!			O-O
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

//!			O/
class ScGridCoGeom: public ScGeom {
public:
    /// Emulate a sphere whose position is the projection of sphere's center on cylinder sphere, and with motion linearly interpolated between nodes
    State fictiousState;
    virtual ~ScGridCoGeom ();
    YADE_CLASS_BASE_DOC_ATTRS_CTOR(ScGridCoGeom,ScGeom,"Geometry of a cylinder-sphere contact.",
                                   ((bool,onNode,false,,"contact on node?"))
                                   ((int,isDuplicate,0,,"this flag is turned true (1) automatically if the contact is shared between two chained cylinders. A duplicated interaction will be skipped once by the constitutive law, so that only one contact at a time is effective. If isDuplicate=2, it means one of the two duplicates has no longer geometric interaction, and should be erased by the constitutive laws."))
                                   ((int,trueInt,-1,,"Defines the body id of the cylinder where the contact is real, when :yref:`CylScGeom::isDuplicate`>0."))
                                   //((Vector3r,start,Vector3r::Zero(),,"position of 1st node |yupdate|"))
                                   //((Vector3r,end,Vector3r::Zero(),,"position of 2nd node |yupdate|"))
                                   ((int,id3,0,,"id of next chained cylinder |yupdate|"))
                                   ((int,id4,0,,"id of next chained cylinder |yupdate|"))
                                   ((Real,relPos,0,,"position of the contact on the cylinder (0: node-, 1:node+) |yupdate|")),
                                   createIndex(); /*ctor*/
                                  );
    REGISTER_CLASS_INDEX(ScGridCoGeom,ScGeom);
};
REGISTER_SERIALIZABLE(ScGridCoGeom);

//!##################	IGeom Functors   #####################

//!			O-O
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

//!			O/
class Ig2_Sphere_GridConnection_ScGridCoGeom: public IGeomFunctor{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);

		YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_GridConnection_ScGridCoGeom,IGeomFunctor,"Create/update a :yref:`ScGridCoGeom6D` instance representing the geometry of a contact point between a GricConnection and a Sphere including relative rotations.",
		((Real,interactionDetectionFactor,1,,"Enlarge both radii by this factor (if >1), to permit creation of distant interactions."))
	);
	FUNCTOR2D(Sphere,GridConnection);
	// needed for the dispatcher, even if it is symmetric
	DEFINE_FUNCTOR_ORDER_2D(Sphere,GridConnection);
};
REGISTER_SERIALIZABLE(Ig2_Sphere_GridConnection_ScGridCoGeom);


//!##################	Laws   #####################

//!			O/
class Law2_ScGridCoGeom_FrictPhys_CundallStrack: public LawFunctor{
	public:
		//OpenMPAccumulator<Real> plasticDissipation;
		virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		//Real elasticEnergy ();
		//Real getPlasticDissipation();
		//void initPlasticDissipation(Real initVal=0);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGridCoGeom_FrictPhys_CundallStrack,LawFunctor,"Law for linear compression, and Mohr-Coulomb plasticity surface without cohesion.\nThis law implements the classical linear elastic-plastic law from [CundallStrack1979]_ (see also [Pfc3dManual30]_). The normal force is (with the convention of positive tensile forces) $F_n=\\min(k_n u_n, 0)$. The shear force is $F_s=k_s u_s$, the plasticity condition defines the maximum value of the shear force : $F_s^{\\max}=F_n\\tan(\\phi)$, with $\\phi$ the friction angle.\n\n.. note::\n This law uses :yref:`ScGeom`.\n\n.. note::\n This law is well tested in the context of triaxial simulation, and has been used for a number of published results (see e.g. [Scholtes2009b]_ and other papers from the same authors). It is generalised by :yref:`Law2_ScGeom6D_CohFrictPhys_CohesionMoment`, which adds cohesion and moments at contact.",
		((bool,neverErase,false,,"Keep interactions even if particles go away from each other (only in case another constitutive law is in the scene, e.g. :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`)"))
		((bool,traceEnergy,false,Attr::hidden,"Define the total energy dissipated in plastic slips at all contacts."))
		((int,plastDissipIx,-1,(Attr::hidden|Attr::noSave),"Index for plastic dissipation (with O.trackEnergy)"))
		((int,elastPotentialIx,-1,(Attr::hidden|Attr::noSave),"Index for elastic potential energy (with O.trackEnergy)"))
		,,
		//.def("elasticEnergy",&Law2_ScGeom_FrictPhys_CundallStrack::elasticEnergy,"Compute and return the total elastic energy in all \"FrictPhys\" contacts")
		//.def("plasticDissipation",&Law2_ScGeom_FrictPhys_CundallStrack::getPlasticDissipation,"Total energy dissipated in plastic slips at all FrictPhys contacts. Computed only if :yref:`Law2_ScGeom_FrictPhys_CundallStrack::traceEnergy` is true.")
		//.def("initPlasticDissipation",&Law2_ScGeom_FrictPhys_CundallStrack::initPlasticDissipation,"Initialize cummulated plastic dissipation to a value (0 by default).")
	);
	FUNCTOR2D(ScGridCoGeom,FrictPhys);
};
REGISTER_SERIALIZABLE(Law2_ScGridCoGeom_FrictPhys_CundallStrack);



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









