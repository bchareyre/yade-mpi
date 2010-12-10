// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once

#include<yade/core/PartialEngine.hpp>

class StepDisplacer: public PartialEngine {
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(StepDisplacer,PartialEngine,"Apply generalized displacement (displacement or rotation) stepwise on subscribed bodies.",
		((Vector3r,mov,Vector3r::Zero(),,"Linear displacement step to be applied per iteration, by addition to :yref:`State.pos`."))
		((Quaternionr,rot,Quaternionr::Identity(),,"Rotation step to be applied per iteration (via rotation composition with :yref:`State.ori`)."))
		((bool,setVelocities,false,,"If true, velocity and angularVelocity are modified in such a way that over one iteration (dt), the body will have the prescribed jump. In this case, :yref:`position<State.pos>` and :yref:`orientation<State.ori>` itself is not updated for :yref:`dynamic<Body::dynamic>` bodies, since they would have the delta applied twice (here and in the :yref:`integrator<NewtonIntegrator>`). For non-dynamic bodies however, they *are* still updated."))
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(StepDisplacer);
	

