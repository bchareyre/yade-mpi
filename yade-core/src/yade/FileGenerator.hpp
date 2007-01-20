/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FILEGENERATOR_HPP
#define FILEGENERATOR_HPP

#include <yade/yade-lib-serialization/Serializable.hpp>

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
		std::string generateAndSave();
		void setFileName(const string& fileName);
		void setSerializationLibrary(const string& lib);
		std::string getFileName() {return outputFileName;}; // stupid? better make that variable public.. ech.
		std::string getSerializationLibrary() {return serializationDynlib;};
		
		FileGenerator ();
		virtual ~FileGenerator ();
		
		virtual void singleAction();

	protected :
		virtual string generate();

		virtual void postProcessAttributes(bool);
		virtual void registerAttributes();

	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(FileGenerator);
	REGISTER_BASE_CLASS_NAME(Serializable);
};

REGISTER_SERIALIZABLE(FileGenerator,false);

#endif // FILEGENERATOR_HPP

