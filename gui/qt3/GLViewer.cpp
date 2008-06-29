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
#include"QtGUI.hpp"
#include"YadeCamera.hpp"
#include"YadeQtMainWindow.hpp"
#include<GL/glut.h>
#include<yade/lib-opengl/FpsTracker.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/Interaction.hpp>

CREATE_LOGGER(GLViewer);

GLViewer::GLViewer(int id, shared_ptr<OpenGLRenderingEngine> _renderer, QWidget * parent, QGLWidget * shareWidget) : QGLViewer(parent,"glview",shareWidget)//, qglThread(this,rendererInit)
{
	isMoving=false;
	renderer=_renderer;
	drawGridXYZ[0]=drawGridXYZ[1]=drawGridXYZ[2]=false;
	viewId = id;
	cut_plane = 0;
	cut_plane_delta = -2;
	resize(550,550);

	if (id==0) setCaption("Primary view");
	else setCaption("Secondary view number "+lexical_cast<string>(id));
	show();
	
	mouseMovesCamera();
	manipulatedClipPlane=-1;

	if(manipulatedFrame()==0) setManipulatedFrame(new qglviewer::ManipulatedFrame());

	xyPlaneConstraint=shared_ptr<qglviewer::LocalConstraint>(new qglviewer::LocalConstraint());
	//xyPlaneConstraint->setTranslationConstraint(qglviewer::AxisPlaneConstraint::AXIS,qglviewer::Vec(0,0,1));
	//xyPlaneConstraint->setRotationConstraint(qglviewer::AxisPlaneConstraint::FORBIDDEN,qglviewer::Vec(0,0,1));
	manipulatedFrame()->setConstraint(NULL);

	setKeyDescription(Qt::Key_C,"Set scene center to the selected body (if any)");
	setKeyDescription(Qt::Key_D,"Toggle Body::isDynamic on selection");
	setKeyDescription(Qt::Key_G,"Toggle YZ grid");
	setKeyDescription(Qt::Key_X,"Toggle YZ grid");
	setKeyDescription(Qt::Key_Y,"Toggle XZ grid");
	setKeyDescription(Qt::Key_Z,"Toggle XY grid");
	setKeyDescription(Qt::Key_Delete,"(lattice) increase isoValue");
	setKeyDescription(Qt::Key_Insert,"(lattice) decrease isoValue");
	setKeyDescription(Qt::Key_Next,  "(lattice) increase isoThic");
	setKeyDescription(Qt::Key_Prior, "(lattice) decrease isoThic");
	setKeyDescription(Qt::Key_End,   "(lattice) decrease isoSec");
	setKeyDescription(Qt::Key_Home,  "(lattice) increase isoSec");
	setKeyDescription(Qt::Key_T,"Switch orthographic / perspective camera");
	setKeyDescription(Qt::Key_O,"Set narrower field of view");
	setKeyDescription(Qt::Key_P,"Set wider field of view");
	setKeyDescription(Qt::Key_R,"Revolve around scene center");
	setKeyDescription(Qt::Key_Plus,    "Cut plane increase");
	setKeyDescription(Qt::Key_Minus,   "Cut plane decrease");
	setKeyDescription(Qt::Key_Slash,   "Cut plane step decrease");
	setKeyDescription(Qt::Key_Asterisk,"Cut plane step increase");
	// unset default shortcuts
 	setPathKey(-Qt::Key_F1);
 	setPathKey(-Qt::Key_F2);
	setKeyDescription(Qt::Key_Escape,"Manipulate scene (default)");
	setKeyDescription(Qt::Key_F1,"Manipulate clipping plane #1 (2,...)");
	setKeyDescription(Qt::Key_1,"Make the manipulated clipping plane parallel with plane #1 (2,...)");
	setKeyDescription(Qt::Key_1 & Qt::ALT,"Add/remove plane #1 (2,...) to/from the bound group");
	setKeyDescription(Qt::Key_0,"Clear the bound group");
	setKeyDescription(Qt::Key_Space,"Activate/deactivate the manipulated clipping plane");
}

