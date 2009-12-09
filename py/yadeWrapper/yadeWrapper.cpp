// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 

#include<sstream>
#include<map>
#include<vector>
#include<unistd.h>
#include<list>
#include<signal.h>

#include<boost/python.hpp>
#include<boost/python/raw_function.hpp>
#include<boost/python/suite/indexing/vector_indexing_suite.hpp>
#include<boost/bind.hpp>
#include<boost/lambda/bind.hpp>
#include<boost/thread/thread.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
#include<boost/any.hpp>
#include<boost/python.hpp>
#include<boost/foreach.hpp>
#include<boost/algorithm/string.hpp>
#include<boost/version.hpp>



#include<yade/lib-base/Logging.hpp>
#include<yade/lib-serialization-xml/XMLFormatManager.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/FileGenerator.hpp>

#include<yade/pkg-dem/STLImporter.hpp>

#include<yade/core/Dispatcher.hpp>
#include<yade/core/StandAloneEngine.hpp>
#include<yade/core/DeusExMachina.hpp>
#include<yade/core/Functor.hpp>
#include<yade/pkg-common/ParallelEngine.hpp>
#include<yade/core/Functor.hpp>

#include<yade/pkg-common/BoundDispatcher.hpp>
#include<yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include<yade/pkg-common/InteractionPhysicsDispatcher.hpp>
#include<yade/pkg-common/LawDispatcher.hpp>
#include<yade/pkg-common/InteractionDispatchers.hpp>
#ifdef YADE_PHYSPAR
	#include<yade/pkg-common/StateMetaEngine.hpp>
	#include<yade/pkg-common/PhysicalActionDamper.hpp>
	#include<yade/pkg-common/PhysicalActionApplier.hpp>
	#include<yade/pkg-common/StateEngineUnit.hpp>
	#include<yade/pkg-common/PhysicalActionDamperUnit.hpp>
	#include<yade/pkg-common/PhysicalActionApplierUnit.hpp>

	#include<yade/pkg-common/ParticleParameters.hpp>
#endif
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/AABB.hpp>

#include<yade/pkg-common/BoundFunctor.hpp>
#include<yade/pkg-common/InteractingGeometryEngineUnit.hpp>
#include<yade/pkg-common/InteractionGeometryFunctor.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-common/LawFunctor.hpp>

#include<yade/pkg-dem/Shop.hpp>
#include<yade/pkg-dem/Clump.hpp>

using namespace boost;
using namespace std;

#include<yade/extra/boost_python_len.hpp>


class RenderingEngine;

/*!

	FIXME: outdated syntax
	
	A regular class (not Omega) is instantiated like this:

		RootClass('optional class name as quoted string',attribute1=value1,attribute2=value2,...)
		
	if class name is not given, the RootClass itself is instantiated

		p=Material() # p is now instance of Material
		p=Material('GranularMat') # p is now instance of RigidBodyParameters, which has PhysicalParameters as the "root" class
		p=Material('GranularMat',mass=100,se3=(Vector3(1,1,2),Quaternion.IDENTITY)) # convenience constructor

	The last statement is equivalent to:

		p=Material('GranularMat')
		p['mass']=100; 
		p['se3']=[1,1,2,1,0,0,0]

	Class attributes are those that are registered as serializable, are accessed using the [] operator and are always read-write (be careful)

		p['se3'] # this will show you the se3 attribute inside p
		p['se3']=[1,2,3,1,0,0,0] # this sets se3 of p

	Those attributes that are not fundamental types (strings, numbers, booleans, se3, vectors, quaternions, arrays of numbers, arrays of strings) can be accessed only through explicit python data members, for example:
		
		b=Body()
		b.mold=Shape("InteractingSphere",radius=1)
		b.shape=GeometricalModel("Sphere",radius=1)
		b.mold # will give you the shape of body
	
	Instances can be queried about attributes and data members they have:

		b.keys() # serializable attributes, accessible via b['attribute']
		dir(b) # python data members, accessible via b.attribute; the __something__ attributes are python internal attributes/metods -- methods are just callable members

	Dispatcher class has special constructor (for convenience):

		m=Dispatcher('class name as string',[list of engine units])

	and it is equivalent to

		m=MetaEntine('class name as string')
		m.functors=[list of engine units]

*/

#ifdef YADE_LOG4CXX
	log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.python");
#endif

/*
Python normally iterates over object it is has __getitem__ and __len__, which BodyContainer does.
However, it will not skip removed bodies automatically, hence this iterator which does just that.
*/
class pyBodyIterator{
	BodyContainer::iterator I, Iend;
	public:
	pyBodyIterator(const shared_ptr<BodyContainer>& bc){ I=bc->begin(); Iend=bc->end(); }
	pyBodyIterator pyIter(){return *this;}
	shared_ptr<Body> pyNext(){
		BodyContainer::iterator ret;
		while(I!=Iend){ ret=I; ++I; if(*ret) return *ret; }
		PyErr_SetNone(PyExc_StopIteration); python::throw_error_already_set(); /* never reached, but makes the compiler happier */ throw;
	}
};

class pyBodyContainer{
	public:
	const shared_ptr<BodyContainer> proxee;
	pyBodyIterator pyIter(){return pyBodyIterator(proxee);}
	pyBodyContainer(const shared_ptr<BodyContainer>& _proxee): proxee(_proxee){}
	shared_ptr<Body> pyGetitem(body_id_t id){
		if((size_t)id>=proxee->size()){ PyErr_SetString(PyExc_IndexError, "Body id out of range."); python::throw_error_already_set(); /* make compiler happy; never reached */ return shared_ptr<Body>(); }
		else return (*proxee)[id];
	}
	body_id_t append(shared_ptr<Body> b){
		// shoud be >=0, but Body is by default created with id 0... :-|
		if(b->getId()>0){ PyErr_SetString(PyExc_IndexError,("Body already has id "+lexical_cast<string>(b->getId())+"set; appending such body is not allowed.").c_str()); python::throw_error_already_set(); }
		return proxee->insert(b);
	}
	vector<body_id_t> appendList(vector<shared_ptr<Body> > bb){
		/* prevent crash when adding lots of bodies (not clear why it happens exactly, bt is like this:

			#3  <signal handler called>
			#4  0x000000000052483f in boost::detail::atomic_increment (pw=0x8089) at /usr/include/boost/detail/sp_counted_base_gcc_x86.hpp:66
			#5  0x00000000005248b3 in boost::detail::sp_counted_base::add_ref_copy (this=0x8081) at /usr/include/boost/detail/sp_counted_base_gcc_x86.hpp:133
			#6  0x00000000005249ca in shared_count (this=0x7fff2e44db48, r=@0x7f08ffd692b8) at /usr/include/boost/detail/shared_count.hpp:227
			#7  0x00000000005258e3 in shared_ptr (this=0x7fff2e44db40) at /usr/include/boost/shared_ptr.hpp:165
			#8  0x0000000000505cff in BodyRedirectionVectorIterator::getValue (this=0x846f040) at /home/vaclav/yade/trunk/core/containers/BodyRedirectionVector.cpp:47
			#9  0x00007f0908af41ce in BodyContainerIteratorPointer::operator* (this=0x7fff2e44db60) at /home/vaclav/yade/build-trunk/include/yade-trunk/yade/core/BodyContainer.hpp:63
			#10 0x00007f0908af420a in boost::foreach_detail_::deref<BodyContainer, mpl_::bool_<false> > (cur=@0x7fff2e44db60) at /usr/include/boost/foreach.hpp:750
			#11 0x00007f0908adc5a9 in OpenGLRenderingEngine::renderGeometricalModel (this=0x77f1240, rootBody=@0x1f49220) at pkg/common/RenderingEngine/OpenGLRenderingEngine/OpenGLRenderingEngine.cpp:441
			#12 0x00007f0908adfb84 in OpenGLRenderingEngine::render (this=0x77f1240, rootBody=@0x1f49220, selection=-1) at pkg/common/RenderingEngine/OpenGLRenderingEngine/OpenGLRenderingEngine.cpp:232

		*/
		#if BOOST_VERSION<103500
			boost::try_mutex::scoped_try_lock lock(Omega::instance().renderMutex,true); // acquire lock on the mutex (true)
		#else
			boost::mutex::scoped_lock lock(Omega::instance().renderMutex);
		#endif
		vector<body_id_t> ret; FOREACH(shared_ptr<Body>& b, bb){ret.push_back(append(b));} return ret;
	}
	python::tuple appendClump(vector<shared_ptr<Body> > bb){/*clump: first add constitutents, then add clump, then add constitutents to the clump, then update clump props*/
		vector<body_id_t> ids(appendList(bb));
		shared_ptr<Clump> clump=shared_ptr<Clump>(new Clump());
		shared_ptr<Body> clumpAsBody=static_pointer_cast<Body>(clump);
		clump->isDynamic=true;
		proxee->insert(clumpAsBody);
		FOREACH(body_id_t id, ids) clump->add(id);
		clump->updateProperties(false);
		return python::make_tuple(clump->getId(),ids);
	}
	vector<body_id_t> replace(vector<shared_ptr<Body> > bb){proxee->clear(); return appendList(bb);}
	long length(){return proxee->size();}
	void clear(){proxee->clear();}
	bool erase(body_id_t id){ return proxee->erase(id); }
};


