#include "YadeQtMainWindow.hpp"

#include "IOManager.hpp"


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

	addMenu("Preprocessor");
	addMenu("Postprocessor");
 	addItem("Preprocessor","HangingCloth");
	addItem("Preprocessor","RotatingBox");
//	addItem("Preprocessor","FEMRock"); // not working
//	addItem("Preprocessor","BoxStack"); // not working
	addItem("Preprocessor","SDECSpheresPlane");
	addItem("Preprocessor","SDECLinkedSpheres");
	addItem("Preprocessor","QtFileGenerator");
	createMenus();


	QVBox *vbox = new QVBox( this );
	vbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	vbox->setMargin( 2 );
	vbox->setLineWidth( 2 );

	workspace = new QWorkspace( vbox );
	workspace->setBackgroundMode( PaletteMid );
	setCentralWidget( vbox );

	// FIXME do something here !
/*	if( Omega::instance().getFileName().size() != 0)
	{
		loadSimulation( Omega::instance().getFileName() );

		if( Omega::instance().getAutomatic() )
			glViewer->startAnimation();
	}*/
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

void YadeQtMainWindow::addItem(string menuName, string itemName)
{
	map<string,QPopupMenu*>::iterator mi = menus.find(menuName);
	if (mi!=menus.end())
	{
		items.push_back(new QAction(this, itemName.c_str()));
		items.back()->setText( itemName.c_str() );
		items.back()->setMenuText( itemName.c_str() );
		items.back()->setToolTip( ("Load library lib"+itemName+".so").c_str() );
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

	simulationController = shared_ptr<SimulationController>(new SimulationController(workspace));
	simulationController->show();
}

void YadeQtMainWindow::fileExit()
{
	exit(0);
}

void YadeQtMainWindow::dynamicMenuClicked()
{
	QAction * action = (QAction*)(this->sender());
	string name = action->text();
	qtWidgets.push_back(ClassFactory::instance().createShared(name));

	shared_ptr<QWidget> widget = dynamic_pointer_cast<QWidget>(qtWidgets.back());
	if (widget) // the library is a QWidget so we set workspace as its parent
	{
		widget->reparent(workspace,QPoint(10,10));
		widget->show();
	}
}

void YadeQtMainWindow::endOfSimulationLoop()
{
	if (simulationController)
		simulationController->glViewer->draw();
}
