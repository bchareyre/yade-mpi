/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "YadeQtMainWindow.hpp"
#include <yade/yade-lib-serialization/IOFormatManager.hpp>
#include <yade/yade-lib-factory/ClassFactory.hpp>
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


YadeQtMainWindow::YadeQtMainWindow() : YadeQtGeneratedMainWindow()
{

	preferences = shared_ptr<QtGUIPreferences>(new QtGUIPreferences);
	filesystem::path yadeQtGUIPrefPath = filesystem::path( Omega::instance().yadeConfigPath + "/QtGUIPreferences.xml", filesystem::native);

	if ( !filesystem::exists( yadeQtGUIPrefPath ) )
	{
		preferences->mainWindowPositionX	= 50;
		preferences->mainWindowPositionY	= 50;
		preferences->mainWindowSizeX		= 1024;
		preferences->mainWindowSizeY		= 768;
		IOFormatManager::saveToFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	}

	try
	{
		cerr << "loading configuration file: " << yadeQtGUIPrefPath.string() << "\n";
		IOFormatManager::loadFromFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	}
	catch(SerializableError&)
	{
		preferences->mainWindowPositionX	= 50;
		preferences->mainWindowPositionY	= 50;
		preferences->mainWindowSizeX		= 1024;
		preferences->mainWindowSizeY		= 768;
		IOFormatManager::saveToFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	}

	resize(preferences->mainWindowSizeX,preferences->mainWindowSizeY);
	move(preferences->mainWindowPositionX,preferences->mainWindowPositionY);
	
	addMenu("Edit");
	addMenu("Preprocessor");
	addMenu("Postprocessor");
	addMenu("Extra");

	addItem("Edit","Preferences...","QtPreferencesEditor");
		
	addItem("Preprocessor","File Generator...","QtFileGenerator");
	//addItem("Preprocessor","Engine Editor...","QtEngineEditor");
	//addItem("Preprocessor","Code Generator...","QtCodeGenerator");

	addItem("Postprocessor","Simulation Player...","QtSimulationPlayer");

	addItem("Extra","Spherical DEM Simulator...","QtSphericalDEM");

	createMenus();

	QVBox *vbox = new QVBox( this );
	vbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	vbox->setMargin( 2 );
	vbox->setLineWidth( 2 );

	workspace = new QWorkspace( vbox );
	workspace->setBackgroundMode( PaletteMid );
	setCentralWidget( vbox );
	
	simulationController = 0;
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
		items.back()->setToolTip( ("Load library "+ClassFactory::instance().libNameToSystemName(className)).c_str() );
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
	simulationController = new SimulationController(workspace);
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
		widget->reparent(workspace,QPoint(10,10));
		widget->show();
	}
}


void YadeQtMainWindow::closeSimulationControllerEvent()
{
	delete simulationController;
	simulationController = 0;
	fileNewSimulationAction->setEnabled(true);
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


