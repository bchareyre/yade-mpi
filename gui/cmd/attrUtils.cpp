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
	//.def("__getattr__",&cxxClass::wrappedPyGet).def("__setattr__",&cxxClass::wrappedPySet)


/*! Helper class for accessing registered attributes through the serialization interface.
 *
 * 4 possible types of attributes are supported: bool, string, number, array of numbers.
 * This class exposes pySet, pyGet and pyKeys methods to python so that associated object supports python syntax for dictionary member access.
 */
class AttrAccess{
	struct AttrDesc{vector<int> types; shared_ptr<Archive> archive;};
	private:
		const shared_ptr<Serializable> ser;
		Serializable::Archives archives;
	public:
		DECLARE_LOGGER;
		typedef map<string,AttrDesc> DescriptorMap;
		//! maps attribute name to its archive and vector of its types (given as ints, from the following enum)
		DescriptorMap descriptors;
		//! allowed types
		enum {BOOL,STRING,NUMBER}; // allowed types
		
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
					// serialize to get size
					stringstream stream; vector<string> values;
					(*ai)->serialize(stream,**ai,0); IOFormatManager::parseFundamental(stream.str(),values);
					any instance=(*ai)->getAddress(); // gets pointer to the stored value
					// 3 possibilities: one BOOL, one STRING, one or more NUMBERs
					if     (values.size()==1 && any_cast<bool*>(&instance))   desc.types.push_back(AttrAccess::BOOL);
					else if(values.size()==1 && any_cast<string*>(&instance)) desc.types.push_back(AttrAccess::STRING);
					else {for(size_t i=0; i<values.size(); i++)               desc.types.push_back(AttrAccess::NUMBER);};
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
			for(size_t i=0; i<desc.types.size(); i++) types+=string(i>0?" ":"")+(desc.types[i]==BOOL?"BOOL":(desc.types[i]==STRING?"STRING":"NUMBER"));
			return name+" =\t"+vals+"\t ("+types+")";
		}
		//! call dumpAttr for all attributes (used for debugging)
		string dumpAttrs(){ string ret; for(DescriptorMap::iterator I=descriptors.begin();I!=descriptors.end();I++) ret+=dumpAttr(I->first)+"\n"; return ret;}
		//! set attribute by name from its serialized value
		void setAttrStr(string name, string value){
			LOG_DEBUG("Will set `"<<name<<"' to `"<<value<<"'.");
			stringstream voidStream;
			descriptors[name].archive->deserialize(voidStream,*(descriptors[name].archive),value);
		}
		//! set attribute from its (non-serialized) value, for all possible types
		void setAttr(string name, bool value){setAttrStr(name,value?"1":"0");}
		void setAttr(string name, string value){setAttrStr(name,value);}
		void setAttr(string name, double value){setAttrStr(name,lexical_cast<string>(value));}
		void setAttr(string name, vector<double> values){string val("{"); for(size_t i=0; i<values.size();i++) val+=(lexical_cast<string>(values[i]))+" "; setAttrStr(name,val+"}"); }

		//! return python list of keys (attribute names)
		boost::python::list pyKeys(){boost::python::list ret; for(DescriptorMap::iterator I=descriptors.begin();I!=descriptors.end();I++)ret.append(I->first); return ret;}

		//! return attribute value as python object
		boost::python::object pyGet(std::string key){
			DescriptorMap::iterator I=descriptors.find(key);
			if(I==descriptors.end()) throw std::invalid_argument(string("Invalid key: `")+key+"'.");
			vector<string> raw=getAttrStr(key);
			if(raw.size()==1){
				if(descriptors[key].types[0]==BOOL) return boost::python::object(lexical_cast<bool>(raw[0]));
				if(descriptors[key].types[0]==STRING) return boost::python::object(raw[0]);
				if(descriptors[key].types[0]==NUMBER) return boost::python::object(lexical_cast<double>(raw[0]));
			} else { // list of numbers
				/*list<double> ret;
				for(vector<string>::iterator I=raw.begin();I!=raw.end();I++)ret.push_back(lexical_cast<double>(*I));
				return boost::python::object<list<double> >(ret);*/
				boost::python::list ret;
				for(vector<string>::iterator I=raw.begin();I!=raw.end();I++)ret.append(lexical_cast<double>(*I));
				return ret;
			}
			return boost::python::object();
		}
		//! set attribute value from python object
		void pySet(std::string key, python::object val){
			DescriptorMap::iterator I=descriptors.find(key);
			if(I==descriptors.end()) throw std::invalid_argument(string("Invalid key: `")+key+"'.");
			if(descriptors[key].types.size()==1){
				if(descriptors[key].types[0]==BOOL) { setAttr(key,python::extract<bool>(val)); return;}
				if(descriptors[key].types[0]==STRING){setAttr(key,python::extract<string>(val)); return;}
				if(descriptors[key].types[0]==NUMBER){setAttr(key,python::extract<double>(val)); return;}
			} else {
				if(PySequence_Check(val.ptr()) && PySequence_Size(val.ptr())==(int)descriptors[key].types.size()){
					vector<double> cval;
					for(int i=0; i<PySequence_Size(val.ptr()); i++) cval.push_back(python::extract<double>(PySequence_GetItem(val.ptr(),i)));
					setAttr(key,cval);
				} else {	throw std::invalid_argument(string("Value for `")+key+"' not sequence or not of the expected length ("+lexical_cast<string>(descriptors[key].types.size())+")"); }
			}
		}
};

CREATE_LOGGER(AttrAccess);

