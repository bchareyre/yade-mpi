/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
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

#ifndef __IOMANAGER_TPP_
#define __IOMANAGER_TPP_

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Archive.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Type>
void IOFormatManager::loadArchive(const string& libName, istream& stream, Type& t, const string& name)
{
	shared_ptr<IOFormatManager> ioManager;
	ioManager = dynamic_pointer_cast<IOFormatManager>(ClassFactory::instance().createShared(libName));

	shared_ptr<Archive> ac = Archive::create(name,t);
	string str = ioManager->beginDeserialization(stream,*ac);
	ac->deserialize(stream, *ac, str);
	ioManager->finalizeDeserialization(stream,*ac);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __IOMANAGER_TPP_
