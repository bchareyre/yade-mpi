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

#ifndef __IOMANAGER__
#define __IOMANAGER__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class IOManager : public Factorable
{
	public   : IOManager();
	public   : virtual ~IOManager();

	public  : virtual void beginSerialization(ostream& ,  Archive& )     {};
	public  : virtual void finalizeSerialization(ostream& ,  Archive& )  {};
	public  : virtual string beginDeserialization(istream& ,  Archive& ) { return "";};
	public  : virtual void finalizeDeserialization(istream& , Archive& ) {};

	public : static void serializeFundamental(ostream& stream, Archive& ac, int depth);
	public : static void deserializeFundamental(istream& stream, Archive& ac, const string& str);

	public : static void serializeCustomClass(ostream& stream, Archive& ac , int depth);
	public : static void deserializeCustomClass(istream& stream, Archive& ac, const string& str="");

	public : static void serializeSmartPointerOfFundamental(ostream& stream, Archive& ac , int depth);
	public : static void deserializeSmartPointerOfFundamental(istream& stream, Archive& ac, const string& str);

	public : template<typename Type>
		 static void loadFromFile(const string& libName, const string& fileName,const string& name, Type& t)
		 {
			shared_ptr<IOManager> ioManager;
			ioManager = dynamic_pointer_cast<IOManager>(ClassFactory::instance().createShared(libName));
			ifstream filei(fileName.c_str());
			ioManager->loadArchive(filei,t,name);
			filei.close();
		 }

	public : template<typename Type>
		 static void saveToFile(const string& libName, const string& fileName,const string& name, Type& t)
		 {
			shared_ptr<IOManager> ioManager;
			ioManager = dynamic_pointer_cast<IOManager>(ClassFactory::instance().createShared(libName));
			ofstream fileo(fileName.c_str());
			ioManager->saveArchive(fileo,t,name);
			fileo.close();
		 }


	public : template<typename Type>
		 void loadArchive(istream& stream, Type& t, const string& name)
		 {
		 	shared_ptr<Archive> ac = Archive::create(name,t);

			string str = beginDeserialization(stream,*ac);

			ac->deserialize(stream, *ac, str);

			finalizeDeserialization(stream,*ac);
		 }

	public : template<typename Type>
		 void saveArchive(ostream& stream, Type& t, const string& name)
		 {
			shared_ptr<Archive> ac = Archive::create(name,t);

			beginSerialization(stream, *ac);

			ac->serialize(stream, *ac, 1);

			finalizeSerialization(stream, *ac);
		 }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_FACTORABLE(IOManager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __IOMANAGER__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
