/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef __FUNDAMENTALHANDLER_H__
#define __FUNDAMENTALHANDLER_H__

#include "SerializationExceptions.hpp"
#include "Archive.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/type_traits.hpp>
#include<boost/algorithm/string.hpp>

using namespace boost; 

template<typename Type>
struct FundamentalHandler
{
	static void creator(Archive& , any& )
	{
		throw HandlerError(SerializationExceptions::FundamentalNotSupported);
	}
	static void accessor(Archive& , any& )
	{
		throw HandlerError(SerializationExceptions::FundamentalNotSupported);
	}
};

template< >
struct FundamentalHandler< string >
{
	static void creator(Archive& ac, any& a)
	{
		if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
		{
			const string * tmpStr = any_cast<const string*>(a);
			string * tmp = any_cast<string*>(ac.getAddress());
			*tmp=*tmpStr;
			#define _HANDLE_SPECIAL(special,escape) boost::algorithm::replace_all(*tmp,escape,special);
			_HANDLE_SPECIAL("\n","&br;"); _HANDLE_SPECIAL("\t","&tab;"); _HANDLE_SPECIAL("<","&lt;"); _HANDLE_SPECIAL(">","&gt;"); _HANDLE_SPECIAL("\"","&quot;"); _HANDLE_SPECIAL("'","&apos;"); _HANDLE_SPECIAL(" ","&nbsp;"); _HANDLE_SPECIAL("[","&lsquare;"); _HANDLE_SPECIAL("]","&rsquare;"); _HANDLE_SPECIAL("{","&lcurly;"); _HANDLE_SPECIAL("}","&rcurly;");
			_HANDLE_SPECIAL("&","&amp;");
			#undef _HANDLE_SPECIAL
			
		}
		else if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
		{
			const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
			string * tmp = any_cast<string*>(ac.getAddress());
			std::vector<unsigned char>::const_iterator ptr = (*tmpBin).begin();
			std::vector<unsigned char>::const_iterator end = (*tmpBin).end();
			(*tmp).resize( (*tmpBin).size() );
			std::copy(ptr,end,(*tmp).begin());
		}
		else
			throw HandlerError(SerializationExceptions::LexicalCopyError);
	}
	static void accessor(Archive& ac, any& a)
	{ // FIXME - throw when trying to serialize a string that has spaces. ( if(string.find(' ') != string.end() ...) : (eudoxos: WHY?!!!!!)
		if (a.type()==typeid(string*)) // serialization - writing to string from some Type
		{ CHK_XML();
			string * tmpStr = any_cast<string*>(a);
			string * tmp = any_cast<string*>(ac.getAddress());
			*tmpStr=*tmp;
			#define _HANDLE_SPECIAL(special,escape) boost::algorithm::replace_all(*tmpStr,special,escape);
			_HANDLE_SPECIAL("&","&amp;");
			_HANDLE_SPECIAL("\n","&br;"); _HANDLE_SPECIAL("\t","&tab;"); _HANDLE_SPECIAL("<","&lt;"); _HANDLE_SPECIAL(">","&gt;"); _HANDLE_SPECIAL("\"","&quot;"); _HANDLE_SPECIAL("'","&apos;"); _HANDLE_SPECIAL(" ","&nbsp;"); _HANDLE_SPECIAL("[","&lsquare;"); _HANDLE_SPECIAL("]","&rsquare;"); _HANDLE_SPECIAL("{","&lcurly;"); _HANDLE_SPECIAL("}","&rcurly;");
			#undef _ESCAPE_SPECIAL
		}
		else if (a.type()==typeid(vector<unsigned char>*)) // from string to binary stream
		{ CHK_BIN();
			vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
			string * tmp = any_cast<string*>(ac.getAddress());
			(*tmpBin).clear();
			string::iterator ptr = (*tmp).begin();
			string::iterator end = (*tmp).end();
			(*tmpBin).resize((*tmp).size());
			std::copy(ptr, end, (*tmpBin).begin() );
		}
		else
			throw HandlerError(SerializationExceptions::LexicalCopyError);
	}
};

