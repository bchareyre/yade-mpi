/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   galizzi@stalactite                                                    *
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
****************************************************************************/

#include "IOManager.hpp"

IOManager::IOManager()
{
	Archive::addSerializablePointer(FactorableTypes::CUSTOM_CLASS, false, serializeCustomClass, deserializeCustomClass);
	Archive::addSerializablePointer(FactorableTypes::FUNDAMENTAL, true,serializeFundamental, deserializeFundamental);
	Archive::addSerializablePointer(FactorableTypes::POINTER, true, serializeSmartPointerOfFundamental, deserializeSmartPointerOfFundamental);
}

IOManager::~IOManager()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void IOManager::deserializeFundamental(istream& , Archive& ac,const string& str)
{
	any v = &str;
//	cout << "deserializing : |" << str << "|" << endl;
	ac.deserializeFundamental(ac,v);
	ac.markProcessed();
}

void IOManager::serializeFundamental(ostream& stream, Archive& ac,int )
{
	string str;
	any v = &str;
	ac.serializeFundamental(ac,v);
	stream << str;
	ac.markProcessed();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void IOManager::deserializeCustomClass(istream& stream, Archive& ac, const string& str)
{
	shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));

	shared_ptr<Archive> tmpAc = Archive::create(ac.getName(),*s);
	tmpAc->deserialize(stream, *tmpAc, str);
	s->deserialize(ac.getAddress());

	ac.markProcessed();
}

void IOManager::serializeCustomClass(ostream& stream, Archive& ac,int depth)
{
	shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));
	s->serialize(ac.getAddress());
	shared_ptr<Archive> tmpAc = Archive::create(ac.getName(),*s);
	tmpAc->serialize(stream,*tmpAc,depth);
	ac.markProcessed();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void IOManager::deserializeSmartPointerOfFundamental(istream& stream, Archive& ac, const string& str)
{
	if (str.size() != 0)
	{
		shared_ptr<Archive> tmpAc;
		ac.createNewPointedArchive(ac,tmpAc,"");
		tmpAc->deserialize(stream, *tmpAc,str);
	}
	ac.markProcessed();
}

void IOManager::serializeSmartPointerOfFundamental(ostream& stream, Archive& ac , int depth)
{
	shared_ptr<Archive> tmpAc;

	if(ac.createPointedArchive(ac,tmpAc))
		tmpAc->serialize(stream, *tmpAc,depth+1);

	ac.markProcessed();
}
