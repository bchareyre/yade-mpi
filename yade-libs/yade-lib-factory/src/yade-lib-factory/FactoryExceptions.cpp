/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FactoryExceptions.hpp"


const char* FactoryExceptions::CantCreateClass   		= "ClassFactory::create - cannot create class named: ";
const char* FactoryExceptions::ClassNotRegistered   		= "ClassFactory::create - library was loaded, but it didn't register itself - so cannot create instance: ";

