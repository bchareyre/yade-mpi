#include "YadeQtMainWindow.hpp"

#include "IOManager.hpp"
#include "QtGUIGenerator.hpp"
#include "RigidBody.hpp"

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
	addItem("Preprocessor","FEMRock");
	addItem("Preprocessor","BoxStack");
	createMenus();


	QVBox *vbox = new QVBox( this );
	vbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	vbox->setMargin( 2 );
	vbox->setLineWidth( 2 );

	workspace = new QWorkspace( vbox );
	workspace->setBackgroundMode( PaletteMid );
	setCentralWidget( vbox );

/*
	QtGUIGenerator guiGen;
	shared_ptr<RigidBody> rb(new RigidBody);
	rb->mass = 11;
	rb->velocity = Vector3(1,2,3);
	rb->angularVelocity = Vector3(1.2,5,1.15);
	rb->isDynamic = true;
	rb->inertia = Vector3(4,4,4);
	frame = new QFrame(workspace);
	guiGen.buildGUI(rb,workspace,frame);
	frame->show();
*/

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
	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName("../data", "XML Yade File (*.xml)", this,"Open File","Choose a file to open",&selectedFilter );

	if (!fileName.isEmpty())
	{
		setCaption( "Yade - "+fileName);
		if (selectedFilter=="XML Yade File (*.xml)")
		{
		// FIXME - resetting the rootBody shouldn't be here. It shoul dbe inside Omega. As well as all Omega variables SHOULD be private!
		// FIXME - and this doesn't work - spheres disappear!
//			Omega::instance().rootBody = shared_ptr<NonConnexBody>(new NonConnexBody);
			IOManager::loadFromFile("XMLManager",fileName,"rootBody",Omega::instance().rootBody);
			Omega::instance().logMessage("Loading file ../data/scene.xml");
			glViewer = shared_ptr<GLViewer>(new GLViewer(workspace));
			glViewer->show();
		}
	}
}

void YadeQtMainWindow::fileExit()
{
	exit(0);
}

void YadeQtMainWindow::dynamicMenuClicked()
{
	QAction * action = (QAction*)(this->sender());
	string name = action->text();
	ClassFactory::instance().createShared(name);
}
