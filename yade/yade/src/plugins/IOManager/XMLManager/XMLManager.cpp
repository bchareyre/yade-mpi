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
#include <string>
#include "ExceptionMessages.hpp"

using namespace std;

XmlSaxParser XMLManager::saxParser;

XMLManager::XMLManager() : IOManager()
{
	Archive::addSerializablePointer(SERIALIZABLE, false, serializeSerializable, deserializeSerializable);	
	Archive::addSerializablePointer(POINTER, false, serializeSmartPointer, deserializeSmartPointer);	
	Archive::addSerializablePointer(CONTAINER, false, serializeContainer, deserializeContainer);

	Archive::addSerializablePointer(CONTAINER, true, serializeContainerOfFundamental, deserializeContainerOfFundamental);
	Archive::addSerializablePointer(CUSTOM_CLASS, true, serializeCustomFundamental, deserializeCustomFundamental);
	Archive::addSerializablePointer(SERIALIZABLE, true, serializeFundamentalSerializable, deserializeFundamentalSerializable);
}


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


void XMLManager::writeOpeningTag(ostream& stream, Archive& ac, int depth)
{
	writeTabs(stream, depth);
	stream << "<" << ac.getName();

	if (ac.isFundamental())
		stream << ">";
}

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
		throw SerializableError(ExceptionMessages::WrongFileHeader);
	//saxParser.readAndParseNextXmlLine(stream);
}

void XMLManager::finalizeDeserialization(istream& , Archive&)
{
	//saxParser.readAndParseNextXmlLine(stream);
		
	//if (saxParser.getTagName()!="Yade")
	//	throw SerializableError(ExceptionMessages::WrongFileFooter);
}

void XMLManager::beginSerialization(ostream& stream, Archive& ac)
{
	stream << "<Yade>" << endl;
	writeOpeningTag(stream,ac,0);

}

