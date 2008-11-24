/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"QtGUI.hpp"
#include"YadeQtMainWindow.hpp"
#include"GLViewer.hpp"
#include<boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
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

QtGUI* QtGUI::self=NULL;

QtGUI::QtGUI(){self=this; mainWindowHidden=false;}
QtGUI::~QtGUI(){}

CREATE_LOGGER(QtGUI);

void QtGUI::help(){
	cerr<<"Qt3 GUI\n=======\n\t-h to get this help\n"<<endl;
}

int QtGUI::run(int argc, char *argv[])
{
	int ch;
	while( ( ch = getopt(argc,argv,"hw") ) != -1)
		switch(ch){
			case 'w'	: mainWindowHidden=true; break;
			case 'h' : help(); return 0; break;
			default: break;	
		}
	if(optind<argc){ // process non-option arguments
		if(boost::algorithm::ends_with(string(argv[optind]),string(".xml")) ||
			boost::algorithm::ends_with(string(argv[optind]),string(".xml.gz")) ||
			boost::algorithm::ends_with(string(argv[optind]),string(".xml.bz2"))) Omega::instance().setSimulationFileName(string(argv[optind]));
		#ifdef EMBED_PYTHON
		else if(boost::algorithm::ends_with(string(argv[optind]),string(".py"))) PythonUI::runScript=string(argv[optind]);
		#endif
		else optind--;
	}
	for (int index=optind+1; index<argc; index++) LOG_ERROR("Unprocessed non-option argument: `"<<argv[index]<<"'");

	XInitThreads();
   QApplication app(argc,argv);
	mainWindow=new YadeQtMainWindow();
	mainWindow->show();
	app.setMainWidget(mainWindow);
	#ifdef EMBED_PYTHON
		LOG_DEBUG("Launching Python thread now...");
		//PyEval_InitThreads();
		boost::thread pyThread(boost::function0<void>(&PythonUI::pythonSession));
	#endif
	int res =  app.exec();

	delete mainWindow;
	return res;
}

