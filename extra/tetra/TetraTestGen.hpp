#ifndef TETRATESTGEN_HPP
#define TETRATESTGEN_HPP

#include<vector>
#include<yade/core/FileGenerator.hpp>
//#include<yade/pkg-common/Tetrahedron.hpp>

/*! Test basic tetra functionality. */
class TetraTestGen: public FileGenerator {
	DECLARE_LOGGER;
	public:
		vector<size_t> gridSize;
		TetraTestGen(){ gridSize.push_back(3); gridSize.push_back(3); gridSize.push_back(3);};
		~TetraTestGen (){};
		string generate();
	protected :
		virtual void postProcessAttributes(bool deserializing){};
		void registerAttributes(){FileGenerator::registerAttributes(); REGISTER_ATTRIBUTE(gridSize);}
		REGISTER_CLASS_NAME(TetraTestGen);
		REGISTER_BASE_CLASS_NAME(FileGenerator);
};
REGISTER_SERIALIZABLE(TetraTestGen,false);

#endif