void XMLManager::finalizeSerialization(ostream& stream, Archive& ac)
{
	writeClosingTag(stream,ac,0);
	stream << "</Yade>" << endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME : a fundamental made of a 2 fundamentalq for example is not parsed correctly ! => {1.2 {1 2 3}}
void XMLManager::tokenizeCustomFundamental(const string& str, vector<string>& tokens)
{
	int openBracketId = 0;
	int closingBracketId = 0;
	string tmpStr;
	
	while (str[openBracketId++]!='{');
	closingBracketId = openBracketId;
	while (str[closingBracketId++]!='}');

	tmpStr = str.substr(openBracketId,closingBracketId-openBracketId-1);
	
	string buf;
	stringstream sstr(tmpStr);
	while (sstr >> buf)
		tokens.push_back(buf);

}

void XMLManager::tokenizeContainerOfFundamental(const string& str, vector<string>& tokens)
{
	unsigned int i = 0;
	unsigned int bracketCount=0;
	unsigned int start;

	int category=-1;
	
	if (str.size()==0 || str=="[]")
		return;
	else
	{
		int nbOpeningBrackets = 0;
		int nbClosingBrackets = 0;

		for(unsigned int j=0;j<str.size();j++)
		{
			if (str[j]!=']')
				nbClosingBrackets++;
			else if (str[j]!='[')
				nbOpeningBrackets++;
			else if (str[j]!=' ' && str[j]!='\t' && str[j]!='\n')
				return;
		}
		if (nbOpeningBrackets==1 && nbClosingBrackets==1)
			return;
	}

	while (str[i++]!='[');

	while (i<str.size())
	{
		while (str[i]==' ' || str[i]=='\t' || str[i]=='\n')
			i++;

		start = i;
		if (str[i]=='[' && (category==-1 || category==1)) // container of container
		{
			category = 1;			
			bracketCount=0;
			do
			{
				while (str[i]==' ' || str[i]=='\t' || str[i]=='\n')
					i++;
				if (str[i]==']')
					bracketCount--;
				else if (str[i]=='[')
					bracketCount++;
				i++;
			} while (bracketCount!=0);
		}
		else if (str[i]=='{' && (category==-1 || category==2)) // container of custom fundamental
		{	
			category = 2;			
			while (str[i++]!='}');				
		}
		else if(category==-1 || category==3) // container of fundamental
		{
			category = 3;
			while (str[i]!=' ' && str[i]!='\t' && str[i]!='\n' && str[i]!=']')
				i++;
		}
		tokens.push_back(str.substr(start,i-start));	
		i++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	
void XMLManager::deserializeContainer(istream& stream, Archive& ac, const string& str)
{
	map<string,string> basicAttributes = saxParser.getBasicAttributes();
	int size = lexical_cast<int>(basicAttributes["size"]);

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

void XMLManager::deserializeSmartPointer(istream& stream, Archive& ac, const string& )
{
	map<string,string> basicAttributes = saxParser.getBasicAttributes();
	
	if (basicAttributes.size()!=0)
	{
		string className = basicAttributes["className"];
				
		saxParser.readAndParseNextXmlLine(stream);

		shared_ptr<Archive> tmpAc;		
		ac.createNewPointedArchive(ac,tmpAc,className);
	
		tmpAc->deserialize(stream, *tmpAc,"");
		
		saxParser.readAndParseNextXmlLine(stream);
	}

	ac.markProcessed();
}

void XMLManager::deserializeSerializable(istream& stream, Archive& ac, const string& )
{		
/*	cerr << "Starting deserialization of " << ac.getName();
	for(unsigned int i=0;i<48-ac.getName().size();i++)
		cerr <<".";
	cerr << "OK"<<endl;*/
	
	shared_ptr<Archive> tmpAc;

	Serializable * s = any_cast<Serializable*>(ac.getAddress());

	s->registerAttributes();
	
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
				string error=ExceptionMessages::AttributeNotFound + saxParser.getTagName();
				throw SerializableError(error.c_str());
			}
			saxParser.readAndParseNextXmlLine(stream);
		}
	}

	ac.markProcessed();
	s->unregisterAttributes();

/*	cerr << "Ending deserialization of " << ac.getName();
	for(unsigned int i=0;i<50-ac.getName().size();i++)
		cerr <<".";
	cerr << "OK"<<endl;*/
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void XMLManager::serializeContainer(ostream& stream, Archive& ac , int depth)
{
	shared_ptr<Archive> tmpAc;
	
	int size=ac.createNextArchive(ac,tmpAc,true);

	stream << " size=\"" << size << "\"";
	stream << ">" << endl;

	do
	{
		writeOpeningTag(stream,*tmpAc,depth);
		tmpAc->serialize(stream,*tmpAc,depth+1);
		writeClosingTag(stream,*tmpAc,depth);
	} while (ac.createNextArchive(ac,tmpAc,false));
			
	ac.markProcessed();
}

void XMLManager::serializeSmartPointer(ostream& stream, Archive& ac , int depth)
{
	shared_ptr<Archive> tmpAc;

	if(ac.createPointedArchive(ac,tmpAc))
	{	
		RecordType type = tmpAc->getRecordType();
		if (type==SERIALIZABLE)
		{			
			Serializable * s = any_cast<Serializable*>(tmpAc->getAddress());
			stream << " className=\"" << s->getClassName() << "\" >" << endl;
			writeOpeningTag(stream,*tmpAc,depth);
			tmpAc->serialize(stream,*tmpAc,depth+1);
			writeClosingTag(stream,*tmpAc,depth);
		}
		else if (type==CUSTOM_CLASS)
		{
			shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(tmpAc->getSerializableClassName()));
			stream << " className=\"" << s->getClassName() << "\" >" << endl;
			writeOpeningTag(stream,*tmpAc,depth);
			tmpAc->serialize(stream, *tmpAc, depth+1);
			writeClosingTag(stream,*tmpAc,depth);
		}
		else
			tmpAc->serialize(stream,*tmpAc, depth);
	}

	ac.markProcessed();
}

void XMLManager::serializeSerializable(ostream& stream, Archive& ac, int depth)
{
	Serializable * s;
	
	s = any_cast<Serializable*>(ac.getAddress());	

	s->registerAttributes();

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
	s->unregisterAttributes();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


// FIXME : provide a tokenize function pointer to parse customfundamental and container of fundamental
// then put (de)-serializeCustomFundamental/(de)-serializeContainerOfFundamental into IOManager
// or better provide a regexp
void XMLManager::deserializeCustomFundamental(istream& stream, Archive& ac,const string& str)
{
	shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));

	s->registerAttributes();

	vector<string> tokens;
	tokenizeCustomFundamental(str,tokens);

	vector<string>::const_iterator si    = tokens.begin();
	vector<string>::const_iterator siEnd = tokens.end();
	Serializable::Archives archives = s->getArchives();
	Serializable::Archives::iterator arci = archives.begin();
	for(;si!=siEnd;++si,++arci)
		(*arci)->deserialize(stream,*(*arci),(*si));
		
	s->deserialize(ac.getAddress());
	ac.markProcessed();
}


