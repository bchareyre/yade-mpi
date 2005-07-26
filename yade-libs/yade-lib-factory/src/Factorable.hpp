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

#ifndef __FACTORABLE__
#define __FACTORABLE__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ClassFactory.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define REGISTER_CLASS_NAME(cn)								\
	public : virtual string getClassName() const { return #cn; };

#define REGISTER_BASE_CLASS_NAME(bcn)							\
	public : virtual string getBaseClassName(unsigned int i=0) const		\
	{										\
		string token;								\
		vector<string> tokens;							\
		string str=#bcn;							\
		istringstream iss(str);							\
		iss >> token;								\
		while (!iss.eof())							\
		{									\
			tokens.push_back(token);					\
			iss >> token;							\
		}									\
		if (i>=token.size())							\
			return "";							\
		else									\
			return tokens[i];						\
	}										\
	public : virtual int getBaseClassNumber()		 			\
	{										\
		string token;								\
		vector<string> tokens;							\
		string str=#bcn;							\
		istringstream iss(str);							\
		iss >> token;								\
		while (!iss.eof())							\
		{									\
			tokens.push_back(token);					\
			iss >> token;							\
		}									\
		return tokens.size();							\
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Factorable
{
	public    : Factorable();
	public    : virtual ~Factorable();

// FIXME - virtual function to return version, long and short description, OR
//         maybe just a file with the same name as class with description inside
//	public    : virtual std::string getVersion();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __FACTORABLE__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


