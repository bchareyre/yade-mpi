/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <iostream>
#include <vector>

#include<yade/lib-serialization/Serializable.hpp>

using namespace std;

class Preferences : public Serializable{
	public :
		string		 defaultGUILibName;
		Preferences ();
	REGISTER_ATTRIBUTES(Serializable,(defaultGUILibName));
	REGISTER_CLASS_AND_BASE(Preferences,Serializable);
};
REGISTER_SERIALIZABLE(Preferences);