void XMLManager::serializeCustomFundamental(ostream& stream, Archive& ac,int depth)
{
	shared_ptr<Serializable> ss = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));
	ss->serialize(ac.getAddress());
	ss->registerAttributes();
	Serializable::Archives archives = ss->getArchives();
	Serializable::Archives::iterator archi = archives.begin();
	Serializable::Archives::iterator archiEnd = archives.end();
	Serializable::Archives::iterator archi2;
	stream << "{";
	for( ; archi!=archiEnd ; ++archi)
	{
		(*archi)->serialize(stream,**archi,depth+1);
		if (++(archi2=archi)!=archiEnd)
			stream << " ";
		(*archi)->markProcessed();
	}
	stream << "}";
	ss->unregisterAttributes();
	ac.markProcessed();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void XMLManager::deserializeContainerOfFundamental(istream& stream, Archive& ac, const string& str)
{
	vector<string> tokens;
	tokenizeContainerOfFundamental(str,tokens);

	shared_ptr<Archive> tmpAc;
	vector<string>::iterator ti = tokens.begin();
	vector<string>::iterator tiEnd = tokens.end();	

	ac.resize(ac,tokens.size());
	ac.createNextArchive(ac,tmpAc,true);

	for( ; ti!=tiEnd ; ++ti)
	{
		shared_ptr<Archive> tmpAc2;
		if (tmpAc->getRecordType()==POINTER)
			tmpAc->createNewPointedArchive(*tmpAc,tmpAc2,"");
		else
			tmpAc2 = tmpAc;
		tmpAc2->deserialize(stream,*tmpAc2,*ti);
		ac.createNextArchive(ac,tmpAc,false);
	}
	
	ac.markProcessed();
}

void XMLManager::serializeContainerOfFundamental(ostream& stream, Archive& ac, int depth)
{
	shared_ptr<Archive> tmpAc;
	int size=ac.createNextArchive(ac,tmpAc,true);
	int i=0;
	stream << "[";
	if (size!=0)
	{
		do
		{
			shared_ptr<Archive> tmpAc2;
			if (tmpAc->getRecordType()==POINTER)
				tmpAc->createPointedArchive(*tmpAc,tmpAc2);
			else
				tmpAc2 = tmpAc;
			tmpAc2->serialize(stream,*tmpAc2,depth+1);
			if (i!=size-1)
				stream << " ";
			i++;
		} while (ac.createNextArchive(ac,tmpAc,false));
	}
	
	stream << "]";
	ac.markProcessed();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void XMLManager::deserializeFundamentalSerializable(istream& stream, Archive& ac, const string& str)
{
	shared_ptr<Archive> tmpAc;

	Serializable * s = any_cast<Serializable*>(ac.getAddress());

	s->registerAttributes();
	
	vector<string> tokens;	
	tokenizeCustomFundamental(str,tokens);

	vector<string>::const_iterator si    = tokens.begin();
	vector<string>::const_iterator siEnd = tokens.end();
	Serializable::Archives archives = s->getArchives();
	Serializable::Archives::iterator arci = archives.begin();
	for(;si!=siEnd;++si,++arci)
		(*arci)->deserialize(stream,*(*arci),(*si));
		
	ac.markProcessed();
	s->unregisterAttributes();
}

void XMLManager::serializeFundamentalSerializable(ostream& stream, Archive& ac, int depth)
{
	Serializable * s;
	s = any_cast<Serializable*>(ac.getAddress());	
	s->registerAttributes();
	
	Serializable::Archives archives = s->getArchives();
	
	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	Serializable::Archives::iterator ai2;
	stream << "{";
	for( ; ai!=aiEnd ; ++ai)
	{
		(*ai)->serialize(stream,**ai,depth+1);
		if (++(ai2=ai)!=aiEnd)
			stream << " ";
		(*ai)->markProcessed();		
	}
	stream << "}";
	
	ac.markProcessed();
	s->unregisterAttributes();
	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
