// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include<yade/pkg-common/InteractionPhysicsDispatcher.hpp>
#include<yade/pkg-common/LawDispatcher.hpp>

class InteractionDispatchers;
shared_ptr<InteractionDispatchers> InteractionDispatchers_ctor_lists(const std::vector<shared_ptr<InteractionGeometryFunctor> >& gff, const std::vector<shared_ptr<InteractionPhysicsFunctor> >& pff, const std::vector<shared_ptr<LawFunctor> >& cff);

class InteractionDispatchers: public GlobalEngine {
	bool alreadyWarnedNoCollider;
	public:
		virtual void action(Scene*);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(InteractionDispatchers,GlobalEngine,"Unified dispatcher for handling interaction loop at every step, for parallel performance reasons.",
			((shared_ptr<InteractionGeometryDispatcher>,geomDispatcher,new InteractionGeometryDispatcher,"[will be overridden]"))
			((shared_ptr<InteractionPhysicsDispatcher>,physDispatcher,new InteractionPhysicsDispatcher,"[will be overridden]"))
			((shared_ptr<LawDispatcher>,lawDispatcher,new LawDispatcher,"[will be overridden]")),
			/*ctor*/ alreadyWarnedNoCollider=false;
				#ifdef IDISP_TIMING
					timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
				#endif
			,
			/*py*/
			.def("__init__",python::make_constructor(InteractionDispatchers_ctor_lists),"Construct from 3 lists of functors, which will be given to geomDispatcher, physDispatcher, lawDispatcher in that order.")
			.def_readonly("geomDispatcher",&InteractionDispatchers::geomDispatcher,"InteractionGeometryDispatcher object that is used for dispatch.")
			.def_readonly("physDispatcher",&InteractionDispatchers::physDispatcher,"InteractionPhysicsDispatcher object used for dispatch.")
			.def_readonly("lawDispatcher",&InteractionDispatchers::lawDispatcher,"LawDispatcher object used for dispatch.");
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InteractionDispatchers);

