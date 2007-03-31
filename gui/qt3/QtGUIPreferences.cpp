/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "QtGUIPreferences.hpp"

QtGUIPreferences::QtGUIPreferences ()
{
	version = 1;
}


void QtGUIPreferences::registerAttributes()
{
	REGISTER_ATTRIBUTE(version);
	REGISTER_ATTRIBUTE(mainWindowPositionX);
	REGISTER_ATTRIBUTE(mainWindowPositionY);
	REGISTER_ATTRIBUTE(mainWindowSizeX);
	REGISTER_ATTRIBUTE(mainWindowSizeY);
}

