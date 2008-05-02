/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "MetaEngine.hpp"

#include<algorithm>
#include<vector>

list<string> MetaEngine::getNeededBex(){
	list<string> ret;
	BOOST_FOREACH(shared_ptr<EngineUnit> e, functorArguments){
		list<string> eBex=e->getNeededBex();
		ret.insert(ret.end(),eBex.begin(),eBex.end());
	}
	ret.sort();
	ret.unique();
	return ret;
}


MetaEngine::MetaEngine()
{
	functorNames.clear();
	functorArguments.clear();
}


MetaEngine::~MetaEngine()
{
}


void MetaEngine::postProcessAttributes(bool deserializing)
{
	Engine::postProcessAttributes(deserializing);
}


void MetaEngine::registerAttributes()
{
	Engine::registerAttributes();
	REGISTER_ATTRIBUTE(functorNames);
	REGISTER_ATTRIBUTE(functorArguments);
}


vector<vector<string> >& MetaEngine::getFunctorNames()
{
	return functorNames;
}

void MetaEngine::clear()
{
	functorNames.clear();
	functorArguments.clear();
}



void MetaEngine::storeFunctorName(const string& baseClassName1, const string& libName, shared_ptr<EngineUnit> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}


void MetaEngine::storeFunctorName( const string& baseClassName1, const string& baseClassName2, const string& libName, shared_ptr<EngineUnit> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(baseClassName2);
	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}


void MetaEngine::storeFunctorName( const string& baseClassName1, const string& baseClassName2, const string& baseClassName3, const string& libName, shared_ptr<EngineUnit> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(baseClassName2);
	v.push_back(baseClassName3);

	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}


void MetaEngine::storeFunctorArguments(shared_ptr<EngineUnit> eu)
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


shared_ptr<EngineUnit> MetaEngine::findFunctorArguments(const string& libName)
{

	EngineUnitListIterator it    = functorArguments.begin();
	EngineUnitListIterator itEnd = functorArguments.end();
	for( ; it != itEnd ; ++it )
		if( (*it)->getClassName() == libName )
			return *it;
	
	return shared_ptr<EngineUnit>();
}

