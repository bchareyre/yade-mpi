/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2005 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"GLViewer.hpp"
#include"YadeCamera.hpp"
#include<GL/glut.h>
#include<yade/lib-opengl/FpsTracker.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/Interaction.hpp>


GLViewer::GLViewer(int id, shared_ptr<RenderingEngine> rendererInit, const QGLFormat& format, QWidget * parent, QGLWidget * shareWidget) : QGLViewer(format,parent,"glview",shareWidget)//, qglThread(this,rendererInit)
{
	isMoving=false;
	renderer=rendererInit;
	drawGrid = false;
	viewId = id;
	cut_plane = 0;
	cut_plane_delta = -2;
	resize(550,550);

	if (id==0)
		setCaption("Primary View (not closable)");
	else
		setCaption("Secondary View number "+lexical_cast<string>(id));
	show();
	
	notMoving();

	if(manipulatedFrame() == 0 )
		setManipulatedFrame(new qglviewer::ManipulatedFrame());
}

void GLViewer::notMoving()
{
	camera()->frame()->setWheelSensitivity(-1.0f);
	setMouseBinding(Qt::LeftButton + Qt::RightButton, CAMERA, ZOOM);
	setMouseBinding(Qt::MidButton, CAMERA, ZOOM);
	setMouseBinding(Qt::LeftButton, CAMERA, ROTATE);
	setMouseBinding(Qt::RightButton, CAMERA, TRANSLATE);
	setWheelBinding(Qt::NoButton, CAMERA, ZOOM);
	setMouseBinding(Qt::SHIFT + Qt::LeftButton, SELECT);
	//setMouseBinding(Qt::RightButton, NO_CLICK_ACTION);

	setMouseBinding(Qt::SHIFT + Qt::LeftButton + Qt::RightButton, FRAME, ZOOM);
	setMouseBinding(Qt::SHIFT + Qt::MidButton, FRAME, TRANSLATE);
	setMouseBinding(Qt::SHIFT + Qt::RightButton, FRAME, ROTATE);
	setWheelBinding(Qt::ShiftButton , FRAME, ZOOM);
};

GLViewer::~GLViewer()
{
//	std::cerr << "GLViewer dtor:" << viewId << "\n";
}


void GLViewer::keyPressEvent(QKeyEvent *e)
{
	if ( e->key()==Qt::Key_M )
		if( !(isMoving = !isMoving ) )
		{
			displayMessage("moving finished");
			notMoving();
		}
		else
		{
			displayMessage("moving selected object");
			setMouseBinding(Qt::LeftButton + Qt::RightButton, FRAME, ZOOM);
			setMouseBinding(Qt::LeftButton, FRAME, TRANSLATE);
			setMouseBinding(Qt::MidButton, FRAME, TRANSLATE);
			setMouseBinding(Qt::RightButton, FRAME, ROTATE);
			setWheelBinding(Qt::NoButton , FRAME, ZOOM);
		}
	else if( e->key()==Qt::Key_C && selectedName() != -1 && (*(Omega::instance().getRootBody()->bodies)).exists(selectedName()))
		setSceneCenter(manipulatedFrame()->position()), updateGL();
	else if( e->key()==Qt::Key_D )
		wasDynamic = true;
	else if( e->key()==Qt::Key_G )
		drawGrid = !drawGrid, updateGL();

// FIXME BEGIN - arguments for GLDraw*ers should be from dialog box, not through Omega !!!
	else if( e->key()==Qt::Key_Delete )
		Omega::instance().isoValue-=0.05, updateGL();
	else if( e->key()==Qt::Key_Insert )
		Omega::instance().isoValue+=0.05, updateGL();

	else if( e->key()==Qt::Key_Next )
		Omega::instance().isoThick-=0.05, updateGL();
	else if( e->key()==Qt::Key_Prior )
		Omega::instance().isoThick+=0.05, updateGL();

	else if( e->key()==Qt::Key_End )
		Omega::instance().isoSec=std::max(1, Omega::instance().isoSec-1), updateGL();
	else if( e->key()==Qt::Key_Home )
		Omega::instance().isoSec+=1, updateGL();
// FIXME END

	else if (e->key() == Qt::Key_T)
	{ // 'T' changes the projection type : perspective or orthogonal
		if (camera()->type() == qglviewer::Camera::ORTHOGRAPHIC)
			camera()->setType(qglviewer::Camera::PERSPECTIVE);
		else
			camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);
		updateGL();
	}
	else if( e->key()==Qt::Key_O )
		camera()->setFieldOfView(camera()->fieldOfView()*0.9), updateGL();
	else if( e->key()==Qt::Key_P )
		camera()->setFieldOfView(camera()->fieldOfView()*1.1), updateGL();

