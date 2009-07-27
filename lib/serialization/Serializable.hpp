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
#ifdef YADE_PYTHON
	#include<boost/python.hpp>
#endif
#include<boost/type_traits.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/preprocessor.hpp>
#include<list>
#include<map>
#include<string>
#include<vector>
#include<iostream>
#include<yade/lib-factory/Factorable.hpp>
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

#define _REGISTER_ATTRIBUTES_BODY(x,y,z) registerAttribute(BOOST_PP_STRINGIZE(z),z);
#define REGISTER_ATTRIBUTES_MANY(attrs) BOOST_PP_SEQ_FOR_EACH(_REGISTER_ATTRIBUTES_BODY,~,attrs)
//! create member function that register attributes; must be parenthesized, without commas: (attr1) (attr2) (attr3) ...
//#define REGISTER_ATTRIBUTES(attrs) protected: void registerAttributes(){ REGISTER_ATTRIBUTES_MANY(attrs) }
//! Same as REGISTER_ATTRIBUTES, but with first argument of base class, of which registerAttributes will be called first
#ifndef YADE_PYTHON
	#define REGISTER_ATTRIBUTES(baseClass,attrs) protected: void registerAttributes(){ baseClass::registerAttributes(); REGISTER_ATTRIBUTES_MANY(attrs); }
#else
	#include<boost/python.hpp>

	namespace{
		boost::python::object pyGetAttr(const std::string& key){ PyErr_SetString(PyExc_KeyError,(std::string("No such attribute: ")+key+".").c_str()); boost::python::throw_error_already_set(); /*never reached; avoids warning*/ throw; }
		void pySetAttr(const std::string& key, const boost::python::object& value){ PyErr_SetString(PyExc_KeyError,(std::string("No such attribute: ")+key+".").c_str()); boost::python::throw_error_already_set(); }
		boost::python::list pyKeys(){ return boost::python::list();}
		bool pyHasKey(const std::string& key) { return false; }
		boost::python::dict pyDict() { return boost::python::dict(); }
	};

	#define _PYGET_ATTR(x,y,z) if(key==BOOST_PP_STRINGIZE(z)) return boost::python::object(z);
	#define _PYSET_ATTR(x,y,z) if(key==BOOST_PP_STRINGIZE(z)) {z=boost::python::extract<typeof(z)>(value); return; }
	#define _PYKEYS_ATTR(x,y,z) ret.append(BOOST_PP_STRINGIZE(z));
	#define _PYHASKEY_ATTR(x,y,z) if(key==BOOST_PP_STRINGIZE(z)) return true;
	#define _PYDICT_ATTR(x,y,z) ret[BOOST_PP_STRINGIZE(z)]=boost::python::object(z);
	#define PYGET_MANY(attrs) BOOST_PP_SEQ_FOR_EACH(_PYGET_ATTR,~,attrs)
	#define PYSET_MANY(attrs) BOOST_PP_SEQ_FOR_EACH(_PYSET_ATTR,~,attrs)
	#define PYKEYS_MANY(attrs) BOOST_PP_SEQ_FOR_EACH(_PYKEYS_ATTR,~,attrs)
	#define PYHASKEY_MANY(attrs) BOOST_PP_SEQ_FOR_EACH(_PYHASKEY_ATTR,~,attrs)
	#define PYDICT_MANY(attrs) BOOST_PP_SEQ_FOR_EACH(_PYDICT_ATTR,~,attrs)

	#define REGISTER_ATTRIBUTES(baseClass,attrs) protected: void registerAttributes(){ baseClass::registerAttributes(); REGISTER_ATTRIBUTES_MANY(attrs); } \
		public: boost::python::object pyGetAttr(const std::string& key) const{ PYGET_MANY(attrs); return baseClass::pyGetAttr(key); } \
		void pySetAttr(const std::string& key, const boost::python::object& value){ PYSET_MANY(attrs); baseClass::pySetAttr(key,value); } \
		boost::python::list pyKeys() const { boost::python::list ret; PYKEYS_MANY(attrs); ret.extend(baseClass::pyKeys()); return ret; } \
		bool pyHasKey(const std::string& key) const { PYHASKEY_MANY(attrs); return baseClass::pyHasKey(key); } \
		boost::python::dict pyDict() const { boost::python::dict ret; PYDICT_MANY(attrs); ret.update(baseClass::pyDict()); return ret; }
#endif


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
		
		virtual void serialize(any& )	{ throw SerializableError(SerializationExceptions::SetFunctionNotDeclared); };
		virtual void deserialize(any& ) { throw SerializableError(SerializationExceptions::GetFunctionNotDeclared); };

		virtual void postProcessAttributes(bool /*deserializing*/) {};

	#ifdef YADE_PYTHON
		virtual boost::python::object pyGetAttr(const std::string& key) const { return ::pyGetAttr(key); }
		virtual void pySetAttr(const std::string& key, const boost::python::object& value){ ::pySetAttr(key,value); };
		virtual boost::python::list pyKeys() const {return ::pyKeys(); };
		virtual bool pyHasKey(const std::string& key) const {return ::pyHasKey(key);}
		virtual boost::python::dict pyDict() const { return ::pyDict(); }
		// override for root classes only (those which have distinct python wrappers
		virtual std::string pyRootClassName() const { return std::string("Serializable"); }
	#endif

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


