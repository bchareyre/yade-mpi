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


#include "ArchiveTypes.hpp"
#include "FundamentalHandler.tpp"
#include "ContainerHandler.tpp"
#include "PointerHandler.tpp"
#include "KnownFundamentalsHandler.tpp"
//#include "MultiTypeHandler.tpp" // this is in Serializable.hpp, should be here, but then it doesn't compile

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-wm3-math/Vector2.hpp>
#include <yade-lib-wm3-math/Vector3.hpp>
#include <yade-lib-wm3-math/Vector4.hpp>
#include <yade-lib-wm3-math/Matrix2.hpp>
#include <yade-lib-wm3-math/Matrix3.hpp>
#include <yade-lib-wm3-math/Matrix4.hpp>
#include <yade-lib-wm3-math/Quaternion.hpp>
#include <yade-lib-wm3-math/Se3.hpp>

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

template<typename Type>
bool isFundamental(Type& )
{
	return	(	boost::is_fundamental< Type >::value 	|| //int, float, char, double ...
			typeid(Type)==typeid(string) 		|| // std::string
			typeid(Type)==typeid(Vector2f)		||
			typeid(Type)==typeid(Vector2d)		||
			typeid(Type)==typeid(Vector3f)		||
			typeid(Type)==typeid(Vector3d)		||
			typeid(Type)==typeid(Vector4f)		||
			typeid(Type)==typeid(Vector4d)		||
			typeid(Type)==typeid(Matrix2f)		||
			typeid(Type)==typeid(Matrix2d)		||
			typeid(Type)==typeid(Matrix3f)		||
			typeid(Type)==typeid(Matrix3d)		||
			typeid(Type)==typeid(Matrix4f)		||
			typeid(Type)==typeid(Matrix4d)		||
			typeid(Type)==typeid(Quaternionf)	||
			typeid(Type)==typeid(Quaterniond)	||
			typeid(Type)==typeid(Se3f)		||
			typeid(Type)==typeid(Se3d)
		);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


/*! Primary template for findtype. Use to catch fundamental type, serializable class and custom class */
template<typename Type>
SerializableTypes::Type findType(Type& instance,bool& fundamental, string& str)
{
	SerializableTypes::Type type;

	if (isFundamental(instance))
	{
		fundamental = true;
		return SerializableTypes::FUNDAMENTAL;
	}
	else if (typeid(Serializable)==typeid(Type))
	{
		fundamental = false;
		return SerializableTypes::SERIALIZABLE;
	}
	else if (SerializableSingleton::instance().findClassInfo(typeid(Type),type,str,fundamental))
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
	shared_ptr<Archive> ac(new Archive(name));

	string SerializableClassName;
	bool fundamental;

	ac->setRecordType(findType(attribute, fundamental, SerializableClassName));

	// At this point serializationMap and serializationMapOfFundamental should be initialized with the correct (de)-serialization function pointer
	if (fundamental)
	{
		ac->serialize	= serializationMapOfFundamental[ac->getRecordType()].first;
		ac->deserialize = serializationMapOfFundamental[ac->getRecordType()].second;
	}
	else // not fundamental
	{
		ac->serialize	= serializationMap[ac->getRecordType()].first;
		ac->deserialize = serializationMap[ac->getRecordType()].second;
	}

	ac->setFundamental(fundamental);

	// according to Type, the Archive is filled with the needed information
	switch (ac->getRecordType())
	{
		case SerializableTypes::SERIALIZABLE :
		{
			// reinterpret_cast is needed beacause this is a template function and when it is compiled it catches everything including int,float .... i.e. non class types
			Serializable * s = reinterpret_cast<Serializable*>(&attribute);
			ac->setAddress(s);
		}
		break;
		case SerializableTypes::FUNDAMENTAL :
			ac->serializeFundamental   = FundamentalHandler< Type >::accessor;
			ac->deserializeFundamental = FundamentalHandler< Type >::creator;
			ac->setAddress(&attribute);
		break;
		case SerializableTypes::CUSTOM_CLASS :
			ac->setAddress(&attribute);
			ac->setSerializableClassName(SerializableClassName);
		break;
		case SerializableTypes::CONTAINER :
			ac->createNextArchive = ContainerHandler< Type >::accessNext;
			ac->resize = ContainerHandler< Type >::resize;
			ac->setAddress(&attribute);
		break;
		case SerializableTypes::POINTER :
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

