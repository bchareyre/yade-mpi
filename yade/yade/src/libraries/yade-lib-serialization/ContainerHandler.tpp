/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#ifndef __CONTAINERHANDLER_H__
#define __CONTAINERHANDLER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SerializationExceptions.hpp"
#include "Archive.hpp"

#include <vector>
#include <list>
#include <set>
#include <deque>
#include <map>
#include <yade-lib-loki/AssocVector.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Type>
struct ContainerHandler
{
	static void resize(Archive& , unsigned long int )
	{
		throw HandlerError(SerializationExceptions::ContainerNotSupported);
	}

	static int accessNext(Archive& , shared_ptr<Archive>& , bool )
	{
		throw HandlerError(SerializationExceptions::ContainerNotSupported);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// vector											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType>
struct ContainerHandler<std::vector<ContainedType> >
{
	static void resize(Archive& ac, unsigned long int size)
	{
		vector<ContainedType> * tmpVec;
		tmpVec=any_cast<vector<ContainedType>*>(ac.getAddress());
		tmpVec->resize(size);
	}

	static int accessNext(Archive& ac, shared_ptr<Archive>& nextAc , bool first )
	{
		typedef typename vector<ContainedType>::iterator VectorIterator;
		static VectorIterator it;
		static VectorIterator itEnd;
		static int i;

		vector<ContainedType> * tmpVec;
		tmpVec=any_cast<vector<ContainedType>*>(ac.getAddress());

		if(first)
		{
			it=tmpVec->begin();
			itEnd=tmpVec->end();
			i=0;
		}

		if(it != itEnd )
		{
			string name = ac.getName()/*+"["+lexical_cast<string>(i)+"]"*/;
			nextAc = Archive::create(name,(*it));
			++it;
			++i;
			return tmpVec->size();
		}
		else
			return 0;

	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType>
SerializableTypes::Type findType( std::vector<ContainedType>& ,bool& fundamental, string& str)
{
// 	ContainedType tmpV;
//
// 	if (	boost::is_fundamental<ContainedType>::value ||
// 		findType(tmpV,str) == SMART_POINTER_OF_FUNDAMENTAL ||
// 		findType(tmpV,str) == CUSTOM_FUNDAMENTAL
// 		|| findType(tmpV,str) == STL_CONTAINER_OF_FUNDAMENTAL)
// 		return STL_CONTAINER_OF_FUNDAMENTAL;
// 	else
// 		return CONTAINER;

	ContainedType tmpV;
	//bool tmpFundamental;

	//FIXME : tmpFundamental ??
	//RecordType t = findType(tmpV,tmpFundamental,str);
	//cerr << str << " " << tmpFundamental << endl;
	//fundamental = ( boost::is_fundamental<ContainedType>::value || ((t==POINTER || t==CUSTOM_CLASS || t==CONTAINER) && tmpFundamental) );

	//RecordType t =
	findType(tmpV,fundamental,str);

	return SerializableTypes::CONTAINER;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// list											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType>
struct ContainerHandler<std::list<ContainedType> >
{
	static void resize(Archive& ac, unsigned long int size)
	{
		std::list<ContainedType> * tmpList;
		tmpList = any_cast<std::list<ContainedType>*> (ac.getAddress());
		tmpList->resize(size);
	}

	static int accessNext(Archive& ac, shared_ptr<Archive>& nextAc , bool first )
	{
		typedef typename std::list<ContainedType>::iterator ListIterator;
		static ListIterator it;
		static ListIterator itEnd;
		static int i;

		std::list<ContainedType> * tmpList;
		tmpList = any_cast<std::list<ContainedType>*> (ac.getAddress());

		if(first)
		{
			it=tmpList->begin();
			itEnd=tmpList->end();
			i=0;
		}

		if(it != itEnd )
		{// FIXME, why "[" is HERE ? it belongs to XMLManger! it is not generic. if somebody else wants to use this Serialization - he will not do that, because of this.
			string name = ac.getName()/*+"["+lexical_cast<string>(i)+"]"*/;
			nextAc = Archive::create(name,(*it));
			++it;
			++i;
			return tmpList->size();
		}
		else
			return 0;

	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType>
SerializableTypes::Type findType( std::list<ContainedType>& ,bool& fundamental, string& str)
{
	ContainedType tmp;
	findType(tmp,fundamental,str);

	return SerializableTypes::CONTAINER;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// set												///
///////////////////////////////////////////////////////////////////////////////////////////////////

// set is 1. a read-only structure
//        2. does not support resize()
//
// se we must do a hack here - introduce talking between both functions
// we can use the this hacked version, or the version without a hack, which is commented below
// good thing is that this hack is encapsulated inside this class, and nobody outside knows about it.

template<typename ContainedType>
struct ContainerHandler<std::set<ContainedType> >
{
	// first  - size of container,
	// second - bool that says if this size is useful
	typedef pair<unsigned long int, bool> hackTalking;

	static void resize(Archive& ac, unsigned long int size)
	{
		static unsigned long int setSize = 0;
		static bool useful = false;

		// FIXME : hack begin - to remove this hack, a deep redesign of Serialization is needed
		hackTalking ** tmpHack = 0;
		tmpHack = any_cast< hackTalking * > (& ac.getAddress());
		if( tmpHack != 0 )
		{
			if( size == 10) // 10 - asks size
			{
				(**tmpHack).first  = setSize;
				(**tmpHack).second = useful;
			}
			else if(size == 20) // 20 - says that size is no longer useful
				useful = false;
			return;
		}
		// hack END

		setSize = size;
		useful = true;

		std::set<ContainedType> * tmpSet;
		tmpSet = any_cast<std::set<ContainedType>*> (ac.getAddress());
		tmpSet->clear();
	}

	static int accessNext(Archive& ac, shared_ptr<Archive>& nextAc , bool first )
	{

		// hack BEGIN
		static Archive talking("talking"); // FIXME : this is a hack, because accessNext must talk with resize, to ask about size. Because it is impossible to have struct-global variables in static template.
		static hackTalking hackData(0,false);
		any a = &hackData;
		talking.setAddress(a);
		// 10 - question,
		// 20 - set it useless (end of deserialization)
		resize(talking,10);
		// hack END

		typedef typename std::set<ContainedType>::iterator SetIterator;
		static SetIterator it;
		static SetIterator itEnd;
		static ContainedType tmpData;

		static unsigned long int i;

		std::set<ContainedType> * tmpSet;
		tmpSet = any_cast<std::set<ContainedType>*> (ac.getAddress());

		bool deserialization = hackData.second;
		unsigned long int maxSize = ((deserialization) ? (hackData.first) : (tmpSet->size()));

		if(first)
			i=0;
		if( i != maxSize )
		{
			string name = ac.getName()/*+"["+lexical_cast<string>(i)+"]"*/;

			it		= tmpSet->begin();
			itEnd		= tmpSet->end();
			for( unsigned long int j=0 ; ( it != itEnd ) && ( j != i) ; ++it , ++j )
				;
			if( deserialization && (!first) )	tmpSet->insert(tmpData);
			if( !deserialization )			tmpData = *it; 	// otherwise we will have a compiler error.
			nextAc = Archive::create(name,(tmpData));
			++i;
			return maxSize; 				// return size of container
		}
		else
		{
			if(deserialization)
			{
				if( !first ) tmpSet->insert(tmpData);
				resize(talking,20);
			}
			return 0; // end of container reached
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType>
SerializableTypes::Type findType( std::set<ContainedType>& ,bool& fundamental, string& str)
{
	ContainedType tmp;
	findType(tmp,fundamental,str);

	return SerializableTypes::CONTAINER;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// version without a hack.

/*
template<typename ContainedType>
struct ContainerHandler<std::set<ContainedType> >
{
	static void resize(Archive& ac, unsigned int size)
	{
	}

	static int accessNext(Archive& ac, shared_ptr<Archive>& nextAc , bool first)
	{
		return 0;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType>
SerializableTypes::Type findType( std::set<ContainedType>& ,bool& fundamental, string& str)
{
	static_assert<false> ________SET_NOT_SUPPORTED________;
	(void) ________SET_NOT_SUPPORTED________;
// set not supported - because there is no set.resize(), you must declare a custom serializable class, that
// will convert set into something else for serialization purposes (eg.: vector or list )";
	exit(1);
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
/// deque											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType>
struct ContainerHandler<std::deque<ContainedType> >
{
	static void resize(Archive& ac, unsigned long int size)
	{
		std::deque<ContainedType> * tmpDeque;
		tmpDeque = any_cast<std::deque<ContainedType>*> (ac.getAddress());
		tmpDeque->resize(size);
	}

	static int accessNext(Archive& ac, shared_ptr<Archive>& nextAc , bool first )
	{
		typedef typename std::deque<ContainedType>::iterator DequeIterator;
		static DequeIterator it;
		static DequeIterator itEnd;
		static int i;

		std::deque<ContainedType> * tmpDeque;
		tmpDeque = any_cast<std::deque<ContainedType>*> (ac.getAddress());

		if(first)
		{
			it=tmpDeque->begin();
			itEnd=tmpDeque->end();
			i=0;
		}

		if(it != itEnd )
		{
			string name = ac.getName()/*+"["+lexical_cast<string>(i)+"]"*/;
			nextAc = Archive::create(name,(*it));
			++it;
			++i;
			return tmpDeque->size();
		}
		else
			return 0;

	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType>
SerializableTypes::Type findType( std::deque<ContainedType>& ,bool& fundamental, string& str)
{
	ContainedType tmp;
	findType(tmp,fundamental,str);

	return SerializableTypes::CONTAINER;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// map												///
///////////////////////////////////////////////////////////////////////////////////////////////////

// map    1. has read-only key
//        2. does not support resize()
//
// se we must do a hack here - introduce talking between both functions
// good thing is that this hack is encapsulated inside this class, and nobody outside knows about it.

template<typename ContainedType1, typename ContainedType2>
struct ContainerHandler<std::map< ContainedType1 , ContainedType2> >
{
	// first  - size of container,
	// second - bool that says if this size is useful
	typedef pair<unsigned long int, bool> hackTalking;

	static void resize(Archive& ac, unsigned long int size)
	{
		static unsigned long int mapSize = 0;
		static bool useful = false;

		// hack begin - to remove this hack, a deep redesign of Serialization is needed
		hackTalking ** tmpHack = 0;
		tmpHack = any_cast< hackTalking * > (& ac.getAddress());
		if( tmpHack != 0 )
		{
			if( size == 10) // 10 - asks size
			{
				(**tmpHack).first  = mapSize;
				(**tmpHack).second = useful;
			}
			else if(size == 20) // 20 - says that size is no longer useful
				useful = false;
			return;
		}
		// hack END

		mapSize = size;
		useful = true;

		std::map< ContainedType1, ContainedType2 > * tmpMap;
		tmpMap = any_cast<std::map< ContainedType1 , ContainedType2 >*> (ac.getAddress());
		tmpMap->clear();
	}

	static int accessNext(Archive& ac, shared_ptr<Archive>& nextAc , bool first )
	{

		// hack BEGIN
		static Archive talking("talking"); // FIXME : this is a hack, because accessNext must talk with resize, to ask about size. Because it is impossible to have struct-global variables in static template.
		static hackTalking hackData(0,false);
		any a = &hackData;
		talking.setAddress(a);
		// 10 - question,
		// 20 - set it useless (end of deserialization)
		resize(talking,10);
		// hack END

		typedef typename std::map< ContainedType1 , ContainedType2 >::iterator MapIterator;
		static MapIterator it;
		static MapIterator itEnd;
		static std::pair< ContainedType1 , ContainedType2 > tmpData;

		static unsigned long int i;

		std::map< ContainedType1 , ContainedType2 > * tmpMap;
		tmpMap = any_cast<std::map< ContainedType1 , ContainedType2 >*> (ac.getAddress());

		bool deserialization = hackData.second;
		unsigned long int maxSize = ((deserialization) ? (hackData.first) : (tmpMap->size()));

		if(first)
			i=0;
		if( i != maxSize )
		{
			string name = ac.getName()/*+"["+lexical_cast<string>(i)+"]"*/;

			it		= tmpMap->begin();
			itEnd		= tmpMap->end();
			for( unsigned long int j=0 ; ( it != itEnd ) && ( j != i) ; ++it , ++j )
				;
			if( deserialization && (!first) ) 	tmpMap->insert(tmpData);
			if( !deserialization )			tmpData = *it; 	// otherwise we will have a compiler error.
			nextAc = Archive::create(name,(tmpData));
			++i;
			return maxSize; 				// return size of container
		}
		else
		{
			if(deserialization)
			{
				if( !first ) tmpMap->insert(tmpData);
				resize(talking,20);
			}
			return 0; // end of container reached
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType1 , typename ContainedType2 >
SerializableTypes::Type findType( std::map< ContainedType1, ContainedType2 >& ,bool& fundamental, string& str)
{
	std::pair< ContainedType1 , ContainedType2 > tmp;
	findType(tmp,fundamental,str);

	return SerializableTypes::CONTAINER;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// AssocVector											///
///////////////////////////////////////////////////////////////////////////////////////////////////

// AssocVector    1. has read-writable key, but I'll do the same as in std::map and std::set
//                2. does not support resize()
//
// se we must do a hack here - introduce talking between both functions
// good thing is that this hack is encapsulated inside this class, and nobody outside knows about it.

template<typename ContainedType1, typename ContainedType2>
struct ContainerHandler<Loki::AssocVector< ContainedType1 , ContainedType2> >
{
	// first  - size of container,
	// second - bool that says if this size is useful
	typedef pair<unsigned long int, bool> hackTalking;

	static void resize(Archive& ac, unsigned long int size)
	{
		static unsigned long int assocVectorSize = 0;
		static bool useful = false;

		// hack begin - to remove this hack, a deep redesign of Serialization is needed
		hackTalking ** tmpHack = 0;
		tmpHack = any_cast< hackTalking * > (& ac.getAddress());
		if( tmpHack != 0 )
		{
			if( size == 10) // 10 - asks size
			{
				(**tmpHack).first  = assocVectorSize;
				(**tmpHack).second = useful;
			}
			else if(size == 20) // 20 - says that size is no longer useful
				useful = false;
			return;
		}
		// hack END

		assocVectorSize = size;
		useful = true;

		Loki::AssocVector< ContainedType1, ContainedType2 > * tmpAssocVector;
		tmpAssocVector = any_cast<Loki::AssocVector< ContainedType1 , ContainedType2 >*> (ac.getAddress());
		tmpAssocVector->clear();
	}

	static int accessNext(Archive& ac, shared_ptr<Archive>& nextAc , bool first )
	{

		// hack BEGIN
		static Archive talking("talking"); // FIXME : this is a hack, because accessNext must talk with resize, to ask about size. Because it is impossible to have struct-global variables in static template.
		static hackTalking hackData(0,false);
		any a = &hackData;
		talking.setAddress(a);
		// 10 - question,
		// 20 - set it useless (end of deserialization)
		resize(talking,10);
		// hack END

		typedef typename Loki::AssocVector< ContainedType1 , ContainedType2 >::iterator AssocVectorIterator;
		static AssocVectorIterator it;
		static AssocVectorIterator itEnd;
		static std::pair< ContainedType1 , ContainedType2 > tmpData;

		static unsigned long int i;

		Loki::AssocVector< ContainedType1 , ContainedType2 > * tmpAssocVector;
		tmpAssocVector = any_cast<Loki::AssocVector< ContainedType1 , ContainedType2 >*> (ac.getAddress());

		bool deserialization = hackData.second;
		unsigned long int maxSize = ((deserialization) ? (hackData.first) : (tmpAssocVector->size()));

		if(first)
			i=0;
		if( i != maxSize )
		{
			string name = ac.getName()/*+"["+lexical_cast<string>(i)+"]"*/;

			it		= tmpAssocVector->begin();
			itEnd		= tmpAssocVector->end();
			for( unsigned long int j=0 ; ( it != itEnd ) && ( j != i) ; ++it , ++j )
				;
			if( deserialization && (!first) ) 	tmpAssocVector->insert(tmpData);
			if( !deserialization )			tmpData = *it; 	// otherwise we will have a compiler error.
			nextAc = Archive::create(name,(tmpData));
			++i;
			return maxSize; 				// return size of container
		}
		else
		{
			if(deserialization)
			{
				if( !first ) tmpAssocVector->insert(tmpData);
				resize(talking,20);
			}
			return 0; // end of container reached
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType1 , typename ContainedType2 >
SerializableTypes::Type findType( Loki::AssocVector< ContainedType1, ContainedType2 >& ,bool& fundamental, string& str)
{
	std::pair< ContainedType1 , ContainedType2 > tmp;
	findType(tmp,fundamental,str);

	return SerializableTypes::CONTAINER;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __CONTAINERHANDLER_H__
