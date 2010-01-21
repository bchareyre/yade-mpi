/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <boost/any.hpp>
#include <boost/foreach.hpp>
#ifndef  __GXX_EXPERIMENTAL_CXX0X__
#	include<boost/shared_ptr.hpp>
	using boost::shared_ptr;
#else
#	include<memory>
	using std::shared_ptr;
#endif
#include<boost/python.hpp>
#include<boost/type_traits.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/preprocessor.hpp>
#include<list>
#include<map>
#include<string>
#include<vector>
#include<iostream>
#include<yade/lib-factory/Factorable.hpp>
#include<yade/lib-pyutil/raw_constructor.hpp>
#include"SerializationExceptions.hpp"
#include"Archive.hpp"




using namespace boost;
using namespace std;
using namespace ArchiveTypes;

#define DECLARE_POINTER_TO_MY_CUSTOM_CLASS(Type,attribute,any)		\
	Type * attribute=any_cast< Type * >(any);

#define REGISTER_ATTRIBUTE_(attribute)                                   \
                registerAttribute( #attribute, attribute );


// placeholder function for registration with empty base
namespace{
	void registerAttributes(){}
};

#ifdef YADE_BOOST_SERIALIZATION
	#define _REGISTER_BOOST_ATTRIBUTES_REPEAT(x,y,z) ar & BOOST_SERIALIZATION_NVP(z);
	#define _REGISTER_BOOST_ATTRIBUTES(baseClass,attrs) \
		friend class boost::serialization::access; private: template<class ArchiveT> void serialize(ArchiveT & ar, unsigned int version){ preProcessAttributes(ArchiveT::is_loading::value); ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(baseClass); BOOST_PP_SEQ_FOR_EACH(_REGISTER_BOOST_ATTRIBUTES_REPEAT,~,attrs) postProcessAttributes(ArchiveT::is_loading::value); }
#else
	#define _REGISTER_BOOST_ATTRIBUTES(baseClass,attrs)
#endif



#define _REGISTER_ATTRIBUTES_REPEAT(x,y,z) registerAttribute(BOOST_PP_STRINGIZE(z),z);
//! create member function that register attributes; must be parenthesized, without commas: (attr1) (attr2) (attr3) ...
//#define REGISTER_ATTRIBUTES(attrs) protected: void registerAttributes(){ REGISTER_ATTRIBUTES_MANY(attrs) }
//! Same as REGISTER_ATTRIBUTES, but with first argument of base class, of which registerAttributes will be called first

// without python
// #define REGISTER_ATTRIBUTES(baseClass,attrs) protected: void registerAttributes(){ baseClass::registerAttributes(); BOOST_PP_SEQ_FOR_EACH(_REGISTER_ATTRIBUTES_REPEAT,~,attrs) } _REGISTER_BOOST_ATTRIBUTES(baseClass,attrs)

#include<boost/python.hpp>

namespace{
	boost::python::object pyGetAttr(const std::string& key){ PyErr_SetString(PyExc_KeyError,(std::string("No such attribute: ")+key+".").c_str()); boost::python::throw_error_already_set(); /*never reached; avoids warning*/ throw; }
	void pySetAttr(const std::string& key, const boost::python::object& /* value */){ PyErr_SetString(PyExc_KeyError,(std::string("No such attribute: ")+key+".").c_str()); boost::python::throw_error_already_set(); }
	boost::python::list pyKeys(){ return boost::python::list();}
	bool pyHasKey(const std::string&) { return false; }
	boost::python::dict pyDict() { return boost::python::dict(); }
};

#define _PYGET_ATTR(x,y,z) if(key==BOOST_PP_STRINGIZE(z)) return boost::python::object(z);
#define _PYSET_ATTR(x,y,z) if(key==BOOST_PP_STRINGIZE(z)) {z=boost::python::extract<typeof(z)>(value); return; }
#define _PYKEYS_ATTR(x,y,z) ret.append(BOOST_PP_STRINGIZE(z));
#define _PYHASKEY_ATTR(x,y,z) if(key==BOOST_PP_STRINGIZE(z)) return true;
#define _PYDICT_ATTR(x,y,z) ret[BOOST_PP_STRINGIZE(z)]=boost::python::object(z);
#define _PYCLASS_DEF(x,thisClass,z) .def_readwrite(BOOST_PP_STRINGIZE(z),&thisClass::z)

#define REGISTER_ATTRIBUTES(baseClass,attrs) protected: void registerAttributes(){ baseClass::registerAttributes(); BOOST_PP_SEQ_FOR_EACH(_REGISTER_ATTRIBUTES_REPEAT,~,attrs) } _REGISTER_BOOST_ATTRIBUTES(baseClass,attrs) \
	public: boost::python::object pyGetAttr(const std::string& key) const{ BOOST_PP_SEQ_FOR_EACH(_PYGET_ATTR,~,attrs); return baseClass::pyGetAttr(key); } \
	void pySetAttr(const std::string& key, const boost::python::object& value){  BOOST_PP_SEQ_FOR_EACH(_PYSET_ATTR,~,attrs); baseClass::pySetAttr(key,value); } \
	boost::python::list pyKeys() const { boost::python::list ret; BOOST_PP_SEQ_FOR_EACH(_PYKEYS_ATTR,~,attrs); ret.extend(baseClass::pyKeys()); return ret; } \
	bool pyHasKey(const std::string& key) const { BOOST_PP_SEQ_FOR_EACH(_PYHASKEY_ATTR,~,attrs); return baseClass::pyHasKey(key); } \
	boost::python::dict pyDict() const { boost::python::dict ret; BOOST_PP_SEQ_FOR_EACH(_PYDICT_ATTR,~,attrs); ret.update(baseClass::pyDict()); return ret; }

#define YADE_CLASS_BASE_ATTRS_PY(thisClass,baseClass,attrs,extras) \
	REGISTER_ATTRIBUTES(baseClass,attrs) \
	REGISTER_CLASS_AND_BASE(thisClass,baseClass) \
	virtual void pyRegisterClass(python::object _scope) const { if(getClassName()!=#thisClass) return; boost::python::scope thisScope(_scope); boost::python::class_<thisClass,shared_ptr<thisClass>,boost::python::bases<baseClass>,boost::noncopyable>(#thisClass).def("__init__",python::raw_constructor(Serializable_ctor_kwAttrs<thisClass>)).def("clone",&Serializable_clone<thisClass>,python::arg("attrs")=python::dict()) BOOST_PP_SEQ_FOR_EACH(_PYCLASS_DEF,thisClass,attrs) extras ; }

#define YADE_CLASS_BASE_ATTRS(thisClass,baseClass,attrs) \
	YADE_CLASS_BASE_ATTRS_PY(thisClass,baseClass,attrs,)


// for both fundamental and non-fundamental cases
#define REGISTER_SERIALIZABLE_GENERIC(name,isFundamental) 						\
	REGISTER_FACTORABLE(name);								\
	REGISTER_SERIALIZABLE_DESCRIPTOR(name,name,SerializableTypes::SERIALIZABLE,isFundamental);

// implied non-fundamental
#define REGISTER_SERIALIZABLE(name) REGISTER_SERIALIZABLE_GENERIC(name,false)
// explicit fundamental
#define REGISTER_SERIALIZABLE_FUNDAMENTAL(name) REGISTER_SERIALIZABLE_GENERIC(name,true);

#define REGISTER_CUSTOM_CLASS(name,sname,isFundamental) 					\
	REGISTER_FACTORABLE(sname);								\
	REGISTER_SERIALIZABLE_DESCRIPTOR(name,sname,SerializableTypes::CUSTOM_CLASS,isFundamental);

// helper functions
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
	instance->pyUpdateAttrs(d);
	return instance;
}

template <typename T>
shared_ptr<T> Serializable_clone(const shared_ptr<T>& self, const python::dict& d){
	shared_ptr<Factorable> inst0=ClassFactory::instance().createShared(self->getClassName());
	if(!inst0) throw runtime_error("Invalid class `"+self->getClassName()+"' (not created by ClassFactory).");
	shared_ptr<T> inst=dynamic_pointer_cast<T>(inst0);
	if(!inst) throw runtime_error("Invalid class `"+self->getClassName()+"' (unable to cast to typeid `"+typeid(T).name()+"')");
	inst->pyUpdateAttrs(self->pyDict());
	// if d not empty (how to test that?)
	inst->pyUpdateAttrs(d);
	inst->postProcessAttributes(/*deserializing*/true);
	return inst;
}



class Serializable : public Factorable
{
	public :
		typedef list< shared_ptr<Archive> >	Archives;
		Serializable() {};
		virtual ~Serializable() {};
	
		void unregisterSerializableAttributes(bool deserializing);
		void registerSerializableAttributes(bool deserializing);
		bool findAttribute(const string& name,shared_ptr<Archive>& arc);
		bool containsOnlyFundamentals();
		Archives& getArchives() 	{ return archives; };
		
		virtual void yadeSerialize(any& a)	{ throw SerializableError((SerializationExceptions::SetFunctionNotDeclared+string(" for type ")+a.type().name()).c_str()); };
		virtual void yadeDeserialize(any& a) { throw SerializableError((SerializationExceptions::GetFunctionNotDeclared+string(" for type ")+a.type().name()).c_str()); };

		virtual void postProcessAttributes(bool /*deserializing*/) {};

		// harmless even if boost::serialization is not used
		template <class ArchiveT> void serialize(ArchiveT & ar, unsigned int version){ };

		virtual boost::python::object pyGetAttr(const std::string& key) const { return ::pyGetAttr(key); }
		virtual void pySetAttr(const std::string& key, const boost::python::object& value){ ::pySetAttr(key,value); };
		virtual boost::python::list pyKeys() const {return ::pyKeys(); };
		virtual bool pyHasKey(const std::string& key) const {return ::pyHasKey(key);}
		virtual boost::python::dict pyDict() const { return ::pyDict(); }
		virtual void pyRegisterClass(boost::python::object _scope) const {
			// hack (string comparison), to catch method that is not overridden
			if(getClassName()!="Serializable"){ if(getenv("YADE_DEBUG")){std::cerr<<"WARN: class "+getClassName()+" did not register with YADE_CLASS_BASE_ATTRS"<<std::endl;} /* throw logic_error("Class "+getClassName()+" did not register with YADE_CLASS_BASE_ATTRS."); */ return; }
			// called properly
			boost::python::scope thisScope(_scope); 
			python::class_<Serializable, shared_ptr<Serializable>, noncopyable >("Serializable")
				.add_property("name",&Serializable::getClassName).def("__str__",&Serializable::pyStr).def("__repr__",&Serializable::pyStr).def("postProcessAttributes",&Serializable::postProcessAttributes,(python::arg("deserializing")=true))
				.def("dict",&Serializable::pyDict).def("__getitem__",&Serializable::pyGetAttr).def("__setitem__",&Serializable::pySetAttr).def("has_key",&Serializable::pyHasKey).def("keys",&Serializable::pyKeys)
				.def("updateAttrs",&Serializable::pyUpdateAttrs).def("updateExistingAttrs",&Serializable::pyUpdateExistingAttrs)
				.def("clone",&Serializable_clone<Serializable>,python::arg("attrs")=python::dict())
				.def("__init__",python::raw_constructor(Serializable_ctor_kwAttrs<Serializable>))
				;
		}
		
		//! update attributes from dictionary
		void pyUpdateAttrs(const boost::python::dict& d);
		//! update attributes from dictionary, silently skipping nonexistent attributes; skipped attributes names are returned as list
		python::list pyUpdateExistingAttrs(const boost::python::dict& d);
		//! string representation of this object
		std::string pyStr() { return "<"+getClassName()+" instance at "+lexical_cast<string>(this)+">"; }
		

	private :
		Archives				archives;
		friend class Archive;
	
	protected :
		virtual void registerAttributes() {};
		virtual void preProcessAttributes(bool /*deserializing*/) {};

		template <typename Type>
		void registerAttribute(const string& name, Type& attribute)
		{
			BOOST_FOREACH(shared_ptr<Archive> a,archives){if(a->getName()==name){ /* cerr<<"Attribute "<<name<<" already registered."<<endl; */ return;}};
			shared_ptr<Archive> ac = Archive::create(name,attribute);
			archives.push_back(ac);
		}

	REGISTER_CLASS_NAME(Serializable);
	REGISTER_BASE_CLASS_NAME(Factorable);
};

#include "MultiTypeHandler.tpp"


