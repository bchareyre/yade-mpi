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


//FIXME : rename to exceptionhandling(er) ??

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __EXECPTIONMESSAGES__
#define __EXECPTIONMESSAGES__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///  SerializableError                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////////

struct SerializableError : public std::runtime_error
{
	explicit SerializableError(const char* msg) : std::runtime_error(msg) {};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct HandlerError : public SerializableError
{
	explicit HandlerError(const char* msg) : SerializableError(msg) {};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct SerializableUnregisterError : public SerializableError
{
	explicit SerializableUnregisterError(const char* msg) : SerializableError(msg) {};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///  IOManagerError                                                                              //
///////////////////////////////////////////////////////////////////////////////////////////////////

struct IOManagerError : public std::runtime_error
{
	explicit IOManagerError(const char* msg) : std::runtime_error(msg) {};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///  FactoryError                                                                                //
///////////////////////////////////////////////////////////////////////////////////////////////////

struct FactoryError : public std::runtime_error
{
	explicit FactoryError(const char* msg) : std::runtime_error(msg) {};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct FactoryCantCreate : public FactoryError
{
	explicit FactoryCantCreate(const char* msg) : FactoryError(msg) {};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct FactoryClassNotRegistered : public FactoryError
{
	explicit FactoryClassNotRegistered(const char* msg) : FactoryError(msg) {};
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

struct ExceptionMessages
{
	static const char* SerializableUnknown;
	static const char* SerializableUnknown2;
	static const char* ExtraAttribute;
	static const char* ArchiveTypeNotSet;
	static const char* UnknownFundamental;
	static const char* WrongFileHeader;
	static const char* WrongFileFooter;
	static const char* WrongExpectedClassName;
	static const char* SetFunctionNotDeclared;
	static const char* GetFunctionNotDeclared;
	static const char* CantCreateClass;
	static const char* ClassNotRegistered;
	static const char* UnsupportedArchiveType;
	static const char* CustomClassNotRegistered;
	static const char* AttributeNotFound;
	static const char* AttributeIsNotDefined;
	static const char* LexicalCopyError;
	static const char* FundamentalNotSupported;
	static const char* SmartPointerNotSupported;
	static const char* ContainerNotSupported;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif //  __EXECPTIONMESSAGES__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