class pyTags{
	public:
		pyTags(const shared_ptr<Scene> _mb): mb(_mb){}
		const shared_ptr<Scene> mb;
		bool hasKey(const string& key){ FOREACH(string val, mb->tags){ if(algorithm::starts_with(val,key+"=")){ return true;} } return false; }
		string getItem(string key){
			FOREACH(string& val, mb->tags){
				if(algorithm::starts_with(val,key+"=")){ string val1(val); algorithm::erase_head(val1,key.size()+1); algorithm::replace_all(val1,"~"," "); return val1;}
			}
			PyErr_SetString(PyExc_KeyError, "Invalid key.");
			python::throw_error_already_set(); /* make compiler happy; never reached */ return string();
		}
		void setItem(string key,string newVal){
			string item=algorithm::replace_all_copy(key+"="+newVal," ","~");
			FOREACH(string& val, mb->tags){if(algorithm::starts_with(val,key+"=")){ val=item; return; } }
			mb->tags.push_back(item);
			}
		python::list keys(){
			python::list ret;
			FOREACH(string val, mb->tags){
				size_t i=val.find("=");
				if(i==string::npos) throw runtime_error("Tags must be in the key=value format");
				algorithm::erase_tail(val,val.size()-i); ret.append(val);
			}
			return ret;
		}
};


class pyInteractionIterator{
	InteractionContainer::iterator I, Iend;
	public:
	pyInteractionIterator(const shared_ptr<InteractionContainer>& ic){ I=ic->begin(); Iend=ic->end(); }
	pyInteractionIterator pyIter(){return *this;}
	shared_ptr<Interaction> pyNext(){ if(!(I!=Iend)){ PyErr_SetNone(PyExc_StopIteration); python::throw_error_already_set(); }
		InteractionContainer::iterator ret=I; ++I; return *ret; }
};

class pyInteractionContainer{
	public:
		const shared_ptr<InteractionContainer> proxee;
		pyInteractionContainer(const shared_ptr<InteractionContainer>& _proxee): proxee(_proxee){}
		pyInteractionIterator pyIter(){return pyInteractionIterator(proxee);}
		shared_ptr<Interaction> pyGetitem(vector<body_id_t> id12){
			//if(!PySequence_Check(id12.ptr())) throw invalid_argument("Key must be a tuple");
			//if(python::len(id12)!=2) throw invalid_argument("Key must be a 2-tuple: id1,id2.");
			if(id12.size()==2){
				shared_ptr<Interaction> i=proxee->find(id12[0],id12[1]);
				if(i) return i; else throw invalid_argument("No such interaction.");
			}
			else if(id12.size()==1){ return (*proxee)[id12[0]];}
			else throw invalid_argument("2 integers (id1,id2) or 1 integer (nth) required.");
		}
		/* return nth _real_ iteration from the container (0-based index); this is to facilitate picking random interaction */
		shared_ptr<Interaction> pyNth(long n){
			long i=0; FOREACH(shared_ptr<Interaction> I, *proxee){ if(!I->isReal()) continue; if(i++==n) return I; }
			throw invalid_argument(string("Interaction number out of range (")+lexical_cast<string>(n)+">="+lexical_cast<string>(i)+").");
		}
		long len(){return proxee->size();}
		void clear(){proxee->clear();}
		python::list withBody(long id){ python::list ret; FOREACH(const shared_ptr<Interaction>& I, *proxee){ if(I->isReal() && (I->getId1()==id || I->getId2()==id)) ret.append(I);} return ret;}
		python::list withBodyAll(long id){ python::list ret; FOREACH(const shared_ptr<Interaction>& I, *proxee){ if(I->getId1()==id || I->getId2()==id) ret.append(I);} return ret; }
		long countReal(){ long ret=0; FOREACH(const shared_ptr<Interaction>& I, *proxee){ if(I->isReal()) ret++; } return ret; }
		bool serializeSorted_get(){return proxee->serializeSorted;}
		void serializeSorted_set(bool ss){proxee->serializeSorted=ss;}
};


class pyBexContainer{
		shared_ptr<Scene> rb;
	public:
		pyBexContainer(){ rb=Omega::instance().getScene(); }
		Vector3r force_get(long id){  rb->bex.sync(); return rb->bex.getForce(id); }
		Vector3r torque_get(long id){ rb->bex.sync(); return rb->bex.getTorque(id); }
		Vector3r move_get(long id){   rb->bex.sync(); return rb->bex.getMove(id); }
		Vector3r rot_get(long id){    rb->bex.sync(); return rb->bex.getRot(id); }
		void force_add(long id, const Vector3r& f){  rb->bex.addForce (id,f); }
		void torque_add(long id, const Vector3r& t){ rb->bex.addTorque(id,t);}
		void move_add(long id, const Vector3r& t){   rb->bex.addMove(id,t);}
		void rot_add(long id, const Vector3r& t){    rb->bex.addRot(id,t);}
};

class pyMaterialContainer{
		shared_ptr<Scene> rb;
	public:
		pyMaterialContainer() {rb=Omega::instance().getScene();}
		shared_ptr<Material> getitem_id(int id){ if(id<0 || (size_t)id>=rb->materials.size()){ PyErr_SetString(PyExc_IndexError, "Material id out of range."); python::throw_error_already_set(); /* never reached */ throw; } return Material::byId(id,rb); }
		shared_ptr<Material> getitem_label(string label){
			// translate runtime_error to KeyError (instead of RuntimeError) if the material doesn't exist
			try { return Material::byLabel(label,rb);	}
			catch (std::runtime_error& e){ PyErr_SetString(PyExc_KeyError,e.what()); python::throw_error_already_set(); /* never reached; avoids warning */ throw; }
		}
		int append(shared_ptr<Material>& m){ rb->materials.push_back(m); m->id=rb->materials.size()-1; return m->id; }
		vector<int> appendList(vector<shared_ptr<Material> > mm){ vector<int> ret; FOREACH(shared_ptr<Material>& m, mm) ret.push_back(append(m)); return ret; }
		int len(){ return (int)rb->materials.size(); }
};

