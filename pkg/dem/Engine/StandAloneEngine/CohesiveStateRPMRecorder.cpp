#include"CohesiveStateRPMRecorder.hpp"

YADE_PLUGIN((CohesiveStateRPMRecorder));
CREATE_LOGGER(CohesiveStateRPMRecorder);

CohesiveStateRPMRecorder::CohesiveStateRPMRecorder() {
	initRun=true;
	numberCohesiveContacts=0;
}

CohesiveStateRPMRecorder::~CohesiveStateRPMRecorder() {
}

void CohesiveStateRPMRecorder::init(MetaBody* rootBody) {
}

void CohesiveStateRPMRecorder::action(MetaBody* rootBody) {
	numberCohesiveContacts=0;
	FOREACH(const shared_ptr<Interaction>& i, *rootBody->interactions){
		if(!i->isReal()) continue;
		const shared_ptr<RpmPhys>& contPhys = YADE_PTR_CAST<RpmPhys>(i->interactionPhysics);
		if (contPhys->isCohesive==true) {
			numberCohesiveContacts++;
		}
	}
	//Temporary solution, while flieRecorder class is not ready
	outFile.open(outFileName.c_str(), ios::out | ios::app);
	outFile<<Omega::instance().getCurrentIteration()<<" "<<numberCohesiveContacts<<"\n";
	outFile.close();
}
