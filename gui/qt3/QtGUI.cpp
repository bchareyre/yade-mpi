/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "QtGUI.hpp"
#include "YadeQtMainWindow.hpp"
//#ifdef HAVE_CONFIG_H
//	#include <config.h>
//#endif
//#ifdef Q_WS_X11
	#include <X11/Xlib.h>
//#endif
#include <qapplication.h>

QtGUI::QtGUI ()
{

}


QtGUI::~QtGUI()
{

}


int QtGUI::run(int argc, char *argv[])
{
//	#ifdef Q_WS_X11
		XInitThreads();
//	#endif

	// FIXME - QtGUI is consuming CPU when idle - when computations are NOT running.

	opterr = 0;
	int ch;
	while( ( ch = getopt(argc,argv,"t:g:") ) != -1)
		switch(ch)
		{
//			case 'H'	: help(); 						return 1;
			case 't'	: Omega::instance().setTimeStep
						(lexical_cast<Real>(optarg));			break;
//			case 'g'	: Omega::instance().setGravity
//						(Vector3r(0,-lexical_cast<Real>(optarg),0));	break;
			default		: break;
		}

	
    	QApplication app( argc,argv );

	mainWindow = new YadeQtMainWindow();

	mainWindow->show();

	app.setMainWidget( mainWindow );
	
	int res =  app.exec();
	
	delete mainWindow;
	
	return res;
}

