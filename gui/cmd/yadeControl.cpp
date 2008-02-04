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

#include<yade/core/MetaDispatchingEngine.hpp>
#include<yade/core/StandAloneEngine.hpp>
#include<yade/core/DeusExMachina.hpp>
#include<yade/core/EngineUnit.hpp>

#include<qapplication.h>
// qt3 sucks
#undef DEBUG

using namespace boost;
using namespace std;

#include"attrUtils.cpp"

class RenderingEngine;

/* TODO:
 * have engine constructors that take engine name as first argument (instead of the .clss('...') method)
 */

class pyEngineUnit{
		shared_ptr<AttrAccess> accessor;
	public:	
		void ensureEng(void){ if(!eng) throw runtime_error("No proxied EngineUnit."); if(!accessor) accessor=shared_ptr<AttrAccess>(new AttrAccess(eng));  }
		shared_ptr<EngineUnit> eng;
		vector<string> bases; // names of classes for which we dispatch
		pyEngineUnit(const shared_ptr<EngineUnit>& _eng, const vector<string>& _bases): eng(_eng), bases(_bases) {}
		pyEngineUnit(string clss, string base1="", string base2=""){
			eng=dynamic_pointer_cast<EngineUnit>(ClassFactory::instance().createShared(clss)); if(!eng) throw runtime_error("Invalid engine class `"+clss+"': either nonexistent, or not unable to cast to `EngineUnit'");
			if(!base1.empty()){ bases.push_back(base1); if(!base2.empty()) bases.push_back(base2); }
		}
		std::string pyStr(void){ ensureEng(); string ret("<"+eng->getClassName()+" EngineUnit {"); for(size_t i=0; i<bases.size(); i++) ret+=bases[i]+(i<bases.size()-1?",":""); return ret+"}>"; }
		ATTR_ACCESS_CXX(accessor,ensureEng);
};

class pyEngine{
	private:
		shared_ptr<AttrAccess> accessor;
	public:	
		void ensureEng(void){ if(!eng) throw runtime_error("No proxied Engine."); if(!accessor) accessor=shared_ptr<AttrAccess>(new AttrAccess(eng)); }
		shared_ptr<Engine> eng;
		//pyEngine(void){throw;}
		pyEngine(const shared_ptr<Engine>& _eng): eng(_eng) {}
		pyEngine(string clss){ eng=dynamic_pointer_cast<Engine>(ClassFactory::instance().createShared(clss)); if(!eng) throw runtime_error("Invalid engine class `"+clss+"': either nonexistent, or not unable to cast to `Engine'"); }
		virtual std::string pyStr(void){ throw; } 
		ATTR_ACCESS_CXX(accessor,ensureEng);
};

class pyStandAloneEngine: public pyEngine{
	public:
		pyStandAloneEngine(const shared_ptr<StandAloneEngine>& _eng): pyEngine(_eng) {}
		pyStandAloneEngine(string clss): pyEngine(clss) {};
		std::string pyStr(void){ ensureEng(); return string("<")+eng->getClassName()+" StandAloneEngine>"; }
};

class pyDeusExMachina: public pyEngine{
	public:
		pyDeusExMachina(const shared_ptr<DeusExMachina>& _eng): pyEngine(_eng) {}
		pyDeusExMachina(string clss): pyEngine(clss) {};
		std::string pyStr(void){ ensureEng(); return string("<")+eng->getClassName()+" DeusExMachina>"; }
};

