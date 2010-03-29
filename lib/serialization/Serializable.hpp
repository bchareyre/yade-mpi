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
#include<yade/lib-pyutil/doc_opts.hpp>
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

#include<boost/python.hpp>

namespace{
	boost::python::object pyGetAttr(const std::string& key){ PyErr_SetString(PyExc_KeyError,(std::string("No such attribute: ")+key+".").c_str()); boost::python::throw_error_already_set(); /*never reached; avoids warning*/ throw; }
	void pySetAttr(const std::string& key, const boost::python::object& /* value */){ PyErr_SetString(PyExc_AttributeError,(std::string("No such attribute: ")+key+".").c_str()); boost::python::throw_error_already_set(); }
	boost::python::list pyKeys(){ return boost::python::list();}
	bool pyHasKey(const std::string&) { return false; }
	boost::python::dict pyDict() { return boost::python::dict(); }
};

#define _DEPREC_OLDNAME(x) BOOST_PP_TUPLE_ELEM(3,0,x)
#define _DEPREC_NEWNAME(x) BOOST_PP_TUPLE_ELEM(3,1,x)
#define _DEPREC_COMMENT(x) BOOST_PP_TUPLE_ELEM(3,2,x) "" // if the argument is omited, return empty string instead of nothing

// loop bodies for attribute access
#define _PYGET_ATTR(x,y,z) if(key==BOOST_PP_STRINGIZE(z)) return boost::python::object(z);
#define _PYSET_ATTR(x,y,z) if(key==BOOST_PP_STRINGIZE(z)) {z=boost::python::extract<typeof(z)>(value); return; }
#define _PYKEYS_ATTR(x,y,z) ret.append(BOOST_PP_STRINGIZE(z));
#define _PYHASKEY_ATTR(x,y,z) if(key==BOOST_PP_STRINGIZE(z)) return true;
#define _PYDICT_ATTR(x,y,z) ret[BOOST_PP_STRINGIZE(z)]=boost::python::object(z);
#define _PYATTR_DEF(x,thisClass,z) .def_readwrite(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2,0,z)),&thisClass::BOOST_PP_TUPLE_ELEM(2,0,z),BOOST_PP_TUPLE_ELEM(2,1,z))

// macros for deprecated attribute access
// gcc<=4.2 is not able to compile this code; we will just not generate any code for deprecated attributes in such case
#if defined(__GNUG__) && __GNUC__ <= 4 && __GNUC_MINOR__ <=2
	#define _PYSET_ATTR_DEPREC(x,y,z)
	#define _PYATTR_DEPREC_DEF(x,y,z)
	#define _PYHASKEY_ATTR_DEPREC(x,y,z)
	#define _ACCESS_DEPREC(x,y,z)
#else 
	// gcc < 4.2 && non-gcc compilers
	#define _PYSET_ATTR_DEPREC(x,thisClass,z) if(key==BOOST_PP_STRINGIZE(_DEPREC_OLDNAME(z))){ _DEPREC_WARN(thisClass,z); _DEPREC_NEWNAME(z)=boost::python::extract<typeof(_DEPREC_NEWNAME(z))>(value); return; }
	#define _PYATTR_DEPREC_DEF(x,thisClass,z) .add_property(BOOST_PP_STRINGIZE(_DEPREC_OLDNAME(z)),&thisClass::BOOST_PP_CAT(_getDeprec_,_DEPREC_OLDNAME(z)),&thisClass::BOOST_PP_CAT(_setDeprec_,_DEPREC_OLDNAME(z)),"|ydeprecated| alias for :yref:`" BOOST_PP_STRINGIZE(_DEPREC_NEWNAME(z)) "<" BOOST_PP_STRINGIZE(thisClass) "." BOOST_PP_STRINGIZE(_DEPREC_NEWNAME(z)) ">` (" _DEPREC_COMMENT(z) ")")
	#define _PYHASKEY_ATTR_DEPREC(x,thisClass,z) if(key==BOOST_PP_STRINGIZE(_DEPREC_OLDNAME(z))) return true;
	/* accessors functions ussing warning */
	#define _ACCESS_DEPREC(x,thisClass,z) /*getter*/ typeof(_DEPREC_NEWNAME(z)) BOOST_PP_CAT(_getDeprec_,_DEPREC_OLDNAME(z))(){_DEPREC_WARN(thisClass,z); return _DEPREC_NEWNAME(z); } /*setter*/ void BOOST_PP_CAT(_setDeprec_,_DEPREC_OLDNAME(z))(const typeof(_DEPREC_NEWNAME(z))& val){_DEPREC_WARN(thisClass,z); _DEPREC_NEWNAME(z)=val; }
