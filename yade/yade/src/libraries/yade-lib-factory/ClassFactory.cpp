/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   galizzi@stalactite                                                    *
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#include "ClassFactory.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Factorable;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ClassFactory::addBaseDirectory(const string& dir)
{
	dlm.addBaseDirectory(dir);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ClassFactory::registerFactorable( std::string name 			   , CreateFactorableFnPtr create,
					 CreateSharedFactorableFnPtr createShared, CreatePureCustomFnPtr createPureCustom)
{

	bool tmp = map.insert( FactorableCreatorsMap::value_type( name , FactorableCreators(create,createShared, createPureCustom) )).second;

	#ifdef DEBUG
		if (tmp)
			std::cout << "registering factorable: " << name << " OK\n";
		else
			std::cout << "registering factorable: " << name << " FAILED\n";
	#endif

	return tmp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

boost::shared_ptr<Factorable> ClassFactory::createShared( std::string name )
{

	FactorableCreatorsMap::const_iterator i = map.find( name );
	if( i == map.end() )
	{
		dlm.load(name);
		if (dlm.isLoaded(name))
		{
			if( map.find( name ) == map.end() )
			{
				std::string error = FactoryExceptions::ClassNotRegistered + name;
				throw FactoryClassNotRegistered(error.c_str());
			}
			return createShared(name);
		}
		std::string error = FactoryExceptions::CantCreateClass + name;
		throw FactoryCantCreate(error.c_str());
	}
	return ( i -> second.createShared ) ();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Factorable* ClassFactory::createPure( std::string name )
{
	FactorableCreatorsMap::const_iterator i = map.find( name );
	if( i == map.end() )
	{
		//cerr << "------------ going to load something" << endl;
		dlm.load(name);
		if (dlm.isLoaded(name))
		{
			if( map.find( name ) == map.end() )
			{
				std::string error = FactoryExceptions::ClassNotRegistered + name;
				throw FactoryClassNotRegistered(error.c_str());
			}
			return createPure(name);
		}
		std::string error = FactoryExceptions::CantCreateClass + name;
		throw FactoryCantCreate(error.c_str());
	}
	return ( i -> second.create ) ();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void * ClassFactory::createPureCustom( std::string name )
{
	FactorableCreatorsMap::const_iterator i = map.find( name );
	if( i == map.end() )
	{
		std::string error = FactoryExceptions::CantCreateClass + name;
		throw FactoryCantCreate(error.c_str());
	}
	return ( i -> second.createPureCustom ) ();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
