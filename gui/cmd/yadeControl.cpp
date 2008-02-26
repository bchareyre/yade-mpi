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
// [boost1.34] #include<boost/python/stl_iterator.hpp>

#include<yade/lib-base/Logging.hpp>
#include<yade/lib-serialization-xml/XMLFormatManager.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/FileGenerator.hpp>


#include<yade/core/MetaDispatchingEngine.hpp>
#include<yade/core/StandAloneEngine.hpp>
#include<yade/core/DeusExMachina.hpp>
#include<yade/core/EngineUnit.hpp>
#include<yade/core/EngineUnit1D.hpp>
#include<yade/core/EngineUnit2D.hpp>

#ifndef NO_PYGLVIEWER
	#include<yade/gui-qt3/GLViewer.hpp>
	#include<qapplication.h>
	// qt3 sucks
	#undef DEBUG
#endif

using namespace boost;
using namespace std;

#include"attrUtils.cpp"

class RenderingEngine;

/*
TODO:
	1. InteractionContainer with iteration
	2. PhysicalActionContainer (constructor with actionName) with iteration
	3. from yadeControl import Omega as _Omega, inherit from that and add other convenience functions
*/

#define BASIC_PY_PROXY_HEAD(pyClass,yadeClass) \
class pyClass{shared_ptr<AttrAccess> accessor; \
	public: shared_ptr<yadeClass> proxee; \
		void ensureAcc(void){ if(!proxee) throw runtime_error(string("No proxied `")+#yadeClass+"'."); if(!accessor) accessor=shared_ptr<AttrAccess>(new AttrAccess(proxee));} \
		pyClass(string clss=""){proxee=dynamic_pointer_cast<yadeClass>(ClassFactory::instance().createShared(clss.empty()? #yadeClass : clss)); if(!proxee) throw runtime_error("Invalid class `"+clss+"': either nonexistent, or unable to cast to `"+#yadeClass+"'"); } \
		pyClass(const shared_ptr<yadeClass>& _proxee): proxee(_proxee) {} \
		std::string pyStr(void){ ensureAcc(); return string(proxee->getClassName()==#yadeClass ? "<"+proxee->getClassName()+">" : "<"+proxee->getClassName()+" "+ #yadeClass +">"); } \
		string className(void){ ensureAcc(); return proxee->getClassName(); } \
		ATTR_ACCESS_CXX(accessor,ensureAcc);

#define BASIC_PY_PROXY_TAIL };

#define BASIC_PY_PROXY(pyClass,yadeClass) BASIC_PY_PROXY_HEAD(pyClass,yadeClass) BASIC_PY_PROXY_TAIL


BASIC_PY_PROXY(pyInteractionGeometry,InteractionGeometry);
BASIC_PY_PROXY(pyInteractionPhysics,InteractionPhysics);

BASIC_PY_PROXY(pyGeometricalModel,GeometricalModel);
BASIC_PY_PROXY(pyPhysicalParameters,PhysicalParameters);
BASIC_PY_PROXY(pyBoundingVolume,BoundingVolume);
BASIC_PY_PROXY(pyInteractingGeometry,InteractingGeometry);

BASIC_PY_PROXY(pyStandAloneEngine,StandAloneEngine);
BASIC_PY_PROXY(pyDeusExMachina,DeusExMachina);

BASIC_PY_PROXY_HEAD(pyEngineUnit,EngineUnit)
	python::list bases_get(void){ python::list ret; vector<string> t=proxee->getFunctorTypes(); for(size_t i=0; i<t.size(); i++) ret.append(t[i]); return ret; }
BASIC_PY_PROXY_TAIL;


BASIC_PY_PROXY_HEAD(pyMetaEngine,MetaDispatchingEngine)
		python::list functors_get(void){
			ensureAcc(); shared_ptr<MetaDispatchingEngine> me=dynamic_pointer_cast<MetaDispatchingEngine>(proxee); if(!me) throw runtime_error("Proxied class not a MetaDispatchingEngine (WTF?)"); python::list ret;
			/* garbage design: functorArguments are instances of EngineUnits, but they may not be present; therefore, only use them if they exist; our pyMetaEngine, however, will always have both names and EnguneUnit objects in the same count */
			for(size_t i=0; i<me->functorNames.size(); i++){
				shared_ptr<EngineUnit> eu;
				string functorName(*(me->functorNames[i].rbegin()));
				if(i<=me->functorArguments.size()){ /* count i-th list member */ size_t j=0;
					for(list<shared_ptr<EngineUnit> >::iterator I=me->functorArguments.begin(); I!=me->functorArguments.end(); I++, j++) { if(j==i) { eu=(*I); break;}}
				}
				if(!eu) /* either list was shorter or empty pointer in the functorArguments list */ { eu=dynamic_pointer_cast<EngineUnit>(ClassFactory::instance().createShared(functorName)); if(!eu) throw runtime_error("Unable to construct `"+string(*(me->functorNames[i].rbegin()))+"' EngineUnit"); }
				assert(eu);
				ret.append(pyEngineUnit(eu));
			}
			return ret;
		}
		void functors_set(python::object ftrs){
			ensureAcc(); shared_ptr<MetaDispatchingEngine> me=dynamic_pointer_cast<MetaDispatchingEngine>(proxee); if(!me) throw runtime_error("Proxied class not a MetaDispatchingEngine. (WTF?)");
			me->clear(); int len=PySequence_Size(ftrs.ptr()) /*[boost1.34] python::len(ftrs)*/;
			for(int i=0; i<len; i++){ const pyEngineUnit& eu=python::extract<pyEngineUnit>(PySequence_GetItem(ftrs.ptr(),i)); me->add(eu.proxee); }
		}
BASIC_PY_PROXY_TAIL;

#define NONPOD_ATTRIBUTE_ACCESS(pyName,pyClass,yadeName) \
	python::object pyName##_get(void){ensureAcc(); return proxee->yadeName ? python::object(pyClass(proxee->yadeName)) : python::object(); } \
	void pyName##_set(pyClass proxy){ensureAcc(); proxee->yadeName=proxy.proxee; }

BASIC_PY_PROXY_HEAD(pyInteraction,Interaction)
	NONPOD_ATTRIBUTE_ACCESS(geom,pyInteractionGeometry,interactionGeometry);
	NONPOD_ATTRIBUTE_ACCESS(phys,pyInteractionPhysics,interactionPhysics);
	/* shorthands */ unsigned id1_get(void){ensureAcc(); return proxee->getId1();} unsigned id2_get(void){ensureAcc(); return proxee->getId2();}
BASIC_PY_PROXY_TAIL;

BASIC_PY_PROXY_HEAD(pyBody,Body)
	NONPOD_ATTRIBUTE_ACCESS(shape,pyGeometricalModel,geometricalModel);
	NONPOD_ATTRIBUTE_ACCESS(mold,pyInteractingGeometry,interactingGeometry);
	NONPOD_ATTRIBUTE_ACCESS(bound,pyBoundingVolume,boundingVolume);
	NONPOD_ATTRIBUTE_ACCESS(phys,pyPhysicalParameters,physicalParameters);
	unsigned id_get(){ensureAcc(); return proxee->getId();}
	bool isStandalone(){ensureAcc(); return proxee->isStandalone();} bool isClumpMember(){ensureAcc(); return proxee->isClumpMember();} bool isClump(){ensureAcc(); return proxee->isClump();}
BASIC_PY_PROXY_TAIL;

class pyBodyContainer{
	public:
	const shared_ptr<BodyContainer> proxee;
	pyBodyContainer(const shared_ptr<BodyContainer>& _proxee): proxee(_proxee){}
	pyBody pyGetitem(unsigned id){
		if(id>=proxee->size()){ PyErr_SetString(PyExc_IndexError, "Body id out of range."); python::throw_error_already_set(); /* make compiler happy; never reached */ return pyBody(); }
		else return pyBody(proxee->operator[](id));
	}
};

BASIC_PY_PROXY_HEAD(pyFileGenerator,FileGenerator)
	bool generate(string outFile){ensureAcc(); proxee->setFileName(outFile); proxee->setSerializationLibrary("XMLFormatManager"); bool ret=proxee->generateAndSave(); cerr<<(ret?"SUCCESS:\n":"FAILURE:\n")<<proxee->message<<endl; return ret; };
BASIC_PY_PROXY_TAIL;


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

	long stopAtIter_get(){return OMEGA.stopAtIteration; }
	void stopAtIter_set(long s){OMEGA.stopAtIteration=s; }

	bool usesTimeStepper_get(){return OMEGA.timeStepperActive();}
	void usesTimeStepper_set(bool use){OMEGA.skipTimeStepper(!use);}

	void run(long int numIter=-1){
		if(numIter>0) OMEGA.stopAtIteration=OMEGA.getCurrentIteration()+numIter;
		//else OMEGA.stopAtIteration=-1;
		OMEGA.startSimulationLoop();
		long toGo=OMEGA.stopAtIteration-OMEGA.getCurrentIteration();
		cerr<<"RUN"<<(toGo>0?string(" ("+lexical_cast<string>(toGo)+" to go)"):string(""))<<"!"<<endl;
	}
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
		assertRootBody(); int len=PySequence_Size(egs.ptr()) /*[boost1.34] python::len(egs)*/; const shared_ptr<MetaBody>& rootBody=OMEGA.getRootBody(); rootBody->engines.clear();
		for(int i=0; i<len; i++){
			#define PUSH_BACK_ENGINE_IF_POSSIBLE(pyEngineType) if(python::extract<pyEngineType>(PySequence_GetItem(egs.ptr(),i)).check()){ pyEngineType e=python::extract<pyEngineType>(PySequence_GetItem(egs.ptr(),i)); rootBody->engines.push_back(e.proxee); /* cerr<<"added "<<e.pyStr()<<", a "<<#pyEngineType<<endl; */ continue; }
			PUSH_BACK_ENGINE_IF_POSSIBLE(pyStandAloneEngine); PUSH_BACK_ENGINE_IF_POSSIBLE(pyMetaEngine); PUSH_BACK_ENGINE_IF_POSSIBLE(pyDeusExMachina);
			throw std::runtime_error("Encountered unknown engine type (unable to extract from python object)");
		}
	}

	//void join(){cerr<<"JOIN!"<<endl; OMEGA.joinSimulationLoop();}
	void wait(){ if(OMEGA.isRunning()){cerr<<"WAIT!"<<endl;} while(OMEGA.isRunning()) usleep(20000 /*20 ms*/); }
	
	pyBodyContainer bodies_get(void){assertRootBody(); return pyBodyContainer(OMEGA.getRootBody()->bodies); }

	boost::python::list listChildClasses(const string& base){
		boost::python::list ret;
		for(map<string,DynlibDescriptor>::const_iterator di=Omega::instance().getDynlibsDescriptor().begin();di!=Omega::instance().getDynlibsDescriptor().end();++di) if (Omega::instance().isInheritingFrom((*di).first,base)) ret.append(di->first);
		return ret;
	}
	#undef OMEGA
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(omega_overloads,run,0,1);

#ifndef NO_PYGLVIEWER
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
			//Omega::instance().stopSimulationLoop();
			viewer->updateGL();
			//Omega::instance().startSimulationLoop();
			usleep(10000000);
		}
	}
	shared_ptr<AttrAccess> accessor;
	void ensureAcc(){if(!accessor)accessor=shared_ptr<AttrAccess>(new AttrAccess(renderer));}
public:
	DECLARE_LOGGER;
	ATTR_ACCESS_CXX(accessor,ensureAcc);	
	pyGLViewer(){
		//throw std::runtime_error("Programming error: Threading in pyGLViewer is broken and crashes; ignored.");
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
#endif


BOOST_PYTHON_MODULE(yadeControl)
{
	/* http://mail.python.org/pipermail/c++-sig/2004-March/007025.html
	http://mail.python.org/pipermail/c++-sig/2004-March/007029.html

	UNUSED, deal with boost::python::list instead

	python::class_<std::vector<std::string> >("_vectSs")
		.def(python::vector_indexing_suite<std::vector<std::string>,true>());   */

	boost::python::class_<pyOmega>("Omega")
		.add_property("iter",&pyOmega::iter)
		.add_property("stopAtIter",&pyOmega::stopAtIter_get,&pyOmega::stopAtIter_set)
		.add_property("time",&pyOmega::simulationTime)
		.add_property("realtime",&pyOmega::realTime)
		.add_property("dt",&pyOmega::dt_get,&pyOmega::dt_set)
		.add_property("usesTimeStepper",&pyOmega::usesTimeStepper_get,&pyOmega::usesTimeStepper_set)
		.def("load",&pyOmega::load)
		.def("save",&pyOmega::save)
		.def("run",&pyOmega::run,omega_overloads())
		.def("pause",&pyOmega::pause)
		.def("step",&pyOmega::step)
		.def("wait",&pyOmega::wait)
		.add_property("engines",&pyOmega::engines_get,&pyOmega::engines_set)
		.add_property("bodies",&pyOmega::bodies_get)
		.def("childClasses",&pyOmega::listChildClasses)
		;
	
	boost::python::class_<pyBodyContainer>("BodyContainer",python::init<pyBodyContainer&>())
		.def("__getitem__",&pyBodyContainer::pyGetitem);

		#if 0
			.def("oneStep",&oneStep)
			.def("newView", &newView)
			.def("centerScene", &centerScene)
		#endif
#ifndef NO_PYGLVIEWER
	boost::python::class_<pyGLViewer>("View")
		.ATTR_ACCESS_PY(pyGLViewer);
#endif

#define BASIC_PY_PROXY_WRAPPER(pyClass,pyName)  \
	boost::python::class_<pyClass>(pyName,python::init<python::optional<string> >()) \
	.ATTR_ACCESS_PY(pyClass) \
	.def("__str__",&pyClass::pyStr).def("__repr__",&pyClass::pyStr) \
	.add_property("name",&pyClass::className)

	BASIC_PY_PROXY_WRAPPER(pyStandAloneEngine,"StandAloneEngine");
	BASIC_PY_PROXY_WRAPPER(pyMetaEngine,"MetaEngine")
		.add_property("functors",&pyMetaEngine::functors_get,&pyMetaEngine::functors_set);
	BASIC_PY_PROXY_WRAPPER(pyDeusExMachina,"DeusExMachina");
	BASIC_PY_PROXY_WRAPPER(pyEngineUnit,"EngineUnit")
		.add_property("bases",&pyEngineUnit::bases_get);

	BASIC_PY_PROXY_WRAPPER(pyGeometricalModel,"Shape");
	BASIC_PY_PROXY_WRAPPER(pyInteractingGeometry,"Mold");
	BASIC_PY_PROXY_WRAPPER(pyPhysicalParameters,"Phys");
	BASIC_PY_PROXY_WRAPPER(pyBoundingVolume,"Bound");
	BASIC_PY_PROXY_WRAPPER(pyInteractionGeometry,"InterGeom");
	BASIC_PY_PROXY_WRAPPER(pyInteractionPhysics,"InterPhys");

	BASIC_PY_PROXY_WRAPPER(pyBody,"Body")
		.add_property("shape",&pyBody::shape_get,&pyBody::shape_set)
		.add_property("mold",&pyBody::mold_get,&pyBody::mold_set)
		.add_property("bound",&pyBody::bound_get,&pyBody::bound_set)
		.add_property("phys",&pyBody::phys_get,&pyBody::phys_set)
		.add_property("id",&pyBody::id_get)
		.add_property("isStandalone",&pyBody::isStandalone)
		.add_property("isClumpMember",&pyBody::isClumpMember)
		.add_property("isClump",&pyBody::isClump);

	BASIC_PY_PROXY_WRAPPER(pyInteraction,"Interaction")
		.add_property("phys",&pyInteraction::phys_get,&pyInteraction::phys_set)
		.add_property("geom",&pyInteraction::geom_get,&pyInteraction::geom_set);

	BASIC_PY_PROXY_WRAPPER(pyFileGenerator,"Preprocessor")
		.def("generate",&pyFileGenerator::generate);

}

