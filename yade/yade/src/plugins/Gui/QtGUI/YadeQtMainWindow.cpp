#include "YadeQtMainWindow.hpp"

#include <yade-lib-serialization/IOManager.hpp>

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


using namespace std;

YadeQtMainWindow::YadeQtMainWindow() : YadeQtGeneratedMainWindow()
{

	resize(1024,768);

	addMenu("Edit");
	addMenu("Preprocessor");
	addMenu("Postprocessor");

	addItem("Edit","Preferences...","QtPreferencesEditor");
		
	addItem("Preprocessor","File Generator...","QtFileGenerator");
	addItem("Preprocessor","Engine Editor...","QtEngineEditor");
	addItem("Preprocessor","Code Generator...","QtCodeGenerator");

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

}

void YadeQtMainWindow::addMenu(string menuName)
{
	map<string,QPopupMenu*>::iterator mi = menus.find(menuName);
	if (mi==menus.end())
		menus[menuName] = new QPopupMenu( this );

}

void YadeQtMainWindow::addItem(string menuName, string itemName,string className)
{
	map<string,QPopupMenu*>::iterator mi = menus.find(menuName);
	if (mi!=menus.end())
	{
		item2ClassName[itemName] = className;
		items.push_back(new QAction(this, itemName.c_str()));
		items.back()->setText( itemName.c_str() );
		items.back()->setMenuText( itemName.c_str() );
		items.back()->setToolTip( ("Load library "+ClassFactory::instance().libNameToSystemName(className)).c_str() );
		items.back()->addTo((*mi).second);
		connect( items.back(), SIGNAL( activated() ), this, SLOT( dynamicMenuClicked() ) );
	}
}

void YadeQtMainWindow::createMenus()
{
	map<string,QPopupMenu*>::reverse_iterator mi    = menus.rbegin();
	map<string,QPopupMenu*>::reverse_iterator miEnd = menus.rend();
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
	fileNewSimulationAction->setEnabled(true);
}

void YadeQtMainWindow::fileExit()
{
	emit close();
}

void YadeQtMainWindow::closeEvent(QCloseEvent *e)
{
	if (simulationController)
		delete simulationController;
	YadeQtGeneratedMainWindow::closeEvent(e);
}
