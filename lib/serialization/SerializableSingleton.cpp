/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SerializableSingleton.hpp"
#include <iostream>

CREATE_LOGGER(SerializableSingleton);

SerializableSingleton::SerializableSingleton() 
{	
	cerr<<"Constructing SerializableSingleton  (if multiple times - check '-rdynamic' flag!)."<<endl;
}


bool SerializableSingleton::registerSerializableDescriptor( string name , VerifyFactorableFnPtr verify, SerializableTypes::Type type, bool f )
{

	bool tmp = map.insert( SerializableDescriptorMap::value_type( name , SerializableDescriptor(verify,type,f) )).second;

	#if 0
		if (tmp)
			cout << "registering serializable: " << name << " OK\n";
		else
			cout << "registering serializable: " << name << " FAILED\n";
	#endif

	return tmp;
}


bool SerializableSingleton::findClassInfo(const type_info& tp,SerializableTypes::Type& type, string& serializableClassName,bool& fundamental)
{
	SerializableDescriptorMap::iterator mi    = map.begin();
	SerializableDescriptorMap::iterator miEnd = map.end();

	for( ; mi!=miEnd ; mi++)
	{
		if (tp==(*mi).second.verify())
		{
			serializableClassName=(*mi).first;
			fundamental = (*mi).second.fundamental;
			type = (*mi).second.type;
			return true;
		}
	}
	return false;
}


