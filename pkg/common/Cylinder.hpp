#pragma once
#include<yade/core/Shape.hpp>
#include<yade/core/State.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/Dispatching.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/core/Scene.hpp>
#ifdef YADE_OPENGL
	#include<yade/pkg-common/GLDrawFunctors.hpp>
#endif


class Cylinder: public Sphere{
	public:
		Cylinder(Real _radius, Real _length): length(_length) { /*segment=Vector3r(0,0,1)*_length;*/ radius=_radius; createIndex(); }
		virtual ~Cylinder ();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Cylinder,Sphere,"Geometry of a cylinder, as Minkowski sum of line and sphere.",
// 		((Real,radius,NaN,,"Radius [m]"))
		((Real,length,NaN,,"Length [m]"))
		((Vector3r,segment,Vector3r::Zero(),,"Length vector")),
		createIndex();
		/*ctor*/
	);
	REGISTER_CLASS_INDEX(Cylinder,Sphere);
};

class ChainedCylinder: public Cylinder{
	public:
		ChainedCylinder(Real _radius, Real _length): Cylinder(_radius,_length){}
		virtual ~ChainedCylinder ();
		 
		//Keep pointers or copies of connected states?
// 		ChainedState st1, st2;
		

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(ChainedCylinder,Cylinder,"Geometry of a deformable chained cylinder, using geometry :yref:`MinkCylinder`.",
  		((Real,initLength,0,,"tensile-free length, used as reference for tensile strain"))
  		((Quaternionr,chainedOrientation,Quaternionr::Identity(),,"Deviation of node1 orientation from node-to-node vector"))
		,createIndex();/*ctor*/
// 		state=shared_ptr<ChainedState>(new ChainedState);

	);
	REGISTER_CLASS_INDEX(ChainedCylinder,Cylinder);
};

class CylScGeom: public ScGeom{
	public:
		virtual ~CylScGeom ();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CylScGeom,ScGeom,"Geometry of a cylinder-sphere contact.",
		((bool,onNode,false,,"contact on node?"))
		((Vector3r,start,Vector3r::Zero(),,"position of 1st node |yupdate|"))
		((Vector3r,end,Vector3r::Zero(),,"position of 2nd node |yupdate|"))
		((Body::id_t,id3,0,,"id of next chained cylinder |yupdate|"))
		((Real,relPos,0,,"position of the contact on the cylinder (0: node-, 1:node+) |yupdate|")),
		createIndex(); /*ctor*/
	);
	REGISTER_CLASS_INDEX(CylScGeom,ScGeom);
};


class ChainedState: public State{
	public:
		static vector<vector<Body::id_t> > chains;
		static unsigned int currentChain;
		vector<Body::id_t> barContacts;
		vector<Body::id_t> nodeContacts;

		virtual ~ChainedState ();
		void addToChain(Body::id_t bodyId) {
			if (chains.size()<=currentChain) chains.resize(currentChain+1);
			chainNumber=currentChain;
 			rank=chains[currentChain].size();
 			chains[currentChain].push_back(bodyId);}

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(ChainedState,State,"State of a chained bodies, containing information on connectivity in order to track contacts jumping over contiguous elements. Chains are 1D lists from which id of chained bodies are retrieved via :yref:rank<ChainedState::rank>` and :yref:chainNumber<ChainedState::chainNumber>`.",
 		((unsigned int,rank,0,,"rank in the chain"))
 		((unsigned int,chainNumber,0,,"chain id"))
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
};



class Ig2_Sphere_ChainedCylinder_CylScGeom: public IGeomFunctor{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	#ifdef YADE_DEVIRT_FUNCTORS
		void* getStaticFuncPtr(){ return (void*)&Ig2_Sphere_ChainedCylinder_CylScGeom::goStatic; }
		static bool goStatic(IGeomFunctor* self, const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& se32, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	#endif
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_ChainedCylinder_CylScGeom,IGeomFunctor,"Create/update a :yref:`ScGeom` instance representing intersection of two :yref:`Spheres<Sphere>`.",
		((Real,interactionDetectionFactor,1,,"Enlarge both radii by this factor (if >1), to permit creation of distant interactions."))
	);
	FUNCTOR2D(Sphere,ChainedCylinder);
	DEFINE_FUNCTOR_ORDER_2D(Sphere,ChainedCylinder);
};

class Ig2_ChainedCylinder_ChainedCylinder_ScGeom: public IGeomFunctor{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	#ifdef YADE_DEVIRT_FUNCTORS
		void* getStaticFuncPtr(){ return (void*)&Ig2_Sphere_ChainedCylinder_CylScGeom::goStatic; }
		static bool goStatic(IGeomFunctor* self, const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& se32, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	#endif
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_ChainedCylinder_ChainedCylinder_ScGeom,IGeomFunctor,"Create/update a :yref:`ScGeom` instance representing connexion between :yref:`chained cylinders<ChainedCylinder>`.",
		((Real,interactionDetectionFactor,1,,"Enlarge both radii by this factor (if >1), to permit creation of distant interactions."))
	);
	FUNCTOR2D(ChainedCylinder,ChainedCylinder);
	// needed for the dispatcher, even if it is symmetric
	DEFINE_FUNCTOR_ORDER_2D(ChainedCylinder,ChainedCylinder);
};



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

class Bo1_ChainedCylinder_Aabb : public BoundFunctor
{
	public :
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
	FUNCTOR1D(ChainedCylinder);
	YADE_CLASS_BASE_DOC_ATTRS(Bo1_ChainedCylinder_Aabb,BoundFunctor,"Functor creating :yref:`Aabb` from :yref:`ChainedCylinder`.",
		((Real,aabbEnlargeFactor,((void)"deactivated",-1),,"Relative enlargement of the bounding box; deactivated if negative.\n\n.. note::\n\tThis attribute is used to create distant interaction, but is only meaningful with an :yref:`IGeomFunctor` which will not simply discard such interactions: :yref:`Ig2_Cylinder_Cylinder_Dem3DofGeom::distFactor` / :yref:`Ig2_Cylinder_Cylinder_ScGeom::interactionDetectionFactor` should have the same value as :yref:`aabbEnlargeFactor<Bo1_Cylinder_Aabb::aabbEnlargeFactor>`."))
	);
};

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

REGISTER_SERIALIZABLE(Bo1_Cylinder_Aabb);
REGISTER_SERIALIZABLE(Bo1_ChainedCylinder_Aabb);
REGISTER_SERIALIZABLE(Cylinder);
REGISTER_SERIALIZABLE(ChainedCylinder);
REGISTER_SERIALIZABLE(ChainedState);
REGISTER_SERIALIZABLE(CylScGeom);
REGISTER_SERIALIZABLE(Ig2_Sphere_ChainedCylinder_CylScGeom);
REGISTER_SERIALIZABLE(Ig2_ChainedCylinder_ChainedCylinder_ScGeom);
#ifdef YADE_OPENGL
REGISTER_SERIALIZABLE(Gl1_Cylinder);
REGISTER_SERIALIZABLE(Gl1_ChainedCylinder);
#endif
