// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<fstream>
#include<limits>
#include<yade/extra/Shop.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/core/DeusExMachina.hpp>

class USCTGen: public FileGenerator {
	private:
		void createEngines();
	public:
		USCTGen(){ axis=1; limitStrain=0; damping=0.2;cohesiveThresholdIter=200; };
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
REGISTER_SERIALIZABLE(USCTGen,false);
/*! Axial displacing two groups of bodies in the opposite direction with given strain rate.
 *
 * Takes two groups of body IDs (in posIds and negIds) and displaces them at each timestep in the direction given by axis∈{0,1,2} (for axes x,y,z respectively). These bodies automatically have Body::isDynamic==false.
 */
class UniaxialStrainer: public DeusExMachina {
	private:
		bool idInVector(body_id_t id, const vector<body_id_t>& V){for(size_t i=0; i<V.size(); i++){ if(V[i]==id) return true; }	return false; }  // unused now
		void computeAxialForce(MetaBody* rootBody);
		ofstream recStream;
		bool needsInit;
		#define USCT_AXIS_COORD(id) (Body::byId(id)->physicalParameters->se3.position[axis])
	public:
		Real strainRate,currentStrainRate,originalLength,limitStrain;
		Real sumPosForces,sumNegForces;
		Real crossSectionArea;
		int axis;
		bool notYetReversed;
		bool clumped;
		vector<body_id_t> posIds, negIds;
		vector<Real> posCoords,negCoords;

		virtual void applyCondition(Body* _rootBody);
		void init();
		UniaxialStrainer(){axis=2; currentStrainRate=0; originalLength=-1; limitStrain=-1; notYetReversed=true; crossSectionArea=-1; needsInit=true; clumped=false; };
		virtual ~UniaxialStrainer(){};
		void registerAttributes(){
			DeusExMachina::registerAttributes();
			REGISTER_ATTRIBUTE(strainRate);
			REGISTER_ATTRIBUTE(currentStrainRate);
			REGISTER_ATTRIBUTE(axis);
			REGISTER_ATTRIBUTE(posIds);
			REGISTER_ATTRIBUTE(negIds);
			REGISTER_ATTRIBUTE(originalLength);
			REGISTER_ATTRIBUTE(limitStrain);
			REGISTER_ATTRIBUTE(notYetReversed);
			REGISTER_ATTRIBUTE(crossSectionArea);
		}
		void postProcessAttributes(bool deserializing){ if(deserializing) needsInit=true;};
	REGISTER_CLASS_NAME(UniaxialStrainer);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(UniaxialStrainer,false);


