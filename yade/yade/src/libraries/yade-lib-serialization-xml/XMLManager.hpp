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

#ifndef __XMLMANAGER__
#define __XMLMANAGER__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-serialization/IOManager.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "XMLSaxParser.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class XMLManager : public IOManager
{
	private   : static XmlSaxParser saxParser;

	public    : XMLManager();
	public    : virtual ~XMLManager();

	public    : void beginSerialization(ostream& stream, Archive& ac);
	public    : void finalizeSerialization(ostream& stream, Archive& ac);
	public    : string beginDeserialization(istream& stream, Archive& ac);
	public    : void finalizeDeserialization(istream& stream, Archive& ac);

	public    : static void serializeSerializable(ostream& stream, Archive& ac, int depth);
	public    : static void serializeContainer(ostream& stream, Archive& ac , int depth);
	public    : static void serializeSmartPointer(ostream& stream, Archive& ac , int depth);

	public    : static void deserializeSerializable(istream& stream, Archive& ac, const string& str="");
	public    : static void deserializeContainer(istream& stream, Archive& ac, const string& str="");
	public    : static void deserializeSmartPointer(istream& stream, Archive& ac, const string& str="");
	public    : static void deserializeFundamental(istream& stream, Archive& ac, const string& str);

	public    : static void writeTabs(ostream& stream, int depth);
	public    : static void writeOpeningTag(ostream& stream, Archive& ac, int depth);
	public    : static void writeClosingTag(ostream& stream, Archive& ac, int depth);


	//public    : static void serializeCustomFundamental(ostream& stream, Archive& ac, int depth);
	//public    : static void serializeContainerOfFundamental(ostream& stream, Archive& ac, int depth);
	//public    : static void serializeFundamentalSerializable(ostream& stream, Archive& ac, int depth);

	//public    : static void deserializeCustomFundamental(istream& stream, Archive& ac,const string& str);
	//public    : static void deserializeContainerOfFundamental(istream& stream, Archive& ac, const string& str);
	//public    : static void deserializeFundamentalSerializable(istream& stream, Archive& ac, const string& str);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_FACTORABLE(XMLManager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __XMLMANAGER__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
