#include "GLViewer.hpp"
#include "Body.hpp"
#include "Interaction.hpp"
#include "GL/glut.h"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"
#include "FpsTracker.hpp"

GLViewer::GLViewer(shared_ptr<RenderingEngine> renderer, const QGLFormat& format, QWidget * parent, QGLWidget * shareWidget) : QGLViewer(format,parent,"glview",shareWidget), qglThread(this,renderer)
{

	setAutoBufferSwap(false);
	resize(320, 240);
	setSceneCenter(0,0,0);
	setSceneRadius(200);
	showEntireScene();

	show();

	wm.addWindow(new FpsTracker(),new GLWindowsManager::EventSubscription());
	
}

GLViewer::~GLViewer()
{

}
void GLViewer::paintGL()
{
	//ThreadSafe::cerr("painGL");
}

void GLViewer::updateGL()
{
	//ThreadSafe::cerr("updateGL");
}
void GLViewer::resizeEvent(QResizeEvent *evt)
{
	qglThread.resize(evt->size().width(),evt->size().height()); 
}

void GLViewer::paintEvent(QPaintEvent *)
{
	// Handled by the GLThread.
}

void GLViewer::closeEvent(QCloseEvent *evt)
{
	QGLViewer::closeEvent(evt);
}

void GLViewer::mouseMoveEvent(QMouseEvent * e)
{
	if (wm.mouseMoveEvent(e->x(),e->y())==-1)
		QGLViewer::mouseMoveEvent(e);
}

void GLViewer::mousePressEvent(QMouseEvent *e)
{
	if (wm.mousePressEvent(e->x(),e->y())==-1)
		QGLViewer::mousePressEvent(e);
}

void GLViewer::mouseReleaseEvent(QMouseEvent *e)
{
	if (wm.mouseReleaseEvent(e->x(),e->y())==-1)
		QGLViewer::mouseReleaseEvent(e);
}

void GLViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (wm.mouseDoubleClickEvent(e->x(),e->y())==-1)
		QGLViewer::mouseDoubleClickEvent(e);
}

void GLViewer::keyPressEvent(QKeyEvent *e)
{
	if (e->key()=='F' || e->key()=='f')
		wm.getWindow(0)->swapDisplayed();
	else
		QGLViewer::keyPressEvent(e);
}
