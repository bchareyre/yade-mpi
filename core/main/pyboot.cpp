#include<yade/core/Omega.hpp>
#include<yade/lib/base/Logging.hpp>

#include<signal.h>
#include<cstdlib>
#include<cstdio>
#include<iostream>
#include<string>
#include<stdexcept>

#include<boost/python.hpp>
#include<boost/filesystem/convenience.hpp>

#ifdef YADE_DEBUG
	void crashHandler(int sig){
	switch(sig){
		case SIGABRT:
		case SIGSEGV:
			signal(SIGSEGV,SIG_DFL); signal(SIGABRT,SIG_DFL); // prevent loops - default handlers
			cerr<<"SIGSEGV/SIGABRT handler called; gdb batch file is `"<<Omega::instance().gdbCrashBatch<<"'"<<endl;
			std::system((string("gdb -x ")+Omega::instance().gdbCrashBatch).c_str());
			raise(sig); // reemit signal after exiting gdb
			break;
		}
	}		
#endif

/* Initialize yade, loading given plugins */
void yadeInitialize(python::list& pp, const std::string& confDir){

	PyEval_InitThreads();

	Omega& O(Omega::instance());
	O.init();
	O.origArgv=NULL; O.origArgc=0; // not needed, anyway
	O.confDir=confDir;
	O.initTemps();
	#ifdef YADE_DEBUG
		ofstream gdbBatch;
		O.gdbCrashBatch=O.tmpFilename();
		gdbBatch.open(O.gdbCrashBatch.c_str()); gdbBatch<<"attach "<<lexical_cast<string>(getpid())<<"\nset pagination off\nthread info\nthread apply all backtrace\ndetach\nquit\n"; gdbBatch.close();
		signal(SIGABRT,crashHandler);
		signal(SIGSEGV,crashHandler);
	#endif
	vector<string> ppp; for(int i=0; i<python::len(pp); i++) ppp.push_back(python::extract<string>(pp[i]));
	Omega::instance().loadPlugins(ppp);
}
void yadeFinalize(){ Omega::instance().cleanupTemps(); }

BOOST_PYTHON_MODULE(boot){
	python::scope().attr("initialize")=&yadeInitialize;
	python::scope().attr("finalize")=&yadeFinalize; //,"Finalize yade (only to be used internally).")
}
