#include<sstream>
#include<boost/any.hpp>
#include<map>
#include<vector>
#include<boost/shared_ptr.hpp>
#include<yade/lib-serialization-xml/XMLFormatManager.hpp>

using namespace std;
using namespace boost;

class AttrAccess{
	struct AttrDesc{vector<int> types; shared_ptr<Archive> archive;};
	private:
		const shared_ptr<Serializable> ser;
		Serializable::Archives archives;
	public:
		typedef map<string,AttrDesc> DescriptorMap;
		DescriptorMap descriptors; // maps attribute name to its archive and vector of its types (given as ints, from the following enum)
		enum {BOOL,STRING,NUMBER}; // allowed types
		
		AttrAccess(Serializable* _ser): ser(shared_ptr<Serializable>(_ser)){init();}
		AttrAccess(shared_ptr<Serializable> _ser):ser(_ser){init();}

		//! create archives and descriptors
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
		//! call dumpAttr for all attributes
		string dumpAttrs(){ string ret; for(DescriptorMap::iterator I=descriptors.begin();I!=descriptors.end();I++) ret+=dumpAttr(I->first)+"\n"; return ret;}

		//! set attribute by name from its serialized value
		void setAttrStr(string name, string value){
			stringstream voidStream;
			descriptors[name].archive->deserialize(voidStream,*(descriptors[name].archive),value);
		}
		//! set attribute from its (non-serialized) value
		void setAttr(string name, bool value){setAttrStr(name,value?"1":"0");}
		void setAttr(string name, string value){setAttrStr(name,value);}
		template<typename containedType> void setAttr(string name, vector<containedType> values){
			string val("{"); for(size_t i=0; i<values.size();i++) val+=(lexical_cast<string>(values[i]))+" "; setAttrStr(name,val+"}");
		}
};

