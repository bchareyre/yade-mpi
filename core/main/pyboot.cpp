#include<yade/core/Omega.hpp>
#include<yade/lib-base/Logging.hpp>

#include<signal.h>
#include<cstdlib>
#include<cstdio>
#include<iostream>
#include<string>
#include<stdexcept>

#include<boost/python.hpp>

#ifdef YADE_LOG4CXX
	#include<log4cxx/consoleappender.h>
	#include<log4cxx/patternlayout.h>
	log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.boot");
	/* initialize here so that ClassFactory can use log4cxx without warnings */
	__attribute__((constructor)) void initLog4cxx() {
		#ifdef LOG4CXX_TRACE
			log4cxx::LevelPtr debugLevel=log4cxx::Level::getDebug(), infoLevel=log4cxx::Level::getInfo(), warnLevel=log4cxx::Level::getWarn();
			// LOG4CXX_STR: http://old.nabble.com/Link-error-when-using-Layout-on-MS-Windows-td20906802.html
			log4cxx::LayoutPtr layout(new log4cxx::PatternLayout(LOG4CXX_STR("%-5r %-5p %-10c %m%n")));
			log4cxx::AppenderPtr appender(new log4cxx::ConsoleAppender(layout));
			log4cxx::LoggerPtr localLogger=log4cxx::Logger::getLogger("yade");
			localLogger->addAppender(appender);
		#else // log4cxx 0.9
			log4cxx::LevelPtr debugLevel=log4cxx::Level::DEBUG, infoLevel=log4cxx::Level::INFO, warnLevel=log4cxx::Level::WARN;
			log4cxx::BasicConfigurator::configure();
			log4cxx::LoggerPtr localLogger=log4cxx::Logger::getLogger("yade");
		#endif
		localLogger->setLevel(getenv("YADE_DEBUG")?debugLevel:warnLevel);
		LOG4CXX_DEBUG(localLogger,"Log4cxx initialized.");
	}
#endif

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
void yadeInitialize(python::list& pp){

	PyEval_InitThreads();

	Omega& O(Omega::instance());
	O.init();
	O.origArgv=NULL; O.origArgc=0; // not needed, anyway
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
