/*! This class is for storing the cohesive contacts number 
 * of RPM model in file
*/
#pragma once
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>
#include<yade/pkg-dem/RockPM.hpp>


class CohesiveStateRPMRecorder: public PeriodicEngine {
	public:
		string outFileName;
		int numberCohesiveContacts;
		CohesiveStateRPMRecorder(); 
		~CohesiveStateRPMRecorder();
		void init(MetaBody*);
		virtual void action(MetaBody*);
		
	private:
		std::ofstream outFile;
		
	REGISTER_ATTRIBUTES(PeriodicEngine,(outFileName));
	REGISTER_CLASS_AND_BASE(CohesiveStateRPMRecorder,PeriodicEngine);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(CohesiveStateRPMRecorder);

