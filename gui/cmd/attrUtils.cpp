#include<sstream>
#include<boost/any.hpp>
#include<map>
#include<vector>
#include<boost/shared_ptr.hpp>
#include<yade/lib-serialization-xml/XMLFormatManager.hpp>
#include<boost/python.hpp>

using namespace std;
using namespace boost;

/*! Define inline proxy functions that access attributes of some object associated with this class.
 * \param accessor is an shared_ptr<AttrAccess> (or AttrAccess*), properly initialized and associated with the object of your choice
 * \param ensureFunc is member function called before every attribute access. It typically would check whether acessor is not NULL, otherwise instantiate it.
 */
#define ATTR_ACCESS_CXX(accessor,ensureFunc) \
	boost::python::object wrappedPyGet(std::string key){ensureFunc();return accessor->pyGet(key);} \
	void wrappedPySet(std::string key,python::object val){ensureFunc(); accessor->pySet(key,val);} \
	boost::python::list wrappedPyKeys(){ensureFunc(); return accessor->pyKeys();} 
	
/*! Python special functions complementing proxies defined by ATTR_ACCESS_CXX, to be used with boost::python::class_<>.
 *
 * They define python special functions that support dictionary operations on this object and calls proxies for them. */
#define ATTR_ACCESS_PY(cxxClass) \
	def("__getitem__",&cxxClass::wrappedPyGet).def("__setitem__",&cxxClass::wrappedPySet).def("keys",&cxxClass::wrappedPyKeys)
	//def("__getattr__",&cxxClass::wrappedPyGet).def("__setattr__",&cxxClass::wrappedPySet).def("attrs",&cxxClass::wrappedPyKeys)


/*! Helper class for accessing registered attributes through the serialization interface.
 *
 * 4 possible types of attributes are supported: bool, string, number, array of numbers.
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
		enum {BOOL,STRING,NUMBER, SEQ_NUMBER, SEQ_STRING }; // allowed types
		
		AttrAccess(Serializable* _ser): ser(shared_ptr<Serializable>(_ser)){init();}
		AttrAccess(shared_ptr<Serializable> _ser):ser(_ser){init();}

		//! create archives and descriptors, always called from the constructor
		void init(){
			if(ser->getArchives().empty()) ser->registerSerializableAttributes(false);
			archives=ser->getArchives();
			for(Serializable::Archives::iterator ai=archives.begin();ai!=archives.end();++ai){
				if((*ai)->isFundamental() && (*ai)->getName()!="serializationDynlib"){
					AttrDesc desc; 
					desc.archive=*ai;
					any instance=(*ai)->getAddress(); // gets pointer to the stored value
					// 3 possibilities: one BOOL, one STRING, one or more NUMBERs
					if      (any_cast<string*>(&instance)) desc.type=AttrAccess::STRING;
					else if (any_cast<bool*>(&instance))   desc.type=AttrAccess::BOOL;
					else if (any_cast<Real*>(&instance) || any_cast<int*>(&instance) || any_cast<unsigned int*>(&instance) || any_cast<long*>(&instance) || any_cast<unsigned long*>(&instance)) desc.type=AttrAccess::NUMBER;
					else if (any_cast<vector<string>*>(&instance)) desc.type=AttrAccess::SEQ_STRING;
					//else if (any_cast<vector<Real>*>(&instance)) desc.type=AttrAccess::SEQ_NUMBER;
					else desc.type=AttrAccess::SEQ_NUMBER;
					descriptors[(*ai)->getName()]=desc;
				}
			}
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
			string typeDesc(desc.type==BOOL?"BOOL":(desc.type==STRING?"STRING":(desc.type==NUMBER?"NUMBER":(desc.type==SEQ_NUMBER?"SEQ_NUMBER":(desc.type==SEQ_STRING?"SEQ_STRING":"<unknown>")))));
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
				case NUMBER: return python::object(lexical_cast<double>(raw[0]));
				case STRING: return python::object(raw[0]);
				case SEQ_STRING: {python::list ret; for(size_t i=0; i<raw.size(); i++) ret.append(python::object(raw[i])); return ret;}
				case SEQ_NUMBER: {python::list ret; for(size_t i=0; i<raw.size(); i++){ ret.append(python::object(lexical_cast<double>(raw[i]))); LOG_TRACE("Appended "<<raw[i]);} return ret; }
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
				case NUMBER: {SAFE_EXTRACT(val.ptr(),extr,double); setAttrStr(key,lexical_cast<string>(extr())); break; }
				case STRING: {SAFE_EXTRACT(val.ptr(),extr,string); setAttrStr(key,extr()); break;}
				case SEQ_STRING:{
					if(!PySequence_Check(val.ptr())) throw invalid_argument("String sequence argument required.");
					string strVal("[");
					for(int i=0; i<PySequence_Size(val.ptr()); i++){SAFE_EXTRACT(PySequence_GetItem(val.ptr(),i),extr,string); strVal+=extr()+" ";}
					setAttrStr(key,strVal+"]");
				} 
				break;
				case SEQ_NUMBER:{
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

