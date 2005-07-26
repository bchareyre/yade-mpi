/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SERIALIZABLE__
#define __SERIALIZABLE__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/lexical_cast.hpp>

#include <list>
#include <map>
#include <string>
#include <vector>

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-factory/Factorable.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SerializationExceptions.hpp"
#include "Archive.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;
using namespace std;
using namespace ArchiveTypes;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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




///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Serializable : public Factorable
{
	public    : Serializable();
	public    : virtual ~Serializable();

	public    : typedef list< shared_ptr<Archive> > Archives;
	private   : Archives archives;
	public    : Archives& getArchives() { return archives;};

	// those two are used only by custom class and MultiTypeHandler
	public    : virtual void serialize(any& ) { throw SerializableError(SerializationExceptions::SetFunctionNotDeclared);};
	public    : virtual void deserialize(any& ) { throw SerializableError(SerializationExceptions::GetFunctionNotDeclared);};

	public    : void unregisterSerializableAttributes(bool deserializing);
	public    : void registerSerializableAttributes(bool deserializing);
	public    : void markAllAttributesProcessed();
	public	  : bool findAttribute(const string& name,shared_ptr<Archive>& arc);

	protected : virtual void preProcessAttributes(bool /*deserializing*/) {};
	protected : virtual void postProcessAttributes(bool /*deserializing*/) {};

	protected : template <typename Type>
		    void registerAttribute(const string& name, Type& attribute)
		    {
			shared_ptr<Archive> ac = Archive::create(name,attribute);
			archives.push_back(ac);
		    }
	public    : bool containsOnlyFundamentals();
	public    : virtual void registerAttributes() {};


	REGISTER_CLASS_NAME(Serializable);
	REGISTER_BASE_CLASS_NAME(Factorable);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME : find a better place to include this file - must be after declaration of Serializable... or try to split this file into more pieces...
#include "MultiTypeHandler.tpp"

#endif // __SERIALIZABLE__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


