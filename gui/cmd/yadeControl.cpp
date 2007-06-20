#include<sstream>
#include<map>
#include<vector>
#include<unistd.h>
#include<list>

#include<boost/python.hpp>
#include<boost/python/suite/indexing/vector_indexing_suite.hpp>
#include<boost/bind.hpp>
#include<boost/thread/thread.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
#include<boost/any.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/python.hpp>

#include<yade/lib-base/Logging.hpp>
#include<yade/lib-serialization-xml/XMLFormatManager.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/gui-qt3/GLViewer.hpp>

#include<qapplication.h>
// qt3 sucks
#undef DEBUG

using namespace boost;
using namespace std;

#include"attrUtils.cpp"

class RenderingEngine;

class pyOmega{
	public:
	#define OMEGA Omega::instance()
	pyOmega(){};

	long iter(){ return OMEGA.getCurrentIteration();}
	double simulationTime(){return OMEGA.getSimulationTime();}
	double realTime(){
		posix_time::time_duration duration=microsec_clock::local_time()-OMEGA.getMsStartingSimulationTime();
		duration-=OMEGA.getSimulationPauseDuration();
		return double(duration.total_seconds())+double(duration.total_milliseconds()%1000)/1000; // FIXME: fractional_seconds are usec or msec or compile-time dependent?! documentation not clear on that.
	}
	// long realTime(){return OMEGA(get...);}
	double dt_get(){return OMEGA.getTimeStep();}
	void dt_set(double dt){OMEGA.setTimeStep(dt);}

	void run(){OMEGA.startSimulationLoop(); cerr<<"RUN!"<<endl;}
	// must stop, then reload
	//void _stop(){OMEGA(resetSimulationLoop());}
	void pause(){OMEGA.stopSimulationLoop(); cerr<<"PAUSE!"<<endl;}
	void step() {OMEGA.spawnSingleSimulationLoop(); cerr<<"STEP!"<<endl;}

	void load(std::string fileName) {
		OMEGA.finishSimulationLoop();
		OMEGA.joinSimulationLoop();
		OMEGA.setSimulationFileName(fileName);
		OMEGA.loadSimulation();
		OMEGA.createSimulationLoop();
		cerr<<"LOAD!"<<endl;
	}
	#undef OMEGA
};

/*! Generic preprocessor (FileGenerator) wrapper, with complete attribute access. */
class pyPreprocessor{
	shared_ptr<FileGenerator> gen;
	shared_ptr<AttrAccess> accessor;
	void ensureGen(void){
		if(!gen || gen->getClassName()!=generator){ // first time or changed generator; shared_ptr should delete the old object if not referenced anymore
			gen=static_pointer_cast<FileGenerator>(ClassFactory::instance().createShared(generator));
			accessor=shared_ptr<AttrAccess>(new AttrAccess(gen));
		}
	}
	public:
		pyPreprocessor(){serializer="XMLFormatManager"; output="../data/scene.xml";}
		std::string generator;
		std::string serializer;
		std::string output;
		//! return list of available generators
		boost::python::list listGenerators(){
			boost::python::list ret;
			for(map<string,DynlibDescriptor>::const_iterator di=Omega::instance().getDynlibsDescriptor().begin();di!=Omega::instance().getDynlibsDescriptor().end();++di) if (Omega::instance().isInheritingFrom((*di).first,"FileGenerator")) ret.append(di->first);
			return ret;
		}
		//! return list of available serializers
		boost::python::list listSerializers(){
			//vector<string> ret;
			boost::python::list ret;
			for(map<string,DynlibDescriptor>::const_iterator di=Omega::instance().getDynlibsDescriptor().begin();di!=Omega::instance().getDynlibsDescriptor().end();++di) if (Omega::instance().isInheritingFrom((*di).first,"IOFormatManager")) ret.append(di->first);
			return ret;
		}
		//! run generator
		bool generate(){
			#define _CHECK_NONEMPTY(s) if(s.empty()) throw std::runtime_error("`" #s "' is empty.")
				_CHECK_NONEMPTY(generator); _CHECK_NONEMPTY(serializer);	_CHECK_NONEMPTY(output);
			#undef _CHECK_NONEMPTY
			ensureGen();
			gen->setFileName(output);
			gen->setSerializationLibrary(serializer);
			bool ret=gen->generateAndSave();
			cerr<<(ret?"SUCCESS:\n":"FAILURE:\n")<<gen->message<<endl;
			return ret;
		}
		ATTR_ACCESS_CXX(accessor,ensureGen);
};
		

/*! GL viewer wrapper, with full attribute access.
 *
 * Creates the 3D view on instantiation. Currently displays nothing (why???), although it redraws just fine.
 * Has many bugs: multiple views will lead to crash, explicit delete crashes (somewhere in qt) as well.
 */
