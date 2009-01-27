/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class XmlSaxParser
{
	private :
		map< string , string > basicAttributes;
		string currentXmlLine;
		string currentLineCopy;
		string tagName;
		int lineLength;

		unsigned long int lineNumber;
		int findCar(int i,char c);
		bool isWhiteSpace(char c);

	public :
		unsigned long int getLineNumber();

		XmlSaxParser ();
		virtual ~XmlSaxParser ();

		bool readNextXmlLine(istream& stream);
		string readNextFundamentalStringValue(istream& stream);
		void parseCurrentXmlLine();
		bool readAndParseNextXmlLine(istream& stream);
		bool isFullTag();
		bool isOpeningTag();
		bool isClosingTag();
		bool isComment();
		string getTagName();
		string getArgumentValue(const string& argName);
		const map<string,string>& getBasicAttributes();
		void deleteBasicAttribute(const string& name);
};


