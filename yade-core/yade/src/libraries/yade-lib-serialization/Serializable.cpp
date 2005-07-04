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

#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Serializable::Serializable() : Factorable()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Serializable::~Serializable()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Serializable::unregisterSerializableAttributes(bool deserializing)
{
	Archives::iterator ai    = archives.begin();
	Archives::iterator aiEnd = archives.end();
	for( ; ai!=aiEnd ; ++ai)
	{
		if (!((*ai)->isProcessed()))
		{
			string error = (*ai)->getName();
			error = SerializationExceptions::AttributeIsNotDefined + error;
			cerr << error << endl;
			//throw SerializableUnregisterError(error.c_str());
		}
	}
	archives.clear();

	postProcessAttributes(deserializing);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Serializable::registerSerializableAttributes(bool deserializing)
{
	preProcessAttributes(deserializing);

	this->registerAttributes();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Serializable::markAllAttributesProcessed()
{
	Archives::iterator ai    = archives.begin();
	Archives::iterator aiEnd = archives.end();
	for( ; ai!=aiEnd ; ++ai)
		(*ai)->markProcessed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Serializable::findAttribute(const string& name,shared_ptr<Archive>& arc)
{
	Archives::iterator ai = archives.begin();
	Archives::iterator aiEnd = archives.end();
	for( ; ai!=aiEnd ; ++ai )
		if((*ai)->getName() == name)
		{
			arc = *ai;
			return true;
		}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Serializable::containsOnlyFundamentals()
{
	Archives::iterator ai    = archives.begin();
	Archives::iterator aiEnd = archives.end();
	for( ; ai!=aiEnd ; ++ai)
	{
		if (!((*ai)->isFundamental()))
			return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
