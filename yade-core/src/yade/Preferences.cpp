/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Preferences.hpp"


Preferences::Preferences ()
{
	version = 1;
}


void Preferences::registerAttributes()
{
	REGISTER_ATTRIBUTE(version);
	REGISTER_ATTRIBUTE(dynlibDirectories);
	REGISTER_ATTRIBUTE(includeDirectories);
	REGISTER_ATTRIBUTE(defaultGUILibName);
}

