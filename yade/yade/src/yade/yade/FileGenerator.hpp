
#ifndef __FILEGENERATOR_H__
#define __FILEGENERATOR_H__

#include "Serializable.hpp"

class FileGenerator : public Serializable
{
	public : string outputFileName;
	public : string serializationDynlib;
	// construction
	public : FileGenerator () : Serializable() {};
	public : ~FileGenerator () {};

	public : virtual void afterDeserialization() {};
	public : virtual void registerAttributes() {};

	public : virtual void generate() {};

	REGISTER_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(FileGenerator,false);

#endif // __FILEGENERATOR_H__

