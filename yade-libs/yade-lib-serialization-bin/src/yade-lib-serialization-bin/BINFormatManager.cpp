/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BINFormatManager.hpp"
#include <yade/yade-lib-serialization/IOManagerExceptions.hpp>
#include <string>
#include <vector>

using namespace std;

std::vector<std::string> BINFormatManager::names;

BINFormatManager::BINFormatManager() : IOFormatManager()
{
	names.clear();
	names.push_back("");
	
	Archive::addSerializablePointer(SerializableTypes::SERIALIZABLE , false, serializeSerializable             , deserializeSerializable);
	Archive::addSerializablePointer(SerializableTypes::SERIALIZABLE , true , serializeSerializable             , deserializeSerializable);
	Archive::addSerializablePointer(SerializableTypes::FUNDAMENTAL  , true , serializeFundamental              , deserializeFundamental);
	Archive::addSerializablePointer(SerializableTypes::CONTAINER    , false, serializeContainer                , deserializeContainer);
	Archive::addSerializablePointer(SerializableTypes::CONTAINER    , true , serializeContainer                , deserializeContainer);
	Archive::addSerializablePointer(SerializableTypes::POINTER      , false, serializeSmartPointer             , deserializeSmartPointer);
	Archive::addSerializablePointer(SerializableTypes::POINTER      , true , serializeSmartPointer             , deserializeSmartPointer);

	Archive::addSerializablePointer(SerializableTypes::CUSTOM_CLASS , false, serializeUnsupported              , deserializeUnsupported);
	//Archive::addSerializablePointer(SerializableTypes::CUSTOM_CLASS , true , serializeUnsupported              , deserializeUnsupported);
	Archive::addSerializablePointer(SerializableTypes::CUSTOM_CLASS , true , serializeCustomFundamental        , deserializeCustomFundamental);
}

BINFormatManager::~BINFormatManager()
{

}

void BINFormatManager::serializeNameMarker(ostream& stream, std::string name)
{
	
	unsigned short i , namesSize = names.size();
	for(i = 0 ; i < namesSize ; ++i)
		if( names[i] == name )
			break;
	
	if( i == namesSize )
	{
		unsigned char ch = 0;
		names.push_back(name);
		stream << ch;
		stream << ch;

/// original, safe version	
		static std::vector<unsigned char> bin;
		bin.clear();
		string::iterator ptr = name.begin();
		string::iterator end = name.end();
		bin.resize(name.size());
		std::copy(ptr, end, bin.begin() );

		unsigned short size=bin.size();
		assert(sizeof(size) == 2);
		stream << (reinterpret_cast<unsigned char*>(&size))[0];
		stream << (reinterpret_cast<unsigned char*>(&size))[1];
		for(int i=0 ; i < size ; ++i )
			stream << bin[i];
///

	}
	else
	{
		stream << (reinterpret_cast<unsigned char*>(&i))[0];
		stream << (reinterpret_cast<unsigned char*>(&i))[1];
	}
	
}

std::string BINFormatManager::deserializeNameMarker(istream& stream)
{
	static std::string result;
	result.clear();

	unsigned short nameIndex; 					// new version (smaller binary)
	stream.read(&((reinterpret_cast<char*>(&nameIndex))[0]),1); 	//
	stream.read(&((reinterpret_cast<char*>(&nameIndex))[1]),1); 	//
	if(nameIndex == 0) 						//
	{ 								//

		unsigned short size;
		stream.read(&((reinterpret_cast<char*>(&size))[0]),1);
		stream.read(&((reinterpret_cast<char*>(&size))[1]),1);
		char ch;
		for(int i=0 ; i<size ; ++i)
		{
			stream.read(&ch,1);
			result.push_back(ch);
		}

		names.push_back(result); 				// new version
	} 								//
	else 								//
	{ 								//
		result = names[nameIndex]; 				//
	} 								//

	return result;
}

string BINFormatManager::beginDeserialization(istream& stream, Archive& ac)
{
	unsigned char ch[5];
	stream.read(reinterpret_cast<char*>(&ch[0]),5);
	unsigned char version = ch[4];
	if(version != 1 || ch[0] != 'Y' || ch[1] != 'A' || ch[2] != 'D' || ch[3]!='E')
	{
		string error=string(IOManagerExceptions::WrongFileHeader) + " - wrong binary file version.";
		throw SerializableError(error.c_str()); 
	}
	return "";
}

