/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   galizzi@stalactite                                                    *
 *   Copyright (C) 2004 by Janek Kozicki                                   *
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

#include "SerializationExceptions.hpp"


const char* SerializationExceptions::SerializableUnknown 		= "UNKNOWN TYPE, YOU MAY CALLED THE WRONG MACRO";
const char* SerializationExceptions::SerializableUnknown2 		= "Cannot determine type with findType()";
const char* SerializationExceptions::SetFunctionNotDeclared   		= "Set function not declared";
const char* SerializationExceptions::GetFunctionNotDeclared  	 	= "Get function not declared";
const char* SerializationExceptions::ContainerNotSupported    		= "Container not supported";
const char* SerializationExceptions::LexicalCopyError			= "lexical_copy fails";
const char* SerializationExceptions::FundamentalNotSupported		= "Fundamental not supported";
const char* SerializationExceptions::SmartPointerNotSupported		= "SmartPointer not supported";

// FIXME : used , but desn't throw
const char* SerializationExceptions::AttributeIsNotDefined 		= "Attribute is not defined : ";
// FIXME : this is not used ! should be used, because this exception is very useful!
const char* SerializationExceptions::ExtraAttribute      		= "An extra attribute has been defined, cannot deserialize it.";
// FIXME : this also is not used, and what type is it? BIN/TXT or what?
const char* SerializationExceptions::ArchiveTypeNotSet   		= "plese SET ARCHIVE TYPE!";
// FIXME : this is not used too
const char* SerializationExceptions::UnknownFundamental  		= "UNKNOWN FUNDAMENTAL TYPE";
// FIXME and this..
const char* SerializationExceptions::WrongExpectedClassName 		= "Wrong expected class name";
// FIXME and this
const char* SerializationExceptions::UnsupportedArchiveType 		= "Unsupported archive type";
// FIXME and this
const char* SerializationExceptions::CustomClassNotRegistered 		= "Custom class not registered";
