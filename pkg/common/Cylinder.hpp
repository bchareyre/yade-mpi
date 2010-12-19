#pragma once
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/core/Shape.hpp>
#include<yade/core/State.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/core/Scene.hpp>
#ifdef YADE_OPENGL
	#include<yade/pkg/common/GLDrawFunctors.hpp>
#endif


class Cylinder: public Sphere{
	public:
// 		Cylinder(Real _radius, Real _length): length(_length) { /*segment=Vector3r(0,0,1)*_length;*/ radius=_radius; createIndex(); }
		virtual ~Cylinder ();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Cylinder,Sphere,"Geometry of a cylinder, as Minkowski sum of line and sphere.",
// 		((Real,radius,NaN,,"Radius [m]"))
		((Real,length,NaN,,"Length [m]"))
		((Vector3r,segment,Vector3r::Zero(),,"Length vector")),
		createIndex();

		/*ctor*/
		segment=Vector3r(0,0,1)*length;
	);
	REGISTER_CLASS_INDEX(Cylinder,Sphere);
};
REGISTER_SERIALIZABLE(Cylinder);

class ChainedCylinder: public Cylinder{
	public:
// 		ChainedCylinder(Real _radius, Real _length);/*: Cylinder(_radius,_length){}*/
		virtual ~ChainedCylinder ();

		//Keep pointers or copies of connected states?
// 		ChainedState st1, st2;


	YADE_CLASS_BASE_DOC_ATTRS_CTOR(ChainedCylinder,Cylinder,"Geometry of a deformable chained cylinder, using geometry :yref:`Cylinder`.",
  		((Real,initLength,0,,"tensile-free length, used as reference for tensile strain"))
  		((Quaternionr,chainedOrientation,Quaternionr::Identity(),,"Deviation of node1 orientation from node-to-node vector"))
		,createIndex();/*ctor*/
// 		state=shared_ptr<ChainedState>(new ChainedState);

	);
	REGISTER_CLASS_INDEX(ChainedCylinder,Cylinder);
};
REGISTER_SERIALIZABLE(ChainedCylinder);

class CylScGeom: public ScGeom{
	public:
		/// Emulate a sphere whose position is the projection of sphere's center on cylinder sphere, and with motion linearly interpolated between nodes
		State fictiousState;
// 		shared_ptr<Interaction> duplicate;

		virtual ~CylScGeom ();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CylScGeom,ScGeom,"Geometry of a cylinder-sphere contact.",
		((bool,onNode,false,,"contact on node?"))
		((int,isDuplicate,0,,"this flag is turned true (1) automaticaly if the contact is shared between two chained cylinders. A duplicated interaction will be skipped once by the constitutive law, so that only one contact at a time is effective. If isDuplicate=2, it means one of the two duplicates has no longer geometric interaction, and should be erased by the constitutive laws."))
		((int,trueInt,-1,,"Defines the body id of the cylinder where the contact is real, when :yref:`CylScGeom::isDuplicate`>0."))
		((Vector3r,start,Vector3r::Zero(),,"position of 1st node |yupdate|"))
		((Vector3r,end,Vector3r::Zero(),,"position of 2nd node |yupdate|"))
		((Body::id_t,id3,0,,"id of next chained cylinder |yupdate|"))
		((Real,relPos,0,,"position of the contact on the cylinder (0: node-, 1:node+) |yupdate|")),
		createIndex(); /*ctor*/
	);
	REGISTER_CLASS_INDEX(CylScGeom,ScGeom);
};
REGISTER_SERIALIZABLE(CylScGeom);


class ChainedState: public State{
	public:
		static vector<vector<Body::id_t> > chains;
		static unsigned int currentChain;
		vector<Body::id_t> barContacts;
		vector<Body::id_t> nodeContacts;
// 		shared_ptr<ChainedState> statePrev;

