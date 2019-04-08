// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 

#include <boost/python/raw_function.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

#include <lib/base/Logging.hpp>
#include <lib/pyutil/gil.hpp>
#include <lib/pyutil/raw_constructor.hpp>
#include <lib/pyutil/doc_opts.hpp>
#include <core/Omega.hpp>
#include <core/ThreadRunner.hpp>
#include <core/FileGenerator.hpp>
#include <core/EnergyTracker.hpp>

#include <pkg/dem/STLImporter.hpp>

#include <pkg/common/Dispatching.hpp>
#include <core/GlobalEngine.hpp>
#include <core/PartialEngine.hpp>
#include <core/Functor.hpp>
#include <pkg/common/ParallelEngine.hpp>
#include <pkg/common/Collider.hpp>

#include <pkg/common/InteractionLoop.hpp>

#include <core/Clump.hpp>
#include <pkg/common/Sphere.hpp>
#include <boost/math/special_functions/nonfinite_num_facets.hpp>

#include <locale>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/archive/codecvt_null.hpp>

#include <core/Timing.hpp>
#include <lib/serialization/ObjectIO.hpp>
#include <csignal>

#include <pkg/common/KinematicEngines.hpp>

namespace py = boost::python;

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
		PyErr_SetNone(PyExc_StopIteration); py::throw_error_already_set(); /* never reached, but makes the compiler happier */ throw;
	}
};

