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

#include "Serializable.hpp"
#include "ArchiveTypes.hpp"
#include "FundamentalHandler.tpp"
#include "ContainerHandler.tpp"
#include "PointerHandler.tpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace ArchiveTypes;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Serializable;

template<typename Type>
struct FundamentalHandler;

template<typename Type>
struct ContainerHandler;

template<typename Type>
struct PointerHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! Primary template for findtype. Use to catch fundamental type, serializable class and custom class */
template<typename Type>
FactorableTypes::Type findType(Type& ,bool& fundamental, string& str)
{
	FactorableTypes::Type type;
	if (boost::is_fundamental< Type >::value || typeid(Type)==typeid(string) )
	{
		fundamental = true;
		return FactorableTypes::FUNDAMENTAL;
	}
	else if (typeid(Serializable)==typeid(Type))
	{
		fundamental = false;
		return FactorableTypes::SERIALIZABLE;
	}
	else if (ClassFactory::instance().findClassInfo(typeid(Type),type,str,fundamental))
	{
		return type;
	}
	else
		throw SerializableError(SerializationExceptions::SerializableUnknown2);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! Fatory used to create an Archive for a given type !*/
template<typename Type>
inline shared_ptr<Archive> Archive::create(const string& name,Type& attribute)
{

	shared_ptr<Archive> ac;

	ac = shared_ptr<Archive>(new Archive(name));

	string SerializableClassName;
	bool fundamental;


	ac->setRecordType(findType(attribute, fundamental, SerializableClassName));

	// At this point serializationMap and serializationMapOfFundamental should be initialized with the correct (de)-serialization function pointer
	if (fundamental)
	{
		ac->serialize	= serializationMapOfFundamental[ac->getRecordType()].first;
		ac->deserialize = serializationMapOfFundamental[ac->getRecordType()].second;
	}
	else
	{
		ac->serialize	= serializationMap[ac->getRecordType()].first;
		ac->deserialize = serializationMap[ac->getRecordType()].second;
	}

	ac->setFundamental(fundamental);

	// according to Type, the Archive is filled with the needed information
	switch (ac->getRecordType())
	{
		case FactorableTypes::SERIALIZABLE :
		{
			// reinterpret_cast is needed beacause this is a template function and when it is compiled it catches everything including int,float .... i.e. non class types
			Serializable * s = reinterpret_cast<Serializable*>(&attribute);
			ac->setAddress(s);
		}
		break;
		case FactorableTypes::FUNDAMENTAL :
			ac->serializeFundamental   = FundamentalHandler< Type >::accessor;
			ac->deserializeFundamental = FundamentalHandler< Type >::creator;
			ac->setAddress(&attribute);
		break;
		case FactorableTypes::CUSTOM_CLASS :
			ac->setAddress(&attribute);
			ac->setSerializableClassName(SerializableClassName);
		break;
		case FactorableTypes::CONTAINER :
			ac->createNextArchive = ContainerHandler< Type >::accessNext;
			ac->resize = ContainerHandler< Type >::resize;
			ac->setAddress(&attribute);
		break;
		case FactorableTypes::POINTER :
			ac->createPointedArchive = PointerHandler< Type >::accessor;
			ac->createNewPointedArchive = PointerHandler< Type >::creator;
			ac->setAddress(&attribute);
		break;
		default :
			throw SerializableError(SerializationExceptions::SerializableUnknown);
	}

	return ac;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

