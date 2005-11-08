/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "MetaDispatchingEngine.hpp"

MetaDispatchingEngine::MetaDispatchingEngine()
{
	functorNames.clear();
	functorArguments.clear();
}


MetaDispatchingEngine::~MetaDispatchingEngine()
{
}


void MetaDispatchingEngine::postProcessAttributes(bool deserializing)
{
	MetaEngine::postProcessAttributes(deserializing);
}


void MetaDispatchingEngine::registerAttributes()
{
	MetaEngine::registerAttributes();
	REGISTER_ATTRIBUTE(functorNames);
	REGISTER_ATTRIBUTE(functorArguments);
}


vector<vector<string> >& MetaDispatchingEngine::getFunctorNames()
{
	return functorNames;
}


void MetaDispatchingEngine::clear()
{
	functorNames.clear();
	functorArguments.clear();
}



void MetaDispatchingEngine::storeFunctorName(const string& baseClassName1, const string& libName, shared_ptr<EngineUnit> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}


void MetaDispatchingEngine::storeFunctorName( const string& baseClassName1, const string& baseClassName2, const string& libName, shared_ptr<EngineUnit> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(baseClassName2);
	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}


void MetaDispatchingEngine::storeFunctorName( const string& baseClassName1, const string& baseClassName2, const string& baseClassName3, const string& libName, shared_ptr<EngineUnit> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(baseClassName2);
	v.push_back(baseClassName3);

	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}


void MetaDispatchingEngine::storeFunctorArguments(shared_ptr<EngineUnit> eu)
{
	if (!eu) 
		return;

	bool dupe = false;

	EngineUnitListIterator it    = functorArguments.begin();
	EngineUnitListIterator itEnd = functorArguments.end();
	for( ; it != itEnd ; ++it )
		if( (*it)->getClassName() == eu->getClassName() )
			dupe = true;
	
	if (!dupe) 
		functorArguments.push_back(eu);
}


shared_ptr<EngineUnit> MetaDispatchingEngine::findFunctorArguments(const string& libName)
{

	EngineUnitListIterator it    = functorArguments.begin();
	EngineUnitListIterator itEnd = functorArguments.end();
	for( ; it != itEnd ; ++it )
		if( (*it)->getClassName() == libName )
			return *it;
	
	return shared_ptr<EngineUnit>();
}

