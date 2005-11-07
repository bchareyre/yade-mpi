/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FACTORYEXECPTIONS_HPP
#define FACTORYEXECPTIONS_HPP

#include <string>
#include <stdexcept>

using namespace std;

struct FactoryError : public std::runtime_error
{
	explicit FactoryError(const char* msg) : std::runtime_error(msg) {};
};

struct FactoryCantCreate : public FactoryError
{
	explicit FactoryCantCreate(const char* msg) : FactoryError(msg) {};
};

struct FactoryClassNotRegistered : public FactoryError
{
	explicit FactoryClassNotRegistered(const char* msg) : FactoryError(msg) {};
};

struct FactoryExceptions
{
	static const char* CantCreateClass;
	static const char* ClassNotRegistered;
};

#endif // FACTORYEXECPTIONS_HPP

