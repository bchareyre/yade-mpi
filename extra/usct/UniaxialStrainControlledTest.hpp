// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<fstream>
#include<limits>
#include<yade/extra/Shop.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/core/DeusExMachina.hpp>

#ifndef FOREACH
#define FOREACH BOOST_FOREACH
#endif

class USCTGen: public FileGenerator {
	private:
		void createEngines();
	public:
		USCTGen(){ axis=1; limitStrain=0; damping=0.2;cohesiveThresholdIter=200;};
		~USCTGen (){};
		bool generate();
		string spheresFile;
		Real strainRate, limitStrain, damping;
		int axis;
		long cohesiveThresholdIter;
	protected :
		void registerAttributes(){
			FileGenerator::registerAttributes();
			REGISTER_ATTRIBUTE(spheresFile);
			REGISTER_ATTRIBUTE(axis);
			REGISTER_ATTRIBUTE(strainRate);
			REGISTER_ATTRIBUTE(limitStrain);
			REGISTER_ATTRIBUTE(damping);
			REGISTER_ATTRIBUTE(cohesiveThresholdIter);
		}
	REGISTER_CLASS_NAME(USCTGen);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(USCTGen);


/*! Axial displacing two groups of bodies in the opposite direction with given strain rate.
 *
 * Takes two groups of body IDs (in posIds and negIds) and displaces them at each timestep in the direction given by axis∈{0,1,2} (for axes x,y,z respectively). These bodies automatically have Body::isDynamic==false.
 *
 * This engine should be run once forces on particles have been computed.
 */
class UniaxialStrainer: public DeusExMachina {
	private:
		bool idInVector(body_id_t id, const vector<body_id_t>& V){for(size_t i=0; i<V.size(); i++){ if(V[i]==id) return true; }	return false; }  // unused now
		void computeAxialForce(MetaBody* rootBody);

		ofstream recStream;
		string recordFile;
		bool needsInit;
		Real& axisCoord(body_id_t id){ return Body::byId(id)->physicalParameters->se3.position[axis]; };
		void init();
	public:
		virtual bool isActivated(){return active;}
		//! strain rate, starting at 0, linearly raising to strainRate
		Real strainRate,currentStrainRate;
		//! alternatively, absolute speed of boundary motion can be specified; this is effective only at the beginning and if strainRate is not set; changing absSpeed directly during simulation wil have no effect.
		Real absSpeed;
		//! strain at which we will pause simulation; inactive (nan) by default; must be reached from below (in absolute value)
		Real stopStrain;
		//! distance of reference bodies in the direction of axis before straining started
		Real originalLength;
		//! invert the sense of straining (sharply, without transition) one this value of strain is reached. Not effective if 0.
		Real limitStrain;
		//! Flag whether the sense of straining has already been reversed
		bool notYetReversed;
		Real sumPosForces,sumNegForces;
		//! crossSection perpendicular to he strained axis, computed from AABB of MetaBody
		Real crossSectionArea;		//! Apply strain along x (0), y (1) or z(2) axis
		int axis;
		//! If 0, straining is symmetric for negIds and posIds; for 1 (or -1), only posIds are strained and negIds don't move (or vice versa)
		int asymmetry;
		//! Whether displacement of boundary bodies perpendicular to the strained axis are blocked of are free
		bool blockDisplacements;
		//! Whether rotations of boundary bodies are blocked.
		bool blockRotations;
		//! Are we activated?
		bool active;
		//! Number of iterations that will pass without straining activity after stopStrain has been reached (default: 0)
		long idleIterations;
		//! Time for strain reaching the requested value (linear interpolation). If negative, the time is dt*(-initAccelTime), where dt is  the timestep at the first iteration.
		Real initAccelTime, initAccelTime_s /* value always in s, computed from initAccelTime */;

		/** bodies on which straining will be applied (on the positive and negative side of axis) */
		vector<body_id_t> posIds, negIds;
		/** coordinates of pos/neg bodies in the direction of axis */
		vector<Real> posCoords,negCoords;
		#if 0
		void pushTransStrainSensors(MetaBody* rb, vector<Real>& widths);
		void setupTransStrainSensors();
		Real transStrainSensorArea;
		//! Pointer to ForceEngine that is used for transStrainSensors
		shared_ptr<UniaxialStrainSensorPusher> sensorsPusher;
		/** Bodies that act as transversal strain sensors;
		 * they should have very little mass (light force is applied on them)
		 * to accurately copy strain of the underlying specimen.
		 *
		 * They should be spacially _inside_ the speciment at the beginning (since MetaBody AABB is used
		 * to calculate crossSection. UniaxialStrainer::init will resize and displace them as needed.
		 * */
		vector<body_id_t> transStrainSensors;
		Real avgTransStrain;
		#endif

		//! Auxiliary vars (serializable, for recording)
		Real strain, avgStress;

		virtual void applyCondition(MetaBody* rootBody);
		UniaxialStrainer(){axis=2; asymmetry=0; currentStrainRate=0; originalLength=-1; limitStrain=0; notYetReversed=true; crossSectionArea=-1; needsInit=true; /* sensorsPusher=shared_ptr<UniaxialStrainSensorPusher>(); */ recordFile=""; strain=avgStress=/*avgTransStrain=*/0; blockRotations=false; blockDisplacements=false;  strainRate=0; absSpeed=stopStrain=numeric_limits<Real>::quiet_NaN(); active=true; idleIterations=0; initAccelTime=-200;};
		virtual ~UniaxialStrainer(){};
		REGISTER_ATTRIBUTES(DeusExMachina,
				(strainRate) 
				(absSpeed)
				(initAccelTime)
				(stopStrain) 
				(active)
				(idleIterations)
				(currentStrainRate) 
				(axis) 
				(asymmetry) 
				(posIds) 
				(negIds) 
				(originalLength) 
				(limitStrain) 
				(notYetReversed) 
				(crossSectionArea) 
				(recordFile) 
				(strain) 
				(avgStress) 
				(blockDisplacements) 
				(blockRotations) 
		);
		void prepareRecStream(void){ if(!recordFile.empty()) recStream.open(recordFile.c_str()); }
		void postProcessAttributes(bool deserializing){ if(deserializing) prepareRecStream(); } 	
	REGISTER_CLASS_AND_BASE(UniaxialStrainer,DeusExMachina);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(UniaxialStrainer);