void GLViewer::mouseMovesCamera(){
	camera()->frame()->setWheelSensitivity(-1.0f);

	setMouseBinding(Qt::SHIFT + Qt::LeftButton, SELECT);
	//setMouseBinding(Qt::RightButton, NO_CLICK_ACTION);
	setMouseBinding(Qt::SHIFT + Qt::LeftButton + Qt::RightButton, FRAME, ZOOM);
	setMouseBinding(Qt::SHIFT + Qt::MidButton, FRAME, TRANSLATE);
	setMouseBinding(Qt::SHIFT + Qt::RightButton, FRAME, ROTATE);
	setWheelBinding(Qt::ShiftButton , FRAME, ZOOM);

	setMouseBinding(Qt::LeftButton + Qt::RightButton, CAMERA, ZOOM);
	setMouseBinding(Qt::MidButton, CAMERA, ZOOM);
	setMouseBinding(Qt::LeftButton, CAMERA, ROTATE);
	setMouseBinding(Qt::RightButton, CAMERA, TRANSLATE);
	setWheelBinding(Qt::NoButton, CAMERA, ZOOM);
};

void GLViewer::mouseMovesManipulatedFrame(qglviewer::Constraint* c){
	setMouseBinding(Qt::LeftButton + Qt::RightButton, FRAME, ZOOM);
	setMouseBinding(Qt::MidButton, FRAME, ZOOM);
	setMouseBinding(Qt::LeftButton, FRAME, ROTATE);
	setMouseBinding(Qt::RightButton, FRAME, TRANSLATE);
	setWheelBinding(Qt::NoButton , FRAME, ZOOM);
	manipulatedFrame()->setConstraint(c);
}

void GLViewer::resetManipulation(){
	mouseMovesCamera();
	setSelectedName(-1);
	isMoving=false;
	manipulatedClipPlane=-1;
}

void GLViewer::startClipPlaneManipulation(int planeNo){
	resetManipulation();
	mouseMovesManipulatedFrame(xyPlaneConstraint.get());
	manipulatedClipPlane=planeNo;
	const Se3r se3(renderer->clipPlaneSe3[planeNo]);
	manipulatedFrame()->setPositionAndOrientation(qglviewer::Vec(se3.position[0],se3.position[1],se3.position[2]),qglviewer::Quaternion(se3.orientation[0],se3.orientation[1],se3.orientation[2],se3.orientation[3]));
	string grp=strBoundGroup();
	displayMessage("Manipulating clip plane #"+lexical_cast<string>(planeNo+1)+(grp.empty()?grp:"(bound planes:"+grp+")"));
}

