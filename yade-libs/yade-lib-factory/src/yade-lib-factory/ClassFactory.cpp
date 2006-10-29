/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ClassFactory.hpp"

class Factorable;

void ClassFactory::addBaseDirectory(const string& dir)
{
	dlm.addBaseDirectory(dir);
}

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

boost::shared_ptr<Factorable> ClassFactory::createShared( std::string name )
{

	FactorableCreatorsMap::const_iterator i = map.find( name );
	if( i == map.end() )
	{
		dlm.loadFromDirectoryList(name);
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

Factorable* ClassFactory::createPure( std::string name )
{
	FactorableCreatorsMap::const_iterator i = map.find( name );
	if( i == map.end() )
	{
		//cerr << "------------ going to load something" << endl;
		dlm.loadFromDirectoryList(name);
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

bool ClassFactory::load(const string& name)
{
        return dlm.loadFromDirectoryList(name);
}

string ClassFactory::lastError()
{
        return dlm.lastError();
}

string ClassFactory::libNameToSystemName(const string& name)
{
        return dlm.libNameToSystemName(name);
}

string ClassFactory::systemNameToLibName(const string& name)
{
	return dlm.systemNameToLibName(name);
}