void termHandlerNormal(int sig){cerr<<"Yade: normal exit."<<endl; raise(SIGTERM);}
void termHandlerError(int sig){cerr<<"Yade: error exit."<<endl; raise(SIGTERM);}

class pyOmega{
	private:
		// can be safely removed now, since pyOmega makes an empty rootBody in the constructor, if there is none
		void assertRootBody(){if(!OMEGA.getScene()) throw std::runtime_error("No root body."); }
		Omega& OMEGA;
	public:
	pyOmega(): OMEGA(Omega::instance()){
		shared_ptr<Scene> rb=OMEGA.getScene();
		if(!rb){
			OMEGA.init();
			rb=OMEGA.getScene();
		}
		assert(rb);
		// if(!rb->physicalParameters){rb->physicalParameters=shared_ptr<PhysicalParameters>(new ParticleParameters);} /* PhysicalParameters crashes StateMetaEngine... why? */
		if(!rb->bound){rb->bound=shared_ptr<AABB>(new AABB);}
		// initialized in constructor now: rb->bound->diffuseColor=Vector3r(1,1,1); 
		if(!rb->shape){rb->shape=shared_ptr<MetaInteractingGeometry>(new MetaInteractingGeometry);}
		//if(!OMEGA.getScene()){shared_ptr<Scene> mb=Shop::rootBody(); OMEGA.setScene(mb);}
		/* this is not true if another instance of Omega is created; flag should be stored inside the Omega singleton for clean solution. */
		if(!OMEGA.hasSimulationLoop()){
			OMEGA.createSimulationLoop();
		}
	};
	/* Create variables in python's __builtin__ namespace that correspond to labeled objects. At this moment, only engines and functors can be labeled (not bodies etc). */
	void mapLabeledEntitiesToVariables(){
		// not sure if we should map materials to variables by default...
		// a call to this functions would have to be added to pyMaterialContainer::append
		#if 0
			FOREACH(const shared_ptr<Material>& m, OMEGA.getScene()->materials){
				if(!m->label.empty()) { PyGILState_STATE gstate; gstate = PyGILState_Ensure(); PyRun_SimpleString(("__builtins__."+m->label+"=Omega().materials["+lexical_cast<string>(m->id)+"]").c_str()); PyGILState_Release(gstate); }
			}
		#endif
		FOREACH(const shared_ptr<Engine>& e, OMEGA.getScene()->engines){
			if(!e->label.empty()){
				PyGILState_STATE gstate; gstate = PyGILState_Ensure();
				PyRun_SimpleString(("__builtins__."+e->label+"=Omega().labeledEngine('"+e->label+"')").c_str());
				PyGILState_Release(gstate);
			}
			if(isChildClassOf(e->getClassName(),"Dispatcher")){
				shared_ptr<Dispatcher> ee=dynamic_pointer_cast<Dispatcher>(e);
				FOREACH(const shared_ptr<Functor>& f, ee->functorArguments){
					if(!f->label.empty()){
						PyGILState_STATE gstate; gstate = PyGILState_Ensure(); PyRun_SimpleString(("__builtins__."+f->label+"=Omega().labeledEngine('"+f->label+"')").c_str()); PyGILState_Release(gstate);
					}
				}
			}
			if(e->getClassName()=="InteractionDispatchers"){
				shared_ptr<InteractionDispatchers> ee=dynamic_pointer_cast<InteractionDispatchers>(e);
				list<shared_ptr<Functor> > eus;
				FOREACH(const shared_ptr<Functor>& eu,ee->geomDispatcher->functorArguments) eus.push_back(eu);
				FOREACH(const shared_ptr<Functor>& eu,ee->physDispatcher->functorArguments) eus.push_back(eu);
				FOREACH(const shared_ptr<Functor>& eu,ee->lawDispatcher->functorArguments) eus.push_back(eu);
				FOREACH(const shared_ptr<Functor>& eu,eus){
					if(!eu->label.empty()){
						PyGILState_STATE gstate; gstate = PyGILState_Ensure(); PyRun_SimpleString(("__builtins__."+eu->label+"=Omega().labeledEngine('"+eu->label+"')").c_str()); PyGILState_Release(gstate);
					}
				}
			}
		}
	}

	long iter(){ return OMEGA.getCurrentIteration();}
	double simulationTime(){return OMEGA.getSimulationTime();}
	double realTime(){ return OMEGA.getComputationTime(); }
	double dt_get(){return OMEGA.getTimeStep();}
	void dt_set(double dt){OMEGA.skipTimeStepper(true); OMEGA.setTimeStep(dt);}

	long stopAtIter_get(){return OMEGA.getScene()->stopAtIteration; }
	void stopAtIter_set(long s){OMEGA.getScene()->stopAtIteration=s; }

	bool usesTimeStepper_get(){return OMEGA.timeStepperActive();}
	void usesTimeStepper_set(bool use){OMEGA.skipTimeStepper(!use);}

	bool timingEnabled_get(){return TimingInfo::enabled;}
	void timingEnabled_set(bool enabled){TimingInfo::enabled=enabled;}
	unsigned long bexSyncCount_get(){ return OMEGA.getScene()->bex.syncCount;}
	void bexSyncCount_set(unsigned long count){ OMEGA.getScene()->bex.syncCount=count;}

	void run(long int numIter=-1,bool doWait=false){
		if(numIter>0) OMEGA.getScene()->stopAtIteration=OMEGA.getCurrentIteration()+numIter;
		OMEGA.startSimulationLoop();
		// timespec t1,t2; t1.tv_sec=0; t1.tv_nsec=40000000; /* 40 ms */
		// while(!OMEGA.isRunning()) nanosleep(&t1,&t2); // wait till we start, so that calling wait() immediately afterwards doesn't return immediately
		LOG_DEBUG("RUN"<<((OMEGA.getScene()->stopAtIteration-OMEGA.getCurrentIteration())>0?string(" ("+lexical_cast<string>(OMEGA.getScene()->stopAtIteration-OMEGA.getCurrentIteration())+" to go)"):string(""))<<"!");
		if(doWait) wait();
	}
	void pause(){Py_BEGIN_ALLOW_THREADS; OMEGA.stopSimulationLoop(); Py_END_ALLOW_THREADS; LOG_DEBUG("PAUSE!");}
	void step() { if(OMEGA.isRunning()) throw runtime_error("Called O.step() while simulation is running."); OMEGA.getScene()->moveToNextTimeStep(); /* LOG_DEBUG("STEP!"); run(1); wait(); */ }
	void wait(){ if(OMEGA.isRunning()){LOG_DEBUG("WAIT!");} else return; timespec t1,t2; t1.tv_sec=0; t1.tv_nsec=40000000; /* 40 ms */ Py_BEGIN_ALLOW_THREADS; while(OMEGA.isRunning()) nanosleep(&t1,&t2); Py_END_ALLOW_THREADS; }

	void load(std::string fileName) {
		Py_BEGIN_ALLOW_THREADS; OMEGA.joinSimulationLoop(); Py_END_ALLOW_THREADS; 
		OMEGA.setSimulationFileName(fileName);
		OMEGA.loadSimulation();
		OMEGA.createSimulationLoop();
		mapLabeledEntitiesToVariables();
		LOG_DEBUG("LOAD!");
	}
	void reload(){	load(OMEGA.getSimulationFileName());}
	void saveTmp(string mark=""){ save(":memory:"+mark);}
	void loadTmp(string mark=""){ load(":memory:"+mark);}
	void tmpToFile(string mark, string filename){
		if(OMEGA.memSavedSimulations.count(":memory:"+mark)==0) throw runtime_error("No memory-saved simulation named "+mark);
		iostreams::filtering_ostream out;
		if(algorithm::ends_with(filename,".bz2")) out.push(iostreams::bzip2_compressor());
		out.push(iostreams::file_sink(filename));
		if(!out.good()) throw runtime_error("Error while opening file `"+filename+"' for writing.");
		LOG_INFO("Saving :memory:"<<mark<<" to "<<filename);
		out<<OMEGA.memSavedSimulations[":memory:"+mark];
	}
	string tmpToString(string mark){
		if(OMEGA.memSavedSimulations.count(":memory:"+mark)==0) throw runtime_error("No memory-saved simulation named "+mark);
		return OMEGA.memSavedSimulations[":memory:"+mark];
	}

