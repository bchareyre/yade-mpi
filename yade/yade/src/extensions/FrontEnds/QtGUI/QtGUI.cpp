/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "QtGUI.hpp"
#include "YadeQtMainWindow.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

//#ifdef Q_WS_X11
	#include <X11/Xlib.h>
//#endif
    
#include <qapplication.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtGUI::QtGUI ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtGUI::~QtGUI()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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
			case 'g'	: Omega::instance().setGravity
						(Vector3r(0,-lexical_cast<Real>(optarg),0));	break;
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
