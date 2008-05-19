/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "QtGUI.hpp"
#include "YadeQtMainWindow.hpp"
#include<boost/algorithm/string.hpp>
//#ifdef HAVE_CONFIG_H
//	#include <config.h>
//#endif
//#ifdef Q_WS_X11
	#include <X11/Xlib.h>
//#endif
#include <qapplication.h>

#ifdef EMBED_PYTHON
	#include<yade/gui-py/PythonUI.hpp>
	#include<boost/thread.hpp>
#endif

QtGUI::QtGUI(){}
QtGUI::~QtGUI(){}

CREATE_LOGGER(QtGUI);

int QtGUI::run(int argc, char *argv[])
{
//	#ifdef Q_WS_X11
		XInitThreads();
//	#endif

	// FIXME - QtGUI is consuming CPU when idle - when computations are NOT running.

	int ch;
	while( ( ch = getopt(argc,argv,"t:g:") ) != -1)
		switch(ch){
//			case 'H'	: help(); 						return 1;
			case 't'	: Omega::instance().setTimeStep(lexical_cast<Real>(optarg)); break;
			default: break;	
		}
	if(optind<argc){ // process non-option arguments
		if(boost::algorithm::ends_with(string(argv[optind]),string(".xml"))) Omega::instance().setSimulationFileName(string(argv[optind]));
		else optind--;
	}
	for (int index=optind+1; index<argc; index++) LOG_ERROR("Unprocessed non-option argument: `"<<argv[index]<<"'");
	
	

	
   QApplication app(argc,argv);
	mainWindow=new YadeQtMainWindow();
	mainWindow->show();
	app.setMainWidget(mainWindow);

	#ifdef EMBED_PYTHON
		LOG_INFO("Launching Python thread now...");
		//PyEval_InitThreads();
		boost::thread pyThread(boost::function0<void>(&PythonUI::pythonSession));
	#endif

	int res =  app.exec();

	delete mainWindow;
	return res;
}

