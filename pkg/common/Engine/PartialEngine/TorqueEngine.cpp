#include"TorqueEngine.hpp"
#include<yade/core/Scene.hpp>

void TorqueEngine::action(){
	FOREACH(const body_id_t id, subscribedBodies){
		// check that body really exists?
		scene->forces.addTorque(id,moment);
	}
}

YADE_PLUGIN((TorqueEngine));