//////////////////////////////////////////////
// FIXME that all should be in some nice GUI
// 
// Cutting plane
	else if( e->key()==Qt::Key_Plus ){
			cut_plane = std::min(1.0, cut_plane + std::pow(10.0,(double)cut_plane_delta));
			static_cast<YadeCamera*>(camera())->setCuttingDistance(cut_plane);
			displayMessage("Cut plane: "+lexical_cast<std::string>(cut_plane));
	}else if( e->key()==Qt::Key_Minus ){
			cut_plane = std::max(0.0, cut_plane - std::pow(10.0,(double)cut_plane_delta));
			static_cast<YadeCamera*>(camera())->setCuttingDistance(cut_plane);
			displayMessage("Cut plane: "+lexical_cast<std::string>(cut_plane));
	}else if( e->key()==Qt::Key_Slash ){
			cut_plane_delta -= 1;
			displayMessage("Cut plane increment: 1e"+(cut_plane_delta>0?std::string("+"):std::string(""))+lexical_cast<std::string>(cut_plane_delta));
	}else if( e->key()==Qt::Key_Asterisk ){
			cut_plane_delta = std::min(1+cut_plane_delta,-1);
			displayMessage("Cut plane increment: 1e"+(cut_plane_delta>0?std::string("+"):std::string(""))+lexical_cast<std::string>(cut_plane_delta));
	}

// FIXME END
//////////////////////////////////////////////



	else if( e->key()!=Qt::Key_Escape && e->key()!=Qt::Key_Space )
		QGLViewer::keyPressEvent(e);
}

void GLViewer::centerScene()
{
	if (!Omega::instance().getRootBody())
		return;

	if(Omega::instance().getRootBody()->bodies->size() < 500)
		displayMessage("Less than 500 bodies, moving possible. Select with shift, press 'm' to move. Use / * - + for cutting plane.", 6000);
	else
		displayMessage("More than 500 bodies. Moving not possible. Use / * - + for cutting plane.", 6000);

	Vector3r min = Omega::instance().getRootBody()->boundingVolume->min;
	Vector3r max = Omega::instance().getRootBody()->boundingVolume->max;
	Vector3r center = (max+min)*0.5;
	Vector3r halfSize = (max-min)*0.5;
	float radius = std::max(halfSize[0] , std::max(halfSize[1] , halfSize[2]) );
	setSceneCenter(qglviewer::Vec(center[0],center[1],center[2]));
	setSceneRadius(radius*1.5);
	showEntireScene();
}

void GLViewer::draw() // FIXME maybe rename to RendererFlowControl, or something like that?
{
	if(Omega::instance().getRootBody())
	{
		int selection = selectedName();
		if(selection != -1 && (*(Omega::instance().getRootBody()->bodies)).exists(selection) )
		{
			Quaternionr& q = (*(Omega::instance().getRootBody()->bodies))[selection]->physicalParameters->se3.orientation;
			Vector3r&    v = (*(Omega::instance().getRootBody()->bodies))[selection]->physicalParameters->se3.position;
			float v0,v1,v2;
			manipulatedFrame()->getPosition(v0,v1,v2);
			v[0]=v0;v[1]=v1;v[2]=v2;
			double q0,q1,q2,q3;
			manipulatedFrame()->getOrientation(q0,q1,q2,q3);
			q[0]=q0;q[1]=q1;q[2]=q2;q[3]=q3;

			(*(Omega::instance().getRootBody()->bodies))[selection]->userForcedDisplacementRedrawHook();	
		}
		
	// FIXME - here we want to actually call all responsible GLDraw Actors
		renderer->render(Omega::instance().getRootBody(), selectedName());
	}
}

