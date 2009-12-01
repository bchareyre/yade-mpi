/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Dispatcher.hpp"

#include<algorithm>
#include<vector>



Dispatcher::Dispatcher()
{
	functorNames.clear();
	functorArguments.clear();
}


Dispatcher::~Dispatcher()
{
}


void Dispatcher::postProcessAttributes(bool deserializing)
{
	Engine::postProcessAttributes(deserializing);
}



vector<vector<string> >& Dispatcher::getFunctorNames()
{
	return functorNames;
}

void Dispatcher::clear()
{
	functorNames.clear();
	functorArguments.clear();
}



void Dispatcher::storeFunctorName(const string& baseClassName1, const string& libName, shared_ptr<Functor> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}


void Dispatcher::storeFunctorName( const string& baseClassName1, const string& baseClassName2, const string& libName, shared_ptr<Functor> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(baseClassName2);
	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}


void Dispatcher::storeFunctorName( const string& baseClassName1, const string& baseClassName2, const string& baseClassName3, const string& libName, shared_ptr<Functor> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(baseClassName2);
	v.push_back(baseClassName3);

	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}


void Dispatcher::storeFunctorArguments(shared_ptr<Functor> eu)
{
	if (!eu) 
		return;

	bool dupe = false;

	FunctorListIterator it    = functorArguments.begin();
	FunctorListIterator itEnd = functorArguments.end();
	for( ; it != itEnd ; ++it )
		if( (*it)->getClassName() == eu->getClassName() )
			dupe = true;
	
	if (!dupe) 
		functorArguments.push_back(eu);
}


shared_ptr<Functor> Dispatcher::findFunctorArguments(const string& libName)
{

	FunctorListIterator it    = functorArguments.begin();
	FunctorListIterator itEnd = functorArguments.end();
	for( ; it != itEnd ; ++it )
		if( (*it)->getClassName() == libName )
			return *it;
	
	return shared_ptr<Functor>();
}

