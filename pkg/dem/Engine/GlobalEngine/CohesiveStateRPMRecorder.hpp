/*! This class is for storing the cohesive contacts number 
 * of RPM model in a file. Class derived from Recorder
*/
#pragma once
#include <yade/pkg-common/Recorder.hpp>
#include <yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include <yade/pkg-dem/RockPM.hpp>

class CohesiveStateRPMRecorder: public Recorder {
		std::ofstream outFile;
	public:
		virtual void action(Scene*);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CohesiveStateRPMRecorder,Recorder,
		"Store number of cohesive contacts in RPM model to file.",
		((int,numberCohesiveContacts,0,"Number of cohesive contacts found at last run. [-]")),
		initRun=true;);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(CohesiveStateRPMRecorder);
