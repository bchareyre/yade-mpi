/***************************************************************************
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

#include <yade-lib-multimethods/MultiMethodsExceptions.hpp>


const char* MultiMethodsExceptions::NotExistingClass	= "DynLibDispatcher::add - cannot find class (or load a library) named : ";
const char* MultiMethodsExceptions::UndefinedOrder	= "FunctorWrapper::checkOrder - unable to determine order in functor named : ";
const char* MultiMethodsExceptions::BadVirtualCall	= "FunctorWrapper::go/goReverse - called function was not overloaded in dynamic library. check if your argument types are correct, remember that only fundamental types and pure pointers are passed by value, all other types (including shared_ptr<>) are passed by reference, for details look into Loki::TypeTraits.hpp::ParametrType.";
