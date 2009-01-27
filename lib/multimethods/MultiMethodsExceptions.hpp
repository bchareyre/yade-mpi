/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <string>
#include <stdexcept>

using namespace std;

struct MultiMethodsError : public std::runtime_error
{
	explicit MultiMethodsError(const char* msg) : std::runtime_error(msg) {};
};

struct MultiMethodsUndefinedOrder : public MultiMethodsError
{
	explicit MultiMethodsUndefinedOrder(const char* msg) : MultiMethodsError(msg) {};
};

struct MultiMethodsNotExistingClass : public MultiMethodsError
{
	explicit MultiMethodsNotExistingClass(const char* msg) : MultiMethodsError(msg) {};
};

struct MultiMethodsBadVirtualCall : public MultiMethodsError
{
	explicit MultiMethodsBadVirtualCall(const char* msg) : MultiMethodsError(msg) {};
};

struct MultiMethodsExceptions
{
	static const char* NotExistingClass;
	static const char* UndefinedOrder;
	static const char* BadVirtualCall;
};


