#include "SerializableSingleton.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SerializableSingleton::SerializableSingleton() 
{	
	cerr << "Constructing SerializableSingleton  (if multiple times - check '-rdynamic' flag!)" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool SerializableSingleton::registerSerializableDescriptor( string name , VerifyFactorableFnPtr verify, SerializableTypes::Type type, bool f )
{

	
	bool tmp = map.insert( SerializableDescriptorMap::value_type( name , SerializableDescriptor(verify,type,f) )).second;

	#ifdef DEBUG
		if (tmp)
			cout << "registering serializable : " << name << " OK\n";
		else
			cout << "registering serializable: " << name << " FAILED\n";
	#endif

	return tmp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


