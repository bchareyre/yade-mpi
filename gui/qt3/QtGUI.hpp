/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QTGUI_HPP
#define QTGUI_HPP

#include<yade/core/FrontEnd.hpp>

class YadeQtMainWindow;

class QtGUI : public FrontEnd
{
	private :
		YadeQtMainWindow  * mainWindow;

	public :
		QtGUI ();
		virtual ~QtGUI ();
		virtual int run(int argc, char *argv[]);
	
	REGISTER_CLASS_NAME(QtGUI);
	REGISTER_BASE_CLASS_NAME(FrontEnd);
};

REGISTER_FACTORABLE(QtGUI);

#endif // QTGUI_HPP