class pyBodyContainer{
	private:
		void checkClump(shared_ptr<Body> b){
			if (!(b->isClump())){
				PyErr_SetString(PyExc_TypeError,("Error: Body"+boost::lexical_cast<string>(b->getId())+" is not a clump.").c_str());
				py::throw_error_already_set();
			}
		}
		typedef std::map<Body::id_t,Se3r> MemberMap;
	public:
	const shared_ptr<BodyContainer> proxee;
	pyBodyIterator pyIter(){return pyBodyIterator(proxee);}
	pyBodyContainer(const shared_ptr<BodyContainer>& _proxee): proxee(_proxee){}
	// raw access to the underlying 
	const shared_ptr<BodyContainer> raw_bodies_get(void) {return proxee;}
	void raw_bodies_set(const shared_ptr<BodyContainer>& source) {proxee->body = source->body;}
	
	
	shared_ptr<Body> pyGetitem(Body::id_t _id){
		int id=(_id>=0 ? _id : proxee->size()+_id);
		if(id<0 || (size_t)id>=proxee->size()){ PyErr_SetString(PyExc_IndexError, "Body id out of range."); py::throw_error_already_set(); /* make compiler happy; never reached */ return shared_ptr<Body>(); }
		else return (*proxee)[id];
	}
	Body::id_t append(shared_ptr<Body> b){
		// shoud be >=0, but Body is by default created with id 0... :-|
		if(b->getId()>=0){ PyErr_SetString(PyExc_IndexError,("Body already has id "+boost::lexical_cast<string>(b->getId())+" set; appending such body (for the second time) is not allowed.").c_str()); py::throw_error_already_set(); }
		return proxee->insert(b);
	}
	Body::id_t insertAtId(shared_ptr<Body> b, Body::id_t pos){
		return proxee->insertAtId(b,pos);
	}
	vector<Body::id_t> appendList(vector<shared_ptr<Body> > bb){
		boost::mutex::scoped_lock lock(Omega::instance().renderMutex);
		vector<Body::id_t> ret; FOREACH(shared_ptr<Body>& b, bb){ret.push_back(append(b));} return ret;
	}
	Body::id_t clump(vector<Body::id_t> ids, unsigned int discretization){
		// create and add clump itself
		Scene* scene(Omega::instance().getScene().get());
		shared_ptr<Body> clumpBody=shared_ptr<Body>(new Body());
		shared_ptr<Clump> clump=shared_ptr<Clump>(new Clump());
		clumpBody->shape=clump;
		clumpBody->setBounded(false);
		proxee->insert(clumpBody);
		// add clump members to the clump
		FOREACH(Body::id_t id, ids) {
			if (Body::byId(id,scene)->isClumpMember()){	//Check, whether the body is clumpMember
				Clump::del(Body::byId(Body::byId(id,scene)->clumpId,scene),Body::byId(id,scene)); //If so, remove it from there
			}
		};
		
		FOREACH(Body::id_t id, ids) Clump::add(clumpBody,Body::byId(id,scene));
		Clump::updateProperties(clumpBody, discretization);
		return clumpBody->getId();
	}
	py::tuple appendClump(vector<shared_ptr<Body> > bb, unsigned int discretization){
		// append constituent particles
		vector<Body::id_t> ids(appendList(bb));
		// clump them together (the clump fcn) and return
		return py::make_tuple(clump(ids, discretization),ids);
	}
	void updateClumpProperties(py::list excludeList,unsigned int discretization){
		//convert excludeList to a c++ list
		vector<Body::id_t> excludeListC;
		for (int ii = 0; ii < py::len(excludeList); ii++) excludeListC.push_back(py::extract<Body::id_t>(excludeList[ii])());
		FOREACH(const shared_ptr<Body>& b, *proxee){
			if ( !(std::find(excludeListC.begin(), excludeListC.end(), b->getId()) != excludeListC.end()) ) {
				if (b->isClump()) Clump::updateProperties(b, discretization);
			}
		}
	}
	void deleteClumpMember(shared_ptr<Body> clumpBody, shared_ptr<Body> memberBody){  //FIXME
		const shared_ptr<Clump> clump(YADE_PTR_CAST<Clump>(clumpBody->shape));
		if (clump->members.size()==1 ){
			Clump::del(clumpBody,memberBody); //phD was not commented out
			for (unsigned i=0; i<clump->ids.size(); i++){
				if (clump->ids[i] == memberBody->getId()){
					clump->ids.erase(clump->ids.begin()+i);
				}
			}
			proxee->erase(memberBody->getId(),false);
			proxee->erase(clumpBody->getId(),false);
			
		}else{
			Clump::del(clumpBody,memberBody); //pHD was not commented out
			for (unsigned i=0; i<clump->ids.size(); i++){
				if (clump->ids[i] == memberBody->getId()){
					clump->ids.erase(clump->ids.begin()+i);
				}
			}
			Clump::updatePropertiesNonSpherical(clumpBody,/*intersecting*/ false);
			proxee->erase(memberBody->getId(),false);
			
		}
		
	}
	void deleteClumpBody(shared_ptr<Body> clumpBody){ //FIXME
		const shared_ptr<Clump> clump(YADE_PTR_CAST<Clump>(clumpBody->shape));

		//if (clump->members.size()==0 ){
		//	proxee->erase(clumpBody->getId());
		//}else{
			Scene* scene(Omega::instance().getScene().get());
			int totalNumber = clump->ids.size();
			int count = 0;
			while(count<totalNumber){
			//for (int i=0; i<clump->ids.size(); i++){
				shared_ptr<Body> memberBody (YADE_PTR_CAST<Body>( Body::byId(clump->ids[/*i */0],scene) ));
				deleteClumpMember(clumpBody,memberBody);
				//clump->ids.erase(clump->ids.begin()+i);
				//proxee->erase(memberBody->getId());
				count++;			
			}

			proxee->erase(clumpBody->getId(),true);
		//}
	}
	void addToClump(vector<Body::id_t> bids, Body::id_t cid, unsigned int discretization){
		Scene* scene(Omega::instance().getScene().get());	// get scene
		shared_ptr<Body> clp = Body::byId(cid,scene);		// get clump pointer
		checkClump(clp);
		vector<Body::id_t> eraseList;
		FOREACH(Body::id_t bid, bids) {
			shared_ptr<Body> bp = Body::byId(bid,scene);		// get body pointer
			if (bp->isClump()){
				if (bp == clp) {PyErr_Warn(PyExc_UserWarning,("Warning: Body "+boost::lexical_cast<string>(bid)+" and clump "+boost::lexical_cast<string>(cid)+" are the same bodies. Body was not added.").c_str()); return;}
				Clump::add(clp,bp);//add clump bid to clump cid
				eraseList.push_back(bid);
			}
			else if (bp->isClumpMember()){
				Body::id_t bpClumpId = bp->clumpId;
				shared_ptr<Body> bpClumpPointer = Body::byId(bpClumpId,scene);
				if (bpClumpPointer == clp) {PyErr_Warn(PyExc_UserWarning,("Warning: Body "+boost::lexical_cast<string>(bid)+" is already a clump member of clump "+boost::lexical_cast<string>(cid)+". Body was not added.").c_str()); return;} 
				Clump::add(clp,bpClumpPointer);//add clump bpClumpId to clump cid
				eraseList.push_back(bpClumpId);
			}
			else Clump::add(clp,bp);// bp must be a standalone!
		}
		Clump::updateProperties(clp, discretization);
		FOREACH(Body::id_t bid, eraseList) proxee->erase(bid,false);//erase old clumps
	}
	void releaseFromClump(Body::id_t bid, Body::id_t cid, unsigned int discretization){
		Scene* scene(Omega::instance().getScene().get());	// get scene
		shared_ptr<Body> bp = Body::byId(bid,scene);		// get body pointer
		shared_ptr<Body> clp = Body::byId(cid,scene);		// get clump pointer
		checkClump(clp);
		if (bp->isClumpMember()){
			Body::id_t bpClumpId = bp->clumpId;
			if (cid == bpClumpId){
				const shared_ptr<Clump>& clump=YADE_PTR_CAST<Clump>(clp->shape);
				std::map<Body::id_t,Se3r>& members = clump->members;
				if (members.size() == 2) {PyErr_Warn(PyExc_UserWarning,("Warning: Body "+boost::lexical_cast<string>(bid)+" not released from clump "+boost::lexical_cast<string>(cid)+", because number of clump members would get < 2!").c_str()); return;}
				Clump::del(clp,bp);//release bid from cid
				Clump::updateProperties(clp, discretization);
			} else { PyErr_Warn(PyExc_UserWarning,("Warning: Body "+boost::lexical_cast<string>(bid)+" must be a clump member of clump "+boost::lexical_cast<string>(cid)+". Body was not released.").c_str()); return;}
		} else { PyErr_Warn(PyExc_UserWarning,("Warning: Body "+boost::lexical_cast<string>(bid)+" is not a clump member. Body was not released.").c_str()); return;}
	}
	py::list replaceByClumps(py::list ctList, vector<Real> amounts, unsigned int discretization){
		py::list ret;
		Real checkSum = 0.0;
		FOREACH(Real amount, amounts) {
			if (amount < 0.0) {
				PyErr_SetString(PyExc_ValueError,("Error: One or more of given amounts are negative!")); 
				py::throw_error_already_set();
			}
			else checkSum += amount;
		}
		if (checkSum > 1.0){
			PyErr_SetString(PyExc_ValueError,("Error: Sum of amounts "+boost::lexical_cast<string>(checkSum)+" should not be bigger than 1.0!").c_str()); 
			py::throw_error_already_set();
		}
		if (py::len(ctList) != (unsigned) amounts.size()) {//avoid unsigned comparison warning
			PyErr_SetString(PyExc_ValueError,("Error: Length of amounts list ("+boost::lexical_cast<string>(amounts.size())+") differs from length of template list ("+boost::lexical_cast<string>(py::len(ctList))+").").c_str()); 
			py::throw_error_already_set();
		}
		//set a random generator (code copied from pkg/dem/SpherePack.cpp):
		static boost::minstd_rand randGen((int)TimingInfo::getNow(/* get the number even if timing is disabled globally */ true));
		typedef boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > UniRandGen;
		static UniRandGen rndUnit(randGen,boost::uniform_real<>(-1,1));
		
		//get number of spherical particles and a list of all spheres:
		vector<shared_ptr<Body> > sphereList;
		shared_ptr<Sphere> sph (new Sphere);
		int Sph_Index = sph->getClassIndexStatic();
		FOREACH(const shared_ptr<Body>& b, *proxee) if ( (b->shape->getClassIndex() == Sph_Index) && (b->isStandalone()) ) sphereList.push_back(b);
		int num = sphereList.size();
		
		//loop over templates:
		int numSphereList = num, numTemplates = amounts.size();
		for (int ii = 0; ii < numTemplates; ii++) {
			//ctList: [<ct1>,<ct2>, ...] = [<int,[double,double, ... ],[Vector3r,Vector3r, ...]>,<int,[double,double, ... ],[Vector3r,Vector3r, ...]>, ...]
			//ct: <len(relRadList),relRadList,relPosList> = <int,[double,double, ... ],[Vector3r,Vector3r, ...]> (python objects)
			//relRadList: [relRad1,relRad2, ...] (list of doubles)
			//relPosList: [relPos1,relPos2, ...] (list of vectors)
			
			//extract attributes from python objects:
			py::object ctTmp = ctList[ii];
			int numCM = py::extract<int>(ctTmp.attr("numCM"))();// number of clump members
			py::list relRadListTmp = py::extract<py::list>(ctTmp.attr("relRadii"))();
			py::list relPosListTmp = py::extract<py::list>(ctTmp.attr("relPositions"))();
			
			//get relative radii and positions; calculate volumes; get balance point: get axis aligned bounding box; get minimum radius;
			vector<Real> relRadTmp(numCM), relVolTmp(numCM);
			vector<Vector3r> relPosTmp(numCM);
			Vector3r relPosTmpMean = Vector3r::Zero();
			Real rMin=1./0.; AlignedBox3r aabb;
			for (int jj = 0; jj < numCM; jj++) {
				relRadTmp[jj] = py::extract<Real>(relRadListTmp[jj])();
				relVolTmp[jj] = (4./3.)*Mathr::PI*pow(relRadTmp[jj],3.);
				relPosTmp[jj] = py::extract<Vector3r>(relPosListTmp[jj])();
				relPosTmpMean += relPosTmp[jj];
				aabb.extend(relPosTmp[jj] + Vector3r::Constant(relRadTmp[jj]));
				aabb.extend(relPosTmp[jj] - Vector3r::Constant(relRadTmp[jj]));
				rMin=min(rMin,relRadTmp[jj]);
			}
			relPosTmpMean /= numCM;//balance point
			
			//get volume of the clump template using regular cubic cell array inside axis aligned bounding box of the clump:
			//(some parts are duplicated from intergration algorithm in Clump::updateProperties)
			Real dx = rMin/5.; 	//edge length of cell
			Real dv = pow(dx,3);		//volume of a single cell
			Vector3r x;			//position vector (center) of cell
			Real relVolSumTmp = 0.0;	//volume of clump template
			for(x.x()=aabb.min().x()+dx/2.; x.x()<aabb.max().x(); x.x()+=dx){
				for(x.y()=aabb.min().y()+dx/2.; x.y()<aabb.max().y(); x.y()+=dx){
					for(x.z()=aabb.min().z()+dx/2.; x.z()<aabb.max().z(); x.z()+=dx){
						for (int jj = 0; jj < numCM; jj++) {
							if((x-relPosTmp[jj]).squaredNorm() < pow(relRadTmp[jj],2)){ relVolSumTmp += dv; break; }
						}
					}
				}
			}
			
			//get pointer lists of spheres, that should be replaced:
			int numReplaceTmp = round(num*amounts[ii]);
			vector<shared_ptr<Body> > bpListTmp(numReplaceTmp);
			int a = 0, c = 0;//counters
			vector<int> posTmp;
			FOREACH (const shared_ptr<Body>& b, sphereList) {
				if (c == a*numSphereList/numReplaceTmp) {
					bpListTmp[a] = b; a++;
					posTmp.push_back(c);//remember position in sphereList
				}
				c++;
			}
			for (int jj = 0; jj < a; jj++) {
				sphereList.erase(sphereList.begin()+posTmp[jj]-jj);//remove bodies from sphereList, that were already found
				numSphereList--;
			}
			
			//adapt position- and radii-informations and replace spheres from bpListTmp by clumps:
			FOREACH (const shared_ptr<Body>& b, bpListTmp) {
				//get sphere, that should be replaced:
				const Sphere* sphere = YADE_CAST<Sphere*> (b->shape.get());
				shared_ptr<Material> matTmp = b->material;
				
				//get a random rotation quaternion:
				Quaternionr randAxisTmp = (Quaternionr) AngleAxisr(2*Mathr::PI*rndUnit(),Vector3r(rndUnit(),rndUnit(),rndUnit()));
				randAxisTmp.normalize();
				
				//convert geometries in global coordinates (scaling):
				Real scalingFactorVolume = ((4./3.)*Mathr::PI*pow(sphere->radius,3.))/relVolSumTmp;
				Real scalingFactor1D = pow(scalingFactorVolume,1./3.);//=((vol. sphere)/(relative clump volume))^(1/3)
				vector<Vector3r> newPosTmp(numCM);
				vector<Real> newRadTmp(numCM);
				vector<Body::id_t> idsTmp(numCM);
				for (int jj = 0; jj < numCM; jj++) {
					newPosTmp[jj] = relPosTmp[jj] - relPosTmpMean;	//shift position, to get balance point at (0,0,0)
					newPosTmp[jj] = randAxisTmp*newPosTmp[jj];	//rotate around balance point
					newRadTmp[jj] = relRadTmp[jj] * scalingFactor1D;//scale radii
					newPosTmp[jj] = newPosTmp[jj] * scalingFactor1D;//scale position
					newPosTmp[jj] += b->state->pos;			//translate new position to spheres center
					
					//create spheres:
					shared_ptr<Body> newSphere = shared_ptr<Body>(new Body());
					newSphere->state->blockedDOFs = State::DOF_NONE;
					newSphere->state->mass = scalingFactorVolume*relVolTmp[jj]*matTmp->density;//vol. corrected mass for clump members
					Real inertiaTmp = 2.0/5.0*newSphere->state->mass*newRadTmp[jj]*newRadTmp[jj];
					newSphere->state->inertia	= Vector3r(inertiaTmp,inertiaTmp,inertiaTmp);
					newSphere->state->pos = newPosTmp[jj];
					newSphere->material = matTmp;
					
					shared_ptr<Sphere> sphereTmp = shared_ptr<Sphere>(new Sphere());
					sphereTmp->radius = newRadTmp[jj];
					sphereTmp->color = Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
					sphereTmp->color.normalize();
					newSphere->shape = sphereTmp;
					
					shared_ptr<Aabb> aabbTmp = shared_ptr<Aabb>(new Aabb());
					aabbTmp->color = Vector3r(0,1,0);
					newSphere->bound = aabbTmp;
					proxee->insert(newSphere);
					LOG_DEBUG("New body (sphere) "<<newSphere->id<<" added.");
					idsTmp[jj] = newSphere->id;
				}
				Body::id_t newClumpId = clump(idsTmp, discretization);
				ret.append(py::make_tuple(newClumpId,idsTmp));
				erase(b->id,false);
			}
		}
		return ret;
	}
	Real getRoundness(py::list excludeList){
		Scene* scene(Omega::instance().getScene().get());	// get scene
		shared_ptr<Sphere> sph (new Sphere);
		int Sph_Index = sph->getClassIndexStatic();		// get sphere index for checking if bodies are spheres
		//convert excludeList to a c++ list
		vector<Body::id_t> excludeListC;
		for (int ii = 0; ii < py::len(excludeList); ii++) excludeListC.push_back(py::extract<Body::id_t>(excludeList[ii])());
		Real RC_sum = 0.0;	//sum of local roundnesses
		Real R1, R2, vol, dens;
		int c = 0;		//counter
		FOREACH(const shared_ptr<Body>& b, *proxee){
			if ( !(std::find(excludeListC.begin(), excludeListC.end(), b->getId()) != excludeListC.end()) ) {
				if ((b->shape->getClassIndex() ==  Sph_Index) && (b->isStandalone())) { RC_sum += 1.0; c += 1; }
				if (b->isClump()){
					R2 = 0.0; dens = 0.0; vol = 0.0;
					const shared_ptr<Clump>& clump=YADE_PTR_CAST<Clump>(b->shape);
					std::map<Body::id_t,Se3r>& members = clump->members;
					for(MemberMap::value_type& mm : members){
						const Body::id_t& memberId=mm.first;
						const shared_ptr<Body>& member=Body::byId(memberId,scene);
						assert(member->isClumpMember());
						if (member->shape->getClassIndex() ==  Sph_Index){//clump member should be a sphere
							const Sphere* sphere = YADE_CAST<Sphere*> (member->shape.get());
							R2 = max((member->state->pos - b->state->pos).norm() + sphere->radius, R2);	//get minimum radius of a sphere, that imbeds clump
							dens = member->material->density;
						}
					}
					if (dens > 0.) vol = b->state->mass/dens;
					R1 = pow((3.*vol)/(4.*Mathr::PI),1./3.);	//get theoretical radius of a sphere, with same volume as clump
					if (R2 < R1) {PyErr_Warn(PyExc_UserWarning,("Something went wrong in getRoundness method (R2 < R1 detected).")); return 0;}
					RC_sum += R1/R2; c += 1;
				}
			}
		}
		if (c == 0) c = 1;	//in case no spheres and no clumps are present in the scene: RC = 0
		return RC_sum/c;	//return roundness coefficient RC
	}
	vector<Body::id_t> replace(vector<shared_ptr<Body> > bb){proxee->clear(); return appendList(bb);}
	long length(){return proxee->size();}
	void clear(){proxee->clear();}
	bool erase(Body::id_t id, bool eraseClumpMembers){ return proxee->erase(id,eraseClumpMembers); }
};


