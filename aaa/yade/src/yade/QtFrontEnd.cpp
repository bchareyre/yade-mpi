#include "QtFrontEnd.hpp"
#include "Body.hpp"
#include "Contact.hpp"
#include <qvbox.h>


using namespace std;

QtFrontEnd::QtFrontEnd() : QtGeneratedFrontEnd(),FrontEnd()
{
	QVBox *vbox = new QVBox( this );
	vbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	vbox->setMargin( 2 );
	vbox->setLineWidth( 2 );

	workspace = new QWorkspace( vbox );	
	workspace->setBackgroundMode( PaletteMid );
	setCentralWidget( vbox );
	
	glViewer = shared_ptr<GLViewer>(new GLViewer(workspace));
	glViewer->show();	

	// FIXME : no hardcoded file generator
	ClassFactory::instance().createShared("RotatingBox");

	loadScene();

}

QtFrontEnd::~QtFrontEnd()
{

}


void QtFrontEnd::loadScene()
{
	IOManager::loadFromFile("XMLManager","../data/scene.xml","rootBody",Omega::instance().rootBody);
	Omega::instance().logMessage("Loading file ../data/scene.xml");
}
