/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "IOFormatManager.hpp"
#include "Serializable.hpp"
#include <boost/spirit.hpp>

using namespace boost::spirit;


char IOFormatManager::cOB	= ' ';
char IOFormatManager::cCB	= ' ';
char IOFormatManager::cS	= ' ';
char IOFormatManager::cfOB	= ' ';
char IOFormatManager::cfCB	= ' ';
char IOFormatManager::cfS	= ' ';


IOFormatManager::IOFormatManager()
{
	Archive::addSerializablePointer(SerializableTypes::CUSTOM_CLASS, false, serializeCustomClass, deserializeCustomClass);
	Archive::addSerializablePointer(SerializableTypes::FUNDAMENTAL, true,serializeFundamental, deserializeFundamental);
	Archive::addSerializablePointer(SerializableTypes::POINTER, true, serializeSmartPointerOfFundamental, deserializeSmartPointerOfFundamental);

	Archive::addSerializablePointer(SerializableTypes::CONTAINER, true, serializeContainerOfFundamental, deserializeContainerOfFundamental);
	Archive::addSerializablePointer(SerializableTypes::CUSTOM_CLASS, true, serializeCustomFundamental, deserializeCustomFundamental);
	Archive::addSerializablePointer(SerializableTypes::SERIALIZABLE, true, serializeFundamentalSerializable, deserializeFundamentalSerializable);

}


IOFormatManager::~IOFormatManager()
{
}


void IOFormatManager::beginSerialization(ostream& ,  Archive& )
{
	cout << "bad begin"<<endl;
	throw; 
}


void IOFormatManager::finalizeSerialization(ostream& ,  Archive& )
{
	throw; 
}


string IOFormatManager::beginDeserialization(istream& ,  Archive& )
{
	throw;
}


void IOFormatManager::finalizeDeserialization(istream& , Archive& )
{
	throw; 
}


// FIXME : this spirit stuff works currently only with ' ' - space - separators.

void IOFormatManager::parseFundamental(const string& top, vector<string>& eval)
{
	eval.clear();

	rule<> inside 		= +( graph_p - cOB - cCB - cfOB - cfCB ); 		// asdf23
	rule<> empty_array 	= *space_p >> cOB >> *space_p >> cCB >> *space_p; 	// [ ]
	rule<> empty_fund 	= *space_p >> cfOB >> *space_p >> cfCB >> *space_p; 	// { }
	rule<> one_fundamental	= *space_p >>
				  		  ( cfOB >> (*space_p) % (inside) >> cfCB )
						| ( cfOB >>
							( (*space_p ) % (inside) )
							%
							( cfOB >> (*space_p ) % (inside) >> cfCB )
				  		  >> cfCB )
				  >> *space_p; 						// { 123, 243 { sdf, sd} qwe ,as }

	rule<> one_array	= *space_p >>
				  		  ( cOB >> (*space_p  ) % (inside) >> cCB )
						| ( cOB >>
							( (*space_p) % (inside) )
							%
							( cOB >> (*space_p) % (inside) >> cCB )
				  		>> cCB )
				  >> *space_p; 						// [ 123 324 243 qwe as ]

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
	
	//FIXME : modified by Olivier because if top contains only one string then eval is empty!
	//
	// I must ask you to reproduce this error, because if there is only one string - it works, it must be something else.
	//
	// if you uncomment these lines in snippets/Serialization - it will crash! - for "[]" - bad_lexical_cast
	//
	// if you really need it for something - uncomment it. but I must know what is thre real bug, so that
	// I can fix spirit!
	//
	if (eval.size()==0)
	{
		bool pushBackTop = true;
		for(unsigned int i=0;i<top.size();i++)
			pushBackTop &=  (top[i]!=']' && top[i]!='[' && top[i]!='}' && top[i]!='{');
		if (pushBackTop)
			eval.push_back(top);
	}
}




void IOFormatManager::deserializeFundamental(istream& , Archive& ac,const string& str)
{
	any v = &str;
//	cout << "deserializing : |" << str << "|" << endl;
	ac.deserializeFundamental(ac,v);
	ac.markProcessed();
}

