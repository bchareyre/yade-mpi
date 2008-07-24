// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2006 Janek Kozicki <cosurgi@berlios.de>

#pragma once

#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/core/BoundingVolume.hpp>
#include<yade/core/PhysicalParameters.hpp>
#include<yade/core/GeometricalModel.hpp>
#include<yade/core/InteractingGeometry.hpp>
#include<yade/core/EngineUnit1D.hpp>
#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/InteractionPhysics.hpp>
#include<yade/core/GeometricalModel.hpp>

#define RENDERS(name) public: virtual string renders() const { return #name;};

class GLDrawBoundingVolumeFunctor: public EngineUnit1D<void, TYPELIST_1(const shared_ptr<BoundingVolume>&)> {
	public:
		virtual ~GLDrawBoundingVolumeFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
	REGISTER_CLASS_NAME(GLDrawBoundingVolumeFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};
REGISTER_SERIALIZABLE(GLDrawBoundingVolumeFunctor,false);

class GLDrawGeometricalModelFunctor: public EngineUnit1D<void,TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool)>{
	public: 
		virtual ~GLDrawGeometricalModelFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
	REGISTER_CLASS_NAME(GLDrawGeometricalModelFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};
REGISTER_SERIALIZABLE(GLDrawGeometricalModelFunctor,false);


class GLDrawInteractingGeometryFunctor: public EngineUnit1D<void, TYPELIST_3(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&,bool)>{
	public:
		virtual ~GLDrawInteractingGeometryFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
	REGISTER_CLASS_NAME(GLDrawInteractingGeometryFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};
REGISTER_SERIALIZABLE(GLDrawInteractingGeometryFunctor,false);

class GLDrawInteractionGeometryFunctor: public EngineUnit1D<void,TYPELIST_5(const shared_ptr<InteractionGeometry>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)>{
	public: 
		virtual ~GLDrawInteractionGeometryFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
	REGISTER_CLASS_NAME(GLDrawInteractionGeometryFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};
REGISTER_SERIALIZABLE(GLDrawInteractionGeometryFunctor,false);

class GLDrawInteractionPhysicsFunctor: public EngineUnit1D<void, TYPELIST_5(const shared_ptr<InteractionPhysics>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)>{
	public: 
		virtual ~GLDrawInteractionPhysicsFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
	REGISTER_CLASS_NAME(GLDrawInteractionPhysicsFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};
REGISTER_SERIALIZABLE(GLDrawInteractionPhysicsFunctor,false);

class GLDrawShadowVolumeFunctor: public EngineUnit1D<void, TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&, const Vector3r&)>{
	public:
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
	REGISTER_CLASS_NAME(GLDrawShadowVolumeFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};
REGISTER_SERIALIZABLE(GLDrawShadowVolumeFunctor,false);

class GLDrawStateFunctor: public EngineUnit1D<void,TYPELIST_1(const shared_ptr<PhysicalParameters>&/*, draw parameters: color, scale, given from OpenGLRenderer, or sth.... */)>{
	public : 
		virtual ~GLDrawStateFunctor() {};
		virtual string renders() const { std::cerr<<"Unregistered gldraw class.\n"; throw; };
	REGISTER_CLASS_NAME(GLDrawStateFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};
REGISTER_SERIALIZABLE(GLDrawStateFunctor,false);

