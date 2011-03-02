#include"ForceTorqueRecorder.hpp"

YADE_PLUGIN((ForceRecorder)(TorqueRecorder));

CREATE_LOGGER(ForceRecorder);
void ForceRecorder::action(){
	totalForce=Vector3r::Zero();
	FOREACH(Body::id_t id, ids){
		assert(scene->bodies->exists(id)); 
		totalForce+=scene->forces.getForce(id);
	};
	
	//Save data to a file
	out<<scene->iter<<" "<<totalForce[0]<<" "<<totalForce[1]<<" "<<totalForce[2]<<" "<<totalForce.norm()<<"\n";
	out.close();
}

CREATE_LOGGER(TorqueRecorder);
void TorqueRecorder::action(){
	totalTorque=0;
	Vector3r tmpAxis = rotationAxis.normalized();
	
	FOREACH(Body::id_t id, ids){
		assert(scene->bodies->exists(id)); 
		Body* b=Body::byId(id,scene).get();
		
		Vector3r tmpPos = b->state->pos;
		Vector3r radiusVector = tmpAxis.cross(tmpAxis.cross(tmpPos - zeroPoint));
		
		totalTorque+=tmpAxis.dot(scene->forces.getTorque(id)+radiusVector.cross(scene->forces.getForce(id)));
	};
	
	//Save data to a file
	out<<scene->iter<<" "<<totalTorque<<"\n";
	out.close();
}
