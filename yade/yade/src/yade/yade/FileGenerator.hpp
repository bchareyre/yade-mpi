
#ifndef __FILEGENERATOR_H__
#define __FILEGENERATOR_H__

#include "Serializable.hpp"

class FileGenerator : public Serializable
{
	protected : string outputFileName;
	protected : string serializationDynlib;
	
	public    : void setFileName(const string& fileName) { outputFileName=fileName;};
	public    : void setSerializationLibrary(const string& lib) { serializationDynlib=lib;};
	// construction
	public : FileGenerator () : Serializable() {};
	public : ~FileGenerator () {};

	protected : virtual void postProcessAttributes(bool) {};
	public : virtual void registerAttributes() {};

	public : virtual void generate() {};

	REGISTER_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(FileGenerator,false);

#endif // __FILEGENERATOR_H__

