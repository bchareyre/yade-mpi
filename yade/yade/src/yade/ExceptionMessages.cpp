/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   galizzi@stalactite                                                    *
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

#include "ExceptionMessages.hpp"


const char* ExceptionMessages::SerializableUnknown 		= "UNKNOWN TYPE, YOU MAY CALLED THE WRONG MACRO";
const char* ExceptionMessages::SerializableUnknown2 		= "Cannot determine type with findType()";
const char* ExceptionMessages::ExtraAttribute      		= "An extra attribute has been defined, cannot deserialize it.";
const char* ExceptionMessages::ArchiveTypeNotSet   		= "SET ARCHIVE TYPE!!";
const char* ExceptionMessages::UnknownFundamental  		= "UNKNOWN FUNDAMENTAL TYPE";
const char* ExceptionMessages::WrongFileHeader   		= "Wrong File Header";
const char* ExceptionMessages::WrongFileFooter   		= "Wrong File Footer";
const char* ExceptionMessages::WrongExpectedClassName 		= "Wrong expected class name";
const char* ExceptionMessages::SetFunctionNotDeclared   	= "Set function not declared";
const char* ExceptionMessages::GetFunctionNotDeclared   	= "Get function not declared";
const char* ExceptionMessages::CantCreateClass   		= "ClassFactory::create - cannot create class named: ";
const char* ExceptionMessages::ClassNotRegistered   		= "ClassFactory::create - library was loaded, but it didn't register itself - so cannot create instance: ";
const char* ExceptionMessages::UnsupportedArchiveType 		= "Unsupported archive type";
const char* ExceptionMessages::CustomClassNotRegistered 	= "Custom class not registered";
const char* ExceptionMessages::ContainerNotSupported    	= "Container not supported";
const char* ExceptionMessages::AttributeNotFound 		= "Attribute not found";
const char* ExceptionMessages::AttributeIsNotDefined 		= "Attribute is not defined : ";
const char* ExceptionMessages::LexicalCopyError			= "lexical_copy fails";
const char* ExceptionMessages::FundamentalNotSupported		= "Fundamental not supported";
const char* ExceptionMessages::SmartPointerNotSupported		= "SmartPointer not supported";
