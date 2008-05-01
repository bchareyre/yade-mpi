//#include<Python.h>
#include<boost/thread/thread.hpp>
#include<boost/python.hpp>
#include<boost/algorithm/string.hpp>
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
	/* -c cmd   run python command (may be specified multiple times, newline is appended every time) */ \n\
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
	string runCommands;
	bool stopAfter=false;
	
	int ch;
	while((ch=getopt(argc,argv,"hs:"))!=-1)
	switch(ch){
		case 'h': help(); return 1;
		case 's': runScript=string(optarg); break;
		case 'x': stopAfter=true; break;
		//case 'c': runCommands+=string(optarg)+"\n"; break;
		default:
			LOG_ERROR("Unhandled option string: `"<<string(optarg)<<"' (try -h for help on options)");
			break;
	}
	if(optind<argc){ // process non-option arguments
		if(boost::algorithm::ends_with(string(argv[optind]),string(".py"))) runScript=string(argv[optind]);
		else if(boost::algorithm::ends_with(string(argv[optind]),string(".xml"))) Omega::instance().setSimulationFileName(string(argv[optind]));
		else optind--;
	}
	for (int index = optind+1; index<argc; index++) LOG_ERROR("Unprocessed non-option argument: `"<<argv[index]<<"'");


	XInitThreads();
	PyEval_InitThreads();

	PyGILState_STATE pyState = PyGILState_Ensure();

		// this is needed to create the yade.runtime namespace
		PyRun_SimpleString("import sys; sys.path.insert(0,'" PREFIX "/lib/yade" SUFFIX "/gui')");
		PyRun_SimpleString("import yade.runtime");

		#define PYTHON_DEFINE_STRING(pyName,cxxName) PyRun_SimpleString((string("yade.runtime." pyName "='")+cxxName+"'").c_str())
		#define PYTHON_DEFINE_BOOL(pyName,cxxName) PyRun_SimpleString((string("yade.runtime." pyName "=")+(cxxName?"True":"False")).c_str())
		// wrap those in python::handle<> ??
		PYTHON_DEFINE_STRING("prefix",PREFIX);
		PYTHON_DEFINE_STRING("suffix",SUFFIX);
		PYTHON_DEFINE_STRING("executable",Omega::instance().origArgv[0]);
		PYTHON_DEFINE_STRING("simulation",Omega::instance().getSimulationFileName());
		PYTHON_DEFINE_STRING("script",runScript);
		PYTHON_DEFINE_STRING("commands",runCommands);
		PYTHON_DEFINE_BOOL("stopAfter",stopAfter);
		#undef PYTHON_DEFINE_STRING
		#undef PYTHON_DEFINE_BOOL
		execScript(PREFIX "/lib/yade" SUFFIX "/gui/cmdGuiInit.py");
		
		//PyRun_InteractiveLoop(stdin,"<console>");

	PyGILState_Release(pyState);

	//boost::thread cmdlineThread(&cmdlineThreadStart);
	//cmdlineThread.join();

	return 0;
}