class pyTags{
	public:
		pyTags(const shared_ptr<Scene> _mb): mb(_mb){}
		const shared_ptr<Scene> mb;
		bool hasKey(const string& key){ FOREACH(string val, mb->tags){ if(boost::algorithm::starts_with(val,key+"=")){ return true;} } return false; }
		string getItem(const string& key){
			FOREACH(string& val, mb->tags){
				if(boost::algorithm::starts_with(val,key+"=")){ string val1(val); boost::algorithm::erase_head(val1,key.size()+1); return val1;}
			}
			PyErr_SetString(PyExc_KeyError,("Invalid key: "+key+".").c_str());
			py::throw_error_already_set(); /* make compiler happy; never reached */ return string();
		}
		void setItem(const string& key,const string& item){
			if(key.find("=")!=string::npos) {
				PyErr_SetString(PyExc_KeyError, "Key must not contain the '=' character (implementation limitation; sorry).");
				py::throw_error_already_set();
			}
			FOREACH(string& val, mb->tags){if(boost::algorithm::starts_with(val,key+"=")){ val=key+"="+item; return; } }
			mb->tags.push_back(key+"="+item);
			}
		py::list keys(){
			py::list ret;
			FOREACH(string val, mb->tags){
				size_t i=val.find("=");
				if(i==string::npos) throw runtime_error("Tags must be in the key=value format (internal error?)");
				boost::algorithm::erase_tail(val,val.size()-i); ret.append(val);
			}
			return ret;
		}
};


class pyInteractionIterator{
	InteractionContainer::iterator I, Iend;
	public:
	pyInteractionIterator(const shared_ptr<InteractionContainer>& ic){ I=ic->begin(); Iend=ic->end(); }
	pyInteractionIterator pyIter(){return *this;}
	shared_ptr<Interaction> pyNext(){
		InteractionContainer::iterator ret;
		while(I!=Iend){ ret=I; ++I; if((*ret)->isReal()) return *ret; }
		PyErr_SetNone(PyExc_StopIteration); py::throw_error_already_set();
		throw; // to avoid compiler warning; never reached
		//InteractionContainer::iterator ret=I; ++I; return *ret;
	}
};

class pyInteractionContainer{
	public:
		const shared_ptr<InteractionContainer> proxee;
		pyInteractionContainer(const shared_ptr<InteractionContainer>& _proxee): proxee(_proxee){}
		pyInteractionIterator pyIter(){return pyInteractionIterator(proxee);}
		bool has(Body::id_t id1, Body::id_t id2){return proxee->found(id1,id2);}
		shared_ptr<Interaction> pyGetitem(vector<Body::id_t> id12){
			//if(!PySequence_Check(id12.ptr())) throw invalid_argument("Key must be a tuple");
			//if(py::len(id12)!=2) throw invalid_argument("Key must be a 2-tuple: id1,id2.");
			if(id12.size()==2){
				//if(max(id12[0],id12[1])>
				shared_ptr<Interaction> i=proxee->find(id12[0],id12[1]);
				if(i) return i; else { PyErr_SetString(PyExc_IndexError,"No such interaction"); py::throw_error_already_set(); /* make compiler happy; never reached */ return shared_ptr<Interaction>(); }
			}
			else if(id12.size()==1){ return (*proxee)[id12[0]];}
			else throw invalid_argument("2 integers (id1,id2) or 1 integer (nth) required.");
		}
		/* return nth _real_ iteration from the container (0-based index); this is to facilitate picking random interaction */
		shared_ptr<Interaction> pyNth(long n){
			long i=0; FOREACH(shared_ptr<Interaction> I, *proxee){ if(!I->isReal()) continue; if(i++==n) return I; }
			PyErr_SetString(PyExc_IndexError,(string("Interaction number out of range (")+boost::lexical_cast<string>(n)+">="+boost::lexical_cast<string>(i)+").").c_str());
			py::throw_error_already_set(); /* make compiler happy; never reached */ return shared_ptr<Interaction>();
		}
		long len(){return proxee->size();}
		void clear(){proxee->clear();}
		py::list withBody(long id){ py::list ret; FOREACH(const shared_ptr<Interaction>& I, *proxee){ if(I->isReal() && (I->getId1()==id || I->getId2()==id)) ret.append(I);} return ret;}
		py::list withBodyAll(long id){ py::list ret; FOREACH(const shared_ptr<Interaction>& I, *proxee){ if(I->getId1()==id || I->getId2()==id) ret.append(I);} return ret; }
		py::list getAll(bool onlyReal){ py::list ret; FOREACH(const shared_ptr<Interaction>& I, *proxee){
			if(onlyReal && !I->isReal()) continue; ret.append(I);}
			return ret;}
		long countReal(){ long ret=0; FOREACH(const shared_ptr<Interaction>& I, *proxee){ if(I->isReal()) ret++; } return ret; }
		bool serializeSorted_get(){return proxee->serializeSorted;}
		void serializeSorted_set(bool ss){proxee->serializeSorted=ss;}
		void eraseNonReal(){ proxee->eraseNonReal(); }
		void erase(Body::id_t id1, Body::id_t id2){ proxee->requestErase(id1,id2); }
};