	void reset(){Py_BEGIN_ALLOW_THREADS; OMEGA.reset(); Py_END_ALLOW_THREADS; }
	void resetThisScene(){Py_BEGIN_ALLOW_THREADS; OMEGA.joinSimulationLoop(); Py_END_ALLOW_THREADS; OMEGA.resetScene(); OMEGA.createSimulationLoop();}
	void resetTime(){ OMEGA.getScene()->currentIteration=0; OMEGA.getScene()->simulationTime=0; OMEGA.timeInit(); }
	void switchScene(){ std::swap(OMEGA.scene,OMEGA.sceneAnother); }

	void save(std::string fileName){
		assertRootBody();
		OMEGA.saveSimulation(fileName);
		OMEGA.setSimulationFileName(fileName);
		LOG_DEBUG("SAVE!");
	}

	void saveSpheres(std::string fileName){ Shop::saveSpheresToFile(fileName); }

	python::list miscParams_get(){
		python::list ret;
		FOREACH(shared_ptr<Serializable>& s, OMEGA.getScene()->miscParams){
			ret.append(s);
		}
		return ret;
	}

	void miscParams_set(vector<shared_ptr<Serializable> > ss){
		vector<shared_ptr<Serializable> >& miscParams=OMEGA.getScene()->miscParams;
		miscParams.clear();
		FOREACH(shared_ptr<Serializable> s, ss){
			miscParams.push_back(s);
		}
	}

	vector<shared_ptr<Engine> > engines_get(void){assertRootBody(); return OMEGA.getScene()->engines;}
	void engines_set(const vector<shared_ptr<Engine> >& egs){assertRootBody(); OMEGA.getScene()->engines.clear(); FOREACH(const shared_ptr<Engine>& e, egs) OMEGA.getScene()->engines.push_back(e); mapLabeledEntitiesToVariables(); }
	vector<shared_ptr<Engine> > initializers_get(void){assertRootBody(); return OMEGA.getScene()->initializers;}
	void initializers_set(const vector<shared_ptr<Engine> >& egs){assertRootBody(); OMEGA.getScene()->initializers.clear(); FOREACH(const shared_ptr<Engine>& e, egs) OMEGA.getScene()->initializers.push_back(e); mapLabeledEntitiesToVariables(); OMEGA.getScene()->needsInitializers=true; }

	python::object labeled_engine_get(string label){
		FOREACH(const shared_ptr<Engine>& eng, OMEGA.getScene()->engines){
			if(eng->label==label){ return python::object(eng); }
			shared_ptr<Dispatcher> me=dynamic_pointer_cast<Dispatcher>(eng);
			if(me){
				FOREACH(const shared_ptr<Functor>& eu, me->functorArguments){
					if(eu->label==label) return python::object(eu);
				}
			}
			shared_ptr<InteractionDispatchers> ee=dynamic_pointer_cast<InteractionDispatchers>(eng);
			if(ee){
				list<shared_ptr<Functor> > eus;
				FOREACH(const shared_ptr<Functor>& eu,ee->geomDispatcher->functorArguments) eus.push_back(eu);
				FOREACH(const shared_ptr<Functor>& eu,ee->physDispatcher->functorArguments) eus.push_back(eu);
				FOREACH(const shared_ptr<Functor>& eu,ee->lawDispatcher->functorArguments) eus.push_back(eu);
				FOREACH(const shared_ptr<Functor>& eu,eus){
					if(eu->label==label) return python::object(eu);
				}
			}
		}
		throw std::invalid_argument(string("No engine labeled `")+label+"'");
	}
	
	pyBodyContainer bodies_get(void){assertRootBody(); return pyBodyContainer(OMEGA.getScene()->bodies); }
	pyInteractionContainer interactions_get(void){assertRootBody(); return pyInteractionContainer(OMEGA.getScene()->interactions); }
	
	pyBexContainer bex_get(void){return pyBexContainer();}
	pyMaterialContainer materials_get(void){return pyMaterialContainer();}
	

	python::list listChildClassesNonrecursive(const string& base){
		python::list ret;
		for(map<string,DynlibDescriptor>::const_iterator di=Omega::instance().getDynlibsDescriptor().begin();di!=Omega::instance().getDynlibsDescriptor().end();++di) if (Omega::instance().isInheritingFrom((*di).first,base)) ret.append(di->first);
		return ret;
	}

	bool isChildClassOf(const string& child, const string& base){
		return (Omega::instance().isInheritingFrom(child,base));
	}

	python::list plugins_get(){
		const map<string,DynlibDescriptor>& plugins=Omega::instance().getDynlibsDescriptor();
		std::pair<string,DynlibDescriptor> p; python::list ret;
		FOREACH(p, plugins) ret.append(p.first);
		return ret;
	}

	pyTags tags_get(void){assertRootBody(); return pyTags(OMEGA.getScene());}

	void interactionContainer_set(string clss){
		Scene* rb=OMEGA.getScene().get();
		if(rb->interactions->size()>0) throw std::runtime_error("Interaction container not empty, will not change its class.");
		shared_ptr<InteractionContainer> ic=dynamic_pointer_cast<InteractionContainer>(ClassFactory::instance().createShared(clss));
		rb->interactions=ic;
	}
	string interactionContainer_get(string clss){ return OMEGA.getScene()->interactions->getClassName(); }

