

#pragma once 
#include<core/State.hpp>

// This is how to turn a body thermal without data loss. Should be done in a loop by a single function, ofc.
// Yade [10]: s=sphere((0,0,1),100)
// Yade [11]: s.state.vel=(3,3,3)
// Yade [12]: thState = ThermalState()
// Yade [13]: thState.updateAttrs(s.state.dict())
// Yade [14]: s.state=thState
// Yade [15]: s.state.tmp
//  ->  [15]: 0.0
// Yade [16]: s.state.vel
//  ->  [16]: Vector3(3,3,3)

// Shorter yet strictly equivalent
// Yade [21]: s.state=ThermalState().__setstate__( s.state.__getstate__())



class ThermalState: public State {
	public:
		virtual ~ThermalState();
		// State is declared boost::noncopyable, so copy constructor seems nearly impossible. The solution is to update inherited attributes using python as show in preamble
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