void IOFormatManager::serializeFundamental(ostream& stream, Archive& ac,int )
{
	string str;
	any v = &str;
	ac.serializeFundamental(ac,v);
	stream << str;
	ac.markProcessed();
}




void IOFormatManager::deserializeCustomClass(istream& stream, Archive& ac, const string& str)
{
	shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));

	shared_ptr<Archive> tmpAc = Archive::create(ac.getName(),*s);
	tmpAc->deserialize(stream, *tmpAc, str);
	s->deserialize(ac.getAddress());

	ac.markProcessed();
}

void IOFormatManager::serializeCustomClass(ostream& stream, Archive& ac,int depth)
{
	shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));
	s->serialize(ac.getAddress());
	shared_ptr<Archive> tmpAc = Archive::create(ac.getName(),*s);
	tmpAc->serialize(stream,*tmpAc,depth);
	ac.markProcessed();
}




void IOFormatManager::deserializeSmartPointerOfFundamental(istream& stream, Archive& ac, const string& str)
{
	if (str.size() != 0)
	{
		shared_ptr<Archive> tmpAc;
		ac.createNewPointedArchive(ac,tmpAc,"");
		tmpAc->deserialize(stream, *tmpAc,str);
	}
	ac.markProcessed();
}

void IOFormatManager::serializeSmartPointerOfFundamental(ostream& stream, Archive& ac , int depth)
{
	shared_ptr<Archive> tmpAc;

	if(ac.createPointedArchive(ac,tmpAc))
		tmpAc->serialize(stream, *tmpAc,depth+1);

	ac.markProcessed();
}





// FIXME : provide a tokenize function pointer to parse customfundamental and container of fundamental
// then put (de)-serializeCustomFundamental/(de)-serializeContainerOfFundamental into IOFormatManager
// or better provide a regexp
void IOFormatManager::deserializeCustomFundamental(istream& stream, Archive& ac,const string& str)
{
	shared_ptr<Serializable> s = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));

	s->registerSerializableAttributes(true);

	vector<string> tokens;
	parseFundamental(str,tokens);

	vector<string>::const_iterator si    = tokens.begin();
	vector<string>::const_iterator siEnd = tokens.end();
	Serializable::Archives archives = s->getArchives();
	Serializable::Archives::iterator arci = archives.begin();
	for(;si!=siEnd;++si,++arci)
		(*arci)->deserialize(stream,*(*arci),(*si));

	s->deserialize(ac.getAddress());
	s->unregisterSerializableAttributes(true); // FIXME - make sure that it was a mistake that this line WAS MISSING.
	ac.markProcessed();
}


void IOFormatManager::serializeCustomFundamental(ostream& stream, Archive& ac,int depth)
{
	shared_ptr<Serializable> ss = dynamic_pointer_cast<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));
	ss->serialize(ac.getAddress());
	ss->registerSerializableAttributes(false);
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
	ss->unregisterSerializableAttributes(false);
	ac.markProcessed();
}




void IOFormatManager::deserializeContainerOfFundamental(istream& stream, Archive& ac, const string& str)
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

void IOFormatManager::serializeContainerOfFundamental(ostream& stream, Archive& ac, int depth)
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




void IOFormatManager::deserializeFundamentalSerializable(istream& stream, Archive& ac, const string& str)
{
	shared_ptr<Archive> tmpAc;

	Serializable * s = any_cast<Serializable*>(ac.getAddress());

	s->registerSerializableAttributes(true);

	vector<string> tokens;
	parseFundamental(str,tokens);

	vector<string>::const_iterator si    = tokens.begin();
	vector<string>::const_iterator siEnd = tokens.end();
	Serializable::Archives archives = s->getArchives();
	Serializable::Archives::iterator arci = archives.begin();
	for(;si!=siEnd;++si,++arci)
		(*arci)->deserialize(stream,*(*arci),(*si));

	ac.markProcessed();
	s->unregisterSerializableAttributes(true);
}

void IOFormatManager::serializeFundamentalSerializable(ostream& stream, Archive& ac, int depth)
{
	Serializable * s;
	s = any_cast<Serializable*>(ac.getAddress());
	s->registerSerializableAttributes(false);

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
	s->unregisterSerializableAttributes(false);

}
