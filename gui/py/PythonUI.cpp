//#include<Python.h>
#include<boost/thread/thread.hpp>
#include<boost/python.hpp>
#include<boost/algorithm/string.hpp>
#include<errno.h>

#include<stdlib.h>

#include"PythonUI.hpp"

#ifdef YADE_OPENGL
	#include <X11/Xlib.h>
#endif

using namespace boost;

struct termios PythonUI::tios, PythonUI::tios_orig;
string PythonUI::runScript;
bool PythonUI::stopAfter=false;
bool PythonUI::nonInteractive=false;
vector<string> PythonUI::scriptArgs;

PythonUI* PythonUI::self=NULL;

CREATE_LOGGER(PythonUI);

void PythonUI::help(){
	cerr<<" PythonUI (python console) frontend.\n\
\n\
	-h       help\n\
	-s file  run this python script before entering interactive prompt\n\
	-x       quit after running the script\n\
	-n       non-interactive (no prompt)\n\
\nNon-option arguments (after options):\n\
	*.py                       run this script (shorthand for -s *.py)\n\
	*.xml *.xml.gz *.xml.bz2   open and run this simulation\n\
\n\
	remaining arguments are copied to yade.runtime.argv (no escaping done) \n\
";
}

int PythonUI::execScript(string script){
	LOG_DEBUG("Python will now run file `"<<script<<"'.");
	FILE* scriptFILE=fopen(script.c_str(),"r");
	if(scriptFILE){
		int ret=PyRun_SimpleFile(scriptFILE,script.c_str());
		return ret;
	}
	else{
		string strerr(strerror(errno));
		LOG_ERROR("Unable to open file `"<<script<<"': "<<strerr<<".");
		return -1;
	}
}

void PythonUI::termSetup(void){
	LOG_DEBUG("Setting terminal discipline (^C kills line, ^U does nothing)");
	tcgetattr(STDIN_FILENO,&PythonUI::tios);
	memcpy(&PythonUI::tios_orig,&PythonUI::tios,sizeof(struct termios));
	atexit(PythonUI::termRestore);
	tios.c_cc[VKILL]=tios.c_cc[VINTR]; // assign ^C what ^U normally does (delete line)
	tios.c_cc[VINTR] = 0; // disable sending SIGINT at ^C
	tcsetattr(STDIN_FILENO,TCSANOW,&PythonUI::tios);
	tcflush(STDIN_FILENO,TCIFLUSH);
}

void PythonUI::termRestore(void){
	LOG_DEBUG("Restoring terminal discipline.");
	tcsetattr(STDIN_FILENO,TCSANOW,&PythonUI::tios_orig);
}

void PythonUI::pythonSession(){
	/* In threaded ipython, receiving SIGINT from ^C leads to segfault (for reasons I don't know).
	 * Hence we remap ^C (keycode in c_cc[VINTR]) to killing the line (c_cc[VKILL]) and disable VINTR afterwards.
	 * The behavior is restored back by the PythonUI::termRestore registered with atexit.
	 * */
	if(getenv("TERM") && !nonInteractive) termSetup();
	else nonInteractive=true;

	LOG_DEBUG("PythonUI::pythonSession");
	PyGILState_STATE pyState = PyGILState_Ensure();
		LOG_DEBUG("Got Global Interpreter Lock, good.");
		/* import yade (for startUI()) and yade.runtime (initially empty) namespaces */
		string prefix=getenv("YADE_PREFIX")?getenv("YADE_PREFIX"):PREFIX;
		PyRun_SimpleString(("import sys; sys.path.insert(0,'"+prefix+"/lib/yade" SUFFIX "/py')").c_str());
		PyRun_SimpleString("import yade");
		PyRun_SimpleString("from __future__ import division");

		#define PYTHON_DEFINE_STRING(pyName,cxxName) PyRun_SimpleString((string("yade.runtime." pyName "='")+cxxName+"'").c_str())
		#define PYTHON_DEFINE_BOOL(pyName,cxxName) PyRun_SimpleString((string("yade.runtime." pyName "=")+(cxxName?"True":"False")).c_str())
			// wrap those in python::handle<> ??
			PYTHON_DEFINE_STRING("prefix",prefix);
			PYTHON_DEFINE_STRING("suffix",SUFFIX);
			PYTHON_DEFINE_STRING("executable",Omega::instance().origArgv[0]);
			PYTHON_DEFINE_STRING("simulation",Omega::instance().getSimulationFileName());
			PYTHON_DEFINE_STRING("script",runScript);
			PYTHON_DEFINE_BOOL("stopAfter",stopAfter);
			PYTHON_DEFINE_BOOL("nonInteractive",nonInteractive);
			{ ostringstream oss; oss<<"yade.runtime.argv=["; if(scriptArgs.size()>0){ FOREACH(string s, scriptArgs) oss<<"'"<<s<<"',"; } oss<<"]"; PyRun_SimpleString(oss.str().c_str()); }
		#undef PYTHON_DEFINE_STRING
		#undef PYTHON_DEFINE_BOOL
		int ret=execScript((prefix+"/lib/yade" SUFFIX "/gui/PythonUI_rc.py").c_str());
		if(ret!=0){ LOG_FATAL("Error executing PythonUI_rc.py, aborting! Please report bug."); abort(); }
	PyGILState_Release(pyState);
}

int PythonUI::run(int argc, char *argv[]) {
	int ch;
	while((ch=getopt(argc,argv,"+hns:x"))!=-1){
		switch(ch){
			case 'h': help(); return 1; break;
			case 's': runScript=string(optarg); break;
			case 'x': stopAfter=true; break;
			case 'n': nonInteractive=true; break;
			default:
				LOG_WARN("Unhandled option string: `"<<string(optarg)<<"' (try -h for help on options)");
				break;
		}
	}
	if(optind<argc){ // process non-option arguments
		if(boost::algorithm::ends_with(string(argv[optind]),string(".py"))) runScript=string(argv[optind]);
		else if(boost::algorithm::ends_with(string(argv[optind]),string(".xml")) ||
			boost::algorithm::ends_with(string(argv[optind]),string(".xml.gz")) ||
			boost::algorithm::ends_with(string(argv[optind]),string(".xml.bz2"))) Omega::instance().setSimulationFileName(string(argv[optind]));
		else optind--;
	}
	for (int index = optind+1; index<argc; index++) scriptArgs.push_back(argv[index]);
		
		// LOG_ERROR("Unprocessed non-option argument: `"<<argv[index]<<"'");

	/** thread setup **/
	#ifdef YADE_OPENGL
		XInitThreads();
	#endif
	PyEval_InitThreads();

	pythonSession();

	return 0;
}

