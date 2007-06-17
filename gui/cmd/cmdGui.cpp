#include<Python.h>
#include<boost/thread/thread.hpp>

#include"cmdGui.hpp"

#include <X11/Xlib.h>

//void cmdlineThreadStart(){
//}

CREATE_LOGGER(cmdGui);


int cmdGui::run(int argc, char *argv[]) {
#if 0
	opterr = 0;
	int ch;
	while((ch=getopt(argc,argv,"t:g:"))!=-1)
		switch(ch){
//			case 'H'	: help(); 						return 1;
			case 't'	: Omega::instance().setTimeStep
						(lexical_cast<Real>(optarg));			break;
//			case 'g'	: Omega::instance().setGravity
//						(Vector3r(0,-lexical_cast<Real>(optarg),0));	break;
			default		: break;
		}
#endif
	XInitThreads();

	// thread init

	PyGILState_STATE pyState = PyGILState_Ensure();	

		PyRun_SimpleString("import sys, readline");
		PyRun_SimpleString("sys.path.insert(0,'" PREFIX "/lib/yade" SUFFIX "/extra');");
		PyRun_SimpleString("sys.path.insert(0,'" PREFIX "/lib/yade" SUFFIX "/gui');");

		//int status=PyRun_SimpleString("from pyade import *"); // pyade will import _pyade by itself
		//if(status){ LOG_ERROR("pyade import failed."); } else LOG_DEBUG("pyade imported.");
		
		PyRun_SimpleString("from yadeControl import *");

	PyGILState_Release(pyState);

	//boost::thread cmdlineThread(&cmdlineThreadStart);
	//cmdlineThread.join();
	PyRun_InteractiveLoop(stdin,"<console>");

	return 0;
}

