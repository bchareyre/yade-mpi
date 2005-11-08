/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Serializable.hpp"

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
			cerr << error << endl; // FIXME - FrontEnd::message()  <- can cerr, or display dialog box, or sth, else.
			//throw SerializableUnregisterError(error.c_str());
		}
	}
	archives.clear();

	postProcessAttributes(deserializing);
}

void Serializable::registerSerializableAttributes(bool deserializing)
{
	preProcessAttributes(deserializing);

	this->registerAttributes();
}

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

