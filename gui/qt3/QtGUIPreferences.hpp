/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QTGUIPREFERENCES_HPP
#define QTGUIPREFERENCES_HPP

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
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(QtGUIPreferences);
	REGISTER_BASE_CLASS_NAME(Serializable);

};

REGISTER_SERIALIZABLE(QtGUIPreferences);

#endif // QTGUIPREFERENCES_HPP

