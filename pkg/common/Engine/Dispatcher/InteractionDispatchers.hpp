// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg-common/Callbacks.hpp>
#include<yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include<yade/pkg-common/InteractionPhysicsDispatcher.hpp>
#include<yade/pkg-common/LawDispatcher.hpp>

class InteractionDispatchers;
shared_ptr<InteractionDispatchers> InteractionDispatchers_ctor_lists(const std::vector<shared_ptr<InteractionGeometryFunctor> >& gff, const std::vector<shared_ptr<InteractionPhysicsFunctor> >& pff, const std::vector<shared_ptr<LawFunctor> >& cff /*, const std::vector<shared_ptr<IntrCallback> >& cb=std::vector<shared_ptr<IntrCallback> >()*/ );

class InteractionDispatchers: public GlobalEngine {
	bool alreadyWarnedNoCollider;
	public:
		virtual void action(Scene*);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(InteractionDispatchers,GlobalEngine,"Unified dispatcher for handling interaction loop at every step, for parallel performance reasons.",
			((shared_ptr<InteractionGeometryDispatcher>,geomDispatcher,new InteractionGeometryDispatcher,"[will be overridden]"))
			((shared_ptr<InteractionPhysicsDispatcher>,physDispatcher,new InteractionPhysicsDispatcher,"[will be overridden]"))
			((shared_ptr<LawDispatcher>,lawDispatcher,new LawDispatcher,"[will be overridden]"))
			((vector<shared_ptr<IntrCallback> >,callbacks,,":yref:`Callbacks<IntrCallback>` which will be called for every :yref:`Interaction`, if activated."))
			,
			/*ctor*/ alreadyWarnedNoCollider=false;
				#ifdef IDISP_TIMING
					timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
				#endif
			,
			/*py*/
			.def("__init__",python::make_constructor(InteractionDispatchers_ctor_lists),
			 	// (python::arg("geomFunctors"),python::arg("physFunctors"),python::arg("lawFunctors") /*,python::arg("callbacks")=std::vector<shared_ptr<IntrCallback> >() */),
				"Construct from lists :yref:`Ig2<InteractionGeometryFunctor>`, :yref:`Ip2<InteractionPhysicsFunctor>`, :yref:`Law<LawFunctor>` functors respectively; they will be passed to interal dispatchers, which you might retrieve. (NOT YET DONE: Optionally, list of :yref:`IntrCallbacks<IntrCallback>` can be provided as fourth argument.)")
			.def_readonly("geomDispatcher",&InteractionDispatchers::geomDispatcher,"InteractionGeometryDispatcher object that is used for dispatch.")
			.def_readonly("physDispatcher",&InteractionDispatchers::physDispatcher,"InteractionPhysicsDispatcher object used for dispatch.")
			.def_readonly("lawDispatcher",&InteractionDispatchers::lawDispatcher,"LawDispatcher object used for dispatch.");
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InteractionDispatchers);

