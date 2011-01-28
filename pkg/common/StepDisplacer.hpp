// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once

#include<yade/core/PartialEngine.hpp>

class StepDisplacer: public PartialEngine {
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(StepDisplacer,PartialEngine,"Apply generalized displacement (displacement or rotation) stepwise on subscribed bodies. Could be used for purposes of contact law tests (by moving one sphere compared to an other), but in this case, see rather :yref:`LawTester`",
		((Vector3r,mov,Vector3r::Zero(),,"Linear displacement step to be applied per iteration, by addition to :yref:`State.pos`."))
		((Quaternionr,rot,Quaternionr::Identity(),,"Rotation step to be applied per iteration (via rotation composition with :yref:`State.ori`)."))
		((bool,setVelocities,false,,"If false, positions and orientations are directly updated, without changing the speeds of concerned bodies. If true, only velocity and angularVelocity are modified. In this second case :yref:`integrator<NewtonIntegrator>` is supposed to be used, so that, thanks to this Engine, the bodies will have the prescribed jump over one iteration (dt)."))
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(StepDisplacer);
	

