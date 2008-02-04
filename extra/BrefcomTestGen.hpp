#pragma once
#include<yade/extra/Shop.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/extra/Brefcom.hpp>

class BrefcomTestGen: public FileGenerator {
	private:
		void createEngines();
	public:
		Real strainRate;
		BrefcomTestGen(){strainRate=1e-2;};
		bool generate();
	protected :
		void registerAttributes(){
			FileGenerator::registerAttributes();
			REGISTER_ATTRIBUTE(strainRate);
		}
	REGISTER_CLASS_NAME(BrefcomTestGen);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(BrefcomTestGen,false);
