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

#ifndef __FUNDAMENTALHANDLER_H__
#define __FUNDAMENTALHANDLER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SerializationExceptions.hpp"
#include "Archive.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Type >
inline void lexical_copy(Archive& ac , any& a )
{
	if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
	{
		const string * tmpStr = any_cast<const string*>(a);
		Type * tmp = any_cast<Type*>(ac.getAddress());
		*tmp = lexical_cast<Type>(*tmpStr);
	}
	else if (a.type()==typeid(string*)) // serialization - writing to string from some Type
	{
		string * tmpStr = any_cast<string*>(a);
		Type * tmp = any_cast<Type*>(ac.getAddress());
		*tmpStr = lexical_cast<string>(*tmp);
	}
	else if (a.type()==typeid(const vector<unsigned char>*))
	{
	}
	else if (a.type()==typeid(vector<unsigned char>*))
	{
	}
	else
		throw HandlerError(SerializationExceptions::LexicalCopyError);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template< >
struct FundamentalHandler< string >
{
	static void creator(Archive& ac, any& a)
	{
		lexical_copy< string >( ac, a);
	}
	static void accessor(Archive& ac, any& a)
	{
		lexical_copy< string >( ac, a);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __FUNDAMENTALHANDLER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
