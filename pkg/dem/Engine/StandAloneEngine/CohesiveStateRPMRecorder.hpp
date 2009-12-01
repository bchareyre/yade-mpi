/*! This class is for storing the cohesive contacts number 
 * of RPM model in a file. Class derived from Recorder
*/
#pragma once
#include <yade/pkg-common/Recorder.hpp>
#include <yade/pkg-common/InteractionPhysicsEngineUnit.hpp>
#include <yade/pkg-dem/RockPM.hpp>


class CohesiveStateRPMRecorder: public Recorder {
	public:
		int numberCohesiveContacts;
		CohesiveStateRPMRecorder(); 
		~CohesiveStateRPMRecorder();
		void init(MetaBody*);
		virtual void action(MetaBody*);
		
	private:
		std::ofstream outFile;
		
	REGISTER_ATTRIBUTES(Recorder,(numberCohesiveContacts));
	REGISTER_CLASS_AND_BASE(CohesiveStateRPMRecorder,Recorder);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(CohesiveStateRPMRecorder);

