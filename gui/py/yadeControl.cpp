// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 

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
#include<boost/python.hpp>
#include<boost/foreach.hpp>
#include<boost/algorithm/string.hpp>

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
#include<yade/pkg-common/ParallelEngine.hpp>
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
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/ParticleParameters.hpp>

#include<yade/pkg-common/BoundingVolumeEngineUnit.hpp>
#include<yade/pkg-common/GeometricalModelEngineUnit.hpp>
#include<yade/pkg-common/InteractingGeometryEngineUnit.hpp>
#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>
#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>
#include<yade/pkg-common/PhysicalParametersEngineUnit.hpp>
#include<yade/pkg-common/PhysicalActionDamperUnit.hpp>
#include<yade/pkg-common/PhysicalActionApplierUnit.hpp>

#include<yade/extra/Shop.hpp>

using namespace boost;
using namespace std;

#include"pyAttrUtils.hpp"

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

BASIC_PY_PROXY(pyGeneric,Serializable);

BASIC_PY_PROXY(pyInteractionGeometry,InteractionGeometry);
BASIC_PY_PROXY(pyInteractionPhysics,InteractionPhysics);

BASIC_PY_PROXY(pyGeometricalModel,GeometricalModel);
BASIC_PY_PROXY_HEAD(pyPhysicalParameters,PhysicalParameters)
	python::list blockedDOFs_get(){
		python::list ret;
		#define _SET_DOF(DOF_ANY,str) if((proxee->blockedDOFs & PhysicalParameters::DOF_ANY)!=0) ret.append(str);
		_SET_DOF(DOF_X,"x"); _SET_DOF(DOF_Y,"y"); _SET_DOF(DOF_Z,"z"); _SET_DOF(DOF_RX,"rx"); _SET_DOF(DOF_RY,"ry"); _SET_DOF(DOF_RZ,"rz");
		#undef _SET_DOF
		return ret;
	}
	void blockedDOFs_set(python::list l){
		proxee->blockedDOFs=PhysicalParameters::DOF_NONE;
		int len=python::len(l);
		for(int i=0; i<len; i++){
			string s=python::extract<string>(l[i])();
			#define _GET_DOF(DOF_ANY,str) if(s==str) { proxee->blockedDOFs|=PhysicalParameters::DOF_ANY; continue; }
			_GET_DOF(DOF_X,"x"); _GET_DOF(DOF_Y,"y"); _GET_DOF(DOF_Z,"z"); _GET_DOF(DOF_RX,"rx"); _GET_DOF(DOF_RY,"ry"); _GET_DOF(DOF_RZ,"rz");
			#undef _GET_DOF
			throw std::invalid_argument("Invalid  DOF specification `"+s+"', must be ∈{x,y,z,rx,ry,rz}.");
		}
	}
	python::tuple pos_get(){const Vector3r& p=proxee->se3.position; return python::make_tuple(p[0],p[1],p[2]);}
	python::tuple refPos_get(){const Vector3r& p=proxee->refSe3.position; return python::make_tuple(p[0],p[1],p[2]);}
	python::tuple ori_get(){Vector3r axis; Real angle; proxee->se3.orientation.ToAxisAngle(axis,angle); return python::make_tuple(axis[0],axis[1],axis[2],angle);}
	void pos_set(python::list l){if(python::len(l)!=3) throw invalid_argument("Wrong number of vector3 elements "+lexical_cast<string>(python::len(l))+", should be 3"); proxee->se3.position=Vector3r(python::extract<double>(l[0])(),python::extract<double>(l[1])(),python::extract<double>(l[2])());}
	void refPos_set(python::list l){if(python::len(l)!=3) throw invalid_argument("Wrong number of vector3 elements "+lexical_cast<string>(python::len(l))+", should be 3"); proxee->refSe3.position=Vector3r(python::extract<double>(l[0])(),python::extract<double>(l[1])(),python::extract<double>(l[2])());}
	void ori_set(python::list l){if(python::len(l)!=4) throw invalid_argument("Wrong number of quaternion elements "+lexical_cast<string>(python::len(l))+", should be 4"); proxee->se3.orientation=Quaternionr(Vector3r(python::extract<double>(l[0])(),python::extract<double>(l[1])(),python::extract<double>(l[2])()),python::extract<double>(l[3])());}
