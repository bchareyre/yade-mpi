/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Archive.hpp"

map<SerializableTypes::Type,pair<SerializeFnPtr,DeserializeFnPtr> > Archive::serializationMap;
map<SerializableTypes::Type,pair<SerializeFnPtr,DeserializeFnPtr> > Archive::serializationMapOfFundamental;
Archive::SerializableDescriptorMap Archive::map;

void Archive::clearSerializablePointers() { serializationMap.clear(); serializationMapOfFundamental.clear(); }

Archive::Archive(const string& n)
{
	processed	= false;
	name		= n;
	fundamental	= false;
}

Archive::~Archive()
{
}

bool Archive::containsOnlyFundamentals()
{
	if (isFundamental())
		return true;
	else
	{
		if (recordType==SerializableTypes::SERIALIZABLE)
		{
			Serializable * s = any_cast<Serializable*>(getAddress());
			s->registerAttributes();
			bool result = s->containsOnlyFundamentals();
			s->archives.clear();
			return result;
		}
		else if (recordType==SerializableTypes::CUSTOM_CLASS)
		{
			shared_ptr<Serializable> s = YADE_PTR_CAST<Serializable>(ClassFactory::instance().createShared(getSerializableClassName()));
			s->registerAttributes();
			bool result = s->containsOnlyFundamentals();
			s->archives.clear();
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

bool Archive::addSerializablePointer(SerializableTypes::Type rt ,bool fundamental, SerializeFnPtr sp, DeserializeFnPtr dsp)
{
	if (fundamental)
		serializationMapOfFundamental[rt] = pair<SerializeFnPtr,DeserializeFnPtr>(sp,dsp);
	else
		serializationMap[rt] = pair<SerializeFnPtr,DeserializeFnPtr>(sp,dsp);
	return true;
}

