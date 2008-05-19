/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "YadeQtMainWindow.hpp"
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/lib-factory/ClassFactory.hpp>
#include <qvbox.h>
#include <qfiledialog.h>
#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

using namespace std;

CREATE_LOGGER(YadeQtMainWindow);

YadeQtMainWindow* YadeQtMainWindow::self=NULL;

YadeQtMainWindow::YadeQtMainWindow() : YadeQtGeneratedMainWindow()
{
	self=this;

	preferences = shared_ptr<QtGUIPreferences>(new QtGUIPreferences);
	filesystem::path yadeQtGUIPrefPath = filesystem::path( Omega::instance().yadeConfigPath + "/QtGUIPreferences.xml", filesystem::native);

	if ( !filesystem::exists( yadeQtGUIPrefPath ) )
	{
		preferences->mainWindowPositionX	= 50;
		preferences->mainWindowPositionY	= 50;
		#ifdef USE_WORKSPACE
			preferences->mainWindowSizeX		= 1024; preferences->mainWindowSizeY		= 768;
		#else
			preferences->mainWindowSizeX		= 150; preferences->mainWindowSizeY		= 150;
		#endif	
		IOFormatManager::saveToFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	}

	try
	{
		LOG_INFO("Loading configuration file: "<<yadeQtGUIPrefPath.string()<<".")
		IOFormatManager::loadFromFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	}
	catch(SerializableError&)
	{
		preferences->mainWindowPositionX	= 50;
		preferences->mainWindowPositionY	= 50;
		#ifdef USE_WORKSPACE
			preferences->mainWindowSizeX		= 1024; preferences->mainWindowSizeY		= 768;
		#else
			preferences->mainWindowSizeX		= 150; preferences->mainWindowSizeY		= 150;
		#endif	
		IOFormatManager::saveToFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	}

	resize(preferences->mainWindowSizeX,preferences->mainWindowSizeY);
	move(preferences->mainWindowPositionX,preferences->mainWindowPositionY);
	
	//addMenu("Edit");
	//addMenu("Preprocessor");
	//addMenu("Postprocessor");
	addMenu("Actions");
		
	addItem("Actions","File Generator","QtFileGenerator");
	addItem("Actions","Simulation Player","QtSimulationPlayer");

	createMenus();

	QVBox *vbox = new QVBox( this );
	vbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	vbox->setMargin( 2 );
	vbox->setLineWidth( 2 );
	#ifdef USE_WORKSPACE
		workspace = new QWorkspace( vbox );
		workspace->setBackgroundMode( PaletteMid );
		setCentralWidget( vbox );
	#endif

	simulationController = 0;
	
	// HACK
	if(Omega::instance().getSimulationFileName()!="") fileNewSimulation();
}


YadeQtMainWindow::~YadeQtMainWindow()
{
	filesystem::path yadeQtGUIPrefPath = filesystem::path( Omega::instance().yadeConfigPath + "/QtGUIPreferences.xml", filesystem::native);

	preferences->mainWindowPositionX	= pos().x();
	preferences->mainWindowPositionY	= pos().y();
	preferences->mainWindowSizeX		= size().width();
	preferences->mainWindowSizeY		= size().height();
	IOFormatManager::saveToFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
}


void YadeQtMainWindow::addMenu(string menuName)
{
	if (!menuNameExists(menuName))
		menus.push_back(pair<string,QPopupMenu*>(menuName,new QPopupMenu(this)));

}


void YadeQtMainWindow::addItem(string menuName, string itemName,string className)
{
	if (menuNameExists(menuName))
	{
		item2ClassName[itemName] = className;
		items.push_back(new QAction(this, itemName.c_str()));
		items.back()->setText( itemName.c_str() );
		items.back()->setMenuText( itemName.c_str() );
		items.back()->setToolTip( ("Load plugin "+ClassFactory::instance().libNameToSystemName(className)).c_str() );
		items.back()->addTo(getPopupMenu(menuName));
		connect( items.back(), SIGNAL( activated() ), this, SLOT( dynamicMenuClicked() ) );
	}
}


void YadeQtMainWindow::createMenus()
{
	vector<pair<string,QPopupMenu*> >::iterator mi    = menus.begin();
	vector<pair<string,QPopupMenu*> >::iterator miEnd = menus.end();
	for(;mi!=miEnd;++mi)
		MenuBar->insertItem( QString((*mi).first), (*mi).second);

}


void YadeQtMainWindow::fileNewSimulation()
{
	if(!simulationController) simulationController =
	#ifdef USE_WORKSPACE
		new SimulationController(workspace);
	#else
		new SimulationController(NULL);
	#endif
	simulationController->show();
	connect( simulationController, SIGNAL( closeSignal() ), this, SLOT( closeSimulationControllerEvent() ) );
	fileNewSimulationAction->setEnabled(false);
}


void YadeQtMainWindow::dynamicMenuClicked()
{
	QAction * action = (QAction*)(this->sender());
	string name = action->text();
	qtWidgets.push_back(ClassFactory::instance().createShared(item2ClassName[name]));

	shared_ptr<QWidget> widget = dynamic_pointer_cast<QWidget>(qtWidgets.back());
	if (widget) // the library is a QWidget so we set workspace as its parent
	{
		#ifdef USE_WORKSPACE
			widget->reparent(workspace,QPoint(10,10));
		#else
			//widget->reparent(this,QPoint(10,10));
		#endif
		widget->show();
	}
}


void YadeQtMainWindow::closeSimulationControllerEvent()
{
	delete simulationController;
	simulationController = 0;
	fileNewSimulationAction->setEnabled(true);
	Omega::instance().setSimulationFileName("");
}


void YadeQtMainWindow::fileExit()
{
	emit close();
}


void YadeQtMainWindow::closeEvent(QCloseEvent *e)
{
	if (simulationController)
	{
		delete simulationController;
		simulationController = 0;
	}
	YadeQtGeneratedMainWindow::closeEvent(e);
}


bool YadeQtMainWindow::menuNameExists(const string name)
{
	vector<pair<string,QPopupMenu*> >::iterator mi    = menus.begin();
	vector<pair<string,QPopupMenu*> >::iterator miEnd = menus.end();
	for( ; mi!=miEnd ; ++mi)
		if ((*mi).first==name)
			return true;
	return false;
}


QPopupMenu* YadeQtMainWindow::getPopupMenu(const string name)
{
	vector<pair<string,QPopupMenu*> >::iterator mi    = menus.begin();
	vector<pair<string,QPopupMenu*> >::iterator miEnd = menus.end();
	for( ; mi!=miEnd ; ++mi)
		if ((*mi).first==name)
			return (*mi).second;
	return 0;
}