void GLViewer::keyPressEvent(QKeyEvent *e)
{
	if (e->key()==Qt::Key_M && selectedName() >= 0){
		if(!(isMoving=!isMoving)){displayMessage("Moving done."); mouseMovesCamera();}
		else{ displayMessage("Moving selected object"); mouseMovesManipulatedFrame();}
	}
	else if(e->key()==Qt::Key_C && selectedName() >= 0 && (*(Omega::instance().getRootBody()->bodies)).exists(selectedName())) setSceneCenter(manipulatedFrame()->position()), updateGL();

	else if(e->key()==Qt::Key_Escape){ resetManipulation();displayMessage("Manipulating scene."); }
	else if(e->key()==Qt::Key_F1 || e->key()==Qt::Key_F2 /* || ... */ ){
		int n=0; if(e->key()==Qt::Key_F1) n=1; else if(e->key()==Qt::Key_F2) n=2; /* ... */ assert(n>0); int planeId=n-1;
		if(planeId!=manipulatedClipPlane) startClipPlaneManipulation(planeId);
		updateGL();
	}
	else if(e->key()==Qt::Key_R){ // reverse the plane
		if(manipulatedClipPlane>=0 && manipulatedClipPlane<renderer->clipPlaneNum){
			manipulatedFrame()->setOrientation(qglviewer::Quaternion(qglviewer::Vec(0,1,0),Mathr::PI)*manipulatedFrame()->orientation());
			displayMessage("Plane #"+lexical_cast<string>(manipulatedClipPlane-1)+" reversed.");
		}
		updateGL();
	}
	else if(e->key()==Qt::Key_F2){ startClipPlaneManipulation(1); updateGL(); }
	else if(e->key()==Qt::Key_Space){
		if(manipulatedClipPlane>=0) {displayMessage("Clip plane #"+lexical_cast<string>(manipulatedClipPlane+1)+(renderer->clipPlaneActive[manipulatedClipPlane]?" de":" ")+"activated"); renderer->clipPlaneActive[manipulatedClipPlane]=!renderer->clipPlaneActive[manipulatedClipPlane]; updateGL(); }
	}
	else if(e->key()==Qt::Key_0 &&(e->state() & AltButton)) {boundClipPlanes.clear(); displayMessage("Cleared the bound group.");}
	else if(e->key()==Qt::Key_1 || e->key()==Qt::Key_2 /* || ... */ ){
		int n=0; if(e->key()==Qt::Key_1) n=1; else if(e->key()==Qt::Key_2) n=2; /* ... */ assert(n>0); int planeId=n-1;
		if(planeId>=renderer->clipPlaneNum) return; // no such clipping plane
		if(e->state() & AltButton){
			if(boundClipPlanes.count(planeId)==0) {boundClipPlanes.insert(planeId); displayMessage("Added plane #"+lexical_cast<string>(planeId+1)+" to the bound group: "+strBoundGroup());}
			else {boundClipPlanes.erase(planeId); displayMessage("Removed plane #"+lexical_cast<string>(planeId+1)+" from the bound group: "+strBoundGroup());}
		}
		else if(manipulatedClipPlane>=0 && manipulatedClipPlane!=planeId) {
			const Quaternionr& o=renderer->clipPlaneSe3[planeId].orientation;
			manipulatedFrame()->setOrientation(qglviewer::Quaternion(o[0],o[1],o[2],o[3]));
			displayMessage("Copied orientation from plane #1");
		}
		updateGL();
	}
	else if(e->key()==Qt::Key_D) wasDynamic = true;
	else if(e->key()==Qt::Key_G) {bool anyDrawn=drawGridXYZ[0]||drawGridXYZ[1]||drawGridXYZ[2]; for(int i=0; i<3; i++)drawGridXYZ[i]=!anyDrawn; updateGL();}
	else if(e->key()==Qt::Key_X) drawGridXYZ[0]=!drawGridXYZ[0], updateGL();
	else if(e->key()==Qt::Key_Y) drawGridXYZ[1]=!drawGridXYZ[1], updateGL();
	else if(e->key()==Qt::Key_Z) drawGridXYZ[2]=!drawGridXYZ[2], updateGL();
// FIXME BEGIN - arguments for GLDraw*ers should be from dialog box, not through Omega !!!
	else if(e->key()==Qt::Key_Delete) Omega::instance().isoValue-=0.05, updateGL();
	else if(e->key()==Qt::Key_Insert) Omega::instance().isoValue+=0.05, updateGL();
	else if(e->key()==Qt::Key_Next) Omega::instance().isoThick-=0.05, updateGL();
	else if(e->key()==Qt::Key_Prior)	Omega::instance().isoThick+=0.05, updateGL();
	else if(e->key()==Qt::Key_End) Omega::instance().isoSec=std::max(1, Omega::instance().isoSec-1), updateGL();
	else if(e->key()==Qt::Key_Home) Omega::instance().isoSec+=1, updateGL();
// FIXME END
	else if (e->key() == Qt::Key_T) camera()->setType(camera()->type()==qglviewer::Camera::ORTHOGRAPHIC ? qglviewer::Camera::PERSPECTIVE : qglviewer::Camera::ORTHOGRAPHIC), updateGL();
	else if(e->key()==Qt::Key_O) camera()->setFieldOfView(camera()->fieldOfView()*0.9), updateGL();
	else if(e->key()==Qt::Key_P) camera()->setFieldOfView(camera()->fieldOfView()*1.1), updateGL();
	else if(e->key()==Qt::Key_R) camera()->setRevolveAroundPoint(sceneCenter()), updateGL();

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
//
	else if( e->key()!=Qt::Key_Escape && e->key()!=Qt::Key_Space )
		QGLViewer::keyPressEvent(e);
}

