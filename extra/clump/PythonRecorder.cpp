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
 */

PythonRecorder::PythonRecorder():DataRecorder(){
	LOG_FATAL("what use of releasing unknown lock?! Otherwise deadlock here.");
	PyEval_ReleaseLock();
	PyGILState_STATE pyState = PyGILState_Ensure();
		PyRun_SimpleString("import sys; sys.path.insert(0,'" PREFIX "/lib/yade" SUFFIX "/extra');");
		int status=PyRun_SimpleString("from pyade import *"); // pyade will import _pyade by itself
		if(status){ LOG_ERROR("pyade import failed."); } else LOG_DEBUG("pyade imported.");
	PyGILState_Release(pyState);

	compiledRunExprCall=NULL;
}

/* Transform deserialized attributes in something useful.
 *
 * - runs initExpr in python.
 * - creates runExpr function definition
 * - attempts its acceleration through psyco
 * - creates bytecode for runExpr call
 *
 */
void PythonRecorder::postProcessAttributes(bool deserializing){
	PyGILState_STATE pyState = PyGILState_Ensure();
		if(outputFile.length()>0){
			LOG_DEBUG("Redirecting python stdout to `"<<outputFile<<"'.");
			PyRun_SimpleString(string("import os; import sys; ofile=file('"+outputFile+"','w+'); sys.stdout=ofile").c_str());
		}
		if(initExpr.length()>0){
			PyRun_SimpleString(initExpr.c_str());
		}

		// if this is not the first time we deserialize, free the old byte-compiled code
		if(compiledRunExprCall) Py_DECREF(compiledRunExprCall);

		if(runExpr.length()>0){
			// transform expression to function definition
			runExprAsDef="def runExpr():\n"+runExpr;
			size_t pos=0; while((pos=runExprAsDef.find('\n',pos))!=string::npos){runExprAsDef.replace(pos,1,"\n\t"); pos+=2;}

			if(PyRun_SimpleString(runExprAsDef.c_str())>=0){ // no error running the user-supplied code...
				// attempt psyco machine-code compilation of runExpr
				PyRun_SimpleString("try:\n\timport psyco\n\tpsyco.bind(runExpr)\nexcept ImportError: pass\n");
				// byte-compile the actual invocation of runExpr
				compiledRunExprCall=Py_CompileString("runExpr()","<runExpr definition>", Py_single_input);
				// mainDict is used later, when calling the byte-compiled piece
				PyObject* __main__=PyImport_AddModule("__main__"); // borrowed reference, no need to Py_DECREF
				mainDict=PyModule_GetDict(__main__); // borrowed reference as well
			}
			else {LOG_WARN("Compiling runExpr failed in Python, engine will not do anything! (expression was:\n"<<runExprAsDef);}
		}
	PyGILState_Release(pyState);
}

void PythonRecorder::action(MetaBody *rootBody)
{
	if(compiledRunExprCall){
		PyGILState_STATE pyState = PyGILState_Ensure();
			PyObject* res=PyEval_EvalCode((PyCodeObject*)compiledRunExprCall,mainDict,mainDict);
			if(res) Py_DECREF(res);
			else { LOG_WARN("runExpr() execution in Python failed?!"); }
		PyGILState_Release(pyState);
	}
}



YADE_PLUGIN();
