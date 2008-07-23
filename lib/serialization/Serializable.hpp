/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#include <boost/any.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/lexical_cast.hpp>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include<yade/lib-factory/Factorable.hpp>
#include "SerializationExceptions.hpp"
#include "Archive.hpp"

using namespace boost;
using namespace std;
using namespace ArchiveTypes;

#define DECLARE_POINTER_TO_MY_CUSTOM_CLASS(Type,attribute,any)		\
	Type * attribute=any_cast< Type * >(any);

#define REGISTER_ATTRIBUTE(attribute)                                   \
                registerAttribute( #attribute, attribute );

#define REGISTER_SERIALIZABLE(name,isFundamental) 						\
	REGISTER_FACTORABLE(name);								\
	REGISTER_SERIALIZABLE_DESCRIPTOR(name,name,SerializableTypes::SERIALIZABLE,isFundamental);

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

#endif // SERIALIZABLE_HPP

