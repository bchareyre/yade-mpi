#include"CohesiveStateRPMRecorder.hpp"

YADE_PLUGIN((CohesiveStateRPMRecorder));
CREATE_LOGGER(CohesiveStateRPMRecorder);

CohesiveStateRPMRecorder::CohesiveStateRPMRecorder() 
{
	initRun=true;
	numberCohesiveContacts=0;
}

CohesiveStateRPMRecorder::~CohesiveStateRPMRecorder()
{
}

void CohesiveStateRPMRecorder::init(MetaBody* rootBody)
{
}

void CohesiveStateRPMRecorder::action(MetaBody* rootBody)
{
	InteractionContainer::iterator ii = rootBody->interactions->begin();
	InteractionContainer::iterator iiEnd = rootBody->interactions->end(); 
	LOG_WARN("START !!!");
	numberCohesiveContacts=0;
	for(; ii!=iiEnd; ++ii ) {
		const shared_ptr<Interaction>& interaction = *ii;
		RpmPhys* contPhys = static_cast<RpmPhys*>(interaction->interactionPhysics.get());
		if (contPhys->isCohesive==true) {
		
		}
	}
}