class pyGLViewer{
	//DECLARE_LOGGER;
	shared_ptr<RenderingEngine> renderer;
	shared_ptr<GLViewer> viewer;
	shared_ptr<boost::thread> redrawThread,appThread;
	QApplication* app;
	void redrawAlarm(void){
		while(true){viewer->updateGL(); usleep(50000);}
	}
	shared_ptr<AttrAccess> accessor;
	void ensureAcc(){if(!accessor)accessor=shared_ptr<AttrAccess>(new AttrAccess(renderer));}
public:
	DECLARE_LOGGER;
	ATTR_ACCESS_CXX(accessor,ensureAcc);	
	pyGLViewer(){
		shared_ptr<Factorable> _renderer=ClassFactory::instance().createShared("OpenGLRenderingEngine");
		renderer=static_pointer_cast<RenderingEngine>(_renderer);

		/* this is broken:
			Type of instance is: 15RenderingEngine
			RuntimeError: Cannot determine type with findType()
		*/
		if(renderer){// TODO: handle exceptions
			filesystem::path rendererConfig=filesystem::path(Omega::instance().yadeConfigPath+"/OpenGLRendererPref.xml");
			if(filesystem::exists(rendererConfig)){
				try{IOFormatManager::loadFromFile("XMLFormatManager",rendererConfig.string(),"renderer",renderer);}
				catch(SerializableError& e){LOG_WARN("Unable to load renderer preferences from `"<<rendererConfig.string()<<"': "<<e.what());}
			}
		} else throw runtime_error("Unable to create renderer!");

		int viewId=0;
		if(viewId==0){	int _argc=0; char* _argv[]={"foo"}; app=new QApplication(_argc,_argv);}

		QGLFormat format;	QGLFormat::setDefaultFormat(format); format.setStencil(TRUE); format.setAlpha(TRUE);
		viewer=shared_ptr<GLViewer>(new GLViewer(viewId,renderer,format,0,0));
		viewer->centerScene();
		viewer->notMoving();

		if(viewId==0){
			app->setMainWidget(viewer.get());
			redrawThread=shared_ptr<boost::thread>(new boost::thread(boost::bind(&pyGLViewer::redrawAlarm,this)));
		}
		appThread=shared_ptr<boost::thread>(new boost::thread(boost::bind(&QApplication::exec,app)));
	}
	~pyGLViewer(){// redrawThread and appThread deleted by the descructor
		//viewer->close();
		//app->quit();
	}
};

CREATE_LOGGER(pyGLViewer);


BOOST_PYTHON_MODULE(yadeControl)
{
	/* http://mail.python.org/pipermail/c++-sig/2004-March/007025.html
	http://mail.python.org/pipermail/c++-sig/2004-March/007029.html

	UNUSED, deal with boost::python::list instead

	python::class_<std::vector<std::string> >("_vectSs")
		.def(python::vector_indexing_suite<std::vector<std::string>,true>());   */

	boost::python::class_<pyOmega>("Omega")
		.add_property("iter",&pyOmega::iter)
		.add_property("time",&pyOmega::simulationTime)
		.add_property("realtime",&pyOmega::realTime)
		.add_property("dt",&pyOmega::dt_get,&pyOmega::dt_set)
		//.add_property("timeStepper",&timeStepper_get,&timeStepper_set)
		.def("load",&pyOmega::load)
		.def("run",&pyOmega::run)
		.def("pause",&pyOmega::pause)
		.def("step",&pyOmega::step);

		#if 0
			.def("oneStep",&oneStep)
			.def("newView", &newView)
			.def("centerScene", &centerScene)
		#endif

	boost::python::class_<pyPreprocessor>("Preprocessor")
		.add_property("generator",&pyPreprocessor::generator,&pyPreprocessor::generator)
		.add_property("serializer",&pyPreprocessor::serializer,&pyPreprocessor::serializer)
		.add_property("output",&pyPreprocessor::output,&pyPreprocessor::output)
		.def("generate",&pyPreprocessor::generate)
		.def("listSerializers",&pyPreprocessor::listSerializers)
		.def("listGenerators",&pyPreprocessor::listGenerators)
		.ATTR_ACCESS_PY(pyPreprocessor);

	boost::python::class_<pyGLViewer>("View")
		.ATTR_ACCESS_PY(pyGLViewer);

	//scope().attr("iter")=OMEGA(getCurrentIteration());
	//scope().attr("time")=realTime(getSimulationTime());
	//scope().attr("verbose")=;
	//scope().attr("verbose") = verbose;

}

