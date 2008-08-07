/*************************************************************************
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<cstdlib>
#include<yade/lib-multimethods/MultiMethodsExceptions.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>

#include"FileGenerator.hpp"

CREATE_LOGGER(FileGenerator);

FileGenerator::FileGenerator () : Serializable() 
{
	outputFileName = "./scene.xml";
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


bool FileGenerator::generate() 
{
	message="FileGenerator (base class) generates nothing.";
	return false;
}


bool FileGenerator::generateAndSave()
{
	bool status;
	message="";
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	try {
		status=generate(); // will modify message
	}
	catch(SerializableError& e){message+=string("SeriazableError: ")+e.what(); return false;}
	catch(FactoryError& e){message+=string("FactoryError: ")+e.what(); return false;}
	catch(MultiMethodsError& e){message+=string("MultiMethodsError: ")+e.what(); return false;}
	catch(std::exception& e){
		LOG_FATAL("Unhandled exception: "<<typeid(e).name()<<" : "<<e.what());
		//abort(); // use abort, since we may want to inspect core
		message = message + "Unhandled exception: " + typeid(e).name() + " : " + e.what();
		return false;
	}
	// generation wasn't successful
	if(status==false) return false;

	if(shouldTerminate()){ message+="Generation aborted."; return false; }
	else {
		boost::posix_time::ptime now2 = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration generationTime = now2 - now; // generation time, without save time
		setStatus("saving...");
		setProgress(1.0);
		try
		{
			IOFormatManager::saveToFile(serializationDynlib, outputFileName, "rootBody", rootBody);
		}
		catch(SerializableError& e)
		{
			message+=std::string("File "+outputFileName+" cannot be saved: "+e.what());
			return false;
		}
		boost::posix_time::ptime now3 = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration saveTime = now3 - now2; // save time
		message=std::string("File "+outputFileName+" generated successfully."
				+ "\ngeneration time: " + boost::posix_time::to_simple_string(generationTime) 
				+ "\nsave time: "       + boost::posix_time::to_simple_string(saveTime)
				+"\n\n")+message;
		return true;
	}
}

void FileGenerator::singleAction()
{
	setStatus("generating model...");
	bool st=generateAndSave();
	setReturnValue(st);
};

