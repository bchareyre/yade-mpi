

#pragma once 
#include<core/State.hpp>

class ThermalState: public State {
	public:
		virtual ~ThermalState();
// 		ThermalState& operator= (const State& source) : State(source) {};//FIXME Thermal.cpp:9:33: error: use of deleted function ‘State& State::operator=(const State&)’
		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(ThermalState,State,"preliminary",
		/*attributes*/
		((Real,tmp,0,,""))
		,
		/* extra initializers */
		,
		/* ctor */ createIndex();
		,
		/* py */
	);
	REGISTER_CLASS_INDEX(ThermalState,State);
};
REGISTER_SERIALIZABLE(ThermalState);

