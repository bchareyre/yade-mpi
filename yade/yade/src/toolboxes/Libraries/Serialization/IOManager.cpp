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
#include <boost/spirit.hpp>
#include "Serializable.hpp"
using namespace boost::spirit;

char IOManager::cOB	= ' ';
char IOManager::cCB	= ' ';
char IOManager::cS	= ' ';
char IOManager::cfOB	= ' ';
char IOManager::cfCB	= ' ';
char IOManager::cfS	= ' ';


IOManager::IOManager()
{
	Archive::addSerializablePointer(SerializableTypes::CUSTOM_CLASS, false, serializeCustomClass, deserializeCustomClass);
	Archive::addSerializablePointer(SerializableTypes::FUNDAMENTAL, true,serializeFundamental, deserializeFundamental);
	Archive::addSerializablePointer(SerializableTypes::POINTER, true, serializeSmartPointerOfFundamental, deserializeSmartPointerOfFundamental);

	Archive::addSerializablePointer(SerializableTypes::CONTAINER, true, serializeContainerOfFundamental, deserializeContainerOfFundamental);
	Archive::addSerializablePointer(SerializableTypes::CUSTOM_CLASS, true, serializeCustomFundamental, deserializeCustomFundamental);
	Archive::addSerializablePointer(SerializableTypes::SERIALIZABLE, true, serializeFundamentalSerializable, deserializeFundamentalSerializable);

}

IOManager::~IOManager()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void IOManager::parseFundamental(const string& top, vector<string>& eval)
{
	eval.clear();

	rule<> inside 		= +( graph_p - cOB - cCB - cfOB - cfCB ); // asdf23
	rule<> empty_array 	= *space_p >> cOB >> *space_p >> cCB >> *space_p; // [ ]
	rule<> empty_fund 	= *space_p >> cfOB >> *space_p >> cfCB >> *space_p; // { }
	rule<> one_fundamental	= *space_p >>
				  		  ( cfOB >> (*space_p) % (inside) >> cfCB )
						| ( cfOB >>
							( (*space_p ) % (inside) )
							%
							( cfOB >> (*space_p ) % (inside) >> cfCB )
				  		  >> cfCB )
				  >> *space_p; // { 123, 243 { sdf, sd} qwe ,as }

	rule<> one_array	= *space_p >>
				  		  ( cOB >> (*space_p  ) % (inside) >> cCB )
						| ( cOB >>
							( (*space_p) % (inside) )
							%
							( cOB >> (*space_p) % (inside) >> cCB )
				  		>> cCB )
				  >> *space_p; // [ 123 324 243 qwe as ]
//	rule<> one_array = *space_p >> '[' >> (*space_p) % (inside) >> ']' >> *space_p; // [ 123 324 243 qwe as ]

	rule<> one_everything = *space_p
			>>	(*space_p) % (
						  (inside)
						| (empty_array)
						| (empty_fund)
						| (one_fundamental)
						| (one_array)
					     ) [push_back_a(eval)]
			>> *space_p;

	rule<> array = *space_p >> cOB >> *space_p >> one_everything >> *space_p >> cCB >> *space_p >> end_p;
	rule<> fundamental = *space_p >> cfOB >> *space_p >> one_everything >> *space_p >> cfCB >> *space_p >> end_p;

	rule<> everything = array | fundamental;

	parse(top.c_str(),everything);
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





// FIXME : provide a tokenize function pointer to parse customfundamental and container of fundamental
// then put (de)-serializeCustomFundamental/(de)-serializeContainerOfFundamental into IOManager
// or better provide a regexp
void IOManager::deserializeCustomFundamental(istream& stream, Archive& ac,const string& str)
{
	shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));

	s->registerAttributes();

	vector<string> tokens;
	parseFundamental(str,tokens);

	vector<string>::const_iterator si    = tokens.begin();
	vector<string>::const_iterator siEnd = tokens.end();
	Serializable::Archives archives = s->getArchives();
	Serializable::Archives::iterator arci = archives.begin();
	for(;si!=siEnd;++si,++arci)
		(*arci)->deserialize(stream,*(*arci),(*si));

	s->deserialize(ac.getAddress());
	ac.markProcessed();
}


void IOManager::serializeCustomFundamental(ostream& stream, Archive& ac,int depth)
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


void IOManager::deserializeContainerOfFundamental(istream& stream, Archive& ac, const string& str)
{
	vector<string> tokens;
	parseFundamental(str,tokens);

	shared_ptr<Archive> tmpAc;
	vector<string>::iterator ti = tokens.begin();
	vector<string>::iterator tiEnd = tokens.end();

	ac.resize(ac,tokens.size());
	ac.createNextArchive(ac,tmpAc,true);

	for( ; ti!=tiEnd ; ++ti)
	{
		shared_ptr<Archive> tmpAc2;
		if (tmpAc->getRecordType()==SerializableTypes::POINTER)
			tmpAc->createNewPointedArchive(*tmpAc,tmpAc2,"");
		else
			tmpAc2 = tmpAc;
		tmpAc2->deserialize(stream,*tmpAc2,*ti);
		ac.createNextArchive(ac,tmpAc,false);
	}

	ac.markProcessed();
}

void IOManager::serializeContainerOfFundamental(ostream& stream, Archive& ac, int depth)
{
	shared_ptr<Archive> tmpAc;
	int size=ac.createNextArchive(ac,tmpAc,true);
	int i=0;
	stream << cOB;
	if (size!=0)
	{
		do
		{
			shared_ptr<Archive> tmpAc2;
			if (tmpAc->getRecordType()==SerializableTypes::POINTER)
				tmpAc->createPointedArchive(*tmpAc,tmpAc2);
			else
				tmpAc2 = tmpAc;
			tmpAc2->serialize(stream,*tmpAc2,depth+1);
			if (i!=size-1)
				stream << cS;
			i++;
		} while (ac.createNextArchive(ac,tmpAc,false));
	}

	stream << cCB;
	ac.markProcessed();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void IOManager::deserializeFundamentalSerializable(istream& stream, Archive& ac, const string& str)
{
	shared_ptr<Archive> tmpAc;

	Serializable * s = any_cast<Serializable*>(ac.getAddress());

	s->registerAttributes();

	vector<string> tokens;
	parseFundamental(str,tokens);

	vector<string>::const_iterator si    = tokens.begin();
	vector<string>::const_iterator siEnd = tokens.end();
	Serializable::Archives archives = s->getArchives();
	Serializable::Archives::iterator arci = archives.begin();
	for(;si!=siEnd;++si,++arci)
		(*arci)->deserialize(stream,*(*arci),(*si));

	ac.markProcessed();
	s->unregisterAttributes();
}

void IOManager::serializeFundamentalSerializable(ostream& stream, Archive& ac, int depth)
{
	Serializable * s;
	s = any_cast<Serializable*>(ac.getAddress());
	s->registerAttributes();

	Serializable::Archives archives = s->getArchives();

	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	Serializable::Archives::iterator ai2;
	stream << cfOB;
	for( ; ai!=aiEnd ; ++ai)
	{
		(*ai)->serialize(stream,**ai,depth+1);
		if (++(ai2=ai)!=aiEnd)
			stream << cfS;
		(*ai)->markProcessed();
	}
	stream << cfCB;

	ac.markProcessed();
	s->unregisterAttributes();

}
