// 2010 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/pkg-common/Callbacks.hpp>
#include<yade/lib-base/openmp-accu.hpp>

class SumIntrForcesCb: public IntrCallback{
	public:
		OpenMPAccumulator<int> numIntr;
		OpenMPAccumulator<Real> force;
		static void go(IntrCallback*,Interaction*);
		virtual IntrCallback::FuncPtr stepInit();
	YADE_CLASS_BASE_DOC(SumIntrForcesCb,IntrCallback,"Callback summing magnitudes of forces over all interactions. :yref:`IPhys` of interactions must derive from :yref:`NormShearPhys` (responsability fo the user).");
};
REGISTER_SERIALIZABLE(SumIntrForcesCb);

class SumBodyForcesCb: public BodyCallback{
	Scene* scene;
	public:
		OpenMPAccumulator<int> numBodies;
		OpenMPAccumulator<Real> force;
		static void go(BodyCallback*,Body*);
		virtual BodyCallback::FuncPtr stepInit();
	YADE_CLASS_BASE_DOC(SumBodyForcesCb,BodyCallback,"Callback summing magnitudes of resultant forces over :yref:`dynamic<Body::dynamic>` bodies.");
};
REGISTER_SERIALIZABLE(SumBodyForcesCb);
