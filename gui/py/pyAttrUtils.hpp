// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include<sstream>
#include<boost/any.hpp>
#include<map>
#include<vector>
#include<yade/lib-serialization/Archive.hpp>
#include<yade/lib-serialization-xml/XMLFormatManager.hpp>
#include<boost/python.hpp>

#include<boost/foreach.hpp>
#ifndef FOREACH
	#define FOREACH BOOST_FOREACH
#endif

using namespace std;
using namespace boost;

/*! Define inline proxy functions that access attributes of some object associated with this class.
 * \param accessor is an shared_ptr<AttrAccess> (or AttrAccess*), properly initialized and associated with the object of your choice
 * \param ensureFunc is member function called before every attribute access. It typically would check whether acessor is not NULL, otherwise instantiate it.
 */
#define ATTR_ACCESS_CXX(accessor,ensureFunc) \
	boost::python::object wrappedPyGet(std::string key){ensureFunc();return accessor->pyGet(key);} \
	void wrappedPySet(std::string key,python::object val){ensureFunc(); accessor->pySet(key,val);} \
	string wrappedGetAttrStr(std::string key){ensureFunc();vector<string> a=accessor->getAttrStr(key); string ret("["); FOREACH(string s, a) ret+=s+" "; return ret+"]";} \
	void wrappedSetAttrStr(std::string key, std::string val){ensureFunc();return accessor->setAttrStr(key,val);} \
	boost::python::list wrappedPyKeys(){ensureFunc(); return accessor->pyKeys();} \
	bool wrappedPyHasKey(std::string key){ensureFunc(); return accessor->descriptors.find(key)!=accessor->descriptors.end();} \
	
	
	//boost::python::object wrappedPyGet_throw(std::string key){ensureFunc(); if(wrappedPyHasKey(key)) return accessor->pyGet(key); PyErr_SetString(PyExc_AttributeError, "No such attribute."); boost::python::throw_error_already_set(); /* make compiler happy*/ return boost::python::object(); }


/*! Python special functions complementing proxies defined by ATTR_ACCESS_CXX, to be used with boost::python::class_<>.
 *
 * They define python special functions that support dictionary operations on this object and calls proxies for them. */
#define ATTR_ACCESS_PY(cxxClass) \
	def("__getitem__",&cxxClass::wrappedPyGet).def("__setitem__",&cxxClass::wrappedPySet).def("keys",&cxxClass::wrappedPyKeys).def("has_key",&cxxClass::wrappedPyHasKey) \
	.def("getRaw",&cxxClass::wrappedGetAttrStr).def("setRaw",&cxxClass::wrappedSetAttrStr)
	//def("__getattr__",&cxxClass::wrappedPyGet).def("__setattr__",&cxxClass::wrappedPySet).def("attrs",&cxxClass::wrappedPyKeys)

	//.def("__getattribute__",&cxxClass::wrappedPyGet_throw)

