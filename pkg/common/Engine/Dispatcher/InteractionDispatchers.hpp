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
		InteractionDispatchers();
		virtual void action(Scene*);
		shared_ptr<InteractionGeometryDispatcher> geomDispatcher;
		shared_ptr<InteractionPhysicsDispatcher> physDispatcher;
		shared_ptr<LawDispatcher> lawDispatcher;
		YADE_CLASS_BASE_DOC_ATTRS_PY(InteractionDispatchers,GlobalEngine,"Unified dispatcher for handling interaction loop at every step, for parallel performance reasons.",
			((geomDispatcher,"[will be overridden]"))
			((physDispatcher,"[will be overridden]"))
			((lawDispatcher,"[will be overridden]")),
			.def("__init__",python::make_constructor(InteractionDispatchers_ctor_lists),"Construct from 3 lists of functors, which will be given to geomDispatcher, physDispatcher, lawDispatcher in that order.")
			.def_readonly("geomDispatcher",&InteractionDispatchers::geomDispatcher,"InteractionGeometryDispatcher object that is used for dispatch.")
			.def_readonly("physDispatcher",&InteractionDispatchers::physDispatcher,"InteractionPhysicsDispatcher object used for dispatch.")
			.def_readonly("lawDispatcher",&InteractionDispatchers::lawDispatcher,"LawDispatcher object used for dispatch.");
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InteractionDispatchers);

