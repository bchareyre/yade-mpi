#pragma once
#include<yade/core/Shape.hpp>
#include<yade/lib-base/Math.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/Scene.hpp>
#include<yade/core/State.hpp>
#include<yade/core/Shape.hpp>
#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/InteractionPhysics.hpp>
#include<yade/core/Functor.hpp>
#include<yade/core/Dispatcher.hpp>
#include<yade/pkg-common/Aabb.hpp>

class VelocityBins;

/********
	functors
*********************/

class BoundFunctor: public Functor1D<
	/*dispatch types*/ Shape,
	/*return type*/    void ,
	/*argument types*/ TYPELIST_4(const shared_ptr<Shape>&, shared_ptr<Bound>&, const Se3r&, const Body*)
>{
	public: virtual ~BoundFunctor();
	YADE_CLASS_BASE_DOC(BoundFunctor,Functor,"Functor for creating/updating :yref:`Body::bound`.");
};
REGISTER_SERIALIZABLE(BoundFunctor);


class InteractionGeometryFunctor: public Functor2D<
	/*dispatch types*/ Shape,Shape,
	/*return type*/    bool,
	/*argument types*/ TYPELIST_7(const shared_ptr<Shape>&, const shared_ptr<Shape>&, const State&, const State&, const Vector3r&, const bool&, const shared_ptr<Interaction>&) 
>{
	public: virtual ~InteractionGeometryFunctor();
	#ifdef YADE_DEVIRT_FUNCTORS
		// type of the pointer to devirtualized functor (static method taking the functor instance as the first argument)
		typedef bool(*StaticFuncPtr)(InteractionGeometryFunctor*, const shared_ptr<Shape>&, const shared_ptr<Shape>&, const State&, const State&, const Vector3r&, const bool&, const shared_ptr<Interaction>&);
		// return devirtualized functor (static method); must be overridden in derived classes
		virtual void* getStaticFuncPtr(){ throw runtime_error(("InteractionGeometryFunctor::getStaticFuncPtr() not overridden in class "+getClassName()+".").c_str()); }
	#endif
	YADE_CLASS_BASE_DOC(InteractionGeometryFunctor,Functor,"Functor for creating/updating :yref:`Interaction::interactionGeometry` objects.");
};
REGISTER_SERIALIZABLE(InteractionGeometryFunctor);


class InteractionPhysicsFunctor: public Functor2D<
	/*dispatch types*/ Material, Material,
	/*retrun type*/    void,
	/*argument types*/ TYPELIST_3(const shared_ptr<Material>&, const shared_ptr<Material>&, const shared_ptr<Interaction>&)
>{
	public: virtual ~InteractionPhysicsFunctor();
	YADE_CLASS_BASE_DOC(InteractionPhysicsFunctor,Functor,"Functor for creating/updating :yref:`Interaction::interactionPhysics` objects.");
};
REGISTER_SERIALIZABLE(InteractionPhysicsFunctor);


class LawFunctor: public Functor2D<
	/*dispatch types*/ InteractionGeometry,InteractionPhysics,
	/*return type*/    void,
	/*argument types*/ TYPELIST_3(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*)
>{
	public: virtual ~LawFunctor();
	/*! Convenience functions to get forces/torques quickly. */
	void addForce (const Body::id_t id, const Vector3r& f,Scene* rb){rb->forces.addForce (id,f);}
	void addTorque(const Body::id_t id, const Vector3r& t,Scene* rb){rb->forces.addTorque(id,t);}
	/*! Convenience function to apply force and torque from one force at contact point. Not sure if this is the right place for it. */
	void applyForceAtContactPoint(const Vector3r& force, const Vector3r& contactPoint, const Body::id_t id1, const Vector3r& pos1, const Body::id_t id2, const Vector3r& pos2){
		addForce(id1, force,scene); addTorque(id1, (contactPoint-pos1).cross(force),scene);
		addForce(id2,-force,scene); addTorque(id2,-(contactPoint-pos2).cross(force),scene);
	}
	YADE_CLASS_BASE_DOC(LawFunctor,Functor,"Functor for applying constitutive laws on :yref:`interactions<Interaction>`.");
};
REGISTER_SERIALIZABLE(LawFunctor);


/********
	dispatchers
*********************/

class BoundDispatcher: public Dispatcher1D<	
	/* functor type*/ BoundFunctor
>{
	public:
		virtual void action();
		virtual bool isActivated(){ return activated; }
		shared_ptr<VelocityBins> velocityBins;
	DECLARE_LOGGER;
	YADE_DISPATCHER1D_FUNCTOR_DOC_ATTRS_CTOR_PY(BoundDispatcher,BoundFunctor,/*optional doc*/,
		/*additional attrs*/
		((bool,activated,true,"Whether the engine is activated (only should be changed by the collider)"))
		((Real,sweepDist,0,"Distance by which enlarge all bounding boxes, to prevent collider from being run at every step (only should be changed by the collider)."))
		,/*ctor*/,/*py*/
	);
};
REGISTER_SERIALIZABLE(BoundDispatcher);


class InteractionGeometryDispatcher:	public Dispatcher2D<	
	/* functor type*/ InteractionGeometryFunctor,
	/* autosymmetry*/ false
>{
	bool alreadyWarnedNoCollider;
	public:
		virtual void action();
		shared_ptr<Interaction> explicitAction(const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool force);
	YADE_DISPATCHER2D_FUNCTOR_DOC_ATTRS_CTOR_PY(InteractionGeometryDispatcher,InteractionGeometryFunctor,/* doc is optional*/,/*attrs*/,/*ctor*/alreadyWarnedNoCollider=false;,/*py*/);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InteractionGeometryDispatcher);


class InteractionPhysicsDispatcher: public Dispatcher2D<
	/*functor type*/ InteractionPhysicsFunctor
>{		
	public:
		virtual void action();
		void explicitAction(shared_ptr<Material>& pp1, shared_ptr<Material>& pp2, shared_ptr<Interaction>& i);
	YADE_DISPATCHER2D_FUNCTOR_DOC_ATTRS_CTOR_PY(InteractionPhysicsDispatcher,InteractionPhysicsFunctor,/*doc is optional*/,/*attrs*/,/*ctor*/,/*py*/);
};
REGISTER_SERIALIZABLE(InteractionPhysicsDispatcher);


class LawDispatcher: public Dispatcher2D<
	/*functor type*/ LawFunctor,
	/*autosymmetry*/ false
>{
	public: virtual void action();
	YADE_DISPATCHER2D_FUNCTOR_DOC_ATTRS_CTOR_PY(LawDispatcher,LawFunctor,/*doc is optional*/,/*attrs*/,/*ctor*/,/*py*/);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(LawDispatcher);


