/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <string>
#include <stdexcept>

using namespace std;

struct SerializableError : public std::runtime_error
{
	explicit SerializableError(const char* msg) : std::runtime_error(msg) {};
};

struct HandlerError : public SerializableError
{
	explicit HandlerError(const char* msg) : SerializableError(msg) {};
};

struct SerializableUnregisterError : public SerializableError
{
	explicit SerializableUnregisterError(const char* msg) : SerializableError(msg) {};
};

struct SerializationExceptions
{
	static const char* SerializableUnknown;
	static const char* SerializableUnknown2;
	static const char* ExtraAttribute;
	static const char* ArchiveTypeNotSet;
	static const char* UnknownFundamental;
	static const char* WrongExpectedClassName;
	static const char* SetFunctionNotDeclared;
	static const char* GetFunctionNotDeclared;
	static const char* UnsupportedArchiveType;
	static const char* CustomClassNotRegistered;
	static const char* AttributeIsNotDefined;
	static const char* LexicalCopyError;
	static const char* FundamentalNotSupported;
	static const char* SmartPointerNotSupported;
	static const char* ContainerNotSupported;
	static const char* LexicalCopyBinError;
	static const char* ExtraCopyError;
};


