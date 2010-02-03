/*! This class is for storing the cohesive contacts number 
 * of RPM model in a file. Class derived from Recorder
*/
#pragma once
#include <yade/pkg-common/Recorder.hpp>
#include <yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include <yade/pkg-dem/RockPM.hpp>


class CohesiveStateRPMRecorder: public Recorder {
	public:
		int numberCohesiveContacts;
		CohesiveStateRPMRecorder(); 
		~CohesiveStateRPMRecorder();
		void init(Scene*);
		virtual void action(Scene*);
		
	private:
		std::ofstream outFile;
		
	REGISTER_ATTRIBUTES(Recorder,(numberCohesiveContacts));
	REGISTER_CLASS_AND_BASE(CohesiveStateRPMRecorder,Recorder);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(CohesiveStateRPMRecorder);

	/*
	YADE_CLASS_BASE_DOC_ATTRDECL_CTOR_PY(CohesiveStateRPMRecorder,Recorder,"This class is for storing the number of a cohesive contacts in RPM model to file. Class derived from Recorder",
		(( int,numberCohesiveContacts,0,"The number of cohesive contacts [-]")),
		initRun=true;,);
	*/
