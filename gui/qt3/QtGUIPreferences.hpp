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

class QtGUIPreferences : public Serializable
{
	public :
		int version;
		int mainWindowPositionX,mainWindowPositionY;	
		int mainWindowSizeX,mainWindowSizeY;
		QtGUIPreferences ();

/// Serialization										///
	REGISTER_ATTRIBUTES(Serializable,(version)(mainWindowPositionX)(mainWindowPositionY)(mainWindowSizeX)(mainWindowSizeY));
	REGISTER_CLASS_AND_BASE(QtGUIPreferences,Serializable);
};
REGISTER_SERIALIZABLE(QtGUIPreferences);


