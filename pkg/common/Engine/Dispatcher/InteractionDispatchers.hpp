// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg-common/Callbacks.hpp>
#include<yade/pkg-common/Dispatching.hpp>

class InteractionDispatchers: public GlobalEngine {
	bool alreadyWarnedNoCollider;
	typedef std::pair<body_id_t, body_id_t> idPair;
	// store interactions that should be deleted after loop in action, not later
	#ifdef YADE_OPENMP
		vector<list<idPair> > eraseAfterLoopIds;
		void eraseAfterLoop(body_id_t id1,body_id_t id2){ eraseAfterLoopIds[omp_get_thread_num()].push_back(idPair(id1,id2)); }
	#else
		list<idPair> eraseAfterLoopIds;
		void eraseAfterLoop(body_id_t id1,body_id_t id2){ eraseAfterLoopIds.push_back(idPair(id1,id2)); }
	#endif
	public:
		virtual void pyHandleCustomCtorArgs(python::tuple& t, python::dict& d);
		virtual void action();
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(InteractionDispatchers,GlobalEngine,"Unified dispatcher for handling interaction loop at every step, for parallel performance reasons.\n\n.. admonition:: Special constructor\n\n\tConstructs from 3 lists of :yref:`Ig2<InteractionGeometryFunctor>`, :yref:`Ip2<InteractionPhysicsFunctor>`, :yref:`Law<LawFunctor>` functors respectively; they will be passed to interal dispatchers, which you might retrieve. (NOT YET DONE: Optionally, list of :yref:`IntrCallbacks<IntrCallback>` can be provided as fourth argument.)",
			((shared_ptr<InteractionGeometryDispatcher>,geomDispatcher,new InteractionGeometryDispatcher,"[will be overridden]"))
			((shared_ptr<InteractionPhysicsDispatcher>,physDispatcher,new InteractionPhysicsDispatcher,"[will be overridden]"))
			((shared_ptr<LawDispatcher>,lawDispatcher,new LawDispatcher,"[will be overridden]"))
			((vector<shared_ptr<IntrCallback> >,callbacks,,":yref:`Callbacks<IntrCallback>` which will be called for every :yref:`Interaction`, if activated."))
			,
			/*ctor*/ alreadyWarnedNoCollider=false;
				#ifdef IDISP_TIMING
					timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
				#endif
				#ifdef YADE_OPENMP
					eraseAfterLoopIds.resize(omp_get_max_threads());
				#endif
			,
			/*py*/
			.def_readonly("geomDispatcher",&InteractionDispatchers::geomDispatcher,"InteractionGeometryDispatcher object that is used for dispatch.")
			.def_readonly("physDispatcher",&InteractionDispatchers::physDispatcher,"InteractionPhysicsDispatcher object used for dispatch.")
			.def_readonly("lawDispatcher",&InteractionDispatchers::lawDispatcher,"LawDispatcher object used for dispatch.");
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InteractionDispatchers);

