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

void cmdGui::execScript(string script){
	LOG_DEBUG("Python will now run file `"<<script<<"'.");
	FILE* scriptFILE=fopen(script.c_str(),"r");
	if(scriptFILE){
		PyRun_SimpleFile(scriptFILE,script.c_str());
	}
	else{
		string strerr(strerror(errno));
		LOG_ERROR("Unable to open file `"<<script<<"': "<<strerr<<".");
	}
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
	PyEval_InitThreads();

	PyGILState_STATE pyState = PyGILState_Ensure();

		#define PYTHON_DEFINE_STRING(pyName,cxxName) PyRun_SimpleString((string(pyName)+"='"+string(cxxName)+"'").c_str())
		// wrap those in python::handle<> ??
		PYTHON_DEFINE_STRING("yadePrefix",PREFIX);
		PYTHON_DEFINE_STRING("yadeSuffix",SUFFIX);
		PYTHON_DEFINE_STRING("yadeRunSimulation",Omega::instance().getSimulationFileName());
		PYTHON_DEFINE_STRING("yadeRunScript",runScript);
		#undef PYTHON_DEFINE_STRING
		execScript(PREFIX "/lib/yade" SUFFIX "/gui/cmdGuiInit.py");
		
		//PyRun_InteractiveLoop(stdin,"<console>");

	PyGILState_Release(pyState);

	//boost::thread cmdlineThread(&cmdlineThreadStart);
	//cmdlineThread.join();

	return 0;
}