BASIC_PY_PROXY_TAIL;

BASIC_PY_PROXY(pyBoundingVolume,BoundingVolume);
BASIC_PY_PROXY(pyInteractingGeometry,InteractingGeometry);

BASIC_PY_PROXY(pyDeusExMachina,DeusExMachina);
BASIC_PY_PROXY(pyStandAloneEngine,StandAloneEngine);

python::list anyEngines_get(const vector<shared_ptr<Engine> >&);
void anyEngines_set(vector<shared_ptr<Engine> >&, python::object);

BASIC_PY_PROXY_HEAD(pyParallelEngine,ParallelEngine)
	pyParallelEngine(python::list slaves){init("ParallelEngine"); slaves_set(slaves);}
	void slaves_set(python::list slaves){
		ensureAcc(); shared_ptr<ParallelEngine> me=dynamic_pointer_cast<ParallelEngine>(proxee); if(!me) throw runtime_error("Proxied class not a ParallelEngine. (WTF?)");
		int len=PySequence_Size(slaves.ptr()); /*[boost1.34] python::len(ftrs)*/;
		me->slaves=ParallelEngine::slaveContainer(); // empty the container
		for(int i=0; i<len; i++){
			python::extract<python::list> grpMaybe(slaves[i]);
			python::list grpList;
			if(grpMaybe.check()){ grpList=grpMaybe(); }
			else{ /* we got a standalone thing; let's wrap it in list */ grpList.append(slaves[i]); }
			vector<shared_ptr<Engine> > grpVec;
			anyEngines_set(grpVec,grpList);
			me->slaves.push_back(grpVec);
		}
	}
	python::list slaves_get(void){	
		ensureAcc(); shared_ptr<ParallelEngine> me=dynamic_pointer_cast<ParallelEngine>(proxee); if(!me) throw runtime_error("Proxied class not a ParallelEngine. (WTF?)");
		python::list ret;
		FOREACH(vector<shared_ptr<Engine > >& grp, me->slaves){
			python::list rret=anyEngines_get(grp);
			if(PySequence_Size(rret.ptr())==1){ ret.append(rret[0]); } else ret.append(rret);
		}
		return ret;
	}
BASIC_PY_PROXY_TAIL;


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

python::list anyEngines_get(const vector<shared_ptr<Engine> >& engContainer){
	python::list ret; 
	FOREACH(const shared_ptr<Engine>& eng, engContainer){
		#define APPEND_ENGINE_IF_POSSIBLE(engineType,pyEngineType) { shared_ptr<engineType> e=dynamic_pointer_cast<engineType>(eng); if(e) { ret.append(pyEngineType(e)); continue; } }
		APPEND_ENGINE_IF_POSSIBLE(MetaEngine,pyMetaEngine); APPEND_ENGINE_IF_POSSIBLE(StandAloneEngine,pyStandAloneEngine); APPEND_ENGINE_IF_POSSIBLE(DeusExMachina,pyDeusExMachina); APPEND_ENGINE_IF_POSSIBLE(ParallelEngine,pyParallelEngine); 
		throw std::runtime_error("Unknown engine type: `"+eng->getClassName()+"' (only MetaEngine, StandAloneEngine, DeusExMachina and ParallelEngine are supported)");
	}
	return ret;
}