		virtual ~ChainedState ();
		void addToChain(Body::id_t bodyId) {
			if (chains.size()<=currentChain) chains.resize(currentChain+1);
			chainNumber=currentChain;
 			rank=chains[currentChain].size();
 			chains[currentChain].push_back(bodyId);
			bId=bodyId;
// 			if (rank>0) statePrev = Body::byId(chains[chainNumber][rank-1],scene)->state;
		}

 		void postLoad (ChainedState&){
			if (bId<0) return;//state has not been chained yet
 			if (chains.size()<=currentChain) chains.resize(currentChain+1);
			if (chains[currentChain].size()<=rank) chains[currentChain].resize(rank+1);
			chains[currentChain][rank]=bId;
// 			if (rank>0) statePrev = Body::byId(chains[chainNumber][rank-1],scene)->state;
		}

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(ChainedState,State,"State of a chained bodies, containing information on connectivity in order to track contacts jumping over contiguous elements. Chains are 1D lists from which id of chained bodies are retrieved via :yref:rank<ChainedState::rank>` and :yref:chainNumber<ChainedState::chainNumber>`.",
 		((unsigned int,rank,0,,"rank in the chain"))
 		((unsigned int,chainNumber,0,,"chain id"))
 		((int,bId,-1,,"id of the body containing - for postLoad operations only"))
		,
		/* additional initializers */
/*			((pos,se3.position))
			((ori,se3.orientation)),*/
		,
		/* ctor */ createIndex(); ,
		/*py*/
// 		.def_readwrite("chains",&ChainedState::chains,"documentation")
		.def_readwrite("currentChain",&ChainedState::currentChain,"Current active chain (where newly created chained bodies will be appended).")
		.def("addToChain",&ChainedState::addToChain,(python::arg("bodyId")),"Add body to current active chain")
	);
	REGISTER_CLASS_INDEX(ChainedState,State);
};
REGISTER_SERIALIZABLE(ChainedState);


class Ig2_Sphere_ChainedCylinder_CylScGeom: public IGeomFunctor{
	public:
// 		virtual ~Ig2_Sphere_ChainedCylinder_CylScGeom ();
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_ChainedCylinder_CylScGeom,IGeomFunctor,"Create/update a :yref:`ScGeom` instance representing intersection of two :yref:`Spheres<Sphere>`.",
		((Real,interactionDetectionFactor,1,,"Enlarge both radii by this factor (if >1), to permit creation of distant interactions."))
	);
	FUNCTOR2D(Sphere,ChainedCylinder);
	DEFINE_FUNCTOR_ORDER_2D(Sphere,ChainedCylinder);
};
REGISTER_SERIALIZABLE(Ig2_Sphere_ChainedCylinder_CylScGeom);

class Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D: public IGeomFunctor{
	public:
// 		virtual ~Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D ()  {};
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D,IGeomFunctor,"Create/update a :yref:`ScGeom` instance representing connexion between :yref:`chained cylinders<ChainedCylinder>`.",
		((Real,interactionDetectionFactor,1,,"Enlarge both radii by this factor (if >1), to permit creation of distant interactions."))
	);
	FUNCTOR2D(ChainedCylinder,ChainedCylinder);
	// needed for the dispatcher, even if it is symmetric
	DEFINE_FUNCTOR_ORDER_2D(ChainedCylinder,ChainedCylinder);
};
REGISTER_SERIALIZABLE(Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D);


#ifdef YADE_OPENGL
class Gl1_Cylinder : public GlShapeFunctor{
	private:
		static int glCylinderList;
		void subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth);
		void drawCylinder(bool wire, Real radius, Real length, const Quaternionr& shift=Quaternionr::Identity()) const;
		void initGlLists(void);
	public:
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
		void out( Quaternionr q );
	YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_Cylinder,GlShapeFunctor,"Renders :yref:`Cylinder` object",
		((bool,wire,false,,"Only show wireframe (controlled by ``glutSlices`` and ``glutStacks``."))
		((bool,glutNormalize,true,,"Fix normals for non-wire rendering"))
		((int,glutSlices,8,,"Number of sphere slices."))
		((int,glutStacks,4,,"Number of sphere stacks."))
	);
	RENDERS(Cylinder);
	friend class Gl1_ChainedCylinder;
};

