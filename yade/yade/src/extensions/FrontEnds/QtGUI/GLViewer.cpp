#include "GLViewer.hpp"
#include "Body.hpp"
#include "Interaction.hpp"
#include "GL/glut.h"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"
#include "FpsTracker.hpp"

GLViewer::GLViewer(int id, shared_ptr<RenderingEngine> renderer, const QGLFormat& format, QWidget * parent, QGLWidget * shareWidget) : QGLViewer(format,parent,"glview",shareWidget), qglThread(this,renderer)
{

	viewId = id;
	setAutoBufferSwap(false);
	resize(320, 240);

	if (id==0)
		setCaption("Primary View (not closable)");
	else
		setCaption("Secondary View number "+lexical_cast<string>(id));
	show();

	wm.addWindow(new FpsTracker(),new GLWindowsManager::EventSubscription());
	
}

GLViewer::~GLViewer()
{
	
}

void GLViewer::initializeGL()
{
	QGLViewer::initializeGL();
}

void GLViewer::finishRendering()
{
	qglThread.finish();
}

void GLViewer::joinRendering()
{
	qglThread.join();
}

void GLViewer::stopRendering()
{
	qglThread.stop();
}

void GLViewer::startRendering()
{	
	qglThread.start();
}

void GLViewer::centerScene()
{
// 	Vector3r min = Omega::instance().rootBody->bv->min;
// 	Vector3r max = Omega::instance().rootBody->bv->max;
// 	Vector3r center = (max+min)*0.5;
// 	Vector3r halfSize = (max-min)*0.5;
// 	float radius = halfSize[0];
// 	if (halfSize[1]>radius)
// 		radius = halfSize[1];
// 	if (halfSize[2]>radius)
// 		radius = halfSize[2];
// 
// 	setSceneCenter(center[0],center[1],center[2]);
// 	setSceneRadius(radius*1.5);
// 	showEntireScene();
	qglThread.centerScene();

}
	
void GLViewer::paintGL()
{
	// Handled by the GLThread.
}

void GLViewer::updateGL()
{
	// Handled by the GLThread.
}
void GLViewer::resizeEvent(QResizeEvent *evt)
{
	qglThread.resize(evt->size().width(),evt->size().height()); 
}

void GLViewer::paintEvent(QPaintEvent *)
{
	// Handled by the GLThread.
}

void GLViewer::closeEvent(QCloseEvent *)
{
	emit closeSignal(viewId);
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
