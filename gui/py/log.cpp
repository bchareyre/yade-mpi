
#ifdef LOG4CXX

#include<boost/python.hpp>
#include<yade/lib-base/Logging.hpp>
#include<log4cxx/logmanager.h>
#include<string>
using namespace boost;

enum{ll_TRACE,ll_DEBUG,ll_INFO,ll_WARN,ll_ERROR,ll_FATAL};

void logSetLevel(std::string loggerName,int level){
	std::string fullName(loggerName.empty()?"yade":("yade."+loggerName));
	if(!log4cxx::LogManager::exists(fullName)) throw std::invalid_argument("No logger named `"+fullName+"'");
	log4cxx::LevelPtr l;
	switch(level){
		case ll_TRACE: l=log4cxx::Level::DEBUG; break;
		case ll_DEBUG: l=log4cxx::Level::DEBUG; break;
		case ll_INFO: l=log4cxx::Level::INFO; break;
		case ll_WARN: l=log4cxx::Level::WARN; break;
		case ll_ERROR: l=log4cxx::Level::ERROR; break;
		case ll_FATAL: l=log4cxx::Level::FATAL; break;
		default: throw std::invalid_argument("Unrecognized logging level "+lexical_cast<std::string>(level));
	}
	log4cxx::LogManager::getLogger("yade."+loggerName)->setLevel(l);
}

BOOST_PYTHON_MODULE(log){
	python::def("setLevel",logSetLevel,"Set minimum severity level (constants TRACE,DEBUG,INFO,WARN,ERROR,FATAL) for given logger\nleading 'yade.' will be appended automatically to the logger name; if logger is '', the root logger 'yade' will be operated on.");
	python::scope().attr("TRACE")=(int)ll_TRACE;
	python::scope().attr("DEBUG")=(int)ll_DEBUG;
	python::scope().attr("INFO")= (int)ll_INFO;
	python::scope().attr("WARN")= (int)ll_WARN;
	python::scope().attr("ERROR")=(int)ll_ERROR;
	python::scope().attr("FATAL")=(int)ll_FATAL;
}

#endif