//!This doesn't work : the 1D dispatcher will pick Gl1_Cylinder to display ChainedCylinders, workaround : add shift to cylinders (should be a variable of chained cylinders only).
class Gl1_ChainedCylinder : public Gl1_Cylinder{
	public:
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>& state, bool,const GLViewInfo&);
	YADE_CLASS_BASE_DOC(Gl1_ChainedCylinder,Gl1_Cylinder,"Renders :yref:`ChainedCylinder` object including a shift for compensating flexion."
	);
	RENDERS(ChainedCylinder);
};


/*
class Gl1_ChainedCylinder : public GlShapeFunctor{
	private:
		static int glCylinderList;
		void subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth);
		void drawCylinder(bool wire, Real radius, Real length, const Quaternionr& shift=Quaternionr::Identity()) const;
		void initGlLists(void);
	public:
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
	YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_ChainedCylinder,GlShapeFunctor,"Renders :yref:`ChainedCylinder` object including a shift for compensating flexion.",
		((bool,wire,false,,"Only show wireframe (controlled by ``glutSlices`` and ``glutStacks``."))
		((bool,glutNormalize,true,,"Fix normals for non-wire rendering"))
		((int,glutSlices,8,,"Number of sphere slices."))
		((int,glutStacks,4,,"Number of sphere stacks."))
	);
	RENDERS(ChainedCylinder);
};*/

#endif


class Bo1_Cylinder_Aabb : public BoundFunctor
{
	public :
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
	FUNCTOR1D(Cylinder);
	YADE_CLASS_BASE_DOC_ATTRS(Bo1_Cylinder_Aabb,BoundFunctor,"Functor creating :yref:`Aabb` from :yref:`Cylinder`.",
		((Real,aabbEnlargeFactor,((void)"deactivated",-1),,"Relative enlargement of the bounding box; deactivated if negative.\n\n.. note::\n\tThis attribute is used to create distant interaction, but is only meaningful with an :yref:`IGeomFunctor` which will not simply discard such interactions: :yref:`Ig2_Cylinder_Cylinder_Dem3DofGeom::distFactor` / :yref:`Ig2_Cylinder_Cylinder_ScGeom::interactionDetectionFactor` should have the same value as :yref:`aabbEnlargeFactor<Bo1_Cylinder_Aabb::aabbEnlargeFactor>`."))
	);
};
REGISTER_SERIALIZABLE(Bo1_Cylinder_Aabb);

class Bo1_ChainedCylinder_Aabb : public BoundFunctor
{
	public :
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
	FUNCTOR1D(ChainedCylinder);
	YADE_CLASS_BASE_DOC_ATTRS(Bo1_ChainedCylinder_Aabb,BoundFunctor,"Functor creating :yref:`Aabb` from :yref:`ChainedCylinder`.",
		((Real,aabbEnlargeFactor,((void)"deactivated",-1),,"Relative enlargement of the bounding box; deactivated if negative.\n\n.. note::\n\tThis attribute is used to create distant interaction, but is only meaningful with an :yref:`IGeomFunctor` which will not simply discard such interactions: :yref:`Ig2_Cylinder_Cylinder_Dem3DofGeom::distFactor` / :yref:`Ig2_Cylinder_Cylinder_ScGeom::interactionDetectionFactor` should have the same value as :yref:`aabbEnlargeFactor<Bo1_Cylinder_Aabb::aabbEnlargeFactor>`."))
	);
};
REGISTER_SERIALIZABLE(Bo1_ChainedCylinder_Aabb);


