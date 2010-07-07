/*! This class is for recording Particle Size Distribution
 * of RPM model in a file. Class derived from Recorder
*/
#pragma once
#include <yade/pkg-common/Recorder.hpp>
#include <yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include <yade/pkg-dem/RockPM.hpp>

class ParticleSizeDistrbutionRPMRecorder: public Recorder {
		std::ofstream outFile;
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(ParticleSizeDistrbutionRPMRecorder,Recorder,
		"Store number of PSD in RPM model to file.",
		((int,numberCohesiveContacts,0,"Number of cohesive contacts found at last run. [-]")),
		initRun=true;);
	DECLARE_LOGGER;
};


struct identicalIds{
	int id1, id2;
	identicalIds (int id1r, int id2r){
		assert(id1r<id2r);
		id1 = id1r;
		id2 = id2r;
	}
	
	static bool checkIdentical(identicalIds param1, identicalIds param2) {
		if ((param1.id1 == param2.id1) and (param1.id2 == param2.id2)) {
			return true;
		} else {
			return false;
		}
	}
};
REGISTER_SERIALIZABLE(ParticleSizeDistrbutionRPMRecorder);