void BINFormatManager::finalizeDeserialization(istream& , Archive&)
{
//	cerr << "finalizeDeserialization\n";
}

void BINFormatManager::beginSerialization(ostream& stream, Archive& ac)
{
	unsigned char version = 1;
	stream << "YADE" << version;
}

void BINFormatManager::finalizeSerialization(ostream& stream, Archive& ac)
{
}

/// Serialization and Deserialization of Serializable

void BINFormatManager::serializeSerializable(ostream& stream, Archive& ac, int depth)
{
	Serializable * s;
	s = any_cast<Serializable*>(ac.getAddress());
	s->registerSerializableAttributes(false);

	Serializable::Archives& archives = s->getArchives();

	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();

	for( ; ai!=aiEnd ; ++ai)
	{
		serializeNameMarker(stream,(*ai)->getName());
		(*ai)->serialize(stream, *(*ai),depth+1);
	}
	serializeNameMarker(stream,ac.getName());
	ac.markProcessed();
	s->unregisterSerializableAttributes(false);
}


void BINFormatManager::deserializeSerializable(istream& stream, Archive& ac, const string& )
{
//cerr << "+ deserializeSerializable start\n";
	shared_ptr<Archive> tmpAc;
	Serializable * s = any_cast<Serializable*>(ac.getAddress());

	s->registerSerializableAttributes(true);
	string name=deserializeNameMarker(stream);
//cerr << "    my name : " << ac.getName() << "\n";
//cerr << "name1: " << name << "\n";
	while ( name != ac.getName() )
	{
		if (s->findAttribute( name ,tmpAc))
		{
			tmpAc->deserialize(stream, *tmpAc,"");
		}
		else
		{
			string error=string(IOManagerExceptions::AttributeNotFound) + " " + name;
			throw SerializableError(error.c_str());
		}
		name = deserializeNameMarker(stream);
//cerr << "name2: " << name << "\n";
	}

	ac.markProcessed();
	s->unregisterSerializableAttributes(true);
//cerr << "- deserializeSerializable exit\n";
}

void BINFormatManager::serializeFundamental(ostream& stream, Archive& ac, int depth)
{
	static std::vector<unsigned char> bin;
	boost::any v = &bin;
	ac.serializeFundamental(ac,v);
	unsigned short size=bin.size();
	assert(sizeof(size) == 2);
	stream << (reinterpret_cast<unsigned char*>(&size))[0];
	stream << (reinterpret_cast<unsigned char*>(&size))[1];
	for(int i=0 ; i < size ; ++i )
		stream << bin[i];
	ac.markProcessed();
}

void BINFormatManager::deserializeFundamental(istream& stream, Archive& ac, const string&)
{
	static std::vector<unsigned char> bin;
	unsigned short size;
	stream.read(&((reinterpret_cast<char*>(&size))[0]),1);
	stream.read(&((reinterpret_cast<char*>(&size))[1]),1);
	char ch;
//cerr << "fund size: " << size << "\n";
	bin.resize(size);
	for(int i=0 ; i<size ; ++i)
	{
		stream.read(&ch,1);
		bin[i]=static_cast<unsigned char>(ch);
	}
	const vector<unsigned char>* binPtr = &bin;
	any v = binPtr;
	ac.deserializeFundamental(ac,v);
	ac.markProcessed();
}

/// Serialization and Deserialization of Container

void BINFormatManager::serializeContainer(ostream& stream, Archive& ac , int depth)
{
	shared_ptr<Archive> tmpAc;
	unsigned int size = ac.createNextArchive(ac,tmpAc,true);
	assert(sizeof(size) == 4);
	stream << (reinterpret_cast<unsigned char*>(&size))[0];
	stream << (reinterpret_cast<unsigned char*>(&size))[1];
	stream << (reinterpret_cast<unsigned char*>(&size))[2];
	stream << (reinterpret_cast<unsigned char*>(&size))[3];
	if (size!=0)
		do
		{
			tmpAc->serialize(stream,*tmpAc,depth+1);
		} while (ac.createNextArchive(ac,tmpAc,false));
	ac.markProcessed();
}


