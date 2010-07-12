// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2006 Janek Kozicki <cosurgi@berlios.de>

#pragma once

#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/core/Bound.hpp>
#include<yade/core/State.hpp>
#include<yade/core/Shape.hpp>
#include<yade/core/Functor.hpp>
#include<yade/core/Dispatcher.hpp>
#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/InteractionPhysics.hpp>

#define RENDERS(name) public: virtual string renders() const { return #name;}; FUNCTOR1D(name);

struct GLViewInfo{
	GLViewInfo(): sceneCenter(Vector3r::Zero()), sceneRadius(1.){}
	Vector3r sceneCenter;
	Real sceneRadius;
};

#define GL_FUNCTOR(klass,typelist,renderedType) class klass: public Functor1D<void,typelist>{public:\
	virtual ~klass(){};\
	virtual string renders() const { throw std::runtime_error(#klass ": unregistered gldraw class.\n"); };\
	virtual void initgl(){/*WARNING: it must deal with static members, because it is called from another instance!*/};\
	YADE_CLASS_BASE_DOC(klass,Functor,"Abstract functor for rendering :yref:`" #renderedType "` objects.");};\
	REGISTER_SERIALIZABLE(klass);
#define GL_DISPATCHER(klass,typelist,functor,disptype) class klass: public Dispatcher1D<disptype,functor,void,typelist>{public:\
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(klass,Dispatcher,"Dispatcher calling :yref:`" #functor"s<" #functor">` based on :yref:`" #disptype "` type.",/*attrs*/,/*ctor*/,/*py*/YADE_PY_DISPATCHER(klass)); };\
	REGISTER_SERIALIZABLE(klass);

GL_FUNCTOR(GlBoundFunctor,TYPELIST_2(const shared_ptr<Bound>&, Scene*),Bound);
GL_FUNCTOR(GlShapeFunctor,TYPELIST_4(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&),Shape);
GL_FUNCTOR(GlInteractionGeometryFunctor,TYPELIST_5(const shared_ptr<InteractionGeometry>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool),InteractionGeometry);
GL_FUNCTOR(GlInteractionPhysicsFunctor,TYPELIST_5(const shared_ptr<InteractionPhysics>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool),InteractionGeometry);
GL_FUNCTOR(GlStateFunctor,TYPELIST_1(const shared_ptr<State>&),State);

GL_DISPATCHER(GlBoundDispatcher,TYPELIST_2(const shared_ptr<Bound>&, Scene*),GlBoundFunctor,Bound);
GL_DISPATCHER(GlShapeDispatcher,TYPELIST_4(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&),GlShapeFunctor,Shape);
GL_DISPATCHER(GlInteractionGeometryDispatcher,TYPELIST_5(const shared_ptr<InteractionGeometry>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool),GlInteractionGeometryFunctor,InteractionGeometry);
GL_DISPATCHER(GlInteractionPhysicsDispatcher,TYPELIST_5(const shared_ptr<InteractionPhysics>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool),GlInteractionPhysicsFunctor,InteractionPhysics);
GL_DISPATCHER(GlStateDispatcher,TYPELIST_1(const shared_ptr<State>&),GlStateFunctor,State);
#undef GL_FUNCTOR
#undef GL_DISPATCHER