class Law2_CylScGeom_FrictPhys_CundallStrack: public LawFunctor{
	public:
		//OpenMPAccumulator<Real> plasticDissipation;
		virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		//Real elasticEnergy ();
		//Real getPlasticDissipation();
		//void initPlasticDissipation(Real initVal=0);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_CylScGeom_FrictPhys_CundallStrack,LawFunctor,"Law for linear compression, and Mohr-Coulomb plasticity surface without cohesion.\nThis law implements the classical linear elastic-plastic law from [CundallStrack1979]_ (see also [Pfc3dManual30]_). The normal force is (with the convention of positive tensile forces) $F_n=\\min(k_n u_n, 0)$. The shear force is $F_s=k_s u_s$, the plasticity condition defines the maximum value of the shear force : $F_s^{\\max}=F_n\\tan(\\phi)$, with $\\phi$ the friction angle.\n\n.. note::\n This law uses :yref:`ScGeom`; there is also functionally equivalent :yref:`Law2_Dem3DofGeom_FrictPhys_CundallStrack`, which uses :yref:`Dem3DofGeom` (sphere-box interactions are not implemented for the latest).\n\n.. note::\n This law is well tested in the context of triaxial simulation, and has been used for a number of published results (see e.g. [Scholtes2009b]_ and other papers from the same authors). It is generalised by :yref:`Law2_ScGeom6D_CohFrictPhys_CohesionMoment`, which adds cohesion and moments at contact.",
		((bool,neverErase,false,,"Keep interactions even if particles go away from each other (only in case another constitutive law is in the scene, e.g. :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`)"))
		((bool,traceEnergy,false,Attr::hidden,"Define the total energy dissipated in plastic slips at all contacts."))
		((int,plastDissipIx,-1,(Attr::hidden|Attr::noSave),"Index for plastic dissipation (with O.trackEnergy)"))
		((int,elastPotentialIx,-1,(Attr::hidden|Attr::noSave),"Index for elastic potential energy (with O.trackEnergy)"))
		,,
		//.def("elasticEnergy",&Law2_ScGeom_FrictPhys_CundallStrack::elasticEnergy,"Compute and return the total elastic energy in all \"FrictPhys\" contacts")
		//.def("plasticDissipation",&Law2_ScGeom_FrictPhys_CundallStrack::getPlasticDissipation,"Total energy dissipated in plastic slips at all FrictPhys contacts. Computed only if :yref:`Law2_ScGeom_FrictPhys_CundallStrack::traceEnergy` is true.")
		//.def("initPlasticDissipation",&Law2_ScGeom_FrictPhys_CundallStrack::initPlasticDissipation,"Initialize cummulated plastic dissipation to a value (0 by default).")
	);
	FUNCTOR2D(CylScGeom,FrictPhys);
};
REGISTER_SERIALIZABLE(Law2_CylScGeom_FrictPhys_CundallStrack);


// Keep this : Cylinders and ChainedCylinders will have different centers maybe.
// class Bo1_ChainedCylinder_Aabb : public Bo1_Cylinder_Aabb
// {
// 	public :
// 		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
// 	FUNCTOR1D(ChainedCylinder);
// 	YADE_CLASS_BASE_DOC_ATTRS(Bo1_ChainedCylinder_Aabb,Bo1_Cylinder_Aabb,"Functor creating :yref:`Aabb` from :yref:`Cylinder`.",
// 		((Real,aabbEnlargeFactor,((void)"deactivated",-1),,"Relative enlargement of the bounding box; deactivated if negative.\n\n.. note::\n\tThis attribute is used to create distant interaction, but is only meaningful with an :yref:`IGeomFunctor` which will not simply discard such interactions: :yref:`Ig2_Cylinder_Cylinder_Dem3DofGeom::distFactor` / :yref:`Ig2_Cylinder_Cylinder_ScGeom::interactionDetectionFactor` should have the same value as :yref:`aabbEnlargeFactor<Bo1_Cylinder_Aabb::aabbEnlargeFactor>`."))
// 	);
// };

#ifdef YADE_OPENGL
REGISTER_SERIALIZABLE(Gl1_Cylinder);
REGISTER_SERIALIZABLE(Gl1_ChainedCylinder);
#endif
