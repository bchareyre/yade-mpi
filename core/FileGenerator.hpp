/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FILEGENERATOR_HPP
#define FILEGENERATOR_HPP

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-base/Logging.hpp>

#include "MetaBody.hpp"
#include "ThreadWorker.hpp"

class FileGenerator
	: public Serializable
	, public ThreadWorker
{
	protected :
		shared_ptr<MetaBody>	 rootBody;

		string			 outputFileName
					,serializationDynlib;
	
	public :
		bool generateAndSave();
		void setFileName(const string& fileName);
		void setSerializationLibrary(const string& lib);
		std::string getFileName() {return outputFileName;}; // stupid? better make that variable public.. ech.
		std::string getSerializationLibrary() {return serializationDynlib;};
		//! Describes the result in a user-readable form.
		std::string message;
		
		FileGenerator ();
		virtual ~FileGenerator ();
		
		virtual void singleAction();

	protected :
	//! Returns whether the generation was successful; message for user is in FileGenerator::message
	virtual bool generate();
	virtual void postProcessAttributes(){};

	REGISTER_ATTRIBUTES(/*no base*/,(outputFileName)(serializationDynlib));
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(FileGenerator,Serializable);
};

REGISTER_SERIALIZABLE(FileGenerator);

#endif // FILEGENERATOR_HPP