#endif

// register class attributes, putting them to both python ['attr'] access functions and yade::serialization (and boost::serialization, if enabled)
#define REGISTER_ATTRIBUTES(baseClass,attrs) REGISTER_ATTRIBUTES_DEPREC(_SOME_CLASS,baseClass,attrs,)
#define REGISTER_ATTRIBUTES_DEPREC(thisClass,baseClass,attrs,deprec) protected: void registerAttributes(){ baseClass::registerAttributes(); BOOST_PP_SEQ_FOR_EACH(_REGISTER_ATTRIBUTES_REPEAT,~,attrs) } _REGISTER_BOOST_ATTRIBUTES(baseClass,attrs) \
	/* get attribute with the deprecated ['attr'] syntax; warn */ public: boost::python::object pyGetAttr(const std::string& key) const{ cerr<<"WARN: object['"<<key<<"'] syntax is deprecated, use object."<<key<<" instead."<<endl; BOOST_PP_SEQ_FOR_EACH(_PYGET_ATTR,~,attrs); return baseClass::pyGetAttr(key); } \
	/* set attribute with the deprecated ['attr'] syntax; warn */ void pySetAttr(const std::string& key, const boost::python::object& value) { cerr<<"WARN: object['"<<key<<"']=value syntax is deprecated, use object."<<key<<"=value instead."<<endl; pySetAttr_nowarn(key,value); } \
	/* set from upadeAttributes, no warning */ void pySetAttr_nowarn(const std::string& key, const boost::python::object& value){BOOST_PP_SEQ_FOR_EACH(_PYSET_ATTR,~,attrs); BOOST_PP_SEQ_FOR_EACH(_PYSET_ATTR_DEPREC,thisClass,deprec); baseClass::pySetAttr_nowarn(key,value); } \
	/* list all attributes (except deprecated ones); could return boost::python::set instead*/ boost::python::list pyKeys() const {  boost::python::list ret; BOOST_PP_SEQ_FOR_EACH(_PYKEYS_ATTR,~,attrs); ret.extend(baseClass::pyKeys()); return ret; } \
	/* whether the attribute is defined; includes deprecated attributes! */ bool pyHasKey(const std::string& key) const { BOOST_PP_SEQ_FOR_EACH(_PYHASKEY_ATTR,~,attrs); BOOST_PP_SEQ_FOR_EACH(_PYHASKEY_ATTR_DEPREC,thisClass,deprec); return baseClass::pyHasKey(key); } \
	/* return dictionary of all acttributes and values; deprecated attributes omitted */ boost::python::dict pyDict() const { boost::python::dict ret; BOOST_PP_SEQ_FOR_EACH(_PYDICT_ATTR,~,attrs); ret.update(baseClass::pyDict()); return ret; }

// print warning about deprecated attribute; thisClass is type name, not string
#define _DEPREC_WARN(thisClass,deprec)  std::cerr<<"WARN: "<<getClassName()<<"."<<BOOST_PP_STRINGIZE(_DEPREC_OLDNAME(deprec))<<" is deprecated, use "<<BOOST_PP_STRINGIZE(thisClass)<<"."<<BOOST_PP_STRINGIZE(_DEPREC_NEWNAME(deprec))<<" instead. "; if(_DEPREC_COMMENT(deprec)){ if(std::string(_DEPREC_COMMENT(deprec))[0]=='!'){ std::cerr<<endl; throw std::invalid_argument(BOOST_PP_STRINGIZE(thisClass) "." BOOST_PP_STRINGIZE(_DEPREC_OLDNAME(deprec)) " is deprecated; throwing exception requested. Reason: " _DEPREC_COMMENT(deprec));} else std::cerr<<"("<<_DEPREC_COMMENT(deprec)<<")"; } std::cerr<<std::endl;