void GLViewer::drawWithNames() // FIXME maybe rename to RendererFlowControl, or something like that?
{
	if (Omega::instance().getRootBody() && Omega::instance().getRootBody()->bodies->size() < 500 )
	// FIXME - here we want to actually call all responsible GLDraw Actors
		renderer->renderWithNames(Omega::instance().getRootBody());
}

// new object selected.
// set frame coordinates, and isDynamic=false;
void GLViewer::postSelection(const QPoint& point) 
{
	int selection = selectedName();
	if(selection == -1)
	{
		if(isMoving)
		{
			displayMessage("moving finished");
			notMoving();
			isMoving=false;
		}
		return;
	}
	if( (*(Omega::instance().getRootBody()->bodies)).exists(selection) )
	{
		if(Body::byId(body_id_t(selection))->isClumpMember()){ // select clump (invisible) instead of its member
			cerr<<"Clump member #"<<selection<<" selected, selecting clump instead."<<endl;
			selection=Body::byId(body_id_t(selection))->clumpId;
			setSelectedName(selection);
		}

		std::cerr << "new selection " << selection << "\n";
		wasDynamic = (*(Omega::instance().getRootBody()->bodies))[selection]->isDynamic;
		(*(Omega::instance().getRootBody()->bodies))[selection]->isDynamic = false;

		Quaternionr& q = (*(Omega::instance().getRootBody()->bodies))[selection]->physicalParameters->se3.orientation;
		Vector3r&    v = (*(Omega::instance().getRootBody()->bodies))[selection]->physicalParameters->se3.position;
		manipulatedFrame()->setPositionAndOrientation(qglviewer::Vec(v[0],v[1],v[2]),qglviewer::Quaternion(q[0],q[1],q[2],q[3]));
	}

}

// maybe new object will be selected.
// if so, then set isDynamic of previous selection, to old value
void GLViewer::endSelection(const QPoint &point)
{
	int old = selectedName();

	QGLViewer::endSelection(point);

	if(old != -1 && old!=selectedName() && (*(Omega::instance().getRootBody()->bodies)).exists(old) )
		(*(Omega::instance().getRootBody()->bodies))[old]->isDynamic = wasDynamic;
}

void GLViewer::postDraw()
{
	if( drawGrid ) // FIXME drawGrid is yet another RendererFlowControl's Actor.
	{
//		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		qglColor(foregroundColor());
		glDisable(GL_LIGHTING);
		glLineWidth(0.1);
		glBegin(GL_LINES);

		float sizef = QGLViewer::camera()->sceneRadius()*3.0f; 
		int size = static_cast<int>(sizef);
		qglviewer::Vec v = QGLViewer::camera()->sceneCenter();
		int x = static_cast<int>(v[0]); int y = static_cast<int>(v[1]);
		float xf = (static_cast<int>(v[0]*100.0))/100.0;
		float yf = (static_cast<int>(v[1]*100.0))/100.0;
//		float nbSubdivisions = size;
//		for (int i=0; i<=nbSubdivisions; ++i)
		for (int i= -size ; i<=size; ++i )
		{
//			const float pos = size*(2.0*i/nbSubdivisions-1.0);
			glVertex2i( i   +x, -size+y);
			glVertex2i( i   +x, +size+y);
			glVertex2i(-size+x, i    +y);
			glVertex2i( size+x, i    +y);
		}
		if(sizef <= 2.0)
		{
			glColor3f(0.9,0.9,0.9);
			for (float i= -(static_cast<int>(sizef*100.0))/100.0 ; i<=sizef; i+=0.01 )
			{
				glVertex2f( i    +xf, -sizef+yf);
				glVertex2f( i    +xf, +sizef+yf);
				glVertex2f(-sizef+xf, i     +yf);
				glVertex2f( sizef+xf, i     +yf);
			}
		}
		
		glEnd();
		glPopAttrib();
		glPopMatrix();
	}
	QGLViewer::postDraw();
}

void GLViewer::closeEvent(QCloseEvent *e)
{
	emit closeSignal(viewId);
}

