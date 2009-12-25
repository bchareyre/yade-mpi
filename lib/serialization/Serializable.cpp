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

void Serializable::pyUpdateAttrs(const python::dict& d){
	python::list l=d.items(); size_t ll=python::len(l);
	for(size_t i=0; i<ll; i++){
		python::tuple t=python::extract<python::tuple>(l[i]);
		string key=python::extract<string>(t[0]);
		pySetAttr(key,t[1]);
	}
}

python::list Serializable::pyUpdateExistingAttrs(const python::dict& d){
	python::list ret; python::list l=d.items(); size_t ll=python::len(l);
	for(size_t i=0; i<ll; i++){
		python::tuple t=python::extract<python::tuple>(l[i]);
		string key=python::extract<string>(t[0]);
		if(pyHasKey(key)) pySetAttr(key,t[1]); else ret.append(t[0]);
	}
	return ret; 
}




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

