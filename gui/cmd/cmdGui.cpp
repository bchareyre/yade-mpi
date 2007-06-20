//#include<Python.h>
#include<boost/thread/thread.hpp>
#include<boost/python.hpp>
#include<errno.h>

#include"cmdGui.hpp"

#include <X11/Xlib.h>

using namespace boost;

//void cmdlineThreadStart(){
//}

CREATE_LOGGER(cmdGui);

void cmdGui::help(){
	cerr<<" cmdGui (python console) frontend.\n\
\n\
	-h       print this help\n\
	-s file  run this python script before entering interactive prompt\n\
\n\
	Sample session:\n\
\n\
	>>> p=Preprocessor\n\
	>>> p.generator='Funnel'\n\
	>>> p['density']=2000\n\
	>>> p.output='/tmp/funnel.xml'\n\
	>>> p.generate()\n\
	>>> o=Omega()\n\
	>>> o.run()\n\
	>>> # ^D to exit\n\
";
}

int cmdGui::run(int argc, char *argv[]) {
	string runScript;
	
	int ch;
	while((ch=getopt(argc,argv,"hs:"))!=-1)
	switch(ch){
		case 'h': help(); return 1;
		case 's': runScript=string(optarg); break;
		default: break;
	}

	XInitThreads();

	PyGILState_STATE pyState = PyGILState_Ensure();
		// wrap those in python::handle<> ??
		PyRun_SimpleString("import sys, readline");
		PyRun_SimpleString("sys.path.insert(0,'" PREFIX "/lib/yade" SUFFIX "/extra');");
		PyRun_SimpleString("sys.path.insert(0,'" PREFIX "/lib/yade" SUFFIX "/gui');");
		//int status=PyRun_SimpleString("from pyade import *"); // pyade will import _pyade by itself
		//if(status){ LOG_ERROR("pyade import failed."); } else LOG_DEBUG("pyade imported.");
		PyRun_SimpleString("from yadeControl import *");

		if(!runScript.empty()){
			LOG_DEBUG("Will now run file `"<<runScript<<"'");
			FILE* runScriptFILE=fopen(runScript.c_str(),"r");
			if(runScriptFILE){
				PyRun_SimpleFile(runScriptFILE,runScript.c_str());
			}
			else{
				string strerr(strerror(errno));
				LOG_ERROR("Unable to open file `"<<runScript<<"':"<<strerr<<".");
			}
		}

	PyGILState_Release(pyState);

	//boost::thread cmdlineThread(&cmdlineThreadStart);
	//cmdlineThread.join();
	PyRun_InteractiveLoop(stdin,"<console>");

	return 0;
}

