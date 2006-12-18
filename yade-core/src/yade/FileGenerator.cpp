/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<cstdlib>
#include<yade/yade-lib-multimethods/MultiMethodsExceptions.hpp>

#include"FileGenerator.hpp"

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

}


string FileGenerator::generate() 
{
	return "FileGenerator (base class) generates nothing.";
}


string FileGenerator::generateAndSave()
{
	string message;
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
		return std::string("File "+outputFileName+" generated successfully.\n\n")+message;
	}
}

void FileGenerator::singleAction()
{
	setMessage("generating model...");
	std::string st=generateAndSave();
	setReturnValue(st);
};

