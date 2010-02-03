#include"CohesiveStateRPMRecorder.hpp"

YADE_PLUGIN((CohesiveStateRPMRecorder));
CREATE_LOGGER(CohesiveStateRPMRecorder);

void CohesiveStateRPMRecorder::action(Scene* rootBody) {
	numberCohesiveContacts=0;
	//Check all interactions
	FOREACH(const shared_ptr<Interaction>& i, *rootBody->interactions){
		if(!i->isReal()) continue;				//Check whether they are real
		const shared_ptr<RpmPhys>& contPhys = YADE_PTR_CAST<RpmPhys>(i->interactionPhysics);
		if (contPhys->isCohesive==true) {	//Check whether they are cohesive
			numberCohesiveContacts++;				//If yes - calculate them
		}
	}
	//Save data to a file
	out<<Omega::instance().getCurrentIteration()<<" "<<numberCohesiveContacts<<"\n";
	out.close();
}