#define BASIC_PY_PROXY_HEAD(pyClass,yadeClass) \
class pyClass{shared_ptr<AttrAccess> accessor; \
	private: void init(string clss){ proxee=dynamic_pointer_cast<yadeClass>(ClassFactory::instance().createShared(clss.empty()? #yadeClass : clss)); if(!proxee) throw runtime_error("Invalid class `"+clss+"': either nonexistent, or unable to cast to `"+#yadeClass+"'"); } \
	public: shared_ptr<yadeClass> proxee; \
		void ensureAcc(void){ if(!proxee) throw runtime_error(string("No proxied `")+#yadeClass+"'."); if(!accessor) accessor=shared_ptr<AttrAccess>(new AttrAccess(proxee));} \
		pyClass(string clss="", python::dict attrs=python::dict()){ init(clss); python::list l=attrs.items(); int len=PySequence_Size(l.ptr()); for(int i=0; i<len; i++){ python::extract<python::tuple> t(l[i]); python::extract<string> keyEx(t()[0]); if(!keyEx.check()) throw invalid_argument("Attribute keys must be strings."); wrappedPySet(keyEx(),t()[1]); } } \
		pyClass(const shared_ptr<yadeClass>& _proxee): proxee(_proxee) {} \
		std::string pyStr(void){ ensureAcc(); return string(proxee->getClassName()==#yadeClass ? "<"+proxee->getClassName()+">" : "<"+proxee->getClassName()+" "+ #yadeClass +">"); } \
		string className(void){ ensureAcc(); return proxee->getClassName(); } \
		void postProcessAttributes(void){ensureAcc(); static_pointer_cast<Serializable>(proxee)->/*__HACK__D@_N@T_ABUSE_*/postProcessAttributes(/*deserializing*/ true); } \
		ATTR_ACCESS_CXX(accessor,ensureAcc);

#define BASIC_PY_PROXY_TAIL };
/*! Basic implementation of python proxy class that provides:
 * 1. constructor with (optional) class name and (optional) dictionary of attributes
 * 2. copy constructor from another proxy class
 * 3. className() returning proxy class name as string 
 * 4. ensureAcc() that makes sure we have proxy _and_ attribute access things ready
 */
#define BASIC_PY_PROXY(pyClass,yadeClass) BASIC_PY_PROXY_HEAD(pyClass,yadeClass) BASIC_PY_PROXY_TAIL

/* Read-write access to some attribute that is not basic-serializable, i.e. must be exported as instance.attribute (not instance['attribute']. That attribute is wrapped in given python class before it is returned. */
#define NONPOD_ATTRIBUTE_ACCESS(pyName,pyClass,yadeName) \
	python::object pyName##_get(void){ensureAcc(); return proxee->yadeName ? python::object(pyClass(proxee->yadeName)) : python::object(); } \
	void pyName##_set(pyClass proxy){ensureAcc(); proxee->yadeName=proxy.proxee; }
/*! Boost.python's definition of python object corresponding to BASIC_PY_PROXY */
#define BASIC_PY_PROXY_WRAPPER(pyClass,pyName)  \
	boost::python::class_<pyClass>(pyName,python::init<python::optional<string,python::dict> >()) \
	.ATTR_ACCESS_PY(pyClass) \
	.def("__str__",&pyClass::pyStr).def("__repr__",&pyClass::pyStr) \
	.add_property("name",&pyClass::className) \
	.def("postProcessAttributes",&pyClass::postProcessAttributes)



/*! Helper class for accessing registered attributes through the serialization interface.
 *
 * 4 possible types of attributes are supported: bool, string, number and arrays (homogeneous) of any of them.
 * This class exposes pySet, pyGet and pyKeys methods to python so that associated object supports python syntax for dictionary member access.
 */
class AttrAccess{
	struct AttrDesc{int type; shared_ptr<Archive> archive;};
	private:
		const shared_ptr<Serializable> ser;
		Serializable::Archives archives;
	public:
		DECLARE_LOGGER;
		typedef map<string,AttrDesc> DescriptorMap;
		//! maps attribute name to its archive and vector of its types (given as ints, from the following enum)
		DescriptorMap descriptors;
		//! allowed types
		enum {BOOL,STRING,FLOAT,INTEGER,SEQ_FLOAT, SEQ_STRING, VEC_VEC }; // allowed types
		
		AttrAccess(Serializable* _ser): ser(shared_ptr<Serializable>(_ser)){init();}
		AttrAccess(shared_ptr<Serializable> _ser):ser(_ser){init();}

		//! create archives and descriptors, always called from the constructor
		void init(){
			//cerr<<typeid(std::vector<Wm3::Vector3<double> >*).name()<<endl;
			if(ser->getArchives().empty()) ser->registerSerializableAttributes(false);
			archives=ser->getArchives();
			for(Serializable::Archives::iterator ai=archives.begin();ai!=archives.end();++ai){
				if((*ai)->isFundamental() && (*ai)->getName()!="serializationDynlib"){
					AttrDesc desc; 
					desc.archive=*ai;
					any instance=(*ai)->getAddress(); // gets pointer to the stored value
					//cerr<<"["<<(*ai)->getName()<<","<<instance.type().name()<<"]";
					// 3 possibilities: one BOOL, one or more STRINGS, one or more FLOATs (fallback if none matches)
					if      (any_cast<string*>(&instance)) { desc.type=AttrAccess::STRING; goto found; }
					else if (any_cast<bool*>(&instance)) { desc.type=AttrAccess::BOOL; goto found; }
					else if (any_cast<Real*>(&instance) || any_cast<long double*>(&instance) || any_cast<double*>(&instance) || any_cast<float*>(&instance)) { desc.type=AttrAccess::FLOAT; goto found;}
					else if (any_cast<int*>(&instance) || any_cast<unsigned int*>(&instance) || any_cast<long*>(&instance) || any_cast<unsigned long*>(&instance)) {desc.type=AttrAccess::INTEGER; goto found; }
					else if (any_cast<vector<string>*>(&instance)) { desc.type=AttrAccess::SEQ_STRING; goto found; }
				#if 0
					else if (any_cast<vector<Vector3r>*>(&instance)) { cerr<<"WWWWWWWWWWWWW"<<endl;}
					//else if (any_cast<vector<Vector3r>*>(&&instance)) { cerr<"QQQQQQQQQQQQQQ"<<endl;}
					#define GET_TYPE_DIRECT(_type,ATTR_ACCESS_TYPE) try{ cerr<<"Try "<<instance.type().name()<<" to "<<typeid(_type).name()<<endl; any_cast<_type >(instance); desc.type=ATTR_ACCESS_TYPE; cerr<<"OK!!!"<<endl; goto found; } catch(boost::bad_any_cast& e){}
					GET_TYPE_DIRECT(std::vector<Wm3::Vector3r>*,AttrAccess::VEC_VEC);
					else if (any_cast<vector<Wm3::Vector3<double> >*>(&instance)) {
						desc.type=AttrAccess::VEC_VEC;
						cerr<<"Attribute "<<(*ai)->getName()<<" is a vector<Vector3r>";
					//else if (any_cast<vector<Real>*>(&instance)) desc.type=AttrAccess::SEQ_FLOAT;
					}
				#endif
					desc.type=AttrAccess::SEQ_FLOAT;
					found:
						descriptors[(*ai)->getName()]=desc;
				}
			}
			//cerr<<dumpAttrs();
		}
		
		//! Return serialized attribute by its name, as vector of strings
		vector<string> getAttrStr(string name){
			vector<string> ret;
			shared_ptr<Archive> arch=descriptors[name].archive;
			stringstream stream;
			arch->serialize(stream,*arch,0);
			IOFormatManager::parseFundamental(stream.str(),ret);
			return ret;
		}
		//! name, values and types of given attribute, on one line as string
		string dumpAttr(string name){
			string vals,types; AttrDesc desc=descriptors[name]; vector<string> values=getAttrStr(name);
			for(size_t i=0; i<values.size(); i++) vals+=(i>0?" ":"")+values[i];
			string typeDesc(desc.type==BOOL?"BOOL":(desc.type==STRING?"STRING":(desc.type==FLOAT?"FLOAT":(desc.type==INTEGER?"INTEGER":(desc.type==SEQ_FLOAT?"SEQ_FLOAT":(desc.type==SEQ_STRING?"SEQ_STRING":(desc.type==VEC_VEC?"VEC_VEC":"<unknown>")))))));
			return name+" =\t"+vals+"\t ("+typeDesc+")";
		}
		//! call dumpAttr for all attributes (used for debugging)
		string dumpAttrs(){ string ret; for(DescriptorMap::iterator I=descriptors.begin();I!=descriptors.end();I++) ret+=dumpAttr(I->first)+"\n"; return ret;}
		//! set attribute by name from its serialized value
		void setAttrStr(string name, string value){
			LOG_DEBUG("Will set `"<<name<<"' to `"<<value<<"'.");
			stringstream voidStream;
			descriptors[name].archive->deserialize(voidStream,*(descriptors[name].archive),value);
		}
		//! return python list of keys (attribute names)
		boost::python::list pyKeys(){boost::python::list ret; for(DescriptorMap::iterator I=descriptors.begin();I!=descriptors.end();I++)ret.append(I->first); return ret;}


		//! return attribute value as python object
		boost::python::object pyGet(std::string key){
			DescriptorMap::iterator I=descriptors.find(key);
			if(I==descriptors.end()) throw std::invalid_argument(string("Invalid key: `")+key+"'.");
			vector<string> raw=getAttrStr(key);
			LOG_DEBUG("Got raw attribute `"<<key<<"'");
			switch(descriptors[key].type){
				case BOOL: return python::object(lexical_cast<bool>(raw[0]));
				case FLOAT: return python::object(lexical_cast_maybeNanInf<double>(raw[0]));
				case INTEGER: return python::object(lexical_cast_maybeNanInf<long>(raw[0]));
				case STRING: return python::object(raw[0]);
				case SEQ_STRING: {python::list ret; for(size_t i=0; i<raw.size(); i++) ret.append(python::object(raw[i])); return ret;}
				case SEQ_FLOAT: {python::list ret; for(size_t i=0; i<raw.size(); i++){ ret.append(python::object(lexical_cast_maybeNanInf<double>(raw[i]))); LOG_TRACE("Appended "<<raw[i]);} return ret; }
				case VEC_VEC: {
					python::list ret; for(size_t i=0; i<raw.size(); i++){
						/* raw[i] has the form "{number number number}" */
						vector<string> s; boost::algorithm::split(s,raw[i],algorithm::is_any_of("{} "),algorithm::token_compress_on);
						python::list subList; FOREACH(string ss, s) subList.append(python::object(lexical_cast_maybeNanInf<double>(ss)));
						ret.append(subList);
					}
				}
				default: throw runtime_error("Unhandled attribute type!");
			}
		}
		//! set attribute value from python object
		void pySet(std::string key, python::object val){
			DescriptorMap::iterator I=descriptors.find(key);
			if(I==descriptors.end()) throw std::invalid_argument(string("Invalid key: `")+key+"'.");
			#define SAFE_EXTRACT(from,to,type) python::extract<type> to(from); if(!to.check()) throw invalid_argument(string("Could not extract type ")+#type);
			switch(descriptors[key].type){
				case BOOL: {SAFE_EXTRACT(val.ptr(),extr,bool); setAttrStr(key,extr()?"1":"0"); break;}
				case FLOAT: {SAFE_EXTRACT(val.ptr(),extr,double); setAttrStr(key,lexical_cast<string>(extr())); break; }
				case INTEGER: {SAFE_EXTRACT(val.ptr(),extr,long); setAttrStr(key,lexical_cast<string>(extr())); break; }
				case STRING: {SAFE_EXTRACT(val.ptr(),extr,string); setAttrStr(key,extr()); break;}
				case SEQ_STRING:{
					if(!PySequence_Check(val.ptr())) throw invalid_argument("String sequence argument required.");
					string strVal("[");
					for(int i=0; i<PySequence_Size(val.ptr()); i++){SAFE_EXTRACT(PySequence_GetItem(val.ptr(),i),extr,string); strVal+=extr()+" ";}
					setAttrStr(key,strVal+"]");
				} 
				break;
				case SEQ_FLOAT:{
					if(!PySequence_Check(val.ptr())) throw invalid_argument("Number sequence argument required.");
					string strVal("{");
					for(int i=0; i<PySequence_Size(val.ptr()); i++){SAFE_EXTRACT(PySequence_GetItem(val.ptr(),i),extr,double); strVal+=lexical_cast<string>(extr())+" ";}
					setAttrStr(key,strVal+"}");
				}
				break;
				default: throw runtime_error("Invalid argument types!!");
			}
		}
};

CREATE_LOGGER(AttrAccess);