class pyMetaEngine: public pyEngine{
	public:
		pyMetaEngine(const shared_ptr<MetaDispatchingEngine>& _eng): pyEngine(_eng) {}
		pyMetaEngine(string clss): pyEngine(clss) {};
		//pyMetaEngine(string clss){ eng=dynamic_pointer_cast<MetaDispatchingEngine>(ClassFactory::instance().createShared(clss)); if(!eng) throw runtime_error("Invalid engine class `"+clss+"': either nonexistent, or not unable to cast to `MetaDispatchingEngine'"); };
		std::string pyStr(void){ ensureEng(); return string("<")+eng->getClassName()+" MetaEngine>"; }
		python::list functors_get(void){
			ensureEng(); shared_ptr<MetaDispatchingEngine> me=dynamic_pointer_cast<MetaDispatchingEngine>(eng); if(!me) throw runtime_error("Proxied class not a MetaDispatchingEngine (FIXME: add checks)"); python::list ret;
			/* garbage design: functorArguments are instances of EngineUnits, but they may not be present; therefore, only use them if they exist; our pyMetaEngine, however, will always have both names and EnguneUnit objects in the same count */
			for(size_t i=0; i<me->functorNames.size(); i++){
				shared_ptr<EngineUnit> eu;
				string functorName(*(me->functorNames[i].rbegin()));
				if(i<=me->functorArguments.size()){ /* count i-th list member */ size_t j=0;
					for(list<shared_ptr<EngineUnit> >::iterator I=me->functorArguments.begin(); I!=me->functorArguments.end(); I++, j++) { if(j==i) { eu=(*I); break;}}
				}
				if(!eu) /* either list was shorter or empty pointer in the functorArguments list */ { eu=dynamic_pointer_cast<EngineUnit>(ClassFactory::instance().createShared(functorName)); if(!eu) throw runtime_error("Unable to construct `"+string(*(me->functorNames[i].rbegin()))+"' EngineUnit"); }
				assert(eu);
				vector<string> fn; for(size_t j=0; j<me->functorNames[i].size()-1; j++) fn.push_back(me->functorNames[i][j]); // all names but the last one, which is the functor class name; TODO: use std::algo for the copy
				ret.append(pyEngineUnit(eu,fn));
			}
			return ret;
		}
		void functors_set(python::object ftrs){
			ensureEng(); shared_ptr<MetaDispatchingEngine> me=dynamic_pointer_cast<MetaDispatchingEngine>(eng); if(!me) throw runtime_error("Proxied class not a MetaDispatchingEngine (FIXME: add checks)");
			me->clear(); int len=python::len(ftrs);
			for(int i=0; i<len; i++){
				const pyEngineUnit& eu=python::extract<pyEngineUnit>(PySequence_GetItem(ftrs.ptr(),i));
				switch(eu.bases.size()){
					case 1: me->add(eu.bases[0],eu.eng->getClassName(),eu.eng);break;
					case 2: me->add(eu.bases[0],eu.bases[1],eu.eng->getClassName(),eu.eng);break;
					default: throw runtime_error("Unhandled number ("+lexical_cast<string>(eu.bases.size())+", must be 1 or 2) of base classes for functor.");
				}
			}
		}
};




class pyOmega{
	#define OMEGA Omega::instance()
	private:
		void assertRootBody(){if(!OMEGA.getRootBody()) throw std::runtime_error("No root body."); }
	public:
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
	void dt_set(double dt){OMEGA.skipTimeStepper(true); OMEGA.setTimeStep(dt);}
	bool usesTimeStepper_get(){return OMEGA.timeStepperActive();}
	void usesTimeStepper_set(bool use){OMEGA.skipTimeStepper(!use);}

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

	void save(std::string fileName){
		assertRootBody();
		OMEGA.saveSimulation(fileName);
		cerr<<"SAVE!"<<endl;
	}

	python::list engines_get(void){
		assertRootBody(); python::list ret; const shared_ptr<MetaBody>& rootBody=OMEGA.getRootBody();
		for(vector<shared_ptr<Engine> >::iterator I=rootBody->engines.begin(); I!=rootBody->engines.end(); ++I){
			#define APPEND_ENGINE_IF_POSSIBLE(engineType,pyEngineType) { shared_ptr<engineType> e=dynamic_pointer_cast<engineType>(*I); if(e) { ret.append(pyEngineType(e)); continue; } }
			APPEND_ENGINE_IF_POSSIBLE(MetaDispatchingEngine,pyMetaEngine); APPEND_ENGINE_IF_POSSIBLE(StandAloneEngine,pyStandAloneEngine); APPEND_ENGINE_IF_POSSIBLE(DeusExMachina,pyDeusExMachina);
			throw std::runtime_error("Unknown engine type: `"+(*I)->getClassName()+"' (only MetaDispatchingEngine, StandAloneEngine and DeusExMachina are supported)");
		}
		return ret;
	}

	void engines_set(python::object egs){
		assertRootBody(); int len=python::len(egs); const shared_ptr<MetaBody>& rootBody=OMEGA.getRootBody(); rootBody->engines.clear();
		for(int i=0; i<len; i++){
			#define PUSH_BACK_ENGINE_IF_POSSIBLE(pyEngineType) if(python::extract<pyEngineType>(PySequence_GetItem(egs.ptr(),i)).check()){ pyEngineType e=python::extract<pyEngineType>(PySequence_GetItem(egs.ptr(),i)); rootBody->engines.push_back(e.eng); /* cerr<<"added "<<e.pyStr()<<", a "<<#pyEngineType<<endl; */ continue; }
			PUSH_BACK_ENGINE_IF_POSSIBLE(pyStandAloneEngine); PUSH_BACK_ENGINE_IF_POSSIBLE(pyMetaEngine); PUSH_BACK_ENGINE_IF_POSSIBLE(pyDeusExMachina);
			throw std::runtime_error("Encountered unknown engine type (unable to extract from python object)");
		}
	}