	void bodyContainer_set(string clss){
		Scene* rb=OMEGA.getScene().get();
		if(rb->bodies->size()>0) throw std::runtime_error("Body container not empty, will not change its class.");
		shared_ptr<BodyContainer> bc=dynamic_pointer_cast<BodyContainer>(ClassFactory::instance().createShared(clss));
		rb->bodies=bc;
	}
	string bodyContainer_get(string clss){ return OMEGA.getScene()->bodies->getClassName(); }
	#ifdef YADE_OPENMP
		int numThreads_get(){ return omp_get_max_threads();}
		void numThreads_set(int n){ int bcn=OMEGA.getScene()->bex.getNumAllocatedThreads(); if(bcn<n) LOG_WARN("BexContainer has only "<<bcn<<" threads allocated. Changing thread number to on "<<bcn<<" instead of "<<n<<" requested."); omp_set_num_threads(min(n,bcn)); LOG_WARN("BUG: Omega().numThreads=n doesn't work as expected (number of threads is not changed globally). Set env var OMP_NUM_THREADS instead."); }
	#else
		int numThreads_get(){return 1;}
		void numThreads_set(int n){ LOG_WARN("Yade was compiled without openMP support, changing number of threads will have no effect."); }
	#endif
	#ifdef YADE_BOOST_SERIALIZATION
	void saveXML(string filename){
		std::ofstream ofs(filename.c_str());
		boost::archive::xml_oarchive oa(ofs);
		shared_ptr<Scene> YadeSimulation=OMEGA.getScene();
		oa << YadeSimulation;
	}
	#endif
	void periodicCell_set(python::tuple& t){
		if(python::len(t)==2){ OMEGA.getScene()->cellMin=python::extract<Vector3r>(t[0]); OMEGA.getScene()->cellMax=python::extract<Vector3r>(t[1]); OMEGA.getScene()->isPeriodic=true; }
		else if (python::len(t)==0) {OMEGA.getScene()->isPeriodic=false; }
		else throw invalid_argument("Must pass either 2 Vector3's to set periodic cell,  or () to disable periodicity (got "+lexical_cast<string>(python::len(t))+" instead).");
	}
	python::tuple periodicCell_get(){
		vector<Vector3r> ret;
		if(OMEGA.getScene()->isPeriodic){ return python::make_tuple(OMEGA.getScene()->cellMin,OMEGA.getScene()->cellMax); }
		return python::make_tuple();
	}
	void disableGdb(){
		signal(SIGSEGV,SIG_DFL);
		signal(SIGABRT,SIG_DFL);
	}
	void exitNoBacktrace(int status=0){
		if(status==0) signal(SIGSEGV,termHandlerNormal); /* unset the handler that runs gdb and prints backtrace */
		else signal(SIGSEGV,termHandlerError);
		exit(status);
	}
	void runEngine(const shared_ptr<Engine>& e){ e->action(OMEGA.getScene().get()); }
};
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(omega_run_overloads,run,0,2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(omega_saveTmp_overloads,saveTmp,0,1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(omega_loadTmp_overloads,loadTmp,0,1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(omega_exitNoBacktrace_overloads,exitNoBacktrace,0,1);

class pySTLImporter : public STLImporter {
    public:
	void py_import(pyBodyContainer bc) { import(bc.proxee); }
};


/*****************************************************************************
********** New helper functions */
void Serializable_updateAttrs(const shared_ptr<Serializable>& self, const python::dict& d){
	python::list l=d.items(); size_t ll=python::len(l);
	for(size_t i=0; i<ll; i++){
		python::tuple t=python::extract<python::tuple>(l[i]); string key=python::extract<string>(t[0]); self->pySetAttr(key,t[1]);
	}
}
python::list Serializable_updateExistingAttrs(const shared_ptr<Serializable>& self, const python::dict& d){
	python::list ret; python::list l=d.items(); size_t ll=python::len(l);
	for(size_t i=0; i<ll; i++){
		python::tuple t=python::extract<python::tuple>(l[i]); string key=python::extract<string>(t[0]);
		if(self->pyHasKey(key)) self->pySetAttr(key,t[1]); else ret.append(t[0]);
	}
	return ret; 
}
std::string Serializable_pyStr(const shared_ptr<Serializable>& self) {
	return "<"+self->getClassName()+" instance at "+lexical_cast<string>(self.get())+">";
}

python::list TimingDeltas_pyData(const shared_ptr<TimingDeltas> self){
	python::list ret;
	for(size_t i=0; i<self->data.size(); i++){ ret.append(python::make_tuple(self->labels[i],self->data[i].nsec,self->data[i].nExec));}
	return ret;
}

TimingInfo::delta Engine_timingInfo_nsec_get(const shared_ptr<Engine>& e){return e->timingInfo.nsec;}; void Engine_timingInfo_nsec_set(const shared_ptr<Engine>& e, TimingInfo::delta d){ e->timingInfo.nsec=d;}
long Engine_timingInfo_nExec_get(const shared_ptr<Engine>& e){return e->timingInfo.nExec;}; void Engine_timingInfo_nExec_set(const shared_ptr<Engine>& e, long d){ e->timingInfo.nExec=d;}

template <typename T>
shared_ptr<T> Serializable_ctor_kwAttrs(const python::tuple& t, const python::dict& d){
	if(python::len(t)>1) throw runtime_error("Zero or one (and not more) non-keyword string argument required");
	string clss;
	if(python::len(t)==1){
		python::extract<string> clss_(t[0]); if(!clss_.check()) throw runtime_error("First argument (if given) must be a string.");
		clss=clss_();
	}
	shared_ptr<T> instance;
	if(clss.empty()){ instance=shared_ptr<T>(new T); }
	else{
		shared_ptr<Factorable> instance0=ClassFactory::instance().createShared(clss);
		if(!instance0) throw runtime_error("Invalid class `"+clss+"' (not created by ClassFactory).");
		instance=dynamic_pointer_cast<T>(instance0);
		if(!instance) throw runtime_error("Invalid class `"+clss+"' (unable to cast to typeid `"+typeid(T).name()+"')");
	}
	Serializable_updateAttrs(instance,d);
	return instance;
}

// FIXME: those could be moved to the c++ classes themselves, right?
template<typename DispatcherT, typename functorT>
shared_ptr<DispatcherT> Dispatcher_ctor_list(const std::vector<shared_ptr<functorT> >& functors){
	shared_ptr<DispatcherT> instance(new DispatcherT);
	FOREACH(shared_ptr<functorT> functor,functors) instance->add(functor);
	return instance;
}
// FIXME: this one as well
template<typename DispatcherT, typename functorT>
std::vector<shared_ptr<functorT> > Dispatcher_functors_get(shared_ptr<DispatcherT> self){
	std::vector<shared_ptr<functorT> > ret;
	FOREACH(const shared_ptr<Functor>& functor, self->functorArguments){ shared_ptr<functorT> functorRightType(dynamic_pointer_cast<functorT>(functor)); if(!functorRightType) throw logic_error("Internal error: Dispatcher of type "+self->getClassName()+" did not contain Functor of the required type "+typeid(functorT).name()+"?"); ret.push_back(functorRightType); }
	return ret;
}
// FIXME: and this one as well
shared_ptr<InteractionDispatchers> InteractionDispatchers_ctor_lists(const std::vector<shared_ptr<InteractionGeometryFunctor> >& gff, const std::vector<shared_ptr<InteractionPhysicsFunctor> >& pff, const std::vector<shared_ptr<LawFunctor> >& cff){
	shared_ptr<InteractionDispatchers> instance(new InteractionDispatchers);
	FOREACH(shared_ptr<InteractionGeometryFunctor> gf, gff) instance->geomDispatcher->add(gf);
	FOREACH(shared_ptr<InteractionPhysicsFunctor> pf, pff) instance->physDispatcher->add(pf);
	FOREACH(shared_ptr<LawFunctor> cf, cff) instance->lawDispatcher->add(cf);
	return instance;
}

template<typename someIndexable>
int Indexable_getClassIndex(const shared_ptr<someIndexable> i){return i->getClassIndex();}

// ParallelEngine
void ParallelEngine_slaves_set(shared_ptr<ParallelEngine> self, const python::list& slaves){
	int len=python::len(slaves);
	self->slaves=ParallelEngine::slaveContainer(); // empty the container
	for(int i=0; i<len; i++){
		python::extract<std::vector<shared_ptr<Engine> > > serialGroup(slaves[i]);
		if (serialGroup.check()){ self->slaves.push_back(serialGroup()); continue; }
		python::extract<shared_ptr<Engine> > serialAlone(slaves[i]);
		if (serialAlone.check()){ vector<shared_ptr<Engine> > aloneWrap; aloneWrap.push_back(serialAlone()); self->slaves.push_back(aloneWrap); continue; }
		PyErr_SetString(PyExc_TypeError,"List elements must be either\n (a) sequences of engines to be executed one after another\n(b) alone engines.");
		python::throw_error_already_set();
	}
}
python::list ParallelEngine_slaves_get(shared_ptr<ParallelEngine> self){
	python::list ret;
	FOREACH(vector<shared_ptr<Engine > >& grp, self->slaves){
		if(grp.size()==1) ret.append(python::object(grp[0]));
		else ret.append(python::object(grp));
	}
	return ret;
}
shared_ptr<ParallelEngine> ParallelEngine_ctor_list(const python::list& slaves){ shared_ptr<ParallelEngine> instance(new ParallelEngine); ParallelEngine_slaves_set(instance,slaves); return instance; }

// injected methods
Vector3r State_displ_get(const shared_ptr<State>& pp){return pp->pos-pp->refPos;}
Vector3r State_rot_get  (const shared_ptr<State>& pp){Quaternionr relRot=pp->refOri.Conjugate()*pp->ori; Vector3r axis; Real angle; relRot.ToAxisAngle(axis,angle); return axis*angle;  }
Vector3r State_pos_get(const shared_ptr<State>& pp){return pp->pos;}
Quaternionr State_ori_get(const shared_ptr<State>& pp){return pp->ori;}
void State_pos_set(const shared_ptr<State>& pp, const Vector3r& p){ pp->pos=p; }
void State_ori_set(const shared_ptr<State>& pp, const Quaternionr& p){ pp->ori=p; }
//Vector3r State_refPos_get(const shared_ptr<State>& pp){return pp->refPos;}
//void State_refPos_set(const shared_ptr<State>& pp, const Vector3r& p){ pp->refPos=p; }

shared_ptr<Shape> Body_shape_deprec_get(const shared_ptr<Body>& b){ LOG_WARN("Body().mold and Body().geom attributes are deprecated, use 'shape' instead."); return b->shape; }
void Body_shape_deprec_set(const shared_ptr<Body>& b, shared_ptr<Shape> ig){ LOG_WARN("Body().mold and Body().geom attributes are deprecated, use 'shape' instead."); b->shape=ig; }


long Interaction_getId1(const shared_ptr<Interaction>& i){ return (long)i->getId1(); }
long Interaction_getId2(const shared_ptr<Interaction>& i){ return (long)i->getId2(); }
python::tuple Interaction_getCellDist(const shared_ptr<Interaction>& i){ return python::make_tuple(i->cellDist[0],i->cellDist[1],i->cellDist[2]); }

void FileGenerator_generate(const shared_ptr<FileGenerator>& fg, string outFile){ fg->setFileName(outFile); fg->setSerializationLibrary("XMLFormatManager"); bool ret=fg->generateAndSave(); LOG_INFO((ret?"SUCCESS:\n":"FAILURE:\n")<<fg->message); if(ret==false) throw runtime_error("Generator reported error: "+fg->message); };
void FileGenerator_load(const shared_ptr<FileGenerator>& fg){ string xml(Omega::instance().tmpFilename()+".xml.bz2"); LOG_DEBUG("Using temp file "<<xml); FileGenerator_generate(fg,xml); pyOmega().load(xml); }

// many thanks to http://markmail.org/message/s4ksg6nfspw2wxwd
namespace boost { namespace python { namespace detail {
	template <class F> struct raw_constructor_dispatcher{
		raw_constructor_dispatcher(F f): f(make_constructor(f)) {}
		PyObject* operator()(PyObject* args, PyObject* keywords)
		{
			 borrowed_reference_t* ra = borrowed_reference(args); object a(ra);
			 return incref(object(f(object(a[0]),object(a.slice(1,len(a))),keywords ? dict(borrowed_reference(keywords)) : dict())).ptr() );
		}
		private: object f;
	};
	}
	template <class F> object raw_constructor(F f, std::size_t min_args = 0){
		return detail::make_raw_function(objects::py_function(detail::raw_constructor_dispatcher<F>(f),mpl::vector2<void, object>(),min_args+1,(std::numeric_limits<unsigned>::max)()));
	}
}} // namespace boost::python

BOOST_PYTHON_MODULE(wrapper)
{
	python::scope().attr("__doc__")="Wrapper for c++ internals of yade.";
	python::class_<pyOmega>("Omega")
		.add_property("iter",&pyOmega::iter,"Get current step number")
		.add_property("stopAtIter",&pyOmega::stopAtIter_get,&pyOmega::stopAtIter_set,"Get/set number of iteration after which the simulation will stop.")
		.add_property("time",&pyOmega::simulationTime,"Return virtual (model world) time of the simulation.")
		.add_property("realtime",&pyOmega::realTime,"Return clock (human world) time the simulation has been running.")
		.add_property("dt",&pyOmega::dt_get,&pyOmega::dt_set,"Current timestep (assignable); note that assigning to dt will not turn of TimeStepper, if used.")
		.add_property("usesTimeStepper",&pyOmega::usesTimeStepper_get,&pyOmega::usesTimeStepper_set,"Enable/disable use of TimeStepper in the simulation")
		.def("load",&pyOmega::load,"Load simulation from file.")
		.def("reload",&pyOmega::reload,"Reload current simulation")
		.def("save",&pyOmega::save,"Save current simulation to file (should be .xml or .xml.bz2)")
		.def("loadTmp",&pyOmega::loadTmp,omega_loadTmp_overloads(python::args("mark"),"Load simulation previously stored in memory by saveTmp.\n @param mark optionally distinguishes multiple saved simulations"))
		.def("saveTmp",&pyOmega::saveTmp,omega_saveTmp_overloads(python::args("mark"),"Save simulation to memory (disappears at shutdown), can be loaded later with loadTmp.\n @param mark optionally distinguishes different memory-saved simulations."))
		.def("tmpToFile",&pyOmega::tmpToFile,"Return XML of saveTmp'd simulation as string.")
		.def("tmpToString",&pyOmega::tmpToString,"Save XML of saveTmp'd simulation in file.")
		.def("saveSpheres",&pyOmega::saveSpheres,"Saves spherical bodies to external ASCII file, one sphere (x y z r) per line.")
		.def("run",&pyOmega::run,omega_run_overloads("Run the simulation.\n@param nSteps how many steps to run, then stop.\n@param wait if True, doesn't return until the simulation will have stopped."))
		.def("pause",&pyOmega::pause,"Stop simulation execution.\n(may be called from within the loop, and it will stop after the current step).")
		.def("step",&pyOmega::step,"Advance the simulation by one step. Returns after the step will have finished.")
		.def("wait",&pyOmega::wait,"Don't return until the simulation will have been paused. (Returns immediately if not running).")
		.def("reset",&pyOmega::reset,"Reset simulations completely (including another scene!).")
		.def("resetThisScene",&pyOmega::resetThisScene,"Reset current scene.")
		.def("switchScene",&pyOmega::switchScene,"Switch to alternative simulation (while keeping the old one). Calling the function again switches back to the first one. Note that most variables from the first simulation will still refer to the first simulation even after the switch\n(e.g. b=O.bodies[4]; O.switchScene(); [b still refers to the body in the first simulation here])")
		.def("labeledEngine",&pyOmega::labeled_engine_get,"Return instance of engine/functor with the given label. This function shouldn't be called by the user directly; every ehange in O.engines will assign respective global python variables according to labels.\n For example: \n\tO.engines=[InsertionSortCollider(label='collider')]\n\tcollider['nBins']=5 ## collider has become a variable after assignment to O.engines automatically)")
		.def("resetTime",&pyOmega::resetTime,"Reset simulation time: step number, virtual and real time. (Doesn't touch anything else, including timings).")
		.def("plugins",&pyOmega::plugins_get,"Return list of all plugins registered in the class factory.")
		.add_property("engines",&pyOmega::engines_get,&pyOmega::engines_set,"List of engines in the simulation (Scene::engines).")
		.add_property("miscParams",&pyOmega::miscParams_get,&pyOmega::miscParams_set,"MiscParams in the simulation (Scene::mistParams), usually used to save serializables that don't fit anywhere else, like GL functors")
		.add_property("initializers",&pyOmega::initializers_get,&pyOmega::initializers_set,"List of initializers (Scene::initializers).")
		.add_property("bodies",&pyOmega::bodies_get,"Bodies in the current simulation (container supporting index access by id and iteration)")
		.add_property("interactions",&pyOmega::interactions_get,"Interactions in the current simulation (container supporting index acces by either (id1,id2) or interactionNumber and iteration)")
		.add_property("materials",&pyOmega::materials_get,"Shared materials; they can be accessed by id or by label")
		.add_property("actions",&pyOmega::bex_get,"Deprecated alias for Omega().bex")
		.add_property("bex",&pyOmega::bex_get,"BodyExternalVariables (forces, torques, ..) in  the current simulation.")
		.add_property("tags",&pyOmega::tags_get,"Tags (string=string dictionary) of the current simulation (container supporting string-index access/assignment)")
		.def("childClassesNonrecursive",&pyOmega::listChildClassesNonrecursive,"Return list of all classes deriving from given class, as registered in the class factory")
		.def("isChildClassOf",&pyOmega::isChildClassOf,"Tells whether the first class derives from the second one (both given as strings).")
		.add_property("bodyContainer",&pyOmega::bodyContainer_get,&pyOmega::bodyContainer_set,"Get/set type of body container (as string); there must be no bodies.")
		.add_property("interactionContainer",&pyOmega::interactionContainer_get,&pyOmega::interactionContainer_set,"Get/set type of interaction container (as string); there must be no interactions.")
		.add_property("timingEnabled",&pyOmega::timingEnabled_get,&pyOmega::timingEnabled_set,"Globally enable/disable timing services (see documentation of yade.timing).")
		.add_property("bexSyncCount",&pyOmega::bexSyncCount_get,&pyOmega::bexSyncCount_set,"Counter for number of syncs in BexContainer, for profiling purposes.")
		.add_property("numThreads",&pyOmega::numThreads_get /* ,&pyOmega::numThreads_set*/ ,"Get maximum number of threads openMP can use.")
		.add_property("periodicCell",&pyOmega::periodicCell_get,&pyOmega::periodicCell_set, "Get/set periodic cell minimum and maximum (tuple of 2 Vector3's), or () for no periodicity.")
		.def("exitNoBacktrace",&pyOmega::exitNoBacktrace,omega_exitNoBacktrace_overloads("Disable SEGV handler and exit."))
		.def("disableGdb",&pyOmega::disableGdb,"Revert SEGV and ABRT handlers to system defaults.")
		#ifdef YADE_BOOST_SERIALIZATION
			.def("saveXML",&pyOmega::saveXML,"[EXPERIMENTAL] function saving to XML file using boost::serialization.")
		#endif
		.def("runEngine",&pyOmega::runEngine,"Run given engine exactly once; simulation time, step number etc. will not be incremented (use only if you know what you do).")
		;
	python::class_<pyTags>("TagsWrapper",python::init<pyTags&>())
		.def("__getitem__",&pyTags::getItem)
		.def("__setitem__",&pyTags::setItem)
		.def("keys",&pyTags::keys)
		.def("has_key",&pyTags::hasKey);
	python::class_<pyBodyContainer>("BodyContainer",python::init<pyBodyContainer&>())
		.def("__getitem__",&pyBodyContainer::pyGetitem)
		.def("__len__",&pyBodyContainer::length)
		.def("__iter__",&pyBodyContainer::pyIter)
		.def("append",&pyBodyContainer::append,"Append one Body instance, return its id.")
		.def("append",&pyBodyContainer::appendList,"Append list of Body instance, return list of ids")
		.def("appendClumped",&pyBodyContainer::appendClump,"Append given list of bodies as a clump (rigid aggregate); return list of ids.")
		.def("clear", &pyBodyContainer::clear,"Remove all bodies (interactions not checked)")
		.def("erase", &pyBodyContainer::erase,"Erase body with the given id; all interaction will be deleted by InteractionDispatchers in the next step.")
		.def("replace",&pyBodyContainer::replace);
	python::class_<pyBodyIterator>("BodyIterator",python::init<pyBodyIterator&>())
		.def("__iter__",&pyBodyIterator::pyIter)
		.def("next",&pyBodyIterator::pyNext);
	python::class_<pyInteractionContainer>("InteractionContainer",python::init<pyInteractionContainer&>())
		.def("__iter__",&pyInteractionContainer::pyIter)
		.def("__getitem__",&pyInteractionContainer::pyGetitem)
		.def("__len__",&pyInteractionContainer::len)
		.def("countReal",&pyInteractionContainer::countReal,"Return number of interactions that are \"real\", i.e. they have phys and geom.")
		.def("nth",&pyInteractionContainer::pyNth,"Return n-th interaction from the container (usable for picking random interaction).")
		.def("withBody",&pyInteractionContainer::withBody,"Return list of real interactions of given body.")
		.def("withBodyAll",&pyInteractionContainer::withBodyAll,"Return list of all (real as well as non-real) interactions of given body.")
		.add_property("serializeSorted",&pyInteractionContainer::serializeSorted_get,&pyInteractionContainer::serializeSorted_set)
		.def("clear",&pyInteractionContainer::clear,"Remove all interactions");
	python::class_<pyInteractionIterator>("InteractionIterator",python::init<pyInteractionIterator&>())
		.def("__iter__",&pyInteractionIterator::pyIter)
		.def("next",&pyInteractionIterator::pyNext);

	python::class_<pyBexContainer>("BexContainer",python::init<pyBexContainer&>())
		.def("f",&pyBexContainer::force_get)
		.def("t",&pyBexContainer::torque_get)
		.def("m",&pyBexContainer::torque_get)
		.def("move",&pyBexContainer::move_get)
		.def("rot",&pyBexContainer::rot_get)
		.def("addF",&pyBexContainer::force_add)
		.def("addT",&pyBexContainer::torque_add)
		.def("addMove",&pyBexContainer::move_add)
		.def("addRot",&pyBexContainer::rot_add);

	python::class_<pyMaterialContainer>("MaterialContainer",python::init<pyMaterialContainer&>())
		.def("append",&pyMaterialContainer::append,"Add new shared material; changes its id and return it.")
		.def("append",&pyMaterialContainer::appendList,"Append list of Material instances, return list of ids.")
		.def("__getitem__",&pyMaterialContainer::getitem_id)
		.def("__getitem__",&pyMaterialContainer::getitem_label)
		.def("__len__",&pyMaterialContainer::len);

	python::class_<pySTLImporter>("STLImporter")
	    .def("open",&pySTLImporter::open)
	    .add_property("number_of_facets",&pySTLImporter::number_of_facets)
	    .def("import_geometry",&pySTLImporter::py_import);

//////////////////////////////////////////////////////////////
///////////// proxyless wrappers 

	python::class_<Serializable, shared_ptr<Serializable>, noncopyable >("Serializable")
		.add_property("name",&Serializable::getClassName).def("__str__",&Serializable_pyStr).def("__repr__",&Serializable_pyStr).def("postProcessAttributes",&Serializable::postProcessAttributes)
		.def("dict",&Serializable::pyDict).def("__getitem__",&Serializable::pyGetAttr).def("__setitem__",&Serializable::pySetAttr).def("has_key",&Serializable::pyHasKey).def("keys",&Serializable::pyKeys)
		.def("updateAttrs",&Serializable_updateAttrs).def("updateExistingAttrs",&Serializable_updateExistingAttrs)
		.def("__init__",python::raw_constructor(Serializable_ctor_kwAttrs<Serializable>))
		// aliases for __getitem__ and __setitem__, but they are used by the property generator code and can be useful if we deprecate the object['attr'] type of access
		.def("_prop_get",&Serializable::pyGetAttr).def("_prop_set",&Serializable::pySetAttr)
		;
	python::class_<Engine, shared_ptr<Engine>, python::bases<Serializable>, noncopyable >("Engine",python::no_init)
		.add_property("execTime",&Engine_timingInfo_nsec_get,&Engine_timingInfo_nsec_set)
		.add_property("execCount",&Engine_timingInfo_nExec_get,&Engine_timingInfo_nExec_set)
		.def_readonly("timingDeltas",&Engine::timingDeltas);
	python::class_<StandAloneEngine,shared_ptr<StandAloneEngine>, python::bases<Engine>, noncopyable>("StandAloneEngine").def("__init__",python::raw_constructor(Serializable_ctor_kwAttrs<StandAloneEngine>));
	python::class_<DeusExMachina,shared_ptr<DeusExMachina>, python::bases<Engine>, noncopyable>("DeusExMachina").def("__init__",python::raw_constructor(Serializable_ctor_kwAttrs<DeusExMachina>));
	python::class_<Functor, shared_ptr<Functor>, python::bases<Serializable>, noncopyable >("Functor",python::no_init)
		.def_readonly("timingDeltas",&Functor::timingDeltas)
		.add_property("bases",&Functor::getFunctorTypes);
	python::class_<Dispatcher, shared_ptr<Dispatcher>, python::bases<Engine>, noncopyable>("Dispatcher",python::no_init);
	python::class_<TimingDeltas, shared_ptr<TimingDeltas>, noncopyable >("TimingDeltas").add_property("data",&TimingDeltas_pyData).def("reset",&TimingDeltas::reset);

	python::class_<InteractionDispatchers,shared_ptr<InteractionDispatchers>, python::bases<Engine>, noncopyable >("InteractionDispatchers")
		.def("__init__",python::make_constructor(InteractionDispatchers_ctor_lists))
		.def_readonly("geomDispatcher",&InteractionDispatchers::geomDispatcher)
		.def_readonly("physDispatcher",&InteractionDispatchers::physDispatcher)
		.def_readonly("lawDispatcher",&InteractionDispatchers::lawDispatcher);
	python::class_<ParallelEngine,shared_ptr<ParallelEngine>, python::bases<Engine>, noncopyable>("ParallelEngine")
		.def("__init__",python::make_constructor(ParallelEngine_ctor_list))
		.add_property("slaves",&ParallelEngine_slaves_get,&ParallelEngine_slaves_set);

	#define EXPOSE_DISPATCHER(DispatcherT,functorT) python::class_<DispatcherT, shared_ptr<DispatcherT>, python::bases<Dispatcher>, noncopyable >(#DispatcherT).def("__init__",python::make_constructor(Dispatcher_ctor_list<DispatcherT,functorT>)).add_property("functors",&Dispatcher_functors_get<DispatcherT,functorT>).def("dump",&DispatcherT::dump);
		EXPOSE_DISPATCHER(BoundDispatcher,BoundFunctor)
		EXPOSE_DISPATCHER(InteractionGeometryDispatcher,InteractionGeometryFunctor)
		EXPOSE_DISPATCHER(InteractionPhysicsDispatcher,InteractionPhysicsFunctor)
		#ifdef YADE_PHYSPAR
			EXPOSE_DISPATCHER(StateMetaEngine,StateEngineUnit)
			EXPOSE_DISPATCHER(PhysicalActionDamper,PhysicalActionDamperUnit)
			EXPOSE_DISPATCHER(PhysicalActionApplier,PhysicalActionApplierUnit)
		#endif
		EXPOSE_DISPATCHER(LawDispatcher,LawFunctor)
	#undef EXPOSE_DISPATCHER

	#define EXPOSE_FUNCTOR(FunctorT) python::class_<FunctorT, shared_ptr<FunctorT>, python::bases<Functor>, noncopyable>(#FunctorT).def("__init__",python::raw_constructor(Serializable_ctor_kwAttrs<FunctorT>));
		EXPOSE_FUNCTOR(BoundFunctor)
		EXPOSE_FUNCTOR(InteractionGeometryFunctor)
		EXPOSE_FUNCTOR(InteractionPhysicsFunctor)
		#ifdef YADE_PHYSPAR
			EXPOSE_FUNCTOR(StateEngineUnit)
			EXPOSE_FUNCTOR(PhysicalActionDamperUnit)
			EXPOSE_FUNCTOR(PhysicalActionApplierUnit)
		#endif
		EXPOSE_FUNCTOR(LawFunctor)
	#undef EXPOSE_FUNCTOR
		
	#define EXPOSE_CXX_CLASS_RENAMED(cxxName,pyName) python::class_<cxxName,shared_ptr<cxxName>, python::bases<Serializable>, noncopyable>(#pyName).def("__init__",python::raw_constructor(Serializable_ctor_kwAttrs<cxxName>))
	#define EXPOSE_CXX_CLASS(className) EXPOSE_CXX_CLASS_RENAMED(className,className)
	// expose indexable class, with access to the index
	#define EXPOSE_CXX_CLASS_IX(className) EXPOSE_CXX_CLASS(className).add_property("classIndex",&Indexable_getClassIndex<className>)

	EXPOSE_CXX_CLASS(Body)
		// mold and geom are deprecated:
		.add_property("mold",&Body_shape_deprec_get,&Body_shape_deprec_set)
		.add_property("geom",&Body_shape_deprec_get,&Body_shape_deprec_set)
		.def_readwrite("shape",&Body::shape)
		.def_readwrite("bound",&Body::bound)
		.def_readwrite("mat",&Body::material)
		.def_readwrite("state",&Body::state)
		.def_readwrite("dynamic",&Body::isDynamic)
		.def_readonly("id",&Body::id)
		.def_readwrite("mask",&Body::groupMask)
		.add_property("isStandalone",&Body::isStandalone)
		.add_property("isClumpMember",&Body::isClumpMember)
		.add_property("isClump",&Body::isClump);
	EXPOSE_CXX_CLASS_IX(Shape);
	EXPOSE_CXX_CLASS_IX(Bound)
		.def_readonly("min",&Bound::min)
		.def_readonly("max",&Bound::max);
	EXPOSE_CXX_CLASS_IX(Material)
		.def_readwrite("label",&Material::label)
		.def("newAssocState",&Material::newAssocState)
		;
	EXPOSE_CXX_CLASS(State)
		.add_property("blockedDOFs",&State::blockedDOFs_vec_get,&State::blockedDOFs_vec_set)
		.add_property("pos",&State_pos_get,&State_pos_set)
		.add_property("ori",&State_ori_get,&State_ori_set)
		.def_readwrite("refPos",&State::refPos)
		.add_property("displ",&State_displ_get)
		.add_property("rot",&State_rot_get);
	// deprecated
	#ifdef YADE_PHYSPAR
	EXPOSE_CXX_CLASS_IX(PhysicalParameters)
		.add_property("blockedDOFs",&PhysicalParameters::blockedDOFs_vec_get,&PhysicalParameters::blockedDOFs_vec_set)
		.add_property("pos",&PhysicalParameters_pos_get,&PhysicalParameters_pos_set)
		.add_property("ori",&PhysicalParameters_ori_get,&PhysicalParameters_ori_set)
		.add_property("refPos",&PhysicalParameters_refPos_get,&PhysicalParameters_refPos_set)
		.add_property("displ",&PhysicalParameters_displ_get)
		.add_property("rot",&PhysicalParameters_rot_get);
	#endif
	// interaction
	EXPOSE_CXX_CLASS(Interaction)
		.def_readwrite("phys",&Interaction::interactionPhysics)
		.def_readwrite("geom",&Interaction::interactionGeometry)
		.add_property("id1",&Interaction_getId1)
		.add_property("id2",&Interaction_getId2)
		.add_property("isReal",&Interaction::isReal)
		.add_property("cellDist",&Interaction_getCellDist);
	EXPOSE_CXX_CLASS_IX(InteractionPhysics);
	EXPOSE_CXX_CLASS_IX(InteractionGeometry);
	EXPOSE_CXX_CLASS(FileGenerator)
		.def("generate",&FileGenerator_generate)
		.def("load",&FileGenerator_load);
	python::scope().attr("O")=pyOmega();
}

