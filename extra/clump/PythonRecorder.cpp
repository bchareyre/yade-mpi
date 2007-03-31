// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

// Python likes to be first..
#include <Python.h>

#include "PythonRecorder.hpp"
//#include<yade/pkg-common/RigidBodyParameters.hpp>
//#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
//#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/stringize.hpp>


CREATE_LOGGER(PythonRecorder);

/*! Constructor loads the pyade module.
 *
 * @todo Import path prepended to sys.path hardcoded now. Should depend on install dir (PREFIX and POSTFIX);
 * @todo Maybe change PythonRecorder::expression to vector<string> so that multiple expressions may be run at a time. This is equivalent to multiline expressions, but more easily manageable from c++.
 *
 */

PythonRecorder::PythonRecorder():DataRecorder(){
	// FIXME: this will have to be set somehow, hardcode for now...
	PyRun_SimpleString("import sys; sys.path.insert(0,'" PREFIX "/lib/yade" SUFFIX "/extra');");

	#if 0
	PyObject* pyModuleName=PyString_FromString("pyade");
	PyObject* pyModule=PyImport_Import(pyModuleName);
	Py_DECREF(pyModuleName);
	//if (!pyModule) { if (PyErr_Occurred()) PyErr_Print();	LOG_ERROR("pyade import failed.");}
	//else LOG_DEBUG("Python imported pyade");
	// for now, do a simpler thing: (FIXME: how to check for import error??)
	#endif
	int status=PyRun_SimpleString("from pyade import *"); // pyade will import _pyade by itself
	if(status){ LOG_ERROR("pyade import failed."); }
	else LOG_DEBUG("pyade imported.");
}

void PythonRecorder::postProcessAttributes(bool deserializing){
	if(outputFile.length()>0){
		LOG_DEBUG("Redirecting python stdout to `"<<outputFile<<"'.");
		PyRun_SimpleString(string("import os; import sys; ofile=file('"+outputFile+"','w+'); sys.stdout=ofile").c_str());
	}
	if(initExpression.length()>0){
		PyRun_SimpleString(initExpression.c_str());
	}
}

void PythonRecorder::action(Body *_rootBody)
{
	//MetaBody* rootBody=YADE_CAST<MetaBody*>(_rootBody);
	PyRun_SimpleString(expression.c_str());
}



