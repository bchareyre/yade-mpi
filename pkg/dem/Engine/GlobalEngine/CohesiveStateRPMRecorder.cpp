#include"CohesiveStateRPMRecorder.hpp"

YADE_PLUGIN((CohesiveStateRPMRecorder));
CREATE_LOGGER(CohesiveStateRPMRecorder);

void CohesiveStateRPMRecorder::action() {
	numberCohesiveContacts=0;
	//Check all interactions
	FOREACH(const shared_ptr<Interaction>& i, *scene->interactions){
		if(!i->isReal()) continue;				//Check whether they are real
		const shared_ptr<RpmPhys>& contPhys = YADE_PTR_CAST<RpmPhys>(i->phys);
		if (contPhys->isCohesive==true) {	//Check whether they are cohesive
			numberCohesiveContacts++;				//If yes - calculate them
		}
	}
	//Save data to a file
	out<<scene->iter<<" "<<numberCohesiveContacts<<"\n";
	out.close();
}
