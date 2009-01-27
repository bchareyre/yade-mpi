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
class RenderingEngine;

class QtGUI : public FrontEnd
{
	private :
		YadeQtMainWindow  * mainWindow;

	public:	
		static QtGUI *self;
		bool mainWindowHidden;

	public :
		QtGUI ();
		virtual ~QtGUI ();
		virtual int run(int argc, char *argv[]);
		void help();
		DECLARE_LOGGER;

	REGISTER_CLASS_NAME(QtGUI);
	REGISTER_BASE_CLASS_NAME(FrontEnd);
};

REGISTER_FACTORABLE(QtGUI);


