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
 ***************************************************************************/

#include "XMLManager.hpp"
#include "IOManagerExceptions.hpp"

#include <string>

using namespace std;

XmlSaxParser XMLManager::saxParser;

XMLManager::XMLManager() : IOManager()
{
	Archive::addSerializablePointer(SerializableTypes::SERIALIZABLE, false, serializeSerializable, deserializeSerializable);
	Archive::addSerializablePointer(SerializableTypes::POINTER, false, serializeSmartPointer, deserializeSmartPointer);
	Archive::addSerializablePointer(SerializableTypes::CONTAINER, false, serializeContainer, deserializeContainer);
	Archive::addSerializablePointer(SerializableTypes::FUNDAMENTAL, true , IOManager::serializeFundamental, deserializeFundamental);

	setContainerOpeningBracket('[');
	setContainerClosingBracket(']');
	setContainerSeparator(' ');

	setCustomFundamentalOpeningBracket('{');
	setCustomFundamentalClosingBracket('}');
	setCustomFundamentalSeparator(' ');
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

XMLManager::~XMLManager()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::writeTabs(ostream& stream, int depth)
{
	for(int i=0;i<depth+1;i++)
		stream << '\t';
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::writeOpeningTag(ostream& stream, Archive& ac, int depth)
{
	writeTabs(stream, depth);
	stream << "<" << ac.getName();

	if (ac.isFundamental())
		stream << ">";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::writeClosingTag(ostream& stream, Archive& ac, int depth)
{
	if (ac.isFundamental())
		stream << "</" << ac.getName() << ">" << endl;
	else if (ac.containsOnlyFundamentals())
		stream << " />" << endl;
	else
	{
		writeTabs(stream, depth);
		stream << "</" << ac.getName() << ">" << endl;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

string XMLManager::beginDeserialization(istream& stream, Archive& ac)
{
	saxParser.readAndParseNextXmlLine(stream);
	if (saxParser.getTagName()=="Yade")
	{
		saxParser.readAndParseNextXmlLine(stream);
		if (ac.isFundamental())
			return saxParser.readNextFundamentalStringValue(stream);
		else
			return "";
	}
	else
	{
		string error = string(IOManagerExceptions::WrongFileHeader) + " line: " + lexical_cast<string>(saxParser.getLineNumber());
		throw SerializableError(error.c_str());
	}
	//saxParser.readAndParseNextXmlLine(stream);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::finalizeDeserialization(istream& , Archive&)
{
	//saxParser.readAndParseNextXmlLine(stream);

	//if (saxParser.getTagName()!="Yade")
	//	throw SerializableError(ExceptionMessages::WrongFileFooter);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::beginSerialization(ostream& stream, Archive& ac)
{
	stream << "<Yade>" << endl;
	writeOpeningTag(stream,ac,0);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::finalizeSerialization(ostream& stream, Archive& ac)
{
	writeClosingTag(stream,ac,0);
	stream << "</Yade>" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization and Deserialization of Serializable						///
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::serializeSerializable(ostream& stream, Archive& ac, int depth)
{
	Serializable * s;

	s = any_cast<Serializable*>(ac.getAddress());

	s->registerSerializableAttributes(false);

	Serializable::Archives archives = s->getArchives();

	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	for( ; ai!=aiEnd ; ++ai)
	{
		if ((*ai)->isFundamental())
		{
			stream << " "  << (*ai)->getName() << "=\"";
			(*ai)->serialize(stream, *(*ai),depth+1);
			stream << "\"";
		}
	}

	if (!s->containsOnlyFundamentals())
	{
		stream << ">" << endl;

		ai    = archives.begin();
		for( ; ai!=aiEnd ; ++ai)
		{
			if (!(*ai)->isFundamental())
			{
				writeOpeningTag(stream,*(*ai),depth);
				(*ai)->serialize(stream, *(*ai),depth+1);
				writeClosingTag(stream,*(*ai),depth);
			}
		}
	}

	ac.markProcessed();
	s->unregisterSerializableAttributes(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::deserializeSerializable(istream& stream, Archive& ac, const string& )
{
	shared_ptr<Archive> tmpAc;

	Serializable * s = any_cast<Serializable*>(ac.getAddress());

	s->registerSerializableAttributes(true);

	// deserializing (custom-)fundamental attributes
	map<string,string> basicAttributes = saxParser.getBasicAttributes();
	map<string,string>::const_iterator bai    = basicAttributes.begin();
	map<string,string>::const_iterator baiEnd = basicAttributes.end();
	for(;bai!=baiEnd;++bai)
	{
		if(s->findAttribute(bai->first,tmpAc))
		{
			if (tmpAc->isFundamental())
				tmpAc->deserialize(stream, *tmpAc,bai->second);
		}

	}

	if(!saxParser.isFullTag())
	{
		saxParser.readAndParseNextXmlLine(stream);

		// deserializing not fundamental attributes
		while (saxParser.getTagName()!=ac.getName())
		{
			if (s->findAttribute(saxParser.getTagName(),tmpAc))
			{
				if (!tmpAc->isFundamental())
					tmpAc->deserialize(stream, *tmpAc,/*bai->second*/""); // FIXME : change
			}
			else
			{
				string error=string(IOManagerExceptions::AttributeNotFound) + " " + saxParser.getTagName() + " line: " + lexical_cast<string>(saxParser.getLineNumber());
				throw SerializableError(error.c_str());
			}
			saxParser.readAndParseNextXmlLine(stream);
		}
	}

	ac.markProcessed();
	s->unregisterSerializableAttributes(true);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization and Deserialization of Container						///
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::serializeContainer(ostream& stream, Archive& ac , int depth)
{
	shared_ptr<Archive> tmpAc;

	int size = ac.createNextArchive(ac,tmpAc,true);

	stream << " size=\"" << size << "\"";

	if (size!=0)
	{
		stream << ">" << endl;
		do
		{
			writeOpeningTag(stream,*tmpAc,depth);
			tmpAc->serialize(stream,*tmpAc,depth+1);
			writeClosingTag(stream,*tmpAc,depth);
		} while (ac.createNextArchive(ac,tmpAc,false));
	}

	ac.markProcessed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::deserializeContainer(istream& stream, Archive& ac, const string& str)
{
	map<string,string> basicAttributes = saxParser.getBasicAttributes();
	unsigned int size = lexical_cast<unsigned int>(basicAttributes["size"]);
	if (size>0)
	{
		ac.resize(ac,size);
		shared_ptr<Archive> tmpAc;

		saxParser.readAndParseNextXmlLine(stream);
		ac.createNextArchive(ac,tmpAc,true);

		do
		{
			tmpAc->deserialize(stream, *tmpAc,str);
			saxParser.readAndParseNextXmlLine(stream);
		} while (ac.createNextArchive(ac,tmpAc,false));
	}

	ac.markProcessed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization and Deserialization of Smart Pointer						///
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::serializeSmartPointer(ostream& stream, Archive& ac , int depth)
{
	shared_ptr<Archive> tmpAc;

	if(ac.createPointedArchive(ac,tmpAc))
	{
		SerializableTypes::Type type = tmpAc->getRecordType();
		if (type==SerializableTypes::SERIALIZABLE)
		{
			Serializable * s = any_cast<Serializable*>(tmpAc->getAddress());
			stream << " _className_=\"" << s->getClassName() << "\" ";//>" << endl;
			tmpAc->serialize(stream,*tmpAc,depth);
		}
		else if (type==SerializableTypes::CUSTOM_CLASS)
		{
			shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(tmpAc->getSerializableClassName()));
			stream << " _className_=\"" << s->getClassName() << "\" ";//>" << endl;
			tmpAc->serialize(stream, *tmpAc, depth+1);
		}
		else
			tmpAc->serialize(stream,*tmpAc, depth);
	}

	ac.markProcessed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::deserializeSmartPointer(istream& stream, Archive& ac, const string& )
{
	map<string,string> basicAttributes = saxParser.getBasicAttributes();

	if (basicAttributes.size()!=0)
	{
		string className = basicAttributes["_className_"];
		shared_ptr<Archive> tmpAc;
		ac.createNewPointedArchive(ac,tmpAc,className);
		tmpAc->deserialize(stream, *tmpAc,"");
	}

	ac.markProcessed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void XMLManager::deserializeFundamental(istream& stream, Archive& ac, const string& str)
{
	try
	{
		IOManager::deserializeFundamental(stream,ac,str);
	}
	catch(boost::bad_lexical_cast& )
	{
		string error=string(IOManagerExceptions::BadAttributeValue) + " (bad lexical_cast) " + saxParser.getTagName() + " line: " + lexical_cast<string>(saxParser.getLineNumber());
		throw SerializableError(error.c_str());
	}
}
