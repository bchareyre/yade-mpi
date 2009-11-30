#include<yade/core/Omega.hpp>
#include<yade/lib-base/Logging.hpp>

#include<signal.h>
#include<cstdlib>
#include<iostream>
#include<string>
#include<stdexcept>

#include<boost/python.hpp>

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

/* Initialize yade, scan given directories for plugins and load them */
void yadeInitialize(python::list& dd, bool gdb){
	#ifdef YADE_LOG4CXX
		#ifdef LOG4CXX_TRACE
			log4cxx::LevelPtr debugLevel=log4cxx::Level::getDebug(), infoLevel=log4cxx::Level::getInfo(), warnLevel=log4cxx::Level::getWarn();
		#else
			log4cxx::LevelPtr debugLevel=log4cxx::Level::DEBUG, infoLevel=log4cxx::Level::INFO, warnLevel=log4cxx::Level::WARN;
		#endif

		log4cxx::BasicConfigurator::configure();
		log4cxx::LoggerPtr localLogger=log4cxx::Logger::getLogger("yade");
		localLogger->setLevel(getenv("YADE_DEBUG")?debugLevel:warnLevel);
	#endif

	#if defined(YADE_OPENMP) || defined(YADE_OPENGL)
		//LOG_ERROR("Yade compiled with openmp/opengl. Using python main will likely crash as soone as an ostream is used.")
	#endif

	PyEval_InitThreads();

	Omega& O(Omega::instance());
	O.init();
	O.origArgv=NULL; O.origArgc=0;
	O.initTemps();
	#ifdef YADE_DEBUG
		if(gdb){
			ofstream gdbBatch;
			O.gdbCrashBatch=O.tmpFilename();
			gdbBatch.open(O.gdbCrashBatch.c_str()); gdbBatch<<"attach "<<lexical_cast<string>(getpid())<<"\nset pagination off\nthread info\nthread apply all backtrace\ndetach\nquit\n"; gdbBatch.close();
			signal(SIGABRT,crashHandler);
			signal(SIGSEGV,crashHandler);
		}
	#endif
	vector<string> dd2; for(int i=0; i<python::len(dd); i++) dd2.push_back(python::extract<string>(dd[i]));
	Omega::instance().scanPlugins(dd2);
}
void yadeFinalize(){ Omega::instance().cleanupTemps(); }

BOOST_PYTHON_MODULE(boot){
	// FIXME: still a crasher with openmp or OpenGL...
	// cerr<<"[boot]"<<endl;
	python::scope().attr("initialize")=&yadeInitialize;
	python::scope().attr("finalize")=&yadeFinalize; //,"Finalize yade (only to be used internally).")
}
