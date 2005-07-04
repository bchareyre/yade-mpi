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

#ifndef __XMLSAXPARSER_H__
#define __XMLSAXPARSER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <map>
#include <vector>
#include <string>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class XmlSaxParser
{
	private   : map< string , string > basicAttributes;
	private   : string currentXmlLine;
	private   : string currentLineCopy;
	private   : string tagName;
	private   : int lineLength;

	private   : unsigned long int lineNumber;
	public    : unsigned long int getLineNumber();

	// construction
	public    : XmlSaxParser ();
	public    : virtual ~XmlSaxParser ();


	public    : bool readNextXmlLine(istream& stream);
	public    : string readNextFundamentalStringValue(istream& stream);
	public    : void parseCurrentXmlLine();
	public    : bool readAndParseNextXmlLine(istream& stream);
	public    : bool isFullTag();
	public    : bool isOpeningTag();
	public    : bool isClosingTag();
	public    : bool isComment();
	public    : string getTagName();
	public    : string getArgumentValue(const string& argName);
	public    : const map<string,string>& getBasicAttributes();
	public    : void deleteBasicAttribute(const string& name);
	private   : int findCar(int i,char c);
	private   : bool isWhiteSpace(char c);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __XMLSAXPARSER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
