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

#include "SerializableSingleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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


