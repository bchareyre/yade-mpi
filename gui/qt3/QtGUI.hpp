/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/FrontEnd.hpp>

class YadeQtMainWindow;
class SimulationController;
class GLViewer;
class QApplication;

class QtGUI : public FrontEnd
{
	private :
		YadeQtMainWindow  * mainWindow;
		// run qtApp in separate thread, without parsing args and launching python

	public:	
		static QApplication* app;
		static QtGUI *self;
		bool mainWindowHidden;

	public :
		QtGUI ();
		// run the QtGUI without showing the main window (called from python when importing yade.qt; yade.qt.Controller() will then show just the Controller and so on)
		bool runNaked();
		// try to open the X display, return false if fails; optionally display error to the log.
		bool checkDisplay(bool quiet=true);
		virtual bool available(){return checkDisplay();}
		virtual ~QtGUI ();
		virtual int run(int argc, char *argv[]);
		void help();
		DECLARE_LOGGER;

	REGISTER_CLASS_NAME(QtGUI);
	REGISTER_BASE_CLASS_NAME(FrontEnd);
};

REGISTER_FACTORABLE(QtGUI);


