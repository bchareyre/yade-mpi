/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef IOMANAGEREXCEPTIONS_HPP
#define IOMANAGEREXCEPTIONS_HPP

#include <string>
#include <stdexcept>

using namespace std;

/*
struct IOManagerError : public std::runtime_error
{
	explicit IOManagerError(const char* msg) : std::runtime_error(msg) {};
};

struct CustomTypesMapperError : public std::runtime_error
{
	explicit CustomTypesMapperError(const char* msg) : std::runtime_error(msg) {};
};
*/

struct IOManagerExceptions
{
	static const char* WrongFileHeader;
	static const char* WrongFileFooter;
	static const char* AttributeNotFound;
	static const char* BadAttributeValue;
};

#endif //  IOMANAGEREXCEPTIONS_HPP

