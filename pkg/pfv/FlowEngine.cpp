/*************************************************************************
*  Copyright (C) 2009 by Emanuele Catalano <catalano@grenoble-inp.fr>    *
*  Copyright (C) 2009 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef YADE_CGAL
#ifdef FLOW_ENGINE

#define TEMPLATE_FLOW_NAME FlowEngineT
#include "FlowEngine.hpp"

// To register properly, we need to first instantiate an intermediate class, then inherit from it with correct class names in YADE_CLASS macro
// The intermediate one would be seen with the name "TemplateFlowEngine" by python, thus it would not work when more than one class are derived, they would all
// be named "TemplateFlowEngine" ...
typedef TemplateFlowEngine<FlowCellInfo,FlowVertexInfo> FlowEngineT;
REGISTER_SERIALIZABLE(FlowEngineT);

class FlowEngine : public FlowEngineT
{
	public :
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(FlowEngine,FlowEngineT,"An engine to solve flow problem in saturated granular media. Model description can be found in [Chareyre2012a]_ and [Catalano2014a]_. See the example script FluidCouplingPFV/oedometer.py. More documentation to come.\n\n.. note::Multi-threading seems to work fine for Cholesky decomposition, but it fails for the solve phase in which -j1 is the fastest, here we specify thread numbers independently using :yref:`FlowEngine::numFactorizeThreads` and :yref:`FlowEngine::numSolveThreads`. These multhreading settings are only impacting the behaviour of openblas library and are relatively independant of :yref:`FlowEngine::multithread`. However, the settings have to be globally consistent. For instance, :yref:`multithread<FlowEngine::multithread>` =True with  yref:`numFactorizeThreads<FlowEngine::numFactorizeThreads>` = yref:`numSolveThreads<FlowEngine::numSolveThreads>` = 4 implies that openblas will mobilize 8 processors at some point. If the system does not have so many procs. it will hurt performance.",
		,,
		,
		//nothing special to define here, we simply re-use FlowEngine methods
		//.def("meanVelocity",&PeriodicFlowEngine::meanVelocity,"measure the mean velocity in the period")
		)
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(FlowEngine);

YADE_PLUGIN((FlowEngineT));
CREATE_LOGGER(FlowEngine );
YADE_PLUGIN((FlowEngine));

#endif //FLOW_ENGINE

#endif /* YADE_CGAL */

