// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2006 Janek Kozicki <cosurgi@berlios.de>

#pragma once

#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/core/Bound.hpp>
#include<yade/core/State.hpp>
#include<yade/core/Shape.hpp>
#include<yade/core/Functor.hpp>
#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/InteractionPhysics.hpp>

#define RENDERS(name) public: virtual string renders() const { return #name;};

struct GLViewInfo{
	GLViewInfo(): sceneCenter(Vector3r::ZERO), sceneRadius(1.){}
	Vector3r sceneCenter;
	Real sceneRadius;
};

class GlBoundFunctor: public Functor1D<void, TYPELIST_2(const shared_ptr<Bound>&, Scene*)> {
	public:
		virtual ~GlBoundFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	YADE_CLASS_BASE_DOC_ATTRS(GlBoundFunctor,Functor,"Abstract functor for rendering :yref:`Body::bound` objects.",/*attrs*/);
};
REGISTER_SERIALIZABLE(GlBoundFunctor);

class GlShapeFunctor: public Functor1D<void, TYPELIST_4(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&)>{
	public:
		virtual ~GlShapeFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	YADE_CLASS_BASE_DOC_ATTRS(GlShapeFunctor,Functor,"Abstract functor for rendering :yref:`Body::shape` objects.",/*attrs*/);
};
REGISTER_SERIALIZABLE(GlShapeFunctor);

class GlInteractionGeometryFunctor: public Functor1D<void,TYPELIST_5(const shared_ptr<InteractionGeometry>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)>{
	public: 
		virtual ~GlInteractionGeometryFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	YADE_CLASS_BASE_DOC_ATTRS(GlInteractionGeometryFunctor,Functor,"Abstract functor for rendering :yref:`Interaction::interactionGeometry` objects.",/*attrs*/);
};
REGISTER_SERIALIZABLE(GlInteractionGeometryFunctor);

class GlInteractionPhysicsFunctor: public Functor1D<void, TYPELIST_5(const shared_ptr<InteractionPhysics>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)>{
	public: 
		virtual ~GlInteractionPhysicsFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	YADE_CLASS_BASE_DOC_ATTRS(GlInteractionPhysicsFunctor,Functor,"Abstract functor for rendering :yref:`Interaction::interactionPhysics` objects.",/*attrs*/);
};
REGISTER_SERIALIZABLE(GlInteractionPhysicsFunctor);

class GlStateFunctor: public Functor1D<void,TYPELIST_1(const shared_ptr<State>&/*, draw parameters: color, scale, given from OpenGLRenderer, or sth.... */)>{
	public : 
		virtual ~GlStateFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
		virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};
	YADE_CLASS_BASE_DOC_ATTRS(GlStateFunctor,Functor,"Abstract functor for rendering :yref:`Body::state` objects.",/*attrs*/);
};
REGISTER_SERIALIZABLE(GlStateFunctor);

