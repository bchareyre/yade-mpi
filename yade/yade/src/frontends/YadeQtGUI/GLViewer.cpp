#include "GLViewer.hpp"
#include "Body.hpp"
#include "Interaction.hpp"
#include "GL/glut.h"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"


GLViewer::GLViewer(const QGLFormat& format, QWidget * parent, QGLWidget * shareWidget) : QGLViewer(format,parent,"glview",shareWidget), qglThread(this)
{

	setAutoBufferSwap(false);
	resize(320, 240);
	setSceneCenter(0,0,0);
	setSceneRadius(200);
	showEntireScene();

	show();

	fpsTracker = shared_ptr<FpsTracker>(new FpsTracker(this));
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
