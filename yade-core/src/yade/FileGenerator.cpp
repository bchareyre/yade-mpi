/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FileGenerator.hpp"


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
	string message = generate();
	if(shouldTerminate())
	{
		return "Generation aborted.";
	}
	else
	{
		setMessage("saving...");
		setProgress(1.0);
		IOFormatManager::saveToFile(serializationDynlib, outputFileName, "rootBody", rootBody);
		return std::string("File "+outputFileName+" generated successfully.\n\n")+message;
	}
}

void FileGenerator::singleAction()
{
	setMessage("generating model...");
	std::string st=generateAndSave();
	setReturnValue(st);
};

