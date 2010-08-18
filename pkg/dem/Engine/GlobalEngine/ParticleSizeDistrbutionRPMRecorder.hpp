/*! This class is for recording Particle Size Distribution
 * of RPM model in a file. Class derived from Recorder
*/
#pragma once
#include <yade/pkg-common/Recorder.hpp>
#include <yade/pkg-common/Dispatching.hpp>
#include <yade/pkg-dem/RockPM.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/Sphere.hpp>

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
	Real mass, maxDistanceBetweenSpheres;
	identicalIds (int id1r, int id2r, Real massr){
		assert(id1r<id2r);
		id1 = id1r;
		id2 = id2r;
		mass = massr;
		maxDistanceBetweenSpheres = 0;
	}
	static bool sortArrayIdentIds (identicalIds i, identicalIds d) {return i.mass>d.mass;}
};

struct materialAnalyze{
	int matId, specId, particleNumber;
	Real mass;
	materialAnalyze (int matIdR, int specIdR, int particleNumberR, Real massR){
		matId = matIdR;
		specId = specIdR;
		particleNumber = particleNumberR;
		mass = massR;
	}
	static bool sortMaterialAnalyze (materialAnalyze i, materialAnalyze d) {return d.specId>i.specId;}
};

REGISTER_SERIALIZABLE(ParticleSizeDistrbutionRPMRecorder);