class pyForceContainer{
		shared_ptr<Scene> scene;
	public:
		pyForceContainer(shared_ptr<Scene> _scene): scene(_scene) { }
		void checkId(long id){ if(id<0 || (size_t)id>=scene->bodies->size()){ PyErr_SetString(PyExc_IndexError, "Body id out of range."); py::throw_error_already_set(); /* never reached */ throw; } }
		Vector3r force_get(long id, bool sync){  checkId(id); if (!sync) return scene->forces.getForceSingle(id); scene->forces.sync(); return scene->forces.getForce(id);}
		Vector3r torque_get(long id, bool sync){ checkId(id); if (!sync) return scene->forces.getTorqueSingle(id); scene->forces.sync(); return scene->forces.getTorque(id);}
		Vector3r move_get(long id){ checkId(id); return scene->forces.getMoveSingle(id); }
		Vector3r rot_get(long id){ checkId(id); return scene->forces.getRotSingle(id); }
		void force_add(long id, const Vector3r& f, bool permanent){  checkId(id); if (!permanent) scene->forces.addForce (id,f); else { LOG_WARN("O.forces.addF(...,permanent=True) is deprecated, use O.forces.setPermF(...) instead"); scene->forces.setPermForce (id,f); } }
		void torque_add(long id, const Vector3r& t, bool permanent){ checkId(id); if (!permanent) scene->forces.addTorque(id,t); else { LOG_WARN("O.forces.addT(...,permanent=True) is deprecated, use O.forces.setPermT(...) instead"); scene->forces.setPermTorque(id,t); } }
		void move_add(long id, const Vector3r& t){   checkId(id); scene->forces.addMove(id,t);}
		void rot_add(long id, const Vector3r& t){    checkId(id); scene->forces.addRot(id,t);}
		Vector3r permForce_get(long id){  checkId(id); return scene->forces.getPermForce(id);}
		Vector3r permTorque_get(long id){  checkId(id); return scene->forces.getPermTorque(id);}
		void permForce_set (long id, const Vector3r& f){ checkId(id); scene->forces.setPermForce (id,f); }
		void permTorque_set(long id, const Vector3r& t){ checkId(id); scene->forces.setPermTorque(id,t); }
		void reset(bool resetAll) {scene->forces.reset(scene->iter,resetAll);}
		long syncCount_get(){ return scene->forces.syncCount;}
		void syncCount_set(long count){ scene->forces.syncCount=count;}
		bool getPermForceUsed() {return scene->forces.getPermForceUsed();}
};

class pyMaterialContainer{
		shared_ptr<Scene> scene;
	public:
		pyMaterialContainer(shared_ptr<Scene> _scene): scene(_scene) { }
		shared_ptr<Material> getitem_id(int _id){ int id=(_id>=0 ? _id : scene->materials.size()+_id); if(id<0 || (size_t)id>=scene->materials.size()){ PyErr_SetString(PyExc_IndexError, "Material id out of range."); py::throw_error_already_set(); /* never reached */ throw; } return Material::byId(id,scene); }
		shared_ptr<Material> getitem_label(string label){
			// translate runtime_error to KeyError (instead of RuntimeError) if the material doesn't exist
			try { return Material::byLabel(label,scene);	}
			catch (std::runtime_error& e){ PyErr_SetString(PyExc_KeyError,e.what()); py::throw_error_already_set(); /* never reached; avoids warning */ throw; }
		}
		int append(shared_ptr<Material> m){ scene->materials.push_back(m); m->id=scene->materials.size()-1; return m->id; }
		vector<int> appendList(vector<shared_ptr<Material> > mm){ vector<int> ret; FOREACH(shared_ptr<Material>& m, mm) ret.push_back(append(m)); return ret; }
		int len(){ return (int)scene->materials.size(); }
		int index(const std::string& label){ return Material::byLabelIndex(label,scene.get()); }
};

void termHandlerNormal(int sig){cerr<<"Yade: normal exit."<<endl; raise(SIGTERM);}
void termHandlerError(int sig){cerr<<"Yade: error exit."<<endl; raise(SIGTERM);}

