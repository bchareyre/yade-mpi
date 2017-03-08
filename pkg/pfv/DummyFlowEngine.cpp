 
/*************************************************************************
*  Copyright (C) 2014 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// This is an example of how to derive a new FlowEngine with additional data and possibly completely new behaviour.
// Every functions of the base engine can be overloaded, and new functions can be added

//keep this #ifdef as long as you don't really want to release a final version publicly, it will save compilation time for everyone else
//when you want it compiled, you can pass -DDUMMYFLOW to cmake, or just uncomment the following line
// #define DUMMYFLOW
#ifdef DUMMYFLOW

#include "FlowEngine_DummyFlowEngineT.hpp"

/// We can add data to the Info types by inheritance
class DummyCellInfo : public FlowCellInfo_DummyFlowEngineT
{
	public:
	Real anotherVariable;
	void anotherFunction() {};
};

class DummyVertexInfo : public FlowVertexInfo_DummyFlowEngineT {
	public:
	//same here if needed
};

typedef TemplateFlowEngine_DummyFlowEngineT<DummyCellInfo,DummyVertexInfo> DummyFlowEngineT;
REGISTER_SERIALIZABLE(DummyFlowEngineT);
YADE_PLUGIN((DummyFlowEngineT));

class DummyFlowEngine : public DummyFlowEngineT
{
	public :
	//We can overload every functions of the base engine to make it behave differently
	//if we overload action() like this, this engine is doing nothing in a standard timestep, it can still have useful functions
	virtual void action() {};
	
	//If a new function is specific to the derived engine, put it here, else go to the base TemplateFlowEngine
	//if it is useful for everyone
	void fancyFunction(Real what);

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(DummyFlowEngine,DummyFlowEngineT,"documentation here",
	((Real, myNewAttribute, 0,,"useless example"))
	,/*DummyFlowEngineT()*/,
	,
	.def("fancyFunction",&DummyFlowEngine::fancyFunction,(boost::python::arg("what")=0),"test function")
	)
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(DummyFlowEngine);
YADE_PLUGIN((DummyFlowEngine));

void DummyFlowEngine::fancyFunction(Real what) {std::cerr<<"yes, I'm a new function"<<std::endl;}
#endif //DummyFLOW
