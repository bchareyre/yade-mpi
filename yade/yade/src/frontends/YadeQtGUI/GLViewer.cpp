#include "GLViewer.hpp"
#include "Body.hpp"
#include "Interaction.hpp"
#include "GL/glut.h"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"

GLViewer::GLViewer(QWidget * parent) : QGLViewer(parent) , frame( Omega::instance().getIterReference() )
{
	resize(300,300);
	setSceneCenter(0,0,0);
	setSceneRadius(200);
	showEntireScene();
	setAnimationPeriod(0);
	fpsTracker = shared_ptr<FpsTracker>(new FpsTracker(this));
	frame = 0;
}

GLViewer::~GLViewer()
{

}

void GLViewer::draw()
{
        
	Omega::instance().synchronizer->wait2();
	
	glEnable(GL_NORMALIZE);
        glEnable(GL_CULL_FACE);

	if (Omega::instance().rootBody) // if the scene is loaded
		Omega::instance().rootBody->glDraw();
	
	frame++;

	fpsTracker->glDraw();

	Omega::instance().synchronizer->go();
}

void GLViewer::animate()
{
	//Omega::instance().rootBody->moveToNextTimeStep();

	fpsTracker->addOneAction();
}

void GLViewer::mouseMoveEvent(QMouseEvent * e)
{
	if (!fpsTracker->mouseMoveEvent(e))
		QGLViewer::mouseMoveEvent(e);
}

void GLViewer::mousePressEvent(QMouseEvent *e)
{
	//Omega::instance().synchronizer->wait2();
	if (!fpsTracker->mousePressEvent(e))
		QGLViewer::mousePressEvent(e);
}

void GLViewer::mouseReleaseEvent(QMouseEvent *e)
{
	if (!fpsTracker->mouseReleaseEvent(e))
		QGLViewer::mouseReleaseEvent(e);
	//Omega::instance().synchronizer->go();
}

void GLViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
	//Omega::instance().synchronizer->wait2();
	if (!fpsTracker->mouseDoubleClickEvent(e))
		QGLViewer::mouseDoubleClickEvent(e);
	//Omega::instance().synchronizer->go();
}


void GLViewer::keyPressEvent(QKeyEvent *e)
{
	if (e->key()=='f' || e->key()=='F')
		fpsTracker->swapDisplayed();
	else
		QGLViewer::keyPressEvent(e);
}
