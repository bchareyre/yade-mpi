#include"ForceRecorder.hpp"

YADE_PLUGIN((ForceRecorder));
CREATE_LOGGER(ForceRecorder);

void ForceRecorder::action(Scene * ncb){
	totalForce=Vector3r::ZERO;
	FOREACH(body_id_t id, subscribedBodies){
		assert(ncb->bodies->exists(id)); 
		totalForce+=ncb->forces.getForce(id);
	};
	
	//Save data to a file
	out<<Omega::instance().getCurrentIteration()<<" "<<totalForce[0]<<" "<<totalForce[1]<<" "<<totalForce[2]<<" "<<totalForce.Length()<<"\n";
	out.close();
}
