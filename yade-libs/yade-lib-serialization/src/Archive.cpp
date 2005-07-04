/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   galizzi@stalactite                                                    *
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

#include "Archive.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

map<SerializableTypes::Type,pair<SerializeFnPtr,DeserializeFnPtr> > Archive::serializationMap;
map<SerializableTypes::Type,pair<SerializeFnPtr,DeserializeFnPtr> > Archive::serializationMapOfFundamental;
Archive::SerializableDescriptorMap Archive::map;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Archive::Archive(const string& n)
{
	processed	= false;
	name		= n;
	fundamental	= false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Archive::~Archive()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Archive::containsOnlyFundamentals()
{
	if (isFundamental())
		return true;
	else
	{
		if (recordType==SerializableTypes::SERIALIZABLE)
		{
			Serializable * s = any_cast<Serializable*>(getAddress());
			s->registerSerializableAttributes(false);
			bool result = s->containsOnlyFundamentals();
			s->markAllAttributesProcessed();
			s->unregisterSerializableAttributes(false); // in fact true/false here doesn't apply - we are not serializing/deserializing - just checking
			return result;
		}
		else if (recordType==SerializableTypes::CUSTOM_CLASS)
		{
			shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(getSerializableClassName()));
			s->registerSerializableAttributes(false);
			bool result = s->containsOnlyFundamentals();
			s->markAllAttributesProcessed();
			s->unregisterSerializableAttributes(false); // in fact true/false here doesn't apply - we are not serializing/deserializing - just checking
			return result;
		}
		else if (recordType==SerializableTypes::POINTER)
		{
			shared_ptr<Archive> tmpAc;
			if(createPointedArchive(*this,tmpAc))
				return tmpAc->containsOnlyFundamentals();
			else
				return true;
		}
		else if (recordType==SerializableTypes::CONTAINER)
		{
			shared_ptr<Archive> tmpAc;
			int size = createNextArchive(*this,tmpAc,true);
			return size == 0;
		}
		else
			return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Archive::addSerializablePointer(SerializableTypes::Type rt ,bool fundamental, SerializeFnPtr sp, DeserializeFnPtr dsp)
{
	if (fundamental)
		serializationMapOfFundamental[rt] = pair<SerializeFnPtr,DeserializeFnPtr>(sp,dsp);
	else
		serializationMap[rt] = pair<SerializeFnPtr,DeserializeFnPtr>(sp,dsp);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// bool Archive::registerSerializableDescriptor( string name , VerifyFactorableFnPtr verify, SerializableTypes::Type type, bool f )
// {
// 	std::cout << "registering serializable : " << name << endl;
//
// 	bool tmp = map.insert( SerializableDescriptorMap::value_type( name , SerializableDescriptor(verify,type,f) )).second;
//
// 	//#ifdef DEBUG
// 		if (tmp)
// 			std::cout << "registering serializable : " << name << " OK\n";
// 		else
// 			std::cout << "registering serializable: " << name << " FAILED\n";
// 	//#endif
//
// 	return tmp;
// }

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// bool Archive::findClassInfo(const type_info& tp,SerializableTypes::Type& type, string& serializableClassName,bool& fundamental)
// {
// 	SerializableDescriptorMap::iterator mi    = map.begin();
// 	SerializableDescriptorMap::iterator miEnd = map.end();
//
// 	for( ; mi!=miEnd ; mi++)
// 	{
// 		if (tp==(*mi).second.verify())
// 		{
// 			serializableClassName=(*mi).first;
// 			fundamental = (*mi).second.fundamental;
// 			type = (*mi).second.type;
// 			return true;
// 		}
// 	}
// 	return false;
// }
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