template<typename Type >
inline void lexical_copy(Archive& ac , any& a )
{
	if(FormatChecker::format==FormatChecker::XML){
	// Textual serialization
	if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
	{
		const string * tmpStr = any_cast<const string*>(a);
		Type * tmp = any_cast<Type*>(ac.getAddress());
		try{
			*tmp = lexical_cast<Type>(*tmpStr);
		} catch(boost::bad_lexical_cast& e){
			if(a.type()==typeid(bool*) && atoi(tmpStr->c_str())!=0) { *tmp=lexical_cast<Type>("true"); cerr<<"Offensive bool value `"<<*tmpStr<<"' encountered.!!"<<endl; }
			else throw e;
		}
	}
	else if (a.type()==typeid(string*)) // serialization - writing to string from some Type
	{ CHK_XML();
		string * tmpStr = any_cast<string*>(a);
		Type * tmp = any_cast<Type*>(ac.getAddress());
			*tmpStr = lexical_cast<string>(*tmp);
	}
	else
		cerr<<"lexical_cast(XML): (de)serialization format mismatch"<<endl;
	}
	else if(FormatChecker::format==FormatChecker::BIN){
	// Binary serialization
	if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
	{
		const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
		Type * tmp = any_cast<Type*>(ac.getAddress());
		BOOST_STATIC_ASSERT((boost::is_POD<Type>::value));
		std::vector<unsigned char>::const_iterator ptr = (*tmpBin).begin();
		std::vector<unsigned char>::const_iterator end = (*tmpBin).end();
		if(sizeof(Type) != (*tmpBin).size())
			throw HandlerError(SerializationExceptions::LexicalCopyBinError);
		unsigned char *ptr2 = reinterpret_cast<unsigned char *>(tmp);
		std::copy(ptr,end,ptr2);
	}
	else if (a.type()==typeid(vector<unsigned char>*)) // from Type to binary stream
	{ CHK_BIN();
		vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
		Type * tmp = any_cast<Type*>(ac.getAddress());
		(*tmpBin).clear();
		const unsigned char* ptr = reinterpret_cast<const unsigned char*>(tmp);
		const unsigned char* end = ptr + sizeof(Type);
		(*tmpBin).resize(sizeof(Type));
		std::copy(ptr, end, (*tmpBin).begin() );
	}
	else
		cerr<<"lexical_cast(BIN): (de)serialization format mismatch"<<endl;
	}
	else // never reached
		throw HandlerError(SerializationExceptions::LexicalCopyError);
}

template< >
struct FundamentalHandler< int >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< int >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< int >( ac, a);
	}
};

template< >
struct FundamentalHandler< float >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< float >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< float >( ac, a);
	}
};

template< >
struct FundamentalHandler< double >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< double >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< double >( ac, a);
	}
};

// bool should accept true,false and 1,0
template< >
struct FundamentalHandler< bool >
{
	static void creator(Archive& ac, any& a)
	{

		lexical_copy< bool >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< bool >( ac, a);
	}
};

template< >
struct FundamentalHandler< unsigned int >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< unsigned int >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< unsigned int >( ac, a);
	}
};

template< >
struct FundamentalHandler< char >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< char >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< char >( ac, a);
	}
};

template< >
struct FundamentalHandler< unsigned char >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< unsigned char >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< unsigned char >( ac, a);
	}
};

template< >
struct FundamentalHandler< short >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< short >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< short >( ac, a);
	}
};

template< >
struct FundamentalHandler< long >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< long >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< long >( ac, a);
	}
};

template< >
struct FundamentalHandler< unsigned short >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< unsigned short >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< unsigned short >( ac, a);
	}
};

template< >
struct FundamentalHandler< unsigned long >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< unsigned long >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< unsigned long >( ac, a);
	}
};

template< >
struct FundamentalHandler< long double >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< long double >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< long double >( ac, a);
	}
};

template< >
struct FundamentalHandler< long long >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< long long >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< long long >( ac, a);
	}
};

#endif // __FUNDAMENTALHANDLER_H__

