/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *   Copyright (C) 2005 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#ifndef __IOMANAGEREXCEPTIONS__
#define __IOMANAGEREXCEPTIONS__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///  IOManagerError                                                                              //
///////////////////////////////////////////////////////////////////////////////////////////////////

struct IOManagerError : public std::runtime_error
{
	explicit IOManagerError(const char* msg) : std::runtime_error(msg) {};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///  CustomTypesMapperError                                                                      //
///////////////////////////////////////////////////////////////////////////////////////////////////

struct CustomTypesMapperError : public std::runtime_error
{
	explicit CustomTypesMapperError(const char* msg) : std::runtime_error(msg) {};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct IOManagerExceptions
{
	static const char* WrongFileHeader;
	static const char* WrongFileFooter;
	static const char* AttributeNotFound;
	static const char* BadAttributeValue;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif //  __IOMANAGEREXCEPTIONS__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
