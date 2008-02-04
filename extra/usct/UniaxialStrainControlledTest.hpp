// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<fstream>
#include<yade/extra/Shop.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/core/DeusExMachina.hpp>

class USCTGen: public FileGenerator {
	private:
		void createEngines();
	public:
		USCTGen(){ axis=1; limitStrain=0; };
		~USCTGen (){};
		bool generate();
		string spheresFile;
		Real strainRate, limitStrain;
		int axis;
	protected :
		void registerAttributes(){
			FileGenerator::registerAttributes();
			REGISTER_ATTRIBUTE(spheresFile);
			REGISTER_ATTRIBUTE(axis);
			REGISTER_ATTRIBUTE(strainRate);
			REGISTER_ATTRIBUTE(limitStrain);
		}
	REGISTER_CLASS_NAME(USCTGen);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(USCTGen,false);

class UniaxialStrainer: public DeusExMachina {
	private:
		bool idInVector(body_id_t id, const vector<body_id_t>& V);
		void computeAxialForce(MetaBody* rootBody);
		ofstream recStream;
		#define USCT_AXIS_COORD(id) (Body::byId(id)->physicalParameters->se3.position[axis])
	public:
		Real strainRate,currentStrainRate,originalLength,limitStrain;
		Real sumPosForces,sumNegForces;
		int axis;
		bool notYetReversed;
		vector<body_id_t> posIds, negIds;
		vector<Real> posCoords,negCoords;

		virtual void applyCondition(Body* _rootBody);
		UniaxialStrainer(){axis=2; currentStrainRate=0; originalLength=-1; limitStrain=-1; notYetReversed=true; };
		virtual ~UniaxialStrainer(){};
		void registerAttributes(){
			DeusExMachina::registerAttributes();
			REGISTER_ATTRIBUTE(strainRate);
			REGISTER_ATTRIBUTE(currentStrainRate);
			REGISTER_ATTRIBUTE(axis);
			REGISTER_ATTRIBUTE(posIds);
			REGISTER_ATTRIBUTE(negIds);
			REGISTER_ATTRIBUTE(posCoords);
			REGISTER_ATTRIBUTE(negCoords);
			REGISTER_ATTRIBUTE(originalLength);
			REGISTER_ATTRIBUTE(limitStrain);
			REGISTER_ATTRIBUTE(notYetReversed);
		}
		void postProcessAttributes(bool deserializing){if(deserializing) recStream.open("/tmp/usct.data"); };
	REGISTER_CLASS_NAME(UniaxialStrainer);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(UniaxialStrainer,false);


