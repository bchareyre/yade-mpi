#include "GLViewer.hpp"
#include "Body.hpp"
#include "Interaction.hpp"
#include "GL/glut.h"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"

GLViewer::GLViewer(QWidget * parent, QGLWidget * shareWidget) : QGLViewer(parent,"glview",shareWidget)
{
	resize(300,300);
	setSceneCenter(0,0,0);
	setSceneRadius(200);
	showEntireScene();
	setAnimationPeriod(0);
	startAnimation();
	fpsTracker = shared_ptr<FpsTracker>(new FpsTracker(this));	
	//turnId = Omega::instance().getNewTurnId();
}

GLViewer::~GLViewer()
{

}
#include <boost/thread/mutex.hpp>

void GLViewer::draw()
{       
	static int turnId = Omega::instance().getNewTurnId();
	
	Omega::instance().waitMyTurn(turnId);
	
	glEnable(GL_NORMALIZE);
        glEnable(GL_CULL_FACE);

	if (Omega::instance().rootBody) // if the scene is loaded
		Omega::instance().rootBody->glDraw();

	fpsTracker->glDraw();
	fpsTracker->addOneAction();
	
	Omega::instance().endMyTurn();
	
}

void GLViewer::mouseMoveEvent(QMouseEvent * e)
{
	if (!fpsTracker->mouseMoveEvent(e))
		QGLViewer::mouseMoveEvent(e);
}

void GLViewer::mousePressEvent(QMouseEvent *e)
{
	if (!fpsTracker->mousePressEvent(e))
		QGLViewer::mousePressEvent(e);
}

void GLViewer::mouseReleaseEvent(QMouseEvent *e)
{
	if (!fpsTracker->mouseReleaseEvent(e))
		QGLViewer::mouseReleaseEvent(e);
}

void GLViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (!fpsTracker->mouseDoubleClickEvent(e))
		QGLViewer::mouseDoubleClickEvent(e);
}

void GLViewer::keyPressEvent(QKeyEvent *e)
{
	if (e->key()=='F' || e->key()=='f')
		fpsTracker->swapDisplayed();
	//else
	//	QGLViewer::keyPressEvent(e);
}
