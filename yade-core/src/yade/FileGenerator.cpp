/*************************************************************************
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <cstdlib>
#include <yade/yade-lib-multimethods/MultiMethodsExceptions.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "FileGenerator.hpp"

CREATE_LOGGER(FileGenerator);

FileGenerator::FileGenerator () : Serializable() 
{
	outputFileName = "../data/scene.xml";
	serializationDynlib = "XMLFormatManager";
}


FileGenerator::~FileGenerator () 
{

}


void FileGenerator::setFileName(const string& fileName) 
{ 
	outputFileName=fileName;
}


void FileGenerator::setSerializationLibrary(const string& lib) 
{ 
	serializationDynlib=lib;
}


void FileGenerator::postProcessAttributes(bool) 
{

}


void FileGenerator::registerAttributes() 
{	
	REGISTER_ATTRIBUTE(outputFileName);
	REGISTER_ATTRIBUTE(serializationDynlib);
}


string FileGenerator::generate() 
{
	return "FileGenerator (base class) generates nothing.";
}


string FileGenerator::generateAndSave()
{
	string message;
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	try {
		message = generate();
	}
	catch(SerializableError& e){return string("SeriazableError: ")+e.what();}
	catch(FactoryError& e){return string("FactoryError: ")+e.what();}
	catch(MultiMethodsError& e){return string("MultiMethodsError: ")+e.what();}
	catch(std::exception& e){
		LOG_FATAL("Unhandled exception: "<<typeid(e).name()<<" : "<<e.what());
		exit(1);
	}

	if(shouldTerminate())
	{
		return "Generation aborted.";
	}
	else
	{
		boost::posix_time::ptime now2 = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration generationTime = now2 - now; // generation time, without save time
		setMessage("saving...");
		setProgress(1.0);
		try
		{
			IOFormatManager::saveToFile(serializationDynlib, outputFileName, "rootBody", rootBody);
		}
		catch(SerializableError& e)
		{
			return std::string("File "+outputFileName+" cannot be saved: "+e.what());
		}
		boost::posix_time::ptime now3 = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration saveTime = now3 - now2; // save time
		return std::string("File "+outputFileName+" generated successfully."
				+ "\ngeneration time: " + boost::posix_time::to_simple_string(generationTime) 
				+ "\nsave time: "       + boost::posix_time::to_simple_string(saveTime)
				+"\n\n")+message;
	}
}

void FileGenerator::singleAction()
{
	setMessage("generating model...");
	std::string st=generateAndSave();
	setReturnValue(st);
};