void anyEngines_set(vector<shared_ptr<Engine> >& engContainer, python::object egs){
	int len=PySequence_Size(egs.ptr()) /*[boost1.34] python::len(egs)*/;
	//const shared_ptr<MetaBody>& rootBody=OMEGA.getRootBody(); rootBody->engines.clear();
	engContainer.clear();
	for(int i=0; i<len; i++){
		#define PUSH_BACK_ENGINE_IF_POSSIBLE(pyEngineType) if(python::extract<pyEngineType>(PySequence_GetItem(egs.ptr(),i)).check()){ pyEngineType e=python::extract<pyEngineType>(PySequence_GetItem(egs.ptr(),i)); engContainer.push_back(e.proxee); /* cerr<<"added "<<e.pyStr()<<", a "<<#pyEngineType<<endl; */ continue; }
		PUSH_BACK_ENGINE_IF_POSSIBLE(pyStandAloneEngine); PUSH_BACK_ENGINE_IF_POSSIBLE(pyMetaEngine); PUSH_BACK_ENGINE_IF_POSSIBLE(pyDeusExMachina); PUSH_BACK_ENGINE_IF_POSSIBLE(pyParallelEngine);
		throw std::runtime_error("Encountered unknown engine type (unable to extract from python object)");
	}
}



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
	int mask_get(){ensureAcc(); return proxee->groupMask;}
	void mask_set(int m){ensureAcc(); proxee->groupMask=m;}
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
	python::list insertList(python::list bb){python::list ret; for(int i=0; i<len(bb); i++){ret.append(insert(python::extract<pyBody>(bb[i])()));} return ret;}
	python::list replace(python::list bb){proxee->clear(); return insertList(bb);}
	long length(){return proxee->size();}
	void clear(){proxee->clear();}
};


class pyTags{
	public:
		pyTags(const shared_ptr<MetaBody> _mb): mb(_mb){}
		const shared_ptr<MetaBody> mb;
		bool hasKey(string key){ FOREACH(string val, mb->tags){ if(algorithm::starts_with(val,key+"=")){ return true;} } return false; }
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
		/* return nth _real_ iteration from the container (0-based index); this is to facilitate picking random interaction */
		pyInteraction pyNth(long n){
			long i=0; FOREACH(const shared_ptr<Interaction>& I, *proxee){ if(!I->isReal) continue; if(i++==n) return pyInteraction(I); }
			throw invalid_argument(string("Interaction number out of range (")+lexical_cast<string>(n)+">="+lexical_cast<string>(i)+").");
		}
		void clear(){proxee->clear();}
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
	python::tuple force_get(long id){ Shop::Bex::initCache(); Vector3r f=Shop::Bex::force(id); return python::make_tuple(f[0],f[1],f[2]);}
	python::tuple momentum_get(long id){ Shop::Bex::initCache(); Vector3r m=Shop::Bex::momentum(id); return python::make_tuple(m[0],m[1],m[2]);}
};



class pyOmega{
	private:
		// can be safely removed now, since pyOmega makes an empty rootBody in the constructor, if there is none
		void assertRootBody(){if(!OMEGA.getRootBody()) throw std::runtime_error("No root body."); }
		Omega& OMEGA;
	public:
	pyOmega(): OMEGA(Omega::instance()){
		shared_ptr<MetaBody> rb=OMEGA.getRootBody();
		assert(rb);
		if(!rb->physicalParameters){rb->physicalParameters=shared_ptr<PhysicalParameters>(new ParticleParameters);} /* PhysicalParameters crashes PhysicalParametersMetaEngine... why? */
		if(!rb->boundingVolume){rb->boundingVolume=shared_ptr<AABB>(new AABB);}
		// initialized in constructor now: rb->boundingVolume->diffuseColor=Vector3r(1,1,1); 
		if(!rb->interactingGeometry){rb->interactingGeometry=shared_ptr<MetaInteractingGeometry>(new MetaInteractingGeometry);}
		//if(!OMEGA.getRootBody()){shared_ptr<MetaBody> mb=Shop::rootBody(); OMEGA.setRootBody(mb);}
		/* this is not true if another instance of Omega is created; flag should be stored inside the Omega singleton for clean solution. */
		if(!OMEGA.hasSimulationLoop()){
			OMEGA.createSimulationLoop();
		}
	};
	/* Create variables in python's __builtin__ namespace that correspond to labeled objects. At this moment, only engines can be labeled. */
	void mapLabeledEntitiesToVariables(){
		FOREACH(const shared_ptr<Engine>& e, OMEGA.getRootBody()->engines){
			if(!e->label.empty()){
				PyGILState_STATE gstate; gstate = PyGILState_Ensure();
				PyRun_SimpleString(("__builtins__."+e->label+"=Omega().labeledEngine('"+e->label+"')").c_str());
				PyGILState_Release(gstate);
			}
		}
	}

