#include"ForceRecorder.hpp"

YADE_PLUGIN((ForceRecorder));
CREATE_LOGGER(ForceRecorder);

void ForceRecorder::action(){
	totalForce=Vector3r::Zero();
	FOREACH(Body::id_t id, subscribedBodies){
		assert(scene->bodies->exists(id)); 
		totalForce+=scene->forces.getForce(id);
	};
	
	//Save data to a file
	out<<Omega::instance().getCurrentIteration()<<" "<<totalForce[0]<<" "<<totalForce[1]<<" "<<totalForce[2]<<" "<<totalForce.norm()<<"\n";
	out.close();
}
