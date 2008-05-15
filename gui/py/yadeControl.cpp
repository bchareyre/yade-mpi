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
#include<boost/foreach.hpp>
// [boost1.34] #include<boost/python/stl_iterator.hpp>

#include<yade/lib-base/Logging.hpp>
#include<yade/lib-serialization-xml/XMLFormatManager.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/FileGenerator.hpp>

#include<yade/lib-import/STLImporter.hpp>

#include<yade/core/MetaEngine.hpp>
#include<yade/core/MetaEngine1D.hpp>
#include<yade/core/MetaEngine2D.hpp>
#include<yade/core/StandAloneEngine.hpp>
#include<yade/core/DeusExMachina.hpp>
#include<yade/core/EngineUnit.hpp>
#include<yade/core/EngineUnit1D.hpp>
#include<yade/core/EngineUnit2D.hpp>

#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/GeometricalModelMetaEngine.hpp>
#include<yade/pkg-common/InteractingGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>

#include<yade/pkg-common/BoundingVolumeEngineUnit.hpp>
#include<yade/pkg-common/GeometricalModelEngineUnit.hpp>
#include<yade/pkg-common/InteractingGeometryEngineUnit.hpp>
#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>
#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>
#include<yade/pkg-common/PhysicalParametersEngineUnit.hpp>
#include<yade/pkg-common/PhysicalActionDamperUnit.hpp>
#include<yade/pkg-common/PhysicalActionApplierUnit.hpp>

#include<yade/extra/Shop.hpp>

#ifdef USE_PYGLVIEWER
	#include"GLViewer4.hpp"
	#include<Qt/qapplication.h>
	#include<Qt/qthread.h>
#endif

using namespace boost;
using namespace std;

#include"attrUtils.cpp"

class RenderingEngine;

/*!
	
	A regular class (not Omega) is instantiated like this:

		RootClass('optional class name as quoted string',{optional dictionary of attributes})
		
	if class name is not given, the RootClass itself is instantiated

		p=PhysicalParameters() # p is now instance of PhysicalParameters
		p=PhysicalParameters('RigidBodyParameters') # p is now instance of RigidBodyParameters, which has PhysicalParameters as the "root" class
		p=PhysicalParameters('RigidBodyParameters',{'mass':100,'se3':[1,1,2,1,0,0,0]}) # convenience constructor

	The last statement is equivalent to:

		p=PhysicalParameters('RigidBodyParameters')
		p['mass']=100; 
		p['se3']=[1,1,2,1,0,0,0]

	Class attributes are those that are registered as serializable, are accessed using the [] operator and are always read-write (be careful)

		p['se3'] # this will show you the se3 attribute inside p
		p['se3']=[1,2,3,1,0,0,0] # this sets se3 of p

	Those attributes that are not fundamental types (strings, numbers, booleans, se3, vectors, quaternions, arrays of numbers, arrays of strings) can be accessed only through explicit python data members, for example:
		
		b=Body()
		b.mold=InteractingGeometry("InteractingSphere",{'radius':1})
		b.shape=GeometricalModel("Sphere",{'radius':1})
		b.mold # will give you the interactingGeometry of body
	
	Instances can be queried about attributes and data members they have:

		b.keys() # serializable attributes, accessible via b['attribute']
		dir(b) # python data members, accessible via b.attribute; the __something__ attributes are python internal attributes/metods -- methods are just callable members

	MetaEngine class has special constructor (for convenience):

		m=MetaEngine('class name as string',[list of engine units])

	and it is equivalent to

		m=MetaEntine('class name as string')
		m.functors=[list of engine units]

	It is your responsibility to pass the right engineUnits, otherwise crash will results. There is currently no way I know of to prevent that. 

*/

/*
TODO:
	1. PhysicalActionContainer (constructor with actionName) with iteration
	2. from yadeControl import Omega as _Omega, inherit from that and add other convenience functions
*/

#ifdef LOG4CXX
	log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.python");
#endif


