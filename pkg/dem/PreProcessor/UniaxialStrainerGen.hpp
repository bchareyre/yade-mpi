// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once
#include<yade/extra/Shop.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/pkg-dem/UniaxialStrainer.hpp>

/*
	Class for demonstrating the use of UniaxialStrainer.
	It is better to use UniaxialStrainer from python, since that way
	stresses can be plot easily etc.
*/
class UniaxialStrainerGen: public FileGenerator {
	private:
		void createEngines();
	public:
		UniaxialStrainerGen(){ axis=1; limitStrain=0; damping=0.2;cohesiveThresholdIter=10;};
		~UniaxialStrainerGen (){};
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
	REGISTER_CLASS_AND_BASE(UniaxialStrainerGen,FileGenerator);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(UniaxialStrainerGen);

