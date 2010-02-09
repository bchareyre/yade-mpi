/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-base/Logging.hpp>

#include "Scene.hpp"
#include "ThreadWorker.hpp"

class FileGenerator
	: public Serializable
	, public ThreadWorker
{
	protected :
		shared_ptr<Scene>	 rootBody;
		string serializationDynlib;
	public :
		bool generateAndSave();
		void setFileName(const string& fileName);
		void setSerializationLibrary(const string& lib);
		std::string getFileName() {return outputFileName;}; // stupid? better make that variable public.. ech.
		std::string getSerializationLibrary() {return serializationDynlib;};
		//! Describes the result in a user-readable form.
		std::string message;
		
		virtual ~FileGenerator ();
		virtual void singleAction();
	protected :
	//! Returns whether the generation was successful; message for user is in FileGenerator::message
	virtual bool generate();

	void pyGenerate(const string& out);
	void pyLoad();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(FileGenerator,Serializable,"Base class for scene generators, preprocessors.",
		((string,outputFileName,"./scene.xml","Filename to write resulting simulation to")),
		/* ctor */ serializationDynlib="XMLFormatManager";,
		.def("generate",&FileGenerator::pyGenerate,(python::arg("out")),"Generate scene, save to given file")
		.def("load",&FileGenerator::pyLoad,"Generate scene, save to temporary file and load immediately");
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(FileGenerator);


