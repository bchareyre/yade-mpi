#ifndef __XMLSAXPARSER_H__
#define __XMLSAXPARSER_H__

#include <map>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

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
	public    : ~XmlSaxParser ();


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



#endif // __XMLSAXPARSER_H__
