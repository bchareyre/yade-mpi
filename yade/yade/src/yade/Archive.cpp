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

map<RecordType,pair<SerializeFnPtr,DeserializeFnPtr> > Archive::serializationMap;
map<RecordType,pair<SerializeFnPtr,DeserializeFnPtr> > Archive::serializationMapOfFundamental;

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
		if (recordType==SERIALIZABLE)
		{
			Serializable * s = any_cast<Serializable*>(getAddress());
			s->registerAttributes();
			bool result = s->containsOnlyFundamentals();
			s->markAllAttributesProcessed();
			s->unregisterAttributes();
			return result;

		}
		else if (recordType==CUSTOM_CLASS)
		{
			shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(getSerializableClassName()));
			s->registerAttributes();
			bool result = s->containsOnlyFundamentals();
			s->markAllAttributesProcessed();
			s->unregisterAttributes();
			return result;
		}
		else if (recordType==SMART_POINTER)
		{
			shared_ptr<Archive> tmpAc;
			return !createPointedArchive(*this,tmpAc);
		}
		else		
			return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Archive::addSerializablePointer(RecordType rt ,bool fundamental, SerializeFnPtr sp, DeserializeFnPtr dsp)
{
	if (fundamental)
		serializationMapOfFundamental[rt] = pair<SerializeFnPtr,DeserializeFnPtr>(sp,dsp);
	else
		serializationMap[rt] = pair<SerializeFnPtr,DeserializeFnPtr>(sp,dsp);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


