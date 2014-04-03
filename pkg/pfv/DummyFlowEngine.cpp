 
/*************************************************************************
*  Copyright (C) 2014 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// This is an example of how to derive a new FlowEngine with additional data and possibly completely new behaviour.
// Every functions of the base engine can be overloaded, and new functions can be added

//keep this #ifdef as long as you don't really want to realize a final version publicly, it will save compilation time for everyone else
//when you want it compiled, you can pass -DDFNFLOW to cmake, or just uncomment the following line
// #define DUMMYFLOW
#ifdef DUMMYFLOW
#define TEMPLATE_FLOW_NAME DummyFlowEngineT
#include <yade/pkg/dem/FlowEngine.hpp>

/// We can add data to the Info types by inheritance
class DummyCellInfo : public FlowCellInfo
{
	public:
	Real anotherVariable;
	void anotherFunction() {};
};

class DummyVertexInfo : public FlowVertexInfo {
	public:
	//same here if needed
};

typedef TemplateFlowEngine<DummyCellInfo,DummyVertexInfo> TEMPLATE_FLOW_NAME;
REGISTER_SERIALIZABLE(TEMPLATE_FLOW_NAME);
YADE_PLUGIN((TEMPLATE_FLOW_NAME));

class DummyFlowEngine : public TEMPLATE_FLOW_NAME
{
	public :
	//We can overload every functions of the base engine to make it behave differently
	//if we overload action() like this, this engine is doing nothing in a standard timestep, it can still have useful functions
	virtual void action() {};
	
	//If a new function is specific to the derived engine, put it here, else go to the base TemplateFlowEngine
	//if it is useful for everyone
	void fancyFunction(Real what); {cerr<<"yes, I'm a new function"<<end;}

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(DummyFlowEngine,TEMPLATE_FLOW_NAME,"documentation here",
	((Real, myNewAttribute, 0,,"useless example"))
	,/*DummyFlowEngineT()*/,
	,
	.def("fancyFunction",&DummyFlowEngine::fancyFunction,(python::arg("what")=0),"test function")
	)
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(DummyFlowEngine);
YADE_PLUGIN((DummyFlowEngine));

void DummyFlowEngine::fancyFunction(Real what) {cerr<<"yes, I'm a new function"<<end;}
#undef TEMPLATE_FLOW_NAME DummyFlowEngineT //To be sure it will not conflict, maybe not needed
#endif //DummyFLOW