#define BASIC_PY_PROXY_HEAD(pyClass,yadeClass) \
class pyClass{shared_ptr<AttrAccess> accessor; \
	private: void init(string clss){ proxee=dynamic_pointer_cast<yadeClass>(ClassFactory::instance().createShared(clss.empty()? #yadeClass : clss)); if(!proxee) throw runtime_error("Invalid class `"+clss+"': either nonexistent, or unable to cast to `"+#yadeClass+"'"); } \
	public: shared_ptr<yadeClass> proxee; \
		void ensureAcc(void){ if(!proxee) throw runtime_error(string("No proxied `")+#yadeClass+"'."); if(!accessor) accessor=shared_ptr<AttrAccess>(new AttrAccess(proxee));} \
		pyClass(string clss="", python::dict attrs=python::dict()){ init(clss); python::list l=attrs.items(); int len=PySequence_Size(l.ptr()); for(int i=0; i<len; i++){ python::extract<python::tuple> t(l[i]); python::extract<string> keyEx(t()[0]); if(!keyEx.check()) throw invalid_argument("Attribute keys must be strings."); wrappedPySet(keyEx(),t()[1]); } } \
		pyClass(const shared_ptr<yadeClass>& _proxee): proxee(_proxee) {} \
		std::string pyStr(void){ ensureAcc(); return string(proxee->getClassName()==#yadeClass ? "<"+proxee->getClassName()+">" : "<"+proxee->getClassName()+" "+ #yadeClass +">"); } \
		string className(void){ ensureAcc(); return proxee->getClassName(); } \
		ATTR_ACCESS_CXX(accessor,ensureAcc);

#define BASIC_PY_PROXY_TAIL };

#define BASIC_PY_PROXY(pyClass,yadeClass) BASIC_PY_PROXY_HEAD(pyClass,yadeClass) BASIC_PY_PROXY_TAIL


BASIC_PY_PROXY(pyGeneric,Serializable);

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


BASIC_PY_PROXY_HEAD(pyMetaEngine,MetaEngine)
		// additional constructor
		pyMetaEngine(string clss, python::list functors){init(clss); functors_set(functors);}
		python::list functors_get(void){
			ensureAcc(); shared_ptr<MetaEngine> me=dynamic_pointer_cast<MetaEngine>(proxee); if(!me) throw runtime_error("Proxied class not a MetaEngine (WTF?)"); python::list ret;
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
		void functors_set(python::list ftrs){
			ensureAcc(); shared_ptr<MetaEngine> me=dynamic_pointer_cast<MetaEngine>(proxee); if(!me) throw runtime_error("Proxied class not a MetaEngine. (WTF?)");
			me->clear(); int len=PySequence_Size(ftrs.ptr()) /*[boost1.34] python::len(ftrs)*/;
			for(int i=0; i<len; i++){
				python::extract<pyEngineUnit> euEx(ftrs[i]); if(!euEx.check()) throw invalid_argument("Unable to extract type EngineUnit from sequence.");
				bool ok=false;
				/* FIXME: casting engine unit to the right type via dynamic_cast doesn't work (always unusuccessful),
				 * do static_cast and if the EngineUnit is of wrong type, it will crash badly immediately. */
				#define TRY_ADD_FUNCTOR(P,Q) {shared_ptr<P> p(dynamic_pointer_cast<P>(me)); shared_ptr<EngineUnit> eu(euEx().proxee); if(p&&eu){p->add(static_pointer_cast<Q>(eu)); ok=true; }}
				// shared_ptr<Q> q(dynamic_pointer_cast<Q>(eu)); cerr<<#P<<" "<<#Q<<":"<<(bool)p<<" "<<(bool)q<<endl;
				TRY_ADD_FUNCTOR(BoundingVolumeMetaEngine,BoundingVolumeEngineUnit);
				TRY_ADD_FUNCTOR(GeometricalModelMetaEngine,GeometricalModelEngineUnit);
				TRY_ADD_FUNCTOR(InteractingGeometryMetaEngine,InteractingGeometryEngineUnit);
				TRY_ADD_FUNCTOR(InteractionGeometryMetaEngine,InteractionGeometryEngineUnit);
				TRY_ADD_FUNCTOR(InteractionPhysicsMetaEngine,InteractionPhysicsEngineUnit);
				TRY_ADD_FUNCTOR(PhysicalParametersMetaEngine,PhysicalParametersEngineUnit);
				TRY_ADD_FUNCTOR(PhysicalActionDamper,PhysicalActionDamperUnit);
				TRY_ADD_FUNCTOR(PhysicalActionApplier,PhysicalActionApplierUnit);
				if(!ok) throw runtime_error(string("Unable to cast to suitable MetaEngine type when adding functor (MetaEngine: ")+me->getClassName()+", functor: "+euEx().proxee->getClassName()+")");
				#undef TRY_ADD_FUNCTOR
			}
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
	body_id_t insert(pyBody b){return proxee->insert(b.proxee);}
	void clear(){proxee->clear();}
};

class pyInteractionIterator{
	InteractionContainer::iterator I, Iend;
	public:
	pyInteractionIterator(const shared_ptr<InteractionContainer>& ic){ I=ic->begin(); Iend=ic->end(); }
	pyInteractionIterator pyIter(){return *this;}
	pyInteraction pyNext(){ if(!(I!=Iend)){ PyErr_SetNone(PyExc_StopIteration); python::throw_error_already_set(); }
		InteractionContainer::iterator ret=I; ++I; return pyInteraction(*ret); }
};

class pyInteractionContainer{
	public:
		const shared_ptr<InteractionContainer> proxee;
		pyInteractionContainer(const shared_ptr<InteractionContainer>& _proxee): proxee(_proxee){}
		pyInteractionIterator pyIter(){return pyInteractionIterator(proxee);}
		pyInteraction pyGetitem(python::object id12){
			if(!PySequence_Check(id12.ptr())) throw invalid_argument("Key must be a tuple");
			if(PySequence_Size(id12.ptr())!=2) throw invalid_argument("Key must be a 2-tuple: id1,id2.");
			python::extract<body_id_t> id1_(PySequence_GetItem(id12.ptr(),0)), id2_(PySequence_GetItem(id12.ptr(),1));
			if(!id1_.check()) throw invalid_argument("Could not extract id1");
			if(!id2_.check()) throw invalid_argument("Could not extract id2");
			shared_ptr<Interaction> i=proxee->find(id1_(),id2_());
			if(i) return pyInteraction(i); else throw invalid_argument("No such interaction.");
		}
};


BASIC_PY_PROXY(pyPhysicalAction,PhysicalAction);

class pyPhysicalActionContainer{
	public:
		const shared_ptr<PhysicalActionContainer> proxee;
		pyPhysicalActionContainer(const shared_ptr<PhysicalActionContainer>& _proxee): proxee(_proxee){}
		pyPhysicalAction pyGetitem(python::object action_and_id){
			if(!PySequence_Check(action_and_id.ptr())) throw invalid_argument("Key must be a tuple");
			if(PySequence_Size(action_and_id.ptr())!=2) throw invalid_argument("Key must be a 2-tuple: [action-name , body id].");
			python::extract<string> actionName_(PySequence_GetItem(action_and_id.ptr(),0));
			python::extract<body_id_t> id_(PySequence_GetItem(action_and_id.ptr(),1));
			if(!actionName_.check()) throw invalid_argument("Could not extract action-name.");
			if(!id_.check()) throw invalid_argument("Could not extract body id.");
			// FIXME: this may be rather slow (at every lookup!)
			int actionClassIndex=dynamic_pointer_cast<Indexable>(ClassFactory::instance().createShared(actionName_()))->getClassIndex();
			LOG_DEBUG("Got class index "<<actionClassIndex<<" for "<<actionName_());
			return pyPhysicalAction(proxee->find(id_(),actionClassIndex));
		}
};


BASIC_PY_PROXY_HEAD(pyFileGenerator,FileGenerator)
	bool generate(string outFile){ensureAcc(); proxee->setFileName(outFile); proxee->setSerializationLibrary("XMLFormatManager"); bool ret=proxee->generateAndSave(); LOG_INFO((ret?"SUCCESS:\n":"FAILURE:\n")<<proxee->message); return ret; };
BASIC_PY_PROXY_TAIL;


class pyOmega{
	#define OMEGA Omega::instance()
	private:
		// can be safely removed now, since pyOmega makes an empty rootBody in the constructor, if there is none
		void assertRootBody(){if(!OMEGA.getRootBody()) throw std::runtime_error("No root body."); }
		void maybeRunInitializers(){if(needsInitializers){OMEGA.getRootBody()->runInitializers(); needsInitializers=false;}}
		/*! do we need initializers before running?
		 * This is set to true in constructor (if we create simulation from scratch) and when we load a simulation.
		 * Initializers are run ad this flag set to false by maybeRunInitializers when running (step or run) */
		bool needsInitializers;
	public:
	pyOmega(){ if(!OMEGA.getRootBody()){shared_ptr<MetaBody> mb=Shop::rootBody(); OMEGA.setRootBody(mb);} OMEGA.createSimulationLoop(); /* this is not true if another instance of Omega is created; flag should be stored inside the Omega singleton for clean solution. */ needsInitializers=true; };

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
		maybeRunInitializers();
		//else OMEGA.stopAtIteration=-1;
		OMEGA.startSimulationLoop();
		long toGo=OMEGA.stopAtIteration-OMEGA.getCurrentIteration();
		LOG_DEBUG("RUN"<<(toGo>0?string(" ("+lexical_cast<string>(toGo)+" to go)"):string(""))<<"!");
	}
	void pause(){OMEGA.stopSimulationLoop(); LOG_DEBUG("PAUSE!");}
	void step() {OMEGA.spawnSingleSimulationLoop(); maybeRunInitializers(); LOG_DEBUG("STEP!");}

	void load(std::string fileName) {
		OMEGA.finishSimulationLoop();
		OMEGA.joinSimulationLoop();
		OMEGA.setSimulationFileName(fileName);
		OMEGA.loadSimulation();
		OMEGA.createSimulationLoop();
		LOG_DEBUG("LOAD!");
		needsInitializers=true;
	}

	void save(std::string fileName){
		assertRootBody();
		OMEGA.saveSimulation(fileName);
		LOG_DEBUG("SAVE!");
	}

	void saveSpheres(std::string fileName){ Shop::saveSpheresToFile(fileName); }

	python::list miscParams_get(){
		python::list ret;
		BOOST_FOREACH(shared_ptr<Serializable>& s, OMEGA.getRootBody()->miscParams){
			ret.append(pyGeneric(s));
		}
		return ret;
	}

	void miscParams_set(python::list l){
		int len=PySequence_Size(l.ptr());
		vector<shared_ptr<Serializable> >& miscParams=OMEGA.getRootBody()->miscParams;
		miscParams.clear();
		for(int i=0; i<len; i++){
			if(python::extract<pyGeneric>(PySequence_GetItem(l.ptr(),i)).check()){ pyGeneric g=python::extract<pyGeneric>(PySequence_GetItem(l.ptr(),i)); miscParams.push_back(g.proxee); }
			else throw std::invalid_argument("Unable to extract `Generic' from item #"+lexical_cast<string>(i)+".");
		}
	}

	python::list anyEngines_get(vector<shared_ptr<Engine> >& engContainer){
		python::list ret; 
		BOOST_FOREACH(shared_ptr<Engine>& eng, engContainer){
			#define APPEND_ENGINE_IF_POSSIBLE(engineType,pyEngineType) { shared_ptr<engineType> e=dynamic_pointer_cast<engineType>(eng); if(e) { ret.append(pyEngineType(e)); continue; } }
			APPEND_ENGINE_IF_POSSIBLE(MetaEngine,pyMetaEngine); APPEND_ENGINE_IF_POSSIBLE(StandAloneEngine,pyStandAloneEngine); APPEND_ENGINE_IF_POSSIBLE(DeusExMachina,pyDeusExMachina);
			throw std::runtime_error("Unknown engine type: `"+eng->getClassName()+"' (only MetaEngine, StandAloneEngine and DeusExMachina are supported)");
		}
		return ret;
	}

	void anyEngines_set(vector<shared_ptr<Engine> >& engContainer, python::object egs){
		assertRootBody(); int len=PySequence_Size(egs.ptr()) /*[boost1.34] python::len(egs)*/;
		//const shared_ptr<MetaBody>& rootBody=OMEGA.getRootBody(); rootBody->engines.clear();
		engContainer.clear();
		for(int i=0; i<len; i++){
			#define PUSH_BACK_ENGINE_IF_POSSIBLE(pyEngineType) if(python::extract<pyEngineType>(PySequence_GetItem(egs.ptr(),i)).check()){ pyEngineType e=python::extract<pyEngineType>(PySequence_GetItem(egs.ptr(),i)); engContainer.push_back(e.proxee); /* cerr<<"added "<<e.pyStr()<<", a "<<#pyEngineType<<endl; */ continue; }
			PUSH_BACK_ENGINE_IF_POSSIBLE(pyStandAloneEngine); PUSH_BACK_ENGINE_IF_POSSIBLE(pyMetaEngine); PUSH_BACK_ENGINE_IF_POSSIBLE(pyDeusExMachina);
			throw std::runtime_error("Encountered unknown engine type (unable to extract from python object)");
		}
	}
	python::list engines_get(void){assertRootBody(); return anyEngines_get(OMEGA.getRootBody()->engines);}
	void engines_set(python::object egs){assertRootBody(); anyEngines_set(OMEGA.getRootBody()->engines,egs);}
	python::list initializers_get(void){assertRootBody(); return anyEngines_get(OMEGA.getRootBody()->initializers);}
	void initializers_set(python::object egs){assertRootBody(); anyEngines_set(OMEGA.getRootBody()->initializers,egs);}


	void wait(){ if(OMEGA.isRunning()){LOG_DEBUG("WAIT!");} while(OMEGA.isRunning()) usleep(50000 /*20 ms*/); }
	
	pyBodyContainer bodies_get(void){assertRootBody(); return pyBodyContainer(OMEGA.getRootBody()->bodies); }
	pyInteractionContainer interactions_get(void){assertRootBody(); return pyInteractionContainer(OMEGA.getRootBody()->transientInteractions); }

	pyPhysicalActionContainer actions_get(void){return pyPhysicalActionContainer(OMEGA.getRootBody()->physicalActions); }

	boost::python::list listChildClasses(const string& base){
		boost::python::list ret;
		for(map<string,DynlibDescriptor>::const_iterator di=Omega::instance().getDynlibsDescriptor().begin();di!=Omega::instance().getDynlibsDescriptor().end();++di) if (Omega::instance().isInheritingFrom((*di).first,base)) ret.append(di->first);
		return ret;
	}
	#undef OMEGA
};

class pySTLImporter : public STLImporter {
    public:
	void py_import(pyBodyContainer bc) { import(bc.proxee); }
};
	
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(omega_overloads,run,0,1);

#ifdef USE_PYGLVIEWER 
/*! GL viewer wrapper, with full attribute access. CURRENTLY DOESN'T EVEN COMPILE!!!
 *
 * Creates the 3D view on instantiation. Currently displays nothing (why???), although it redraws just fine.
 * Has many bugs: multiple views will lead to crash, explicit delete crashes (somewhere in qt) as well.
 */
class pyGLViewer{
	//DECLARE_LOGGER;
	shared_ptr<RenderingEngine> renderer;
	shared_ptr<GLViewer4> viewer;
#if 0
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
#endif 
	class DrawThread: public QThread{
		//Q_OBJECT;
		const shared_ptr<GLViewer4> vw;
		shared_ptr<QTimer> timer;
		public:
			DrawThread(shared_ptr<GLViewer4> _viewer): vw(_viewer){}
			virtual void run(){
				LOG_WARN("drawThread::run() with viewer "<<vw);
				timer=shared_ptr<QTimer>(new QTimer());
				connect(timer.get(),SIGNAL(timeout()),vw.get(),SLOT(callUpdateGL()));
				timer->start(100);
				//while(true){ cerr<<"@"; vw->updateGL(); usleep(50000);}
				exec();
			};
		//public slots:
		//	void updateGL(){cerr<<"@";}
	};
	shared_ptr<AttrAccess> accessor;
	void ensureAcc(){if(!accessor)accessor=shared_ptr<AttrAccess>(new AttrAccess(renderer));}
	shared_ptr<DrawThread> drawThread;
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
		//if(viewId==0){	int _argc=0; char _argvv[]="foo"; app=new QApplication(_argc,(char**) &_argvv);}
		//app->setMainWidget(viewer.get());
		int _argc=0; char _argvv[]="foo"; //QApplication* app=new 
		QApplication* app=new QApplication(_argc,(char**) &_argvv);

		QGLFormat format;	QGLFormat::setDefaultFormat(format); format.setStencil(TRUE); format.setAlpha(TRUE);
		viewer=shared_ptr<GLViewer4>(new GLViewer4(viewId,renderer,format,/*parent*/0,0/* non-primary views will have to share widgets, though */)); //
		viewer->setWindowTitle("GL Viewer");
		viewer->centerScene();
		viewer->show();
		drawThread=shared_ptr<DrawThread>(new DrawThread(viewer));
		drawThread->start(); // is the same as DrawThread->run() except for check that it isn't running already?!

		/*if(viewId==0){
			app->setMainWidget(viewer.get());
			redrawThread=shared_ptr<boost::thread>(new boost::thread(boost::bind(&pyGLViewer::redrawAlarm,this)));
		}
		appThread=shared_ptr<boost::thread>(new boost::thread(boost::bind(&QApplication::exec,app))); */
	}
	~pyGLViewer(){// redrawThread and appThread deleted by the descructor
		//viewer->close();
		//app->quit();
	}
};
CREATE_LOGGER(pyGLViewer);
#endif /* USE_PYGLVIEWER */


BOOST_PYTHON_MODULE(wrapper)
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
		.def("saveSpheres",&pyOmega::saveSpheres)
		.def("run",&pyOmega::run,omega_overloads())
		.def("pause",&pyOmega::pause)
		.def("step",&pyOmega::step)
		.def("wait",&pyOmega::wait)
		.add_property("engines",&pyOmega::engines_get,&pyOmega::engines_set)
		.add_property("miscParams",&pyOmega::miscParams_get,&pyOmega::miscParams_set)
		.add_property("initializers",&pyOmega::initializers_get,&pyOmega::initializers_set)
		.add_property("bodies",&pyOmega::bodies_get)
		.add_property("interactions",&pyOmega::interactions_get)
		.add_property("actions",&pyOmega::actions_get)
		.def("childClasses",&pyOmega::listChildClasses)
		;
	
	boost::python::class_<pyBodyContainer>("BodyContainer",python::init<pyBodyContainer&>())
		.def("__getitem__",&pyBodyContainer::pyGetitem)
		.def("append",&pyBodyContainer::insert)
		.def("clear", &pyBodyContainer::clear);
	boost::python::class_<pyInteractionContainer>("InteractionContainer",python::init<pyInteractionContainer&>())
		.def("__iter__",&pyInteractionContainer::pyIter)
		.def("__getitem__",&pyInteractionContainer::pyGetitem);
	boost::python::class_<pyInteractionIterator>("InteractionIterator",python::init<pyInteractionIterator&>())
		.def("__iter__",&pyInteractionIterator::pyIter)
		.def("next",&pyInteractionIterator::pyNext);

	boost::python::class_<pyPhysicalActionContainer>("ActionContainer",python::init<pyPhysicalActionContainer&>())
		.def("__getitem__",&pyPhysicalActionContainer::pyGetitem);
	

//	boost::python::class_<pyBodyContainer>("BodyContainer",python::init<pyBodyContainer&>())
//		.def("__getitem__",&pyBodyContainer::pyGetitem);

		#if 0
			.def("oneStep",&oneStep)
			.def("newView", &newView)
			.def("centerScene", &centerScene)
		#endif
#ifdef USE_PYGLVIEWER
	boost::python::class_<pyGLViewer>("View")
		.ATTR_ACCESS_PY(pyGLViewer);
#endif

#define BASIC_PY_PROXY_WRAPPER(pyClass,pyName)  \
	boost::python::class_<pyClass>(pyName,python::init<python::optional<string,python::dict> >()) \
	.ATTR_ACCESS_PY(pyClass) \
	.def("__str__",&pyClass::pyStr).def("__repr__",&pyClass::pyStr) \
	.add_property("name",&pyClass::className)

	BASIC_PY_PROXY_WRAPPER(pyStandAloneEngine,"StandAloneEngine");
	BASIC_PY_PROXY_WRAPPER(pyMetaEngine,"MetaEngine")
		.add_property("functors",&pyMetaEngine::functors_get,&pyMetaEngine::functors_set)
		.def(python::init<string,python::list>());
	BASIC_PY_PROXY_WRAPPER(pyDeusExMachina,"DeusExMachina");
	BASIC_PY_PROXY_WRAPPER(pyEngineUnit,"EngineUnit")
		.add_property("bases",&pyEngineUnit::bases_get);

	BASIC_PY_PROXY_WRAPPER(pyGeometricalModel,"GeometricalModel");
	BASIC_PY_PROXY_WRAPPER(pyInteractingGeometry,"InteractingGeometry");
	BASIC_PY_PROXY_WRAPPER(pyPhysicalParameters,"PhysicalParameters");
	BASIC_PY_PROXY_WRAPPER(pyBoundingVolume,"BoundingVolume");
	BASIC_PY_PROXY_WRAPPER(pyInteractionGeometry,"InteractionGeometry");
	BASIC_PY_PROXY_WRAPPER(pyInteractionPhysics,"InteractionPhysics");

	BASIC_PY_PROXY_WRAPPER(pyGeneric,"Generic");

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
		.add_property("geom",&pyInteraction::geom_get,&pyInteraction::geom_set)
		.add_property("id1",&pyInteraction::id1_get)
		.add_property("id2",&pyInteraction::id2_get);

	BASIC_PY_PROXY_WRAPPER(pyPhysicalAction,"Action");

	BASIC_PY_PROXY_WRAPPER(pyFileGenerator,"Preprocessor")
		.def("generate",&pyFileGenerator::generate);

	boost::python::class_<pySTLImporter>("STLImporter")
	    .def("open",&pySTLImporter::open)
	    .add_property("number_of_all_imported",&pySTLImporter::number_of_all_imported)
	    .add_property("number_of_imported_vertices",&pySTLImporter::number_of_imported_vertices)
	    .add_property("number_of_imported_edges",&pySTLImporter::number_of_imported_edges)
	    .add_property("number_of_imported_facets",&pySTLImporter::number_of_imported_facets)
	    .add_property("number_of_vertices",&pySTLImporter::number_of_vertices)
	    .add_property("number_of_corner_vertices",&pySTLImporter::number_of_corner_vertices)
	    .add_property("number_of_flat_vertices",&pySTLImporter::number_of_flat_vertices)
	    .add_property("number_of_facets",&pySTLImporter::number_of_facets)
	    .add_property("number_of_edges",&pySTLImporter::number_of_edges)
	    .add_property("number_of_corner_edges",&pySTLImporter::number_of_corner_edges)
	    .add_property("number_of_flat_edges",&pySTLImporter::number_of_flat_edges)
	    .def_readwrite("max_vertices_in_facet",&pySTLImporter::max_vertices_in_facet)
	    .def("set_imported_stuff",&pySTLImporter::set_imported_stuff)
	    .def_readwrite("import_vertices",&pySTLImporter::import_vertices)
	    .def_readwrite("import_edges",&pySTLImporter::import_edges)
	    .def_readwrite("import_facets",&pySTLImporter::import_facets)
	    .def_readwrite("facets_wire",&pySTLImporter::facets_wire)
	    .def_readwrite("import_flat_edges_flag",&pySTLImporter::import_flat_edges_flag)
	    .def_readwrite("import_flat_vertices_flag",&pySTLImporter::import_flat_vertices_flag)
	    .def("import_geometry",&pySTLImporter::py_import);
}