	long iter(){ return OMEGA.getCurrentIteration();}
	double simulationTime(){return OMEGA.getSimulationTime();}
	double realTime(){ return OMEGA.getComputationTime(); }
	// long realTime(){return OMEGA(get...);}
	double dt_get(){return OMEGA.getTimeStep();}
	void dt_set(double dt){OMEGA.skipTimeStepper(true); OMEGA.setTimeStep(dt);}

	long stopAtIter_get(){return OMEGA.getRootBody()->stopAtIteration; }
	void stopAtIter_set(long s){OMEGA.getRootBody()->stopAtIteration=s; }

	bool usesTimeStepper_get(){return OMEGA.timeStepperActive();}
	void usesTimeStepper_set(bool use){OMEGA.skipTimeStepper(!use);}

	void run(long int numIter=-1,bool doWait=false){
		if(numIter>0) OMEGA.getRootBody()->stopAtIteration=OMEGA.getCurrentIteration()+numIter;
		OMEGA.startSimulationLoop();
		LOG_DEBUG("RUN"<<((OMEGA.getRootBody()->stopAtIteration-OMEGA.getCurrentIteration())>0?string(" ("+lexical_cast<string>(OMEGA.getRootBody()->stopAtIteration-OMEGA.getCurrentIteration())+" to go)"):string(""))<<"!");
		if(doWait) wait();
	}
	void pause(){Py_BEGIN_ALLOW_THREADS; OMEGA.stopSimulationLoop(); Py_END_ALLOW_THREADS; LOG_DEBUG("PAUSE!");}
	void step() { LOG_DEBUG("STEP!"); run(1); wait();  }
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
	void saveTmp(string mark){ save(":memory:"+mark);}
	void loadTmp(string mark){ load(":memory:"+mark);}
	void tmpToFile(string mark, string filename){
		// FIXME: memSavedSimulations are private, but I don't want to recompile all yade now; move it to public and uncomment these few lines at some point
		// if(OMEGA.memSavedSimulations.count(":memory:"+mark)==0) throw runtime_error("No memory-saved simulation named "+mark);
		iostreams::filtering_ostream out;
		if(boost::algorithm::ends_with(filename,".bz2")) out.push(iostreams::bzip2_compressor());
		out.push(iostreams::file_sink(filename));
		if(!out.good()) throw runtime_error("Error while opening file `"+filename+"' for writing.");
		LOG_INFO("Saving :memory:"<<mark<<" to "<<filename);
		//out<<OMEGA.memSavedSimulations[":memory:"+mark];
	}



	void reset(){Py_BEGIN_ALLOW_THREADS; OMEGA.reset(); Py_END_ALLOW_THREADS; }

	void save(std::string fileName){
		assertRootBody();
		OMEGA.saveSimulation(fileName);
		OMEGA.setSimulationFileName(fileName);
		LOG_DEBUG("SAVE!");
	}

	void saveSpheres(std::string fileName){ Shop::saveSpheresToFile(fileName); }

