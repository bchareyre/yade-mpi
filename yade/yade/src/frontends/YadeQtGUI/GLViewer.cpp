#include "GLViewer.hpp"
#include "Body.hpp"
#include "Interaction.hpp"
#include "GL/glut.h"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"

GLViewer::GLViewer(QWidget * parent, QGLWidget * shareWidget) : QGLViewer(parent,"glview",shareWidget)
{
// 	glView = new GLView(parent,shareWidget);
// 	glView->show();
// 	createThread(Omega::instance().synchronizer);
	fpsTracker = shared_ptr<FpsTracker>(new FpsTracker(this));
	resize(300,300);
	setSceneCenter(0,0,0);
	setSceneRadius(200);
	showEntireScene();
	setAnimationPeriod(0);
	startAnimation();
	show();

}

GLViewer::~GLViewer()
{

}

// bool GLViewer::notEnd()
// {
// 	return true;
// }
// 
// void GLViewer::oneLoop()
// {
// 	glView->safedraw();
// }


void GLViewer::draw()
{
	static int drawTurn = Omega::instance().synchronizer->insertThread();
	
	Omega::instance().synchronizer->wait(drawTurn);
	
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);

	if (Omega::instance().rootBody) // if the scene is loaded
		Omega::instance().rootBody->glDraw();
		
	fpsTracker->glDraw();
	fpsTracker->addOneAction();
	
	Omega::instance().synchronizer->signal();

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
	else
		QGLViewer::keyPressEvent(e);
}
