/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <iostream>
#include <vector>

#include<yade/lib-serialization/Serializable.hpp>

using namespace std;

class Preferences : public Serializable
{
	public :
		int version;	
		vector<string>	 dynlibDirectories;

		string		 defaultGUILibName;
		Preferences ();

/// Serialization										///
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(Preferences);
	REGISTER_BASE_CLASS_NAME(Serializable);
};

REGISTER_SERIALIZABLE(Preferences);

#endif // PREFERENCES_HPP