	python::list miscParams_get(){
		python::list ret;
		FOREACH(shared_ptr<Serializable>& s, OMEGA.getRootBody()->miscParams){
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

	python::list engines_get(void){assertRootBody(); return anyEngines_get(OMEGA.getRootBody()->engines);}
	void engines_set(python::object egs){assertRootBody(); anyEngines_set(OMEGA.getRootBody()->engines,egs); mapLabeledEntitiesToVariables(); }
	python::list initializers_get(void){assertRootBody(); return anyEngines_get(OMEGA.getRootBody()->initializers);}
	void initializers_set(python::object egs){assertRootBody(); anyEngines_set(OMEGA.getRootBody()->initializers,egs); OMEGA.getRootBody()->needsInitializers=true; }

	python::object labeled_engine_get(string label){
		FOREACH(const shared_ptr<Engine>& eng, OMEGA.getRootBody()->engines){
			if(eng->label==label){
				#define RETURN_ENGINE_IF_POSSIBLE(engineType,pyEngineType) { shared_ptr<engineType> e=dynamic_pointer_cast<engineType>(eng); if(e) return python::object(pyEngineType(e)); }
				RETURN_ENGINE_IF_POSSIBLE(MetaEngine,pyMetaEngine);
				RETURN_ENGINE_IF_POSSIBLE(StandAloneEngine,pyStandAloneEngine);
				RETURN_ENGINE_IF_POSSIBLE(DeusExMachina,pyDeusExMachina);
				RETURN_ENGINE_IF_POSSIBLE(ParallelEngine,pyParallelEngine);
				throw std::runtime_error("Unable to cast engine to MetaEngine, StandAloneEngine, DeusExMachina or ParallelEngine? ??");
			}
		}
		throw std::invalid_argument(string("No engine labeled `")+label+"'");
	}


	
	pyBodyContainer bodies_get(void){assertRootBody(); return pyBodyContainer(OMEGA.getRootBody()->bodies); }
	pyInteractionContainer interactions_get(void){assertRootBody(); return pyInteractionContainer(OMEGA.getRootBody()->transientInteractions); }

	pyPhysicalActionContainer actions_get(void){return pyPhysicalActionContainer(OMEGA.getRootBody()->physicalActions); }

	boost::python::list listChildClasses(const string& base){
		boost::python::list ret;
		for(map<string,DynlibDescriptor>::const_iterator di=Omega::instance().getDynlibsDescriptor().begin();di!=Omega::instance().getDynlibsDescriptor().end();++di) if (Omega::instance().isInheritingFrom((*di).first,base)) ret.append(di->first);
		return ret;
	}

	pyTags tags_get(void){assertRootBody(); return pyTags(OMEGA.getRootBody());}
};
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(omega_run_overloads,run,0,2);

BASIC_PY_PROXY_HEAD(pyFileGenerator,FileGenerator)
	void generate(string outFile){ensureAcc(); proxee->setFileName(outFile); proxee->setSerializationLibrary("XMLFormatManager"); bool ret=proxee->generateAndSave(); LOG_INFO((ret?"SUCCESS:\n":"FAILURE:\n")<<proxee->message); if(ret==false) throw runtime_error("Generator reported error: "+proxee->message); };
	void load(){ ensureAcc(); char tmpnam_str [L_tmpnam]; tmpnam(tmpnam_str); string xml(tmpnam_str+string(".xml.bz2")); LOG_DEBUG("Using temp file "<<xml); this->generate(xml); pyOmega().load(xml); }
BASIC_PY_PROXY_TAIL;

class pySTLImporter : public STLImporter {
    public:
	void py_import(pyBodyContainer bc, unsigned int begin=0) { import(bc.proxee,begin); }
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(STLImporter_import_overloads,py_import,1,2);

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
		.def("reload",&pyOmega::reload)
		.def("save",&pyOmega::save)
		.def("loadTmp",&pyOmega::loadTmp)
		.def("saveTmp",&pyOmega::saveTmp)
		.def("tmpToFile",&pyOmega::tmpToFile)
		.def("saveSpheres",&pyOmega::saveSpheres)
		.def("run",&pyOmega::run,omega_run_overloads())
		.def("pause",&pyOmega::pause)
		.def("step",&pyOmega::step)
		.def("wait",&pyOmega::wait)
		.def("reset",&pyOmega::reset)
		.def("labeledEngine",&pyOmega::labeled_engine_get)
		.add_property("engines",&pyOmega::engines_get,&pyOmega::engines_set)
		.add_property("miscParams",&pyOmega::miscParams_get,&pyOmega::miscParams_set)
		.add_property("initializers",&pyOmega::initializers_get,&pyOmega::initializers_set)
		.add_property("bodies",&pyOmega::bodies_get)
		.add_property("interactions",&pyOmega::interactions_get)
		.add_property("actions",&pyOmega::actions_get)
		.add_property("tags",&pyOmega::tags_get)
		.def("childClasses",&pyOmega::listChildClasses)
		;
	boost::python::class_<pyTags>("TagsWrapper",python::init<pyTags&>())
		.def("__getitem__",&pyTags::getItem)
		.def("__setitem__",&pyTags::setItem)
		.def("keys",&pyTags::keys);
	
	boost::python::class_<pyBodyContainer>("BodyContainer",python::init<pyBodyContainer&>())
		.def("__getitem__",&pyBodyContainer::pyGetitem)
		.def("__len__",&pyBodyContainer::length)
		.def("append",&pyBodyContainer::insert)
		.def("append",&pyBodyContainer::insertList)
		.def("clear", &pyBodyContainer::clear)
		.def("replace",&pyBodyContainer::replace);
	boost::python::class_<pyInteractionContainer>("InteractionContainer",python::init<pyInteractionContainer&>())
		.def("__iter__",&pyInteractionContainer::pyIter)
		.def("__getitem__",&pyInteractionContainer::pyGetitem)
		.def("nth",&pyInteractionContainer::pyNth)
		.def("clear",&pyInteractionContainer::clear);
	boost::python::class_<pyInteractionIterator>("InteractionIterator",python::init<pyInteractionIterator&>())
		.def("__iter__",&pyInteractionIterator::pyIter)
		.def("next",&pyInteractionIterator::pyNext);

	boost::python::class_<pyPhysicalActionContainer>("ActionContainer",python::init<pyPhysicalActionContainer&>())
		.def("__getitem__",&pyPhysicalActionContainer::pyGetitem)
		.def("f",&pyPhysicalActionContainer::force_get)
		.def("m",&pyPhysicalActionContainer::momentum_get);
	

	BASIC_PY_PROXY_WRAPPER(pyStandAloneEngine,"StandAloneEngine");
	BASIC_PY_PROXY_WRAPPER(pyMetaEngine,"MetaEngine")
		.add_property("functors",&pyMetaEngine::functors_get,&pyMetaEngine::functors_set)
		.def(python::init<string,python::list>());
	BASIC_PY_PROXY_WRAPPER(pyParallelEngine,"ParallelEngine")
		.add_property("slaves",&pyParallelEngine::slaves_get,&pyParallelEngine::slaves_set)
		.def(python::init<python::list>());
	BASIC_PY_PROXY_WRAPPER(pyDeusExMachina,"DeusExMachina");
	BASIC_PY_PROXY_WRAPPER(pyEngineUnit,"EngineUnit")
		.add_property("bases",&pyEngineUnit::bases_get);

	BASIC_PY_PROXY_WRAPPER(pyGeometricalModel,"GeometricalModel");
	BASIC_PY_PROXY_WRAPPER(pyInteractingGeometry,"InteractingGeometry");
	BASIC_PY_PROXY_WRAPPER(pyPhysicalParameters,"PhysicalParameters")	
		.add_property("blockedDOFs",&pyPhysicalParameters::blockedDOFs_get,&pyPhysicalParameters::blockedDOFs_set)
		.add_property("pos",&pyPhysicalParameters::pos_get,&pyPhysicalParameters::pos_set)
		.add_property("ori",&pyPhysicalParameters::ori_get,&pyPhysicalParameters::ori_set)
		.add_property("refPos",&pyPhysicalParameters::refPos_get,&pyPhysicalParameters::refPos_set)
		;
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
		.add_property("mask",&pyBody::mask_get,&pyBody::mask_set)
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
		.def("generate",&pyFileGenerator::generate)
		.def("load",&pyFileGenerator::load);

	boost::python::class_<pySTLImporter>("STLImporter")
	    .def("open",&pySTLImporter::open)
	    .add_property("number_of_facets",&pySTLImporter::number_of_facets)
	    .def_readwrite("wire",&pySTLImporter::wire)
	    .def("import_geometry",&pySTLImporter::py_import,STLImporter_import_overloads());
	
}