// filter away 2nd element of a 2-tuple
#define _STRIPDOC2(x,y,z) (BOOST_PP_TUPLE_ELEM(2,0,z))
#define YADE_CLASS_BASE_DOC_ATTRS_DEPREC_PY(thisClass,baseClass,docString,attrs,deprec,extras) \
	REGISTER_ATTRIBUTES_DEPREC(thisClass,baseClass,BOOST_PP_SEQ_FOR_EACH(_STRIPDOC2,thisClass,attrs),deprec) \
	REGISTER_CLASS_AND_BASE(thisClass,baseClass) \
	/* accessors for deprecated attributes, with warnings */ BOOST_PP_SEQ_FOR_EACH(_ACCESS_DEPREC,thisClass,deprec) \
	/* python class registration */ virtual void pyRegisterClass(python::object _scope) const { if(!checkPyClassRegistersItself(#thisClass)) return; boost::python::scope thisScope(_scope); YADE_SET_DOCSTRING_OPTS; boost::python::class_<thisClass,shared_ptr<thisClass>,boost::python::bases<baseClass>,boost::noncopyable>(#thisClass,docString).def("__init__",python::raw_constructor(Serializable_ctor_kwAttrs<thisClass>)).def("clone",&Serializable_clone<thisClass>,python::arg("attrs")=python::dict()) BOOST_PP_SEQ_FOR_EACH(_PYATTR_DEF,thisClass,attrs) BOOST_PP_SEQ_FOR_EACH(_PYATTR_DEPREC_DEF,thisClass,deprec) extras ; }
	// use later: void must_use_both_YADE_CLASS_BASE_DOC_ATTRS_and_YADE_PLUGIN(); 
#define YADE_CLASS_BASE_DOC_ATTRS_PY(thisClass,baseClass,docString,attrs,extras) \
	YADE_CLASS_BASE_DOC_ATTRS_DEPREC_PY(thisClass,baseClass,docString,attrs,extras)

#define _STRIPDECL4(x,y,z) (( BOOST_PP_TUPLE_ELEM(4,1,z),BOOST_PP_TUPLE_ELEM(4,3,z) " :ydefault:`" BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(4,2,z)) "`" ))
// return type name; (for declaration)
#define _ATTR_DECL(x,y,z) BOOST_PP_TUPLE_ELEM(4,0,z) BOOST_PP_TUPLE_ELEM(4,1,z);
// return name(default), (for initializers list); TRICKY: last one must not have the comma
#define _ATTR_INI(x,maxIndex,i,z) BOOST_PP_TUPLE_ELEM(2,0,z)(BOOST_PP_TUPLE_ELEM(2,1,z)) BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(maxIndex,i))
// attrDecl is (type,name,defaultValue,docString)

#define _DECLINI4(x,y,z) (( BOOST_PP_TUPLE_ELEM(4,1,z),BOOST_PP_TUPLE_ELEM(4,2,z) ))

#define YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(thisClass,baseClass,docString,attrDecls,inits,ctor,extras) \
	YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(thisClass,baseClass,docString,attrDecls,,inits,ctor,extras)
// the most general here
#define YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(thisClass,baseClass,docString,attrDecls,deprec,inits,ctor,extras) \
	public: BOOST_PP_SEQ_FOR_EACH(_ATTR_DECL,~,attrDecls) /* attribute declarations */ \
	thisClass() BOOST_PP_IF(BOOST_PP_SEQ_SIZE(inits attrDecls),:,) BOOST_PP_SEQ_FOR_EACH_I(_ATTR_INI,BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(inits attrDecls)), inits BOOST_PP_SEQ_FOR_EACH(_DECLINI4,~,attrDecls)) { ctor ; } /* ctor, with initialization of defaults */ \
	YADE_CLASS_BASE_DOC_ATTRS_DEPREC_PY(thisClass,baseClass,docString,BOOST_PP_SEQ_FOR_EACH(_STRIPDECL4,~,attrDecls),deprec,extras)

#define _STATATTR_PY(x,thisClass,z) .def_readwrite(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(4,1,z)), &thisClass::BOOST_PP_TUPLE_ELEM(4,1,z), "|ystatic| :ydefault:`" BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(4,2,z)) "` " BOOST_PP_TUPLE_ELEM(4,3,z))
#define _STATATTR_DECL(x,y,z) static BOOST_PP_TUPLE_ELEM(4,0,z) BOOST_PP_TUPLE_ELEM(4,1,z);
#define _STRIP_TYPE_DEFAULT_DOC(x,y,z) (BOOST_PP_TUPLE_ELEM(4,1,z))

#define YADE_CLASS_BASE_DOC_STATICATTRS(thisClass,baseClass,docString,attrs)\
	public: BOOST_PP_SEQ_FOR_EACH(_STATATTR_DECL,~,attrs) /* attribute declarations */ \
	/* no ctor */ \
	REGISTER_CLASS_AND_BASE(thisClass,baseClass); \
	REGISTER_ATTRIBUTES(baseClass,BOOST_PP_SEQ_FOR_EACH(_STRIP_TYPE_DEFAULT_DOC,~,attrs)) \
	virtual void pyRegisterClass(python::object _scope) const { if(!checkPyClassRegistersItself(#thisClass)) return; boost::python::scope thisScope(_scope); YADE_SET_DOCSTRING_OPTS; \
		boost::python::object _klass=boost::python::class_<thisClass,shared_ptr<thisClass>,boost::python::bases<baseClass>,boost::noncopyable>(#thisClass,docString) \
		BOOST_PP_SEQ_FOR_EACH(_STATATTR_PY,thisClass,attrs);  \
	}

/* Macros for registering classes in python, serialization framework, class factory. These macros create default (parameterless) constructor, declare all attributes (delete those from the class elsewhere) and initialize them at construction time.

Each part of macro name is one argument you have to provide:

CLASS: class name (without quotes)
BASE: base class name (without quotes)
DOC: docstring for this class (quoted; uses the special text syntax, see below)
ATTRS: list of attributes ((type,attributeName,defaultValue,"documentation for the attribute"))
	type is c++ type
	attributeName: the identifier name; it will appear under the same name in python
	defaultValue: value that will be set by the default ctor; can be empty. Is automatically documented.
	documentation: arbitrary string, NOT EMPTY. Can contain the following strings (including the |pipes|):
		|yupdate| will expand to (updated automatically); use for attributes that are, well, updated automatically at every step for instance
		|ycompute| will expand to (computed automatically); use for attributes that are computed (at the beginning, for instance, and there is no point in setting them)
		[m²]: specify dimensions for all attributes where this is appropriate
CTOR: what should be run in the default ctor; for some classes, this will be createIndex();, mostly it will be nothing
PY: extra initializations for python, mostly nothing
INIT: sequence of ((attribute,value)) to be intiialized with the default ctor; useful for initialization of references or constants, which cannot be done in the ctor

Pick the simplest macro you need (equivalent to passing empty arguments):

YADE_CLASS_BASE_DOC
YADE_CLASS_BASE_DOC_ATTRS
YADE_CLASS_BASE_DOC_ATTRS_CTOR
YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY
YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY

documentation text syntax
-------------------------
Erite plain text. Paragraphs are separated by empty lines (\n\n in c strings). Use hyperlinks abundantly in both class and attribute documentation, with the special :yref:`target` syntax: :yref:`Body` will expand to hyperlink to the Body class documentation, :yref:`dynamic<Body::dynamic>` will the hyperlink to Body::dynamic, but text will show 'dynamic'.

(Note: for classes with static data members (Gl1_*), there is a special YADE_CLASS_BASE_DOC_STATICATTRS. Those mentioned above will not do.)

*/
// attrs is (type,name,init-value,docstring)
#define YADE_CLASS_BASE_DOC(klass,base,doc)                             YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(klass,base,doc,,,,)
#define YADE_CLASS_BASE_DOC_ATTRS(klass,base,doc,attrs)                 YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(klass,base,doc,attrs,,,)
#define YADE_CLASS_BASE_DOC_ATTRS_CTOR(klass,base,doc,attrs,ctor)       YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(klass,base,doc,attrs,,ctor,)
#define YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(klass,base,doc,attrs,ctor,py) YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(klass,base,doc,attrs,,ctor,py)

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
		cerr<<"WARN: Constructing class using the Serializable('"<<clss<<"') syntax is deprecated. Use directly "<<clss<<"() instead."<<endl;
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
		virtual void pySetAttr_nowarn(const std::string& key, const boost::python::object& value){ ::pySetAttr(key,value); };
		virtual boost::python::list pyKeys() const {return ::pyKeys(); };
		virtual bool pyHasKey(const std::string& key) const {return ::pyHasKey(key);}
		virtual boost::python::dict pyDict() const { return ::pyDict(); }
		// check whether the class registers itself or whether it calls virtual function of some base class;
		// that means that the class doesn't register itself properly
		virtual bool checkPyClassRegistersItself(const std::string& thisClassName) const;
		// perform class registration; overridden in all classes
		virtual void pyRegisterClass(boost::python::object _scope) const;
		
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