class pyOmega{
	private:
		// can be safely removed now, since pyOmega makes an empty scene in the constructor, if there is none
		void assertScene(){if(!OMEGA.getScene()) throw std::runtime_error("No Scene instance?!"); }
		Omega& OMEGA;
	public:
	pyOmega(): OMEGA(Omega::instance()){
		shared_ptr<Scene> rb=OMEGA.getScene();
		if(!rb){
			OMEGA.init();
			rb=OMEGA.getScene();
		}
		assert(rb);
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
				if(!m->label.empty()) { PyGILState_STATE gstate; gstate = PyGILState_Ensure(); PyRun_SimpleString(("__builtins__."+m->label+"=Omega().materials["+boost::lexical_cast<string>(m->id)+"]").c_str()); PyGILState_Release(gstate); }
			}
		#endif
		FOREACH(const shared_ptr<Engine>& e, OMEGA.getScene()->engines){
			if(!e->label.empty()){
				pyRunString("__builtins__."+e->label+"=Omega().labeledEngine('"+e->label+"')");
			}
			#define _DO_FUNCTORS(functors,FunctorT){ FOREACH(const shared_ptr<FunctorT>& f, functors){ if(!f->label.empty()){ pyRunString("__builtins__."+f->label+"=Omega().labeledEngine('"+f->label+"')");}} }
			#define _TRY_DISPATCHER(DispatcherT) { DispatcherT* d=dynamic_cast<DispatcherT*>(e.get()); if(d){ _DO_FUNCTORS(d->functors,DispatcherT::FunctorType); } }
			_TRY_DISPATCHER(BoundDispatcher); _TRY_DISPATCHER(IGeomDispatcher); _TRY_DISPATCHER(IPhysDispatcher); _TRY_DISPATCHER(LawDispatcher);
			InteractionLoop* id=dynamic_cast<InteractionLoop*>(e.get());
			if(id){
				_DO_FUNCTORS(id->geomDispatcher->functors,IGeomFunctor);
				_DO_FUNCTORS(id->physDispatcher->functors,IPhysFunctor);
				_DO_FUNCTORS(id->lawDispatcher->functors,LawFunctor);
			}
			Collider* coll=dynamic_cast<Collider*>(e.get());
			if(coll){ _DO_FUNCTORS(coll->boundDispatcher->functors,BoundFunctor); }
			#undef _DO_FUNCTORS
			#undef _TRY_DISPATCHER
			CombinedKinematicEngine* cke=dynamic_cast<CombinedKinematicEngine*>(e.get());
			if (cke) {
				FOREACH(const shared_ptr<KinematicEngine>& ke, cke->comb){ if(!ke->label.empty()){ pyRunString("__builtins__."+ke->label+"=Omega().labeledEngine('"+ke->label+"')"); } }
			}
		}
	}
	py::object labeled_engine_get(string label){
		FOREACH(const shared_ptr<Engine>& e, OMEGA.getScene()->engines){
			#define _DO_FUNCTORS(functors,FunctorT){ FOREACH(const shared_ptr<FunctorT>& f, functors){ if(f->label==label) return py::object(f); }}
			#define _TRY_DISPATCHER(DispatcherT) { DispatcherT* d=dynamic_cast<DispatcherT*>(e.get()); if(d){ _DO_FUNCTORS(d->functors,DispatcherT::FunctorType); } }
			if(e->label==label){ return py::object(e); }
			_TRY_DISPATCHER(BoundDispatcher); _TRY_DISPATCHER(IGeomDispatcher); _TRY_DISPATCHER(IPhysDispatcher); _TRY_DISPATCHER(LawDispatcher);
			InteractionLoop* id=dynamic_cast<InteractionLoop*>(e.get());
			if(id){
				_DO_FUNCTORS(id->geomDispatcher->functors,IGeomFunctor);
				_DO_FUNCTORS(id->physDispatcher->functors,IPhysFunctor);
				_DO_FUNCTORS(id->lawDispatcher->functors,LawFunctor);
			}
			Collider* coll=dynamic_cast<Collider*>(e.get());
			if(coll){ _DO_FUNCTORS(coll->boundDispatcher->functors,BoundFunctor); }
			#undef _DO_FUNCTORS
			#undef _TRY_DISPATCHER
			CombinedKinematicEngine* cke=dynamic_cast<CombinedKinematicEngine*>(e.get());
			if (cke) {
				FOREACH(const shared_ptr<KinematicEngine>& ke, cke->comb){ if(ke->label==label) return py::object(ke); }
			}
		}
		throw std::invalid_argument(string("No engine labeled `")+label+"'");
	}

	long iter(){ return OMEGA.getScene()->iter;}
	int subStep(){ return OMEGA.getScene()->subStep; }
	bool subStepping_get(){ return OMEGA.getScene()->subStepping; }
	void subStepping_set(bool val){ OMEGA.getScene()->subStepping=val; }

	double time(){return OMEGA.getScene()->time;}
	double realTime(){ return OMEGA.getRealTime(); }
	double speed(){ return OMEGA.getScene()->speed; }
	double dt_get(){return OMEGA.getScene()->dt;}
	void dt_set(double dt){
		Scene* scene=OMEGA.getScene().get();
		// activate timestepper, if possible (throw exception if there is none)
		if(dt<0){ if(!scene->timeStepperActivate(true)) /* not activated*/ throw runtime_error("No TimeStepper found in O.engines."); }
		else { scene->dt=dt; }
	}
	bool dynDt_get(){return OMEGA.getScene()->timeStepperActive();}
	bool dynDt_set(bool activate){if(!OMEGA.getScene()->timeStepperActivate(activate)) /* not activated*/ throw runtime_error("No TimeStepper found in O.engines."); return true;}
	bool dynDtAvailable_get(){ return OMEGA.getScene()->timeStepperPresent(); }
	long stopAtIter_get(){return OMEGA.getScene()->stopAtIter; }
	void stopAtIter_set(long s){OMEGA.getScene()->stopAtIter=s; }
	Real stopAtTime_get(){return OMEGA.getScene()->stopAtTime; }
	void stopAtTime_set(long s){OMEGA.getScene()->stopAtTime=s; }


	bool timingEnabled_get(){return TimingInfo::enabled;}
	void timingEnabled_set(bool enabled){TimingInfo::enabled=enabled;}
	// deprecated:
		unsigned long forceSyncCount_get(){ return OMEGA.getScene()->forces.syncCount;}
		void forceSyncCount_set(unsigned long count){ OMEGA.getScene()->forces.syncCount=count;}

	void run(long int numIter=-1,bool doWait=false){
		Scene* scene=OMEGA.getScene().get();
		if(numIter>0) scene->stopAtIter=scene->iter+numIter;
		OMEGA.run();
		// timespec t1,t2; t1.tv_sec=0; t1.tv_nsec=40000000; /* 40 ms */
		// while(!OMEGA.isRunning()) nanosleep(&t1,&t2); // wait till we start, so that calling wait() immediately afterwards doesn't return immediately
		LOG_DEBUG("RUN"<<((scene->stopAtIter-scene->iter)>0?string(" ("+boost::lexical_cast<string>(scene->stopAtIter-scene->iter)+" to go)"):string(""))<<"!");
		if(doWait) wait();
	}
	void pause(){Py_BEGIN_ALLOW_THREADS; OMEGA.pause(); Py_END_ALLOW_THREADS; LOG_DEBUG("PAUSE!");}
	void step() { if(OMEGA.isRunning()) throw runtime_error("Called O.step() while simulation is running."); OMEGA.getScene()->moveToNextTimeStep(); /* LOG_DEBUG("STEP!"); run(1); wait(); */ }
	void wait(){
		if(OMEGA.isRunning()){LOG_DEBUG("WAIT!");} else return;
		timespec t1,t2; t1.tv_sec=0; t1.tv_nsec=40000000; /* 40 ms */ Py_BEGIN_ALLOW_THREADS; while(OMEGA.isRunning()) nanosleep(&t1,&t2); Py_END_ALLOW_THREADS;
		if(!OMEGA.simulationLoop->workerThrew) return;
		LOG_ERROR("Simulation error encountered."); OMEGA.simulationLoop->workerThrew=false; throw OMEGA.simulationLoop->workerException;
	}
	bool isRunning(){ return OMEGA.isRunning(); }
	py::object get_filename(){ string f=OMEGA.sceneFile; if(f.size()>0) return py::object(f); return py::object();}
	void load(std::string fileName,bool quiet=false) {
		Py_BEGIN_ALLOW_THREADS; OMEGA.stop(); Py_END_ALLOW_THREADS; 
		OMEGA.loadSimulation(fileName,quiet);
		OMEGA.createSimulationLoop();
		mapLabeledEntitiesToVariables();
	}
	void reload(bool quiet=false){	load(OMEGA.sceneFile,quiet);}
	void saveTmp(string mark="", bool quiet=false){ save(":memory:"+mark,quiet);}
	void loadTmp(string mark="", bool quiet=false){ load(":memory:"+mark,quiet);}
	py::list lsTmp(){ py::list ret; typedef pair<std::string,string> strstr; FOREACH(const strstr& sim,OMEGA.memSavedSimulations){ string mark=sim.first; boost::algorithm::replace_first(mark,":memory:",""); ret.append(mark); } return ret; }
	void tmpToFile(string mark, string filename){
		if(OMEGA.memSavedSimulations.count(":memory:"+mark)==0) throw runtime_error("No memory-saved simulation named "+mark);
		boost::iostreams::filtering_ostream out;
		if(boost::algorithm::ends_with(filename,".bz2")) out.push(boost::iostreams::bzip2_compressor());
		out.push(boost::iostreams::file_sink(filename));
		if(!out.good()) throw runtime_error("Error while opening file `"+filename+"' for writing.");
		LOG_INFO("Saving :memory:"<<mark<<" to "<<filename);
		out<<OMEGA.memSavedSimulations[":memory:"+mark];
	}
	string tmpToString(string mark){
		if(OMEGA.memSavedSimulations.count(":memory:"+mark)==0) throw runtime_error("No memory-saved simulation named "+mark);
		return OMEGA.memSavedSimulations[":memory:"+mark];
	}

	void reset(){OMEGA.stop(); OMEGA.reset(); }
	void resetThisScene(){Py_BEGIN_ALLOW_THREADS; OMEGA.stop(); Py_END_ALLOW_THREADS; OMEGA.resetCurrentScene(); OMEGA.createSimulationLoop();}
	void resetCurrentScene(){Py_BEGIN_ALLOW_THREADS; OMEGA.stop(); Py_END_ALLOW_THREADS; OMEGA.resetCurrentScene(); OMEGA.createSimulationLoop();}
	void resetTime(){ OMEGA.getScene()->iter=0; OMEGA.getScene()->time=0; OMEGA.timeInit(); }
	void switchScene(){ std::swap(OMEGA.scenes[OMEGA.currentSceneNb],OMEGA.sceneAnother); }
	void resetAllScenes(){Py_BEGIN_ALLOW_THREADS; OMEGA.stop(); Py_END_ALLOW_THREADS; OMEGA.resetAllScenes(); OMEGA.createSimulationLoop();}
	void switchToScene(int i){OMEGA.switchToScene(i);}
	int addScene(){return OMEGA.addScene();}
	
	// Scene manipulation in multithread situations as python object or as a string (e.g. FEMxDEM, MPI, ...)
	shared_ptr<Scene> scene_get(){ return OMEGA.getScene(); }
	void scene_set(const shared_ptr<Scene>& source){ Py_BEGIN_ALLOW_THREADS; reset(); Py_END_ALLOW_THREADS; assertScene(); OMEGA.setScene(source); }
	
	string sceneToString(){
		ostringstream oss;
		yade::ObjectIO::save<decltype(OMEGA.getScene()),boost::archive::binary_oarchive>(oss,"scene",OMEGA.getScene());
		oss.flush();
		return oss.str();
	}
	void stringToScene(const string &sstring, string mark=""){
		Py_BEGIN_ALLOW_THREADS; OMEGA.stop(); Py_END_ALLOW_THREADS;
		assertScene();
		OMEGA.memSavedSimulations[":memory:"+mark]=sstring;
		OMEGA.sceneFile=":memory:"+mark;
		load(OMEGA.sceneFile,true);
	}
	int thisScene(){return OMEGA.currentSceneNb;}

	void save(std::string fileName,bool quiet=false){
		assertScene();
		OMEGA.saveSimulation(fileName,quiet);
		// OMEGA.sceneFile=fileName; // done in Omega::saveSimulation;
	}
	
	py::list miscParams_get(){
		py::list ret;
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


	vector<shared_ptr<Engine> > engines_get(void){assertScene(); Scene* scene=OMEGA.getScene().get(); return scene->_nextEngines.empty()?scene->engines:scene->_nextEngines;}
	void engines_set(const vector<shared_ptr<Engine> >& egs){
		assertScene(); Scene* scene=OMEGA.getScene().get();
		if(scene->subStep<0) scene->engines=egs; // not inside the engine loop right now, ok to update directly
		else scene->_nextEngines=egs; // inside the engine loop, update _nextEngines; O.engines picks that up automatically, and Scene::moveToNextTimestep will put them in place of engines at the start of the next loop
		mapLabeledEntitiesToVariables();
	}
	// raw access to engines/_nextEngines, for debugging
	vector<shared_ptr<Engine> > currEngines_get(){ return OMEGA.getScene()->engines; }
	vector<shared_ptr<Engine> > nextEngines_get(){ return OMEGA.getScene()->_nextEngines; }

	pyBodyContainer bodies_get(void){assertScene(); return pyBodyContainer(OMEGA.getScene()->bodies); }
	pyInteractionContainer interactions_get(void){assertScene(); return pyInteractionContainer(OMEGA.getScene()->interactions); }
	
	pyForceContainer forces_get(void){return pyForceContainer(OMEGA.getScene());}
	pyMaterialContainer materials_get(void){return pyMaterialContainer(OMEGA.getScene());}
	

	py::list listChildClassesNonrecursive(const string& base){
		py::list ret;
		for(map<string,DynlibDescriptor>::const_iterator di=Omega::instance().getDynlibsDescriptor().begin();di!=Omega::instance().getDynlibsDescriptor().end();++di) if (Omega::instance().isInheritingFrom((*di).first,base)) ret.append(di->first);
		return ret;
	}

	bool isChildClassOf(const string& child, const string& base){
		return (Omega::instance().isInheritingFrom_recursive(child,base));
	}

	py::list plugins_get(){
		const map<string,DynlibDescriptor>& plugins=Omega::instance().getDynlibsDescriptor();
		std::pair<string,DynlibDescriptor> p; py::list ret;
		FOREACH(p, plugins) ret.append(p.first);
		return ret;
	}

	pyTags tags_get(void){assertScene(); return pyTags(OMEGA.getScene());}

	void interactionContainer_set(string clss){
		Scene* rb=OMEGA.getScene().get();
		if(rb->interactions->size()>0) throw std::runtime_error("Interaction container not empty, will not change its class.");
		shared_ptr<InteractionContainer> ic=YADE_PTR_DYN_CAST<InteractionContainer>(ClassFactory::instance().createShared(clss));
		rb->interactions=ic;
	}
	string interactionContainer_get(string clss){ return OMEGA.getScene()->interactions->getClassName(); }

	void bodyContainer_set(string clss){
		Scene* rb=OMEGA.getScene().get();
		if(rb->bodies->size()>0) throw std::runtime_error("Body container not empty, will not change its class.");
		shared_ptr<BodyContainer> bc=YADE_PTR_DYN_CAST<BodyContainer>(ClassFactory::instance().createShared(clss));
		rb->bodies=bc;
	}
	string bodyContainer_get(string clss){ return OMEGA.getScene()->bodies->getClassName(); }
	#ifdef YADE_OPENMP
		int numThreads_get(){ return omp_get_max_threads();}
		void numThreads_set(int n){ int bcn=OMEGA.getScene()->forces.getNumAllocatedThreads(); if(bcn<n) LOG_WARN("ForceContainer has only "<<bcn<<" threads allocated. Changing thread number to on "<<bcn<<" instead of "<<n<<" requested."); omp_set_num_threads(min(n,bcn)); LOG_WARN("BUG: Omega().numThreads=n doesn't work as expected (number of threads is not changed globally). Set env var OMP_NUM_THREADS instead."); }
	#else
		int numThreads_get(){return 1;}
		void numThreads_set(int n){ LOG_WARN("Yade was compiled without openMP support, changing number of threads will have no effect."); }
	#endif
	
	shared_ptr<Cell> cell_get(){ if(OMEGA.getScene()->isPeriodic) return OMEGA.getScene()->cell; return shared_ptr<Cell>(); }
	bool periodic_get(void){ return OMEGA.getScene()->isPeriodic; } 
	void periodic_set(bool v){ OMEGA.getScene()->isPeriodic=v; }

	shared_ptr<EnergyTracker> energy_get(){ return OMEGA.getScene()->energy; }
	bool trackEnergy_get(void){ return OMEGA.getScene()->trackEnergy; }
	void trackEnergy_set(bool e){ OMEGA.getScene()->trackEnergy=e; }

	void disableGdb(){
		signal(SIGSEGV,SIG_DFL);
		signal(SIGABRT,SIG_DFL);
	}
	void exitNoBacktrace(int status=0){
		if(status==0) signal(SIGSEGV,termHandlerNormal); /* unset the handler that runs gdb and prints backtrace */
		else signal(SIGSEGV,termHandlerError);
		// try to clean our mess
		Omega::instance().cleanupTemps();
		// flush all streams (so that in case we crash at exit, unflushed buffers are not lost)
		fflush(NULL);
		// attempt exit
		exit(status);
	}
	void runEngine(const shared_ptr<Engine>& e){ LOG_WARN("Omega().runEngine(): deprecated, use __call__ method of the engine instance directly instead; will be removed in the future."); e->scene=OMEGA.getScene().get(); e->action(); }
	std::string tmpFilename(){ return OMEGA.tmpFilename(); }
};