	//void join(){cerr<<"JOIN!"<<endl; OMEGA.joinSimulationLoop();}
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
		while(true){
			Omega::instance().stopSimulationLoop();
			viewer->updateGL();
			Omega::instance().startSimulationLoop();
			usleep(10000000);
		}
	}
	shared_ptr<AttrAccess> accessor;
	void ensureAcc(){if(!accessor)accessor=shared_ptr<AttrAccess>(new AttrAccess(renderer));}
public:
	DECLARE_LOGGER;
	ATTR_ACCESS_CXX(accessor,ensureAcc);	
	pyGLViewer(){
		throw std::runtime_error("Programming error: Threading in pyGLViewer is broken and crashes; ignored.");
		// LOG_WARN("Thread locking not correctly implemented, will pause Omega for redraw every 10s instead!");
		shared_ptr<Factorable> _renderer=ClassFactory::instance().createShared("OpenGLRenderingEngine");
		renderer=static_pointer_cast<RenderingEngine>(_renderer);

		/* this is broken:
			Type of instance is: 15RenderingEngine
			RuntimeError: Cannot determine type with findType()
		*/
		#if 0
		if(renderer){// TODO: handle exceptions
			filesystem::path rendererConfig=filesystem::path(Omega::instance().yadeConfigPath+"/OpenGLRendererPref.xml");
			if(filesystem::exists(rendererConfig)){
				try{IOFormatManager::loadFromFile("XMLFormatManager",rendererConfig.string(),"renderer",renderer);}
				catch(SerializableError& e){LOG_WARN("Unable to load renderer preferences from `"<<rendererConfig.string()<<"': "<<e.what());}
			}
		} else throw runtime_error("Unable to create renderer!");
		#endif
		if(!renderer) throw runtime_error("Unable to create renderer!");

		int viewId=0;
		if(viewId==0){	int _argc=0; char _argvv[]="foo"; app=new QApplication(_argc,(char**) &_argvv);}

		QGLFormat format;	QGLFormat::setDefaultFormat(format); // format.setStencil(TRUE); format.setAlpha(TRUE);
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
		.add_property("usesTimeStepper",&pyOmega::usesTimeStepper_get,&pyOmega::usesTimeStepper_set)
		.def("load",&pyOmega::load)
		.def("save",&pyOmega::save)
		.def("run",&pyOmega::run)
		// .def("join",&pyOmega::join)
		.def("pause",&pyOmega::pause)
		.def("step",&pyOmega::step)
		.add_property("engines",&pyOmega::engines_get,&pyOmega::engines_set)
		;

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

	
	/* FIXME: __str__, __repr__ etc should be inherited from one parent class in python; how to do that? */
	/* clss currently doesn't check whether StandAloneEngine is not MetaDispatchingEngine, for example. Fix this later. */
	python::class_<pyStandAloneEngine>("StandAloneEngine",python::init<string>())
	.ATTR_ACCESS_PY(pyStandAloneEngine)
	.def("__str__",&pyStandAloneEngine::pyStr).def("__repr__",&pyStandAloneEngine::pyStr)
	//.def("clss",&pyStandAloneEngine::pyInit);
	;

	python::class_<pyMetaEngine>("MetaEngine",python::init<string>())
	.ATTR_ACCESS_PY(pyMetaEngine)
	.def("__str__",&pyMetaEngine::pyStr).def("__repr__",&pyMetaEngine::pyStr)
	//.def("clss",&pyMetaEngine::pyInit)
	.add_property("functors",&pyMetaEngine::functors_get,&pyMetaEngine::functors_set)
	;

	boost::python::class_<pyDeusExMachina>("DeusExMachina",python::init<string>())
	.ATTR_ACCESS_PY(pyDeusExMachina)
	.def("__str__",&pyDeusExMachina::pyStr).def("__repr__",&pyDeusExMachina::pyStr)
	//.def("clss",&pyStandAloneEngine::pyInit);
	;

	boost::python::class_<pyEngineUnit>("EngineUnit",python::init<string, python::optional<string,string> >())
	.ATTR_ACCESS_PY(pyEngineUnit)
	.def("__str__",&pyEngineUnit::pyStr).def("__repr__",&pyEngineUnit::pyStr)
	// .def("clss",&pyEngineUnit::pyInit)
	.add_property("bases",&pyEngineUnit::bases,&pyEngineUnit::bases) // not yet functional
	;
	

	//scope().attr("iter")=OMEGA(getCurrentIteration());
	//scope().attr("time")=realTime(getSimulationTime());
	//scope().attr("verbose")=;
	//scope().attr("verbose") = verbose;

}