void BINFormatManager::deserializeContainer(istream& stream, Archive& ac, const string& str)
{
//cerr << "+ deserializeContainer start\n"; int cccc=0;
	unsigned int size;
	stream.read(&((reinterpret_cast<char*>(&size))[0]),1);
	stream.read(&((reinterpret_cast<char*>(&size))[1]),1);
	stream.read(&((reinterpret_cast<char*>(&size))[2]),1);
	stream.read(&((reinterpret_cast<char*>(&size))[3]),1);
	if (size>0)
	{
		ac.resize(ac,size);
		shared_ptr<Archive> tmpAc;
		ac.createNextArchive(ac,tmpAc,true);
		do
		{
//cerr << "  deserializeContainer el: " << cccc++ << "\n";
			tmpAc->deserialize(stream, *tmpAc,str);
		} while (ac.createNextArchive(ac,tmpAc,false));
	}
	ac.markProcessed();
//cerr << "- deserializeContainer exit\n";
}


/// Serialization and Deserialization of Smart Pointer

void BINFormatManager::serializeSmartPointer(ostream& stream, Archive& ac , int depth)
{
	shared_ptr<Archive> tmpAc;
	unsigned char ch;
	if(ac.createPointedArchive(ac,tmpAc))
	{
		ch = 0xff; // non-empty pointer
		stream << ch;
		SerializableTypes::Type type = tmpAc->getRecordType();
		if (type == SerializableTypes::SERIALIZABLE)
		{
			Serializable * s = any_cast<Serializable*>(tmpAc->getAddress());
			serializeNameMarker(stream,s->getClassName());
			tmpAc->serialize(stream,*tmpAc,depth);
		}
		else
		{
			string error=string(IOManagerExceptions::BadAttributeValue) + " - only pointers to Serializable class are supported in binary serialization, you can try xml frontend.";
			throw SerializableError(error.c_str());
		}
	} 
	else
	{
		ch = 0x42; // empty pointer
		stream << ch;
	}
	ac.markProcessed();
}


void BINFormatManager::deserializeSmartPointer(istream& stream, Archive& ac, const string& )
{
//cerr << "+ deserializeSmartPointer start\n";
	unsigned char ch;
	stream.read(reinterpret_cast<char*>(&ch),1);

	if (ch == 0xff) // non-empty
	{
		string className = deserializeNameMarker(stream);
		shared_ptr<Archive> tmpAc;
//cerr << "className: " << className << "\n";
		ac.createNewPointedArchive(ac,tmpAc,className);
		tmpAc->deserialize(stream, *tmpAc,"");
	}
	else if(ch != 0x42 )
	{
		string error=string(IOManagerExceptions::BadAttributeValue) + " - empty pointer expected, got sth. else, archive is broken (different architecture maybe?).";
		throw SerializableError(error.c_str());
	}

	ac.markProcessed();
//cerr << "- deserializeSmartPointer exit\n";
}

void BINFormatManager::serializeUnsupported(ostream&, Archive&, int)
{
	string error=string(IOManagerExceptions::BadAttributeValue) + " - custom class is not supported in binary, I'm too lazy to write this stuff, but you can write it ;)";
	throw SerializableError(error.c_str());
}

void BINFormatManager::deserializeUnsupported(istream&, Archive&,const string&)
{
	string error=string(IOManagerExceptions::BadAttributeValue) + " - custom class is not supported in binary, I'm too lazy to write this stuff, but you can write it ;)";
	throw SerializableError(error.c_str());
}

void BINFormatManager::deserializeCustomFundamental(istream& stream, Archive& ac,const string& str)
{
	shared_ptr<Serializable> s = YADE_PTR_CAST<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));

	s->registerSerializableAttributes(true);

	Serializable::Archives archives = s->getArchives();
	Serializable::Archives::iterator archi = archives.begin();
	Serializable::Archives::iterator archiEnd = archives.end();
	for( ; archi!=archiEnd ; ++archi)
		(*archi)->deserialize(stream,**archi,str);

	s->deserialize(ac.getAddress());
	s->unregisterSerializableAttributes(true);
	ac.markProcessed();
}


void BINFormatManager::serializeCustomFundamental(ostream& stream, Archive& ac,int depth)
{
	shared_ptr<Serializable> ss = YADE_PTR_CAST<Serializable>(ClassFactory::instance().createShared(ac.getSerializableClassName()));
	ss->serialize(ac.getAddress());
	ss->registerSerializableAttributes(false);
	Serializable::Archives archives = ss->getArchives();
	Serializable::Archives::iterator archi = archives.begin();
	Serializable::Archives::iterator archiEnd = archives.end();

	for( ; archi!=archiEnd ; ++archi)
	{
		(*archi)->serialize(stream,**archi,depth+1);
		(*archi)->markProcessed();
	}

	ss->unregisterSerializableAttributes(false);
	ac.markProcessed();
}