void GLViewer::centerScene()
{
	MetaBody* rb=Omega::instance().getRootBody().get();
	if (!rb) return;

	if(rb->bodies->size() < 500)
		displayMessage("Less than 500 bodies, moving possible. Select with shift, press 'm' to move. Use / * - + for cutting plane.", 6000);
	else
		displayMessage("More than 500 bodies. Moving not possible. Use / * - + for cutting plane.", 6000);
	Vector3r min,max;	
	if(rb->boundingVolume){
		min=rb->boundingVolume->min; max=rb->boundingVolume->max;
		if(std::max(max[0]-min[0],std::max(max[1]-min[1],max[2]-min[2]))<=0){
			// AABB is not yet calculated...
			LOG_DEBUG("rootBody's AABB not yet calculated or has one dimension zero, attempt get that from bodies' positions.");
			Real inf=std::numeric_limits<Real>::infinity();
			min=Vector3r(inf,inf,inf); max=Vector3r(-inf,-inf,-inf);
			FOREACH(const shared_ptr<Body>& b, *rb->bodies){
				max=componentMaxVector(max,b->physicalParameters->se3.position);
				min=componentMinVector(min,b->physicalParameters->se3.position);
			}
		} else {LOG_DEBUG("Using rootBody's AABB");}
	} else {
		LOG_DEBUG("No rootBody's AABB; setting scene in cube (-1,-1,-1)x(1,1,1)");
		min=Vector3r(-1,-1,-1); max=Vector3r(1,1,1);
	}
	LOG_DEBUG("Got scene box min="<<min<<" and max="<<max);
	Vector3r center = (max+min)*0.5;
	Vector3r halfSize = (max-min)*0.5;
	float radius=std::max(halfSize[0],std::max(halfSize[1],halfSize[2])); if(radius==0) radius=1;
	LOG_DEBUG("Scene center="<<center<<", halfSize="<<halfSize<<", radius="<<radius);
	setSceneCenter(qglviewer::Vec(center[0],center[1],center[2]));
	setSceneRadius(radius*1.5);
	showEntireScene();
}

void GLViewer::draw()
{
	if(Omega::instance().getRootBody()){
		int selection = selectedName();
		if(selection!=-1 && (*(Omega::instance().getRootBody()->bodies)).exists(selection)){
			Quaternionr& q = (*(Omega::instance().getRootBody()->bodies))[selection]->physicalParameters->se3.orientation;
			Vector3r&    v = (*(Omega::instance().getRootBody()->bodies))[selection]->physicalParameters->se3.position;
			float v0,v1,v2; manipulatedFrame()->getPosition(v0,v1,v2);v[0]=v0;v[1]=v1;v[2]=v2;
			double q0,q1,q2,q3; manipulatedFrame()->getOrientation(q0,q1,q2,q3);	q[0]=q0;q[1]=q1;q[2]=q2;q[3]=q3;
			(*(Omega::instance().getRootBody()->bodies))[selection]->userForcedDisplacementRedrawHook();	
		}
		if(manipulatedClipPlane>=0){
			assert(manipulatedClipPlane<renderer->clipPlaneNum);
			float v0,v1,v2; manipulatedFrame()->getPosition(v0,v1,v2);
			double q0,q1,q2,q3; manipulatedFrame()->getOrientation(q0,q1,q2,q3);
			Se3r newSe3(Vector3r(v0,v1,v2),Quaternionr(q0,q1,q2,q3)); newSe3.orientation.Normalize();
			const Se3r& oldSe3=renderer->clipPlaneSe3[manipulatedClipPlane];
			FOREACH(int planeId, boundClipPlanes){
				if(planeId>=renderer->clipPlaneNum || !renderer->clipPlaneActive[planeId] || planeId==manipulatedClipPlane) continue;
				Se3r& boundSe3=renderer->clipPlaneSe3[planeId];
				Quaternionr relOrient=oldSe3.orientation.Conjugate()*boundSe3.orientation; relOrient.Normalize();
				Vector3r relPos=oldSe3.orientation.Conjugate()*(boundSe3.position-oldSe3.position);
				boundSe3.position=newSe3.position+newSe3.orientation*relPos;
				boundSe3.orientation=newSe3.orientation*relOrient;
				boundSe3.orientation.Normalize();
			}
			renderer->clipPlaneSe3[manipulatedClipPlane]=newSe3;
		}
		renderer->render(Omega::instance().getRootBody(), selectedName());
	}
}

void GLViewer::drawWithNames(){
	if(Omega::instance().getRootBody() && Omega::instance().getRootBody()->bodies->size()<500) renderer->renderWithNames(Omega::instance().getRootBody());
}

