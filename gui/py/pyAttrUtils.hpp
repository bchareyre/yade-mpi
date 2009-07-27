// 2007,2008,2009 © Václav Šmilauer <eudoxos@arcig.cz> 
#include<sstream>
#include<boost/any.hpp>
#include<map>
#include<vector>
#include<yade/lib-serialization/Archive.hpp>
#include<yade/lib-serialization-xml/XMLFormatManager.hpp>
#include<boost/python.hpp>
#include<yade/extra/boost_python_len.hpp>

#include<boost/foreach.hpp>
#ifndef FOREACH
	#define FOREACH BOOST_FOREACH
#endif

using namespace std;
using namespace boost;

/*! Define proxy functions that access attributes of the proxee object associated with this class.
 */
#define ATTR_ACCESS_CXX() \
	python::object wrappedPyGetAttr2(const string& key) const {return proxee->pyGetAttr(key);}\
	void wrappedPySetAttr2(const string& key, const python::object& value){proxee->pySetAttr(key,value);}\
	python::list wrappedPyKeys2() const {return proxee->pyKeys();}\
	bool wrappedPyHasKey2(const string& key) const {return proxee->pyHasKey(key);}\
	python::dict wrappedPyDict2() const {return proxee->pyDict();}  \
	python::list wrappedUpdateExistingAttrs2(const python::dict& d){ python::list ret; python::list l=d.items(); size_t ll=python::len(l); for(size_t i=0; i<ll; i++){ python::tuple t=python::extract<python::tuple>(l[i]); string key=python::extract<string>(t[0]); if(wrappedPyHasKey2(key)) wrappedPySetAttr2(key,t[1]); else ret.append(t[0]); } return ret; } \
	void wrappedUpdateAttrs2(const python::dict& d){ python::list l=d.items(); size_t ll=python::len(l); for(size_t i=0; i<ll; i++){ python::tuple t=python::extract<python::tuple>(l[i]); string key=python::extract<string>(t[0]); wrappedPySetAttr2(key,t[1]); } }
	

/*! Python special functions complementing proxies defined by ATTR_ACCESS_CXX, to be used with boost::python::class_<>.
 *
 * They define python special functions that support dictionary operations on this object and calls proxies for them. */
#define ATTR_ACCESS_PY(cxxClass) \
	def("__getitem__",&cxxClass::wrappedPyGetAttr2).def("__setitem__",&cxxClass::wrappedPySetAttr2).def("keys",&cxxClass::wrappedPyKeys2).def("has_key",&cxxClass::wrappedPyHasKey2).def("dict",&cxxClass::wrappedPyDict2).def("updateExistingAttrs",&cxxClass::wrappedUpdateExistingAttrs2).def("updateAttrs",&cxxClass::wrappedUpdateAttrs2)

#define BASIC_PY_PROXY_HEAD(pyClass,yadeClass) \
class pyClass{ \
	private: void init(string clss){ proxee=dynamic_pointer_cast<yadeClass>(ClassFactory::instance().createShared(clss.empty()? #yadeClass : clss)); if(!proxee) throw runtime_error("Invalid class `"+clss+"': either nonexistent, or unable to cast to `"+#yadeClass+"'"); } \
	public: shared_ptr<yadeClass> proxee; \
		typedef yadeClass wrappedClass; \
		/* void ensureProxee(){ if(!proxee) throw runtime_error(string("No proxied `")+#yadeClass+"' (programming error, please report!)"); } */ \
		pyClass(string clss="", python::dict attrs=python::dict()){ init(clss); python::list l=attrs.items(); int len=PySequence_Size(l.ptr()); for(int i=0; i<len; i++){ python::extract<python::tuple> t(l[i]); python::extract<string> keyEx(t()[0]); if(!keyEx.check()) throw invalid_argument("Attribute keys must be strings."); wrappedPySetAttr2(keyEx(),t()[1]); } } \
		pyClass(const shared_ptr<yadeClass>& _proxee): proxee(_proxee) {} \
		std::string pyStr(void){ return string(proxee->getClassName()==#yadeClass ? "<"+proxee->getClassName()+">" : "<"+proxee->getClassName()+" "+ #yadeClass +">"); } \
		string className(void){ return proxee->getClassName(); } \
		void postProcessAttributes(void){ static_pointer_cast<Serializable>(proxee)->postProcessAttributes(/*deserializing*/ true); } \
		ATTR_ACCESS_CXX();

#define BASIC_PY_PROXY_TAIL };
/*! Basic implementation of python proxy class that provides:
 * 1. constructor with (optional) class name and (optional) dictionary of attributes
 * 2. copy constructor from another proxy class
 * 3. className() returning proxy class name as string 
 */
#define BASIC_PY_PROXY(pyClass,yadeClass) BASIC_PY_PROXY_HEAD(pyClass,yadeClass) BASIC_PY_PROXY_TAIL

/* Read-write access to some attribute that is not basic-serializable, i.e. must be exported as instance.attribute (not instance['attribute']. That attribute is wrapped in given python class before it is returned. */
#define NONPOD_ATTRIBUTE_ACCESS(pyName,pyClass,yadeName) \
	python::object pyName##_get(void){ return proxee->yadeName ? python::object(pyClass(proxee->yadeName)) : python::object(); } \
	void pyName##_set(pyClass proxy){ proxee->yadeName=proxy.proxee; }
/*! Boost.python's definition of python object corresponding to BASIC_PY_PROXY */
#define BASIC_PY_PROXY_WRAPPER(pyClass,pyName)  \
	boost::python::class_<pyClass /*, shared_ptr<pyClass::wrappedClass> --- see http://wiki.python.org/moin/boost.python/PointersAndSmartPointers */ >(pyName,python::init<python::optional<string,python::dict> >()) \
	.ATTR_ACCESS_PY(pyClass) \
	.def("__str__",&pyClass::pyStr).def("__repr__",&pyClass::pyStr) \
	.add_property("name",&pyClass::className) \
	.def("postProcessAttributes",&pyClass::postProcessAttributes)

