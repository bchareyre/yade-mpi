/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
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

#include "MetaDispatchingEngine1D.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MetaDispatchingEngine::MetaDispatchingEngine()
{
	functorNames.clear();
	functorArguments.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MetaDispatchingEngine::~MetaDispatchingEngine()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void MetaDispatchingEngine::postProcessAttributes(bool deserializing)
{
	MetaEngine::postProcessAttributes(deserializing);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void MetaDispatchingEngine::registerAttributes()
{
	MetaEngine::registerAttributes();
	REGISTER_ATTRIBUTE(functorNames);
	REGISTER_ATTRIBUTE(functorArguments);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

vector<vector<string> >& MetaDispatchingEngine::getFunctorNames()
{
	return functorNames;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void MetaDispatchingEngine::clear()
{
	functorNames.clear();
	functorArguments.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void MetaDispatchingEngine::storeFunctorName(const string& baseClassName1, const string& libName, shared_ptr<EngineUnit> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void MetaDispatchingEngine::storeFunctorName( const string& baseClassName1, const string& baseClassName2, const string& libName, shared_ptr<EngineUnit> eu)
{
	vector<string> v;

	v.push_back(baseClassName1);
	v.push_back(baseClassName2);
	v.push_back(libName);

	functorNames.push_back(v);

	storeFunctorArguments(eu);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<EngineUnit> MetaDispatchingEngine::findFunctorArguments(const string& libName)
{

	EngineUnitListIterator it    = functorArguments.begin();
	EngineUnitListIterator itEnd = functorArguments.end();
	for( ; it != itEnd ; ++it )
		if( (*it)->getClassName() == libName )
			return *it;
	
	return shared_ptr<EngineUnit>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