// new object selected.
// set frame coordinates, and isDynamic=false;
void GLViewer::postSelection(const QPoint& point) 
{
	int selection = selectedName();
	if(selection<0){
		if(isMoving){
			displayMessage("Moving finished"); mouseMovesCamera(); isMoving=false;
		}
		return;
	}
	if((*(Omega::instance().getRootBody()->bodies)).exists(selection)){
		resetManipulation();
		if(Body::byId(body_id_t(selection))->isClumpMember()){ // select clump (invisible) instead of its member
			LOG_DEBUG("Clump member #"<<selection<<" selected, selecting clump instead.");
			selection=Body::byId(body_id_t(selection))->clumpId;
		}
		setSelectedName(selection);
		//LOG_DEBUG("New selection "<<selection);
		displayMessage("Selected body #"+lexical_cast<string>(selection)+(Body::byId(selection)->isClump()?" (clump)":""));
		wasDynamic=Body::byId(selection)->isDynamic;
		Body::byId(selection)->isDynamic = false;
		Quaternionr& q = Body::byId(selection)->physicalParameters->se3.orientation;
		Vector3r&    v = Body::byId(selection)->physicalParameters->se3.position;
		manipulatedFrame()->setPositionAndOrientation(qglviewer::Vec(v[0],v[1],v[2]),qglviewer::Quaternion(q[0],q[1],q[2],q[3]));
	}

}

// maybe new object will be selected.
// if so, then set isDynamic of previous selection, to old value
void GLViewer::endSelection(const QPoint &point){
	manipulatedClipPlane=-1;
	int old = selectedName();
	QGLViewer::endSelection(point);
	if(old != -1 && old!=selectedName() && (*(Omega::instance().getRootBody()->bodies)).exists(old)) Body::byId(old)->isDynamic = wasDynamic;
}

void GLViewer::postDraw(){
	//if(!(drawGridXYZ[0]||drawGridXYZ[1]||drawGridXYZ[2])) return;
	Real diameter=QGLViewer::camera()->sceneRadius()*2;
	//qglviewer::Vec center=QGLViewer::camera()->sceneCenter();
	Real gridStep=pow(10,(floor(log10(diameter)-.5)));
	int nSegments=((int)(diameter/gridStep));
	Real realSize=nSegments*gridStep;
	//LOG_DEBUG("nSegments="<<nSegments<<",gridStep="<<gridStep<<",realSize="<<realSize);
	glPushMatrix();

	if(drawGridXYZ[0]) {glColor3f(0.6,0.3,0.3); glPushMatrix(); glRotated(90.,0.,1.,0.); QGLViewer::drawGrid(realSize,nSegments); glPopMatrix();}
	if(drawGridXYZ[1]) {glColor3f(0.3,0.6,0.3); glPushMatrix(); glRotated(90.,1.,0.,0.); QGLViewer::drawGrid(realSize,nSegments); glPopMatrix();}
	if(drawGridXYZ[2]) {glColor3f(0.3,0.3,0.6); glPushMatrix(); /*glRotated(90.,0.,1.,0.);*/ QGLViewer::drawGrid(realSize,nSegments); glPopMatrix();}

	for(int planeId=0; planeId<renderer->clipPlaneNum; planeId++){
		if(!renderer->clipPlaneActive[planeId] && planeId!=manipulatedClipPlane) continue;
		glPushMatrix();
			Real angle; Vector3r axis;	
			const Se3r& se3=renderer->clipPlaneSe3[planeId];
			se3.orientation.ToAxisAngle(axis,angle);	
			glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
			glRotated(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
			Real cff=1;
			if(!renderer->clipPlaneActive[planeId]) cff=.4;
			glColor3f(max(0.,cff*cos(planeId)),max(0.,cff*sin(planeId)),planeId==manipulatedClipPlane); // variable colors
			QGLViewer::drawGrid(realSize,2*nSegments);
			drawArrow(diameter/6);
		glPopMatrix();
	}
	QGLViewer::postDraw();
}

void GLViewer::closeEvent(QCloseEvent *e){
	//emit closeSignal(viewId);
	YadeQtMainWindow::self->closeView(this);
}

