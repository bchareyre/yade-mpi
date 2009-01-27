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

struct yadeError : public std::runtime_error
{
	explicit yadeError(const char* msg) : std::runtime_error(msg) {};
};

struct yadeBadFile : public yadeError
{
	explicit yadeBadFile(const char* msg) : yadeError(msg) {};
};

struct yadeExceptions
{
	static const char* BadFile;
};


