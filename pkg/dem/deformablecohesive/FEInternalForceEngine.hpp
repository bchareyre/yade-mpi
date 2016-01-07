/*************************************************************************
*  Copyright (C) 2013 by Burak ER  burak.er@btu.edu.tr                	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include <core/GlobalEngine.hpp>
#include <pkg/common/Callbacks.hpp>
#include <pkg/dem/deformablecohesive/FEInternalForceDispatchers.hpp>

#ifdef USE_TIMING_DELTAS
	#define TIMING_DELTAS_CHECKPOINT(cpt) timingDeltas->checkpoint(cpt)
	#define TIMING_DELTAS_START() timingDeltas->start()
#else
	#define TIMING_DELTAS_CHECKPOINT(cpt)
	#define TIMING_DELTAS_START()
#endif

class FEInternalForceEngine: public GlobalEngine {

//	#ifdef YADE_OPENMP
//		std::vector<std::list<idPair> > eraseAfterLoopIds;
//		void eraseAfterLoop(Body::id_t id1,Body::id_t id2){ eraseAfterLoopIds[omp_get_thread_num()].push_back(idPair(id1,id2)); }
//	#else
//		list<idPair> eraseAfterLoopIds;
//		void eraseAfterLoop(Body::id_t id1,Body::id_t id2){ eraseAfterLoopIds.push_back(idPair(id1,id2)); }
//	#endif
	public:
		virtual void pyHandleCustomCtorArgs(boost::python::tuple& t, boost::python::dict& d);
		virtual void action();
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(FEInternalForceEngine,GlobalEngine,"Unified dispatcher for handling Finite Element internal force loop at every step, for parallel performance reasons.\n\n.. admonition:: Special constructor\n\n\tConstructs from 3 lists of :yref:`Ig2<IGeomFunctor>`, :yref:`Ip2<IPhysFunctor>`, :yref:`Law<LawFunctor>` functors respectively; they will be passed to interal dispatchers, which you might retrieve.",
			((shared_ptr<InternalForceDispatcher>,internalforcedispatcher,new InternalForceDispatcher,Attr::readonly,":yref:`InternalForceDispatcher` object that is used for dispatching of element types."))
			,
			/*ctor*/
			,
			/*py*/
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(FEInternalForceEngine);
