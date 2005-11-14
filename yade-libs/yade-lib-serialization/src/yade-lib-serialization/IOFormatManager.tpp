/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef IOMANAGER_TPP
#define IOMANAGER_TPP

#include "Archive.hpp"

template<typename Type>
void IOFormatManager::loadFromFile(const string& libName, const string& fileName,const string& name, Type& t)
{
	ifstream filei(fileName.c_str());

	shared_ptr<IOFormatManager> ioManager;
	ioManager = dynamic_pointer_cast<IOFormatManager>(ClassFactory::instance().createShared(libName));

	shared_ptr<Archive> ac = Archive::create(name,t);
	string str = ioManager->beginDeserialization(filei,*ac);
	ac->deserialize(filei, *ac, str);
	ioManager->finalizeDeserialization(filei,*ac);

	filei.close();
}


template<typename Type>
void IOFormatManager::saveToFile(const string& libName, const string& fileName,const string& name, Type& t)
{
	ofstream fileo(fileName.c_str());
	
	shared_ptr<IOFormatManager> ioManager;
	ioManager = static_pointer_cast<IOFormatManager>(ClassFactory::instance().createShared(libName));

	shared_ptr<Archive> ac = Archive::create(name,t);
	ioManager->beginSerialization(fileo, *ac);
	ac->serialize(fileo, *ac, 1);
	ioManager->finalizeSerialization(fileo, *ac);

	fileo.close();
}


template<typename Type>
void IOFormatManager::loadArchive(const string& libName, istream& stream, Type& t, const string& name)
{
	shared_ptr<IOFormatManager> ioManager;
	ioManager = dynamic_pointer_cast<IOFormatManager>(ClassFactory::instance().createShared(libName));

	shared_ptr<Archive> ac = Archive::create(name,t);
	string str = ioManager->beginDeserialization(stream,*ac);
	ac->deserialize(stream, *ac, str);
	ioManager->finalizeDeserialization(stream,*ac);
	cerr << "loadFromFile 3a\n";
}


template<typename Type>
void IOFormatManager::saveArchive(const string& libName, ostream& stream, Type& t, const string& name)
{
	shared_ptr<IOFormatManager> ioManager;
	ioManager = dynamic_pointer_cast<IOFormatManager>(ClassFactory::instance().createShared(libName));

	shared_ptr<Archive> ac = Archive::create(name,t);
	ioManager->beginSerialization(stream, *ac);
	ac->serialize(stream, *ac, 1);
	ioManager->finalizeSerialization(stream, *ac);
}


#endif // IOMANAGER_TPP