BOOST_PYTHON_MODULE(wrapper)
{
	py::scope().attr("__doc__")="Wrapper for c++ internals of yade.";

	YADE_SET_DOCSTRING_OPTS;

	py::enum_<yade::Attr::flags>("AttrFlags")
		.value("noSave",yade::Attr::noSave)
		.value("readonly",yade::Attr::readonly)
		.value("triggerPostLoad",yade::Attr::triggerPostLoad)
		.value("noResize",yade::Attr::noResize)
    ;

	py::class_<pyOmega>("Omega")
		.add_property("iter",&pyOmega::iter,"Get current step number")
		.add_property("subStep",&pyOmega::subStep,"Get the current subStep number (only meaningful if O.subStepping==True); -1 when outside the loop, otherwise either 0 (O.subStepping==False) or number of engine to be run (O.subStepping==True)")
		.add_property("subStepping",&pyOmega::subStepping_get,&pyOmega::subStepping_set,"Get/set whether subStepping is active.")
		.add_property("stopAtIter",&pyOmega::stopAtIter_get,&pyOmega::stopAtIter_set,"Get/set number of iteration after which the simulation will stop.")
		.add_property("stopAtTime",&pyOmega::stopAtTime_get,&pyOmega::stopAtTime_set,"Get/set time after which the simulation will stop.")
		.add_property("time",&pyOmega::time,"Return virtual (model world) time of the simulation.")
		.add_property("realtime",&pyOmega::realTime,"Return clock (human world) time the simulation has been running.")
		.add_property("speed",&pyOmega::speed,"Return current calculation speed [iter/sec].")
		.add_property("dt",&pyOmega::dt_get,&pyOmega::dt_set,"Current timestep (Δt) value.")
		.add_property("dynDt",&pyOmega::dynDt_get,&pyOmega::dynDt_set,"Whether a :yref:`TimeStepper` is used for dynamic Δt control. See :yref:`dt<Omega.dt>` on how to enable/disable :yref:`TimeStepper`.")
		.add_property("dynDtAvailable",&pyOmega::dynDtAvailable_get,"Whether a :yref:`TimeStepper` is amongst :yref:`O.engines<Omega.engines>`, activated or not.")
		.def("load",&pyOmega::load,(py::arg("file"),py::arg("quiet")=false),"Load simulation from file. The file should be :yref:`saved<Omega.save>` in the same version of Yade, otherwise compatibility is not guaranteed.")
		.def("reload",&pyOmega::reload,(py::arg("quiet")=false),"Reload current simulation")
		.def("save",&pyOmega::save,(py::arg("file"),py::arg("quiet")=false),"Save current simulation to file (should be .xml or .xml.bz2 or .yade or .yade.gz). .xml files are bigger than .yade, but can be more or less easily (due to their size) opened and edited, e.g. with text editors. .bz2 and .gz correspond both to compressed versions. All saved files should be :yref:`loaded<Omega.load>` in the same version of Yade, otherwise compatibility is not guaranteed.")
		.def("loadTmp",&pyOmega::loadTmp,(py::arg("mark")="",py::arg("quiet")=false),"Load simulation previously stored in memory by saveTmp. *mark* optionally distinguishes multiple saved simulations")
		.def("saveTmp",&pyOmega::saveTmp,(py::arg("mark")="",py::arg("quiet")=false),"Save simulation to memory (disappears at shutdown), can be loaded later with loadTmp. *mark* optionally distinguishes different memory-saved simulations.")
		.def("lsTmp",&pyOmega::lsTmp,"Return list of all memory-saved simulations.")
		.def("tmpToFile",&pyOmega::tmpToFile,(py::arg("fileName"),py::arg("mark")=""),"Save XML of :yref:`saveTmp<Omega.saveTmp>`'d simulation into *fileName*.")
		.def("tmpToString",&pyOmega::tmpToString,(py::arg("mark")=""),"Return XML of :yref:`saveTmp<Omega.saveTmp>`'d simulation as string.")
		.def("run",&pyOmega::run,(py::arg("nSteps")=-1,py::arg("wait")=false),"Run the simulation. *nSteps* how many steps to run, then stop (if positive); *wait* will cause not returning to python until simulation will have stopped.")
		.def("pause",&pyOmega::pause,"Stop simulation execution. (May be called from within the loop, and it will stop after the current step).")
		.def("step",&pyOmega::step,"Advance the simulation by one step. Returns after the step will have finished.")
		.def("wait",&pyOmega::wait,"Don't return until the simulation will have been paused. (Returns immediately if not running).")
		.add_property("running",&pyOmega::isRunning,"Whether background thread is currently running a simulation.")
		.add_property("filename",&pyOmega::get_filename,"Filename under which the current simulation was saved (None if never saved).")
		.def("reset",&pyOmega::reset,"Reset simulations completely (including another scenes!).")
		.def("resetThisScene",&pyOmega::resetThisScene,"Reset current scene.")
		.def("resetCurrentScene",&pyOmega::resetCurrentScene,"Reset current scene.")
		.def("resetAllScenes",&pyOmega::resetAllScenes,"Reset all scenes.")
		.def("addScene",&pyOmega::addScene,"Add new scene to Omega, returns its number")
		.def("switchToScene",&pyOmega::switchToScene,"Switch to defined scene. Default scene has number 0, other scenes have to be created by addScene method.")
		.def("switchScene",&pyOmega::switchScene,"Switch to alternative simulation (while keeping the old one). Calling the function again switches back to the first one. Note that most variables from the first simulation will still refer to the first simulation even after the switch\n(e.g. b=O.bodies[4]; O.switchScene(); [b still refers to the body in the first simulation here])")
		.add_property("thisScene",&pyOmega::thisScene,"Return current scene's id.")
		.def("sceneToString",&pyOmega::sceneToString,"Return the entire scene as a string. Equivalent to using O.save(...) except that the scene goes to a string instead of a file. (see also stringToScene())")
		.def("stringToScene",&pyOmega::stringToScene,(py::arg("mark")=""),"Load simulation from a string passed as argument (see also sceneToString).")
		.def("labeledEngine",&pyOmega::labeled_engine_get,"Return instance of engine/functor with the given label. This function shouldn't be called by the user directly; every ehange in O.engines will assign respective global python variables according to labels.\n\nFor example:\n\n\t *O.engines=[InsertionSortCollider(label='collider')]*\n\n\t *collider.nBins=5 # collider has become a variable after assignment to O.engines automatically*")
		.def("resetTime",&pyOmega::resetTime,"Reset simulation time: step number, virtual and real time. (Doesn't touch anything else, including timings).")
		.def("plugins",&pyOmega::plugins_get,"Return list of all plugins registered in the class factory.")
// 		.def("_sceneObj",&pyOmega::scene_get,"Return the :yref:`scene <Scene>` object. Debugging only, all (or most) :yref:`Scene` functionality is proxies through :yref:`Omega`.")
		.add_property("_sceneObj",&pyOmega::scene_get,&pyOmega::scene_set,"Return the :yref:`scene <Scene>` object. Debugging only, all (or most) :yref:`Scene` functionality is proxies through :yref:`Omega`.")
		.add_property("engines",&pyOmega::engines_get,&pyOmega::engines_set,"List of engines in the simulation (corresponds to Scene::engines in C++ source code).")
		.add_property("_currEngines",&pyOmega::currEngines_get,"Currently running engines; debugging only!")
		.add_property("_nextEngines",&pyOmega::nextEngines_get,"Engines for the next step, if different from the current ones, otherwise empty; debugging only!")
		.add_property("miscParams",&pyOmega::miscParams_get,&pyOmega::miscParams_set,"MiscParams in the simulation (Scene::mistParams), usually used to save serializables that don't fit anywhere else, like GL functors")
		.add_property("bodies",&pyOmega::bodies_get,"Bodies in the current simulation (container supporting index access by id and iteration)")
		.add_property("interactions",&pyOmega::interactions_get,"Access to :yref:`interactions<Interaction>` of simulation, by using \n\n#. id's of both :yref:`Bodies<Body>` of the interactions, e.g. ``O.interactions[23,65]``\n#. iteraction over the whole container::\n\n\tfor i in O.interactions: print i.id1,i.id2\n\n.. note::\n\tIteration silently skips interactions that are not :yref:`real<Interaction.isReal>`.")
		.add_property("materials",&pyOmega::materials_get,"Shared materials; they can be accessed by id or by label")
		.add_property("forces",&pyOmega::forces_get,":yref:`ForceContainer` (forces, torques, displacements) in the current simulation.")
		.add_property("energy",&pyOmega::energy_get,":yref:`EnergyTracker` of the current simulation. (meaningful only with :yref:`O.trackEnergy<Omega.trackEnergy>`)")
		.add_property("trackEnergy",&pyOmega::trackEnergy_get,&pyOmega::trackEnergy_set,"When energy tracking is enabled or disabled in this simulation.")
		.add_property("tags",&pyOmega::tags_get,"Tags (string=string dictionary) of the current simulation (container supporting string-index access/assignment)")
		.def("childClassesNonrecursive",&pyOmega::listChildClassesNonrecursive,"Return list of all classes deriving from given class, as registered in the class factory")
		.def("isChildClassOf",&pyOmega::isChildClassOf,"Tells whether the first class derives from the second one (both given as strings).")
		.add_property("timingEnabled",&pyOmega::timingEnabled_get,&pyOmega::timingEnabled_set,"Globally enable/disable timing services (see documentation of the :yref:`timing module<yade.timing>`).")
		.add_property("forceSyncCount",&pyOmega::forceSyncCount_get,&pyOmega::forceSyncCount_set,"Counter for number of syncs in ForceContainer, for profiling purposes.")
		.add_property("numThreads",&pyOmega::numThreads_get /* ,&pyOmega::numThreads_set*/ ,"Get maximum number of threads openMP can use.")
		.add_property("cell",&pyOmega::cell_get,"Periodic cell of the current scene (None if the scene is aperiodic).")
		.add_property("periodic",&pyOmega::periodic_get,&pyOmega::periodic_set,"Get/set whether the scene is periodic or not (True/False).")
		.def("exitNoBacktrace",&pyOmega::exitNoBacktrace,(py::arg("status")=0),"Disable SEGV handler and exit, optionally with given status number.")
		.def("disableGdb",&pyOmega::disableGdb,"Revert SEGV and ABRT handlers to system defaults.")
		.def("runEngine",&pyOmega::runEngine,"Run given engine exactly once; simulation time, step number etc. will not be incremented (use only if you know what you do).")
		.def("tmpFilename",&pyOmega::tmpFilename,"Return unique name of file in temporary directory which will be deleted when yade exits.")
		;
	py::class_<pyTags>("TagsWrapper","Container emulating dictionary semantics for accessing tags associated with simulation. Tags are accesed by strings.",py::init<pyTags&>())
		.def("__getitem__",&pyTags::getItem)
		.def("__setitem__",&pyTags::setItem)
		.def("keys",&pyTags::keys)
		.def("has_key",&pyTags::hasKey);
	py::class_<pyBodyContainer>("BodyContainer",py::init<pyBodyContainer&>())
		.def("__getitem__",&pyBodyContainer::pyGetitem)
		.def("__len__",&pyBodyContainer::length)
		.def("__iter__",&pyBodyContainer::pyIter)
		.def("append",&pyBodyContainer::append,"Append one Body instance, return its id.")
		.def("append",&pyBodyContainer::appendList,"Append list of Body instance, return list of ids")
		.def("appendClumped",&pyBodyContainer::appendClump,(py::arg("discretization")=0),"Append given list of bodies as a clump (rigid aggregate); returns a tuple of ``(clumpId,[memberId1,memberId2,...])``. Clump masses and inertia are adapted automatically (for details see :yref:`clump()<BodyContainer.clump>`).")
		.def("clump",&pyBodyContainer::clump,(py::arg("discretization")=0),"Clump given bodies together (creating a rigid aggregate); returns ``clumpId``. Clump masses and inertia are adapted automatically when discretization>0. If clump members are overlapping this is done by integration/summation over mass points using a regular grid of cells (grid cells length is defined as $R_{min}/discretization$, where $R_{min}$ is minimum clump member radius). For non-overlapping members inertia of the clump is the sum of inertias from members. If discretization<=0 sum of inertias from members is used (faster, but inaccurate).")
		.def("updateClumpProperties",&pyBodyContainer::updateClumpProperties,(py::arg("excludeList")=py::list(),py::arg("discretization")=5),"Manually force Yade to update clump properties mass, volume and inertia (for details of 'discretization' value see :yref:`clump()<BodyContainer.clump>`). Can be used, when clumps are modified or erased during a simulation. Clumps can be excluded from the calculation by giving a list of ids: *O.bodies.updateProperties([ids])*.")
		.def("deleteClumpMember", &pyBodyContainer::deleteClumpMember,"Erase clump member.") //FIXME
		.def("deleteClumpBody", &pyBodyContainer::deleteClumpBody,"Erase clump member.")//FIXME
		.def("addToClump",&pyBodyContainer::addToClump,(py::arg("discretization")=0),"Add body b (or a list of bodies) to an existing clump c. c must be clump and b may not be a clump member of c. Clump masses and inertia are adapted automatically (for details see :yref:`clump()<BodyContainer.clump>`).\n\nSee :ysrc:`examples/clumps/addToClump-example.py` for an example script.\n\n.. note:: If b is a clump itself, then all members will be added to c and b will be deleted. If b is a clump member of clump d, then all members from d will be added to c and d will be deleted. If you need to add just clump member b, :yref:`release<BodyContainer.releaseFromClump>` this member from d first.")
		.def("releaseFromClump",&pyBodyContainer::releaseFromClump,(py::arg("discretization")=0),"Release body b from clump c. b must be a clump member of c. Clump masses and inertia are adapted automatically (for details see :yref:`clump()<BodyContainer.clump>`).\n\nSee :ysrc:`examples/clumps/releaseFromClump-example.py` for an example script.\n\n.. note:: If c contains only 2 members b will not be released and a warning will appear. In this case clump c should be :yref:`erased<BodyContainer.erase>`.")
		.def("replaceByClumps",&pyBodyContainer::replaceByClumps,(py::arg("discretization")=0),"Replace spheres by clumps using a list of clump templates and a list of amounts; returns a list of tuples: ``[(clumpId1,[memberId1,memberId2,...]),(clumpId2,[memberId1,memberId2,...]),...]``. A new clump will have the same volume as the sphere, that was replaced. Clump masses and inertia are adapted automatically (for details see :yref:`clump()<BodyContainer.clump>`). \n\n\t *O.bodies.replaceByClumps( [utils.clumpTemplate([1,1],[.5,.5])] , [.9] ) #will replace 90 % of all standalone spheres by 'dyads'*\n\nSee :ysrc:`examples/clumps/replaceByClumps-example.py` for an example script.")
		.def("getRoundness",&pyBodyContainer::getRoundness,(py::arg("excludeList")=py::list()),"Returns roundness coefficient RC = R2/R1. R1 is the equivalent sphere radius of a clump. R2 is the minimum radius of a sphere, that imbeds the clump. If just spheres are present RC = 1. If clumps are present 0 < RC < 1. Bodies can be excluded from the calculation by giving a list of ids: *O.bodies.getRoundness([ids])*.\n\nSee :ysrc:`examples/clumps/replaceByClumps-example.py` for an example script.")
		.def("clear", &pyBodyContainer::clear,"Remove all bodies (interactions not checked)")
		.def("erase", &pyBodyContainer::erase,(py::arg("eraseClumpMembers")=0),"Erase body with the given id; all interaction will be deleted by InteractionLoop in the next step. If a clump is erased use *O.bodies.erase(clumpId,True)* to erase the clump AND its members.")
		.def("replace",&pyBodyContainer::replace)
	  .def("insertAtId",&pyBodyContainer::insertAtId,(py::arg("insertatid")),"Insert a body at theid, (no body should exist in this id)")
		.add_property("rawBodies",&pyBodyContainer::raw_bodies_get,&pyBodyContainer::raw_bodies_set,"Bodies in the current simulation in the form of pickle-friendly raw container. In typical situations it is better to access bodies as 'O.bodies', which offers better python support. This one may be used for debugging or advanced manipulations.");
	py::class_<pyBodyIterator>("BodyIterator",py::init<pyBodyIterator&>())
		.def("__iter__",&pyBodyIterator::pyIter)
		.def("next",&pyBodyIterator::pyNext);
	py::class_<pyInteractionContainer>("InteractionContainer","Access to :yref:`interactions<Interaction>` of simulation, by using \n\n#. id's of both :yref:`Bodies<Body>` of the interactions, e.g. ``O.interactions[23,65]``\n#. iteraction over the whole container::\n\n\tfor i in O.interactions: print i.id1,i.id2\n\n.. note::\n\tIteration silently skips interactions that are not :yref:`real<Interaction.isReal>`.",py::init<pyInteractionContainer&>())
		.def("__iter__",&pyInteractionContainer::pyIter)
		.def("__getitem__",&pyInteractionContainer::pyGetitem)
		.def("__len__",&pyInteractionContainer::len)
		.def("has",&pyInteractionContainer::has,"Tell if a pair of ids corresponds to an existing interaction (real or not)")
		.def("countReal",&pyInteractionContainer::countReal,"Return number of interactions that are \"real\", i.e. they have phys and geom.")
		.def("nth",&pyInteractionContainer::pyNth,"Return n-th interaction from the container (usable for picking random interaction). The virtual interactions are not reached.")
		.def("withBody",&pyInteractionContainer::withBody,"Return list of real interactions of given body.")
		.def("withBodyAll",&pyInteractionContainer::withBodyAll,"Return list of all (real as well as non-real) interactions of given body.")
		.def("all",&pyInteractionContainer::getAll,(py::arg("onlyReal")=false),"Return list of all interactions. Virtual interaction are filtered out if onlyReal=True, else (default) it dumps the full content.")
		.def("eraseNonReal",&pyInteractionContainer::eraseNonReal,"Erase all interactions that are not :yref:`real <Interaction.isReal>`.")
		.def("erase",&pyInteractionContainer::erase,"Erase one interaction, given by id1, id2 (internally, ``requestErase`` is called -- the interaction might still exist as potential, if the :yref:`Collider` decides so).")
		.add_property("serializeSorted",&pyInteractionContainer::serializeSorted_get,&pyInteractionContainer::serializeSorted_set)
		.def("clear",&pyInteractionContainer::clear,"Remove all interactions, and invalidate persistent collider data (if the collider supports it).");
	py::class_<pyInteractionIterator>("InteractionIterator",py::init<pyInteractionIterator&>())
		.def("__iter__",&pyInteractionIterator::pyIter)
		.def("next",&pyInteractionIterator::pyNext);

	py::class_<pyForceContainer>("ForceContainer",py::init<pyForceContainer&>())
		.def("f",&pyForceContainer::force_get,(py::arg("id"),py::arg("sync")=false),"Force applied on body. For clumps in openMP, synchronize the force container with sync=True, else the value will be wrong.")
		.def("t",&pyForceContainer::torque_get,(py::arg("id"),py::arg("sync")=false),"Torque applied on body. For clumps in openMP, synchronize the force container with sync=True, else the value will be wrong.")
		.def("m",&pyForceContainer::torque_get,(py::arg("id"),py::arg("sync")=false),"Deprecated alias for t (torque).")
		.def("move",&pyForceContainer::move_get,(py::arg("id")),"Displacement applied on body.")
		.def("rot",&pyForceContainer::rot_get,(py::arg("id")),"Rotation applied on body.")
		.def("addF",&pyForceContainer::force_add,(py::arg("id"),py::arg("f"),py::arg("permanent")=false),"Apply force on body (accumulates). The force applies for one iteration, then it is reset by ForceResetter. \n # permanent parameter is deprecated, instead of addF(...,permanent=True) use setPermF(...).")
		.def("addT",&pyForceContainer::torque_add,(py::arg("id"),py::arg("t"),py::arg("permanent")=false),"Apply torque on body (accumulates). The torque applies for one iteration, then it is reset by ForceResetter. \n # permanent parameter is deprecated, instead of addT(...,permanent=True) use setPermT(...).")
		.def("permF",&pyForceContainer::permForce_get,(py::arg("id")),"read the value of permanent force on body (set with setPermF()).")
		.def("permT",&pyForceContainer::permTorque_get,(py::arg("id")),"read the value of permanent torque on body (set with setPermT()).")
		.def("setPermF",&pyForceContainer::permForce_set, "set the value of permanent force on body.")
		.def("setPermT",&pyForceContainer::permTorque_set,"set the value of permanent torque on body.")
		.def("addMove",&pyForceContainer::move_add,(py::arg("id"),py::arg("m")),"Apply displacement on body (accumulates).")
		.def("addRot",&pyForceContainer::rot_add,(py::arg("id"),py::arg("r")),"Apply rotation on body (accumulates).")
		.def("reset",&pyForceContainer::reset,(py::arg("resetAll")=true),"Reset the force container, including user defined permanent forces/torques. resetAll=False will keep permanent forces/torques unchanged.")
		.def("getPermForceUsed",&pyForceContainer::getPermForceUsed,"Check wether permanent forces are present.")
		.add_property("syncCount",&pyForceContainer::syncCount_get,&pyForceContainer::syncCount_set,"Number of synchronizations  of ForceContainer (cummulative); if significantly higher than number of steps, there might be unnecessary syncs hurting performance.")
		;

	py::class_<pyMaterialContainer>("MaterialContainer","Container for :yref:`Materials<Material>`. A material can be accessed using \n\n #. numerical index in range(0,len(cont)), like cont[2]; \n #. textual label that was given to the material, like cont['steel']. This etails traversing all materials and should not be used frequently.",py::init<pyMaterialContainer&>())
		.def("append",&pyMaterialContainer::append,"Add new shared :yref:`Material`; changes its id and return it.")
		.def("append",&pyMaterialContainer::appendList,"Append list of :yref:`Material` instances, return list of ids.")
		.def("index",&pyMaterialContainer::index,"Return id of material, given its label.")
		.def("__getitem__",&pyMaterialContainer::getitem_id)
		.def("__getitem__",&pyMaterialContainer::getitem_label)
		.def("__len__",&pyMaterialContainer::len);

	py::class_<STLImporter>("STLImporter").def("ymport",&STLImporter::import);

//////////////////////////////////////////////////////////////
///////////// proxyless wrappers 
	Serializable().pyRegisterClass(py::scope());

	py::class_<TimingDeltas, shared_ptr<TimingDeltas>, boost::noncopyable >("TimingDeltas").add_property("data",&TimingDeltas::pyData,"Get timing data as list of tuples (label, execTime[nsec], execCount) (one tuple per checkpoint)").def("reset",&TimingDeltas::reset,"Reset timing information");

	py::scope().attr("O")=pyOmega();
}

