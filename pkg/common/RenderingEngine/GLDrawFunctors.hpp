// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2006 Janek Kozicki <cosurgi@berlios.de>

#pragma once

#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/core/BoundingVolume.hpp>
#include<yade/core/State.hpp>
#ifdef YADE_SHAPE
	#include<yade/core/GeometricalModel.hpp>
#endif
#include<yade/core/InteractingGeometry.hpp>
#include<yade/core/Functor.hpp>
#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/InteractionPhysics.hpp>
#include<yade/core/GeometricalModel.hpp>

#define RENDERS(name) public: virtual string renders() const { return #name;};

struct GLViewInfo{
	GLViewInfo(): sceneCenter(Vector3r::ZERO), sceneRadius(1.){}
	Vector3r sceneCenter;
	Real sceneRadius;
};

class GLDrawBoundingVolumeFunctor: public Functor1D<void, TYPELIST_1(const shared_ptr<BoundingVolume>&)> {
	public:
		virtual ~GLDrawBoundingVolumeFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GLDrawBoundingVolumeFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GLDrawBoundingVolumeFunctor);
#ifdef YADE_SHAPE
	class GLDrawGeometricalModelFunctor: public Functor1D<void,TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<State>&,bool)>{
		public: 
			virtual ~GLDrawGeometricalModelFunctor() {};
			virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
			virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
		REGISTER_CLASS_NAME(GLDrawGeometricalModelFunctor);
		REGISTER_BASE_CLASS_NAME(Functor1D);
	};
	REGISTER_SERIALIZABLE(GLDrawGeometricalModelFunctor);
#endif

class GLDrawInteractingGeometryFunctor: public Functor1D<void, TYPELIST_4(const shared_ptr<InteractingGeometry>&, const shared_ptr<State>&,bool,const GLViewInfo&)>{
	public:
		virtual ~GLDrawInteractingGeometryFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GLDrawInteractingGeometryFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GLDrawInteractingGeometryFunctor);

class GLDrawInteractionGeometryFunctor: public Functor1D<void,TYPELIST_5(const shared_ptr<InteractionGeometry>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)>{
	public: 
		virtual ~GLDrawInteractionGeometryFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GLDrawInteractionGeometryFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GLDrawInteractionGeometryFunctor);

class GLDrawInteractionPhysicsFunctor: public Functor1D<void, TYPELIST_5(const shared_ptr<InteractionPhysics>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)>{
	public: 
		virtual ~GLDrawInteractionPhysicsFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GLDrawInteractionPhysicsFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GLDrawInteractionPhysicsFunctor);

#ifdef YADE_SHAPE
	class GLDrawShadowVolumeFunctor: public Functor1D<void, TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<State>&, const Vector3r&)>{
		public:
			virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		REGISTER_CLASS_NAME(GLDrawShadowVolumeFunctor);
		REGISTER_BASE_CLASS_NAME(Functor1D);
	};
	REGISTER_SERIALIZABLE(GLDrawShadowVolumeFunctor);
#endif

class GLDrawStateFunctor: public Functor1D<void,TYPELIST_1(const shared_ptr<State>&/*, draw parameters: color, scale, given from OpenGLRenderer, or sth.... */)>{
	public : 
		virtual ~GLDrawStateFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GLDrawStateFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GLDrawStateFunctor);

