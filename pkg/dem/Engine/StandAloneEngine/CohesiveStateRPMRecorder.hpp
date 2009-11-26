#pragma once
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>
#include<yade/pkg-dem/RockPM.hpp>


class CohesiveStateRPMRecorder: public PeriodicEngine {
	public:
		string fileName;
		int numberCohesiveContacts;
		CohesiveStateRPMRecorder(); 
		~CohesiveStateRPMRecorder();
		void init(MetaBody*);
		virtual void action(MetaBody*);
	private:
		
	REGISTER_ATTRIBUTES(PeriodicEngine,(fileName));
	REGISTER_CLASS_AND_BASE(CohesiveStateRPMRecorder,PeriodicEngine);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(CohesiveStateRPMRecorder);

