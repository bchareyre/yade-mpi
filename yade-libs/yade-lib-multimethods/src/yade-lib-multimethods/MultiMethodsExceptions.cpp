/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "MultiMethodsExceptions.hpp"


const char* MultiMethodsExceptions::NotExistingClass	= "DynLibDispatcher::add - cannot find class (or load a library) named : ";
const char* MultiMethodsExceptions::UndefinedOrder	= "FunctorWrapper::checkOrder - unable to determine order in functor named : ";
const char* MultiMethodsExceptions::BadVirtualCall	= "FunctorWrapper::go/goReverse - called function was not overloaded in dynamic library. check if your argument types are correct, remember that only fundamental types and pure pointers are passed by value, all other types (including shared_ptr<>) are passed by reference, for details look into Loki::TypeTraits.hpp::ParametrType.";

