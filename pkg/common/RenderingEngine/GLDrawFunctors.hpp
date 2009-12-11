// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2006 Janek Kozicki <cosurgi@berlios.de>

#pragma once

#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/core/Bound.hpp>
#include<yade/core/State.hpp>
#ifdef YADE_GEOMETRICALMODEL
	#include<yade/core/GeometricalModel.hpp>
#endif
#include<yade/core/Shape.hpp>
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

class GlBoundFunctor: public Functor1D<void, TYPELIST_1(const shared_ptr<Bound>&)> {
	public:
		virtual ~GlBoundFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GlBoundFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GlBoundFunctor);
#ifdef YADE_GEOMETRICALMODEL
	class GlGeometricalModelFunctor: public Functor1D<void,TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<State>&,bool)>{
		public: 
			virtual ~GlGeometricalModelFunctor() {};
			virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
			virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
		REGISTER_CLASS_NAME(GlGeometricalModelFunctor);
		REGISTER_BASE_CLASS_NAME(Functor1D);
	};
	REGISTER_SERIALIZABLE(GlGeometricalModelFunctor);
#endif

class GlShapeFunctor: public Functor1D<void, TYPELIST_4(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&)>{
	public:
		virtual ~GlShapeFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GlShapeFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GlShapeFunctor);

class GlInteractionGeometryFunctor: public Functor1D<void,TYPELIST_5(const shared_ptr<InteractionGeometry>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)>{
	public: 
		virtual ~GlInteractionGeometryFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GlInteractionGeometryFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GlInteractionGeometryFunctor);

class GlInteractionPhysicsFunctor: public Functor1D<void, TYPELIST_5(const shared_ptr<InteractionPhysics>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)>{
	public: 
		virtual ~GlInteractionPhysicsFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GlInteractionPhysicsFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GlInteractionPhysicsFunctor);

#ifdef YADE_GEOMETRICALMODEL
	class GLDrawShadowVolumeFunctor: public Functor1D<void, TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<State>&, const Vector3r&)>{
		public:
			virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		REGISTER_CLASS_NAME(GLDrawShadowVolumeFunctor);
		REGISTER_BASE_CLASS_NAME(Functor1D);
	};
	REGISTER_SERIALIZABLE(GLDrawShadowVolumeFunctor);
#endif

class GlStateFunctor: public Functor1D<void,TYPELIST_1(const shared_ptr<State>&/*, draw parameters: color, scale, given from OpenGLRenderer, or sth.... */)>{
	public : 
		virtual ~GlStateFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	REGISTER_CLASS_NAME(GlStateFunctor);
	REGISTER_BASE_CLASS_NAME(Functor1D);
};
REGISTER_SERIALIZABLE(GlStateFunctor);

