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
#include<boost/filesystem/operations.hpp>

CREATE_LOGGER(GLViewer);

GLViewer::GLViewer(int id, shared_ptr<OpenGLRenderingEngine> _renderer, QWidget * parent, QGLWidget * shareWidget) : QGLViewer(parent,"glview",shareWidget)//, qglThread(this,rendererInit)
{
	isMoving=false;
	renderer=_renderer; renderer->init();
	drawGridXYZ[0]=drawGridXYZ[1]=drawGridXYZ[2]=false;
	drawScale=true;
	timeDispMask=TIME_REAL|TIME_VIRT|TIME_ITER;
	viewId = id;
	cut_plane = 0;
	cut_plane_delta = -2;
	resize(550,550);

	if (id==0) setCaption("Primary view");
	else if(id==-1) { setCaption("Player view"); viewId=0; }
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
	setKeyDescription(Qt::Key_C & Qt::ALT,"Set scene center to median body position");
	setKeyDescription(Qt::Key_D,"Toggle time display mask");
	setKeyDescription(Qt::Key_G,"Toggle grid");
	setKeyDescription(Qt::Key_X,"Toggle YZ grid (or: align manipulated clip plane normal with +X)");
	setKeyDescription(Qt::Key_Y,"Toggle XZ grid (or: align manipulated clip plane normal with +Y)");
	setKeyDescription(Qt::Key_Z,"Toggle XY grid (or: align manipulated clip plane normal with +Z)");
	setKeyDescription(Qt::Key_S & Qt::ALT,   "Save QGLViewer state to /tmp/qglviewerState.xml");
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
	assert(planeNo<renderer->clipPlaneNum);
	resetManipulation();
	mouseMovesManipulatedFrame(xyPlaneConstraint.get());
	manipulatedClipPlane=planeNo;
	const Se3r se3(renderer->clipPlaneSe3[planeNo]);
	manipulatedFrame()->setPositionAndOrientation(qglviewer::Vec(se3.position[0],se3.position[1],se3.position[2]),qglviewer::Quaternion(se3.orientation[0],se3.orientation[1],se3.orientation[2],se3.orientation[3]));
	string grp=strBoundGroup();
	displayMessage("Manipulating clip plane #"+lexical_cast<string>(planeNo+1)+(grp.empty()?grp:"(bound planes:"+grp+")"));
}

void GLViewer::useDisplayParameters(size_t n){
	LOG_DEBUG("Loading display parameters from #"<<n);
	vector<shared_ptr<DisplayParameters> >& dispParams=Omega::instance().getRootBody()->dispParams;
	if(dispParams.size()<=(size_t)n){LOG_ERROR("Display parameters #"<<n<<" don't exist (number of entries "<<dispParams.size()<<")"); return;}
	const shared_ptr<DisplayParameters>& dp=dispParams[n];
	string val;
	if(dp->getValue("OpenGLRenderingEngine",val)){ istringstream oglre(val); IOFormatManager::loadFromStream("XMLFormatManager",oglre,"renderer",renderer);}
	else { LOG_WARN("OpenGLRenderingEngine configuration not found in display parameters, skipped.");}
	if(dp->getValue("GLViewer",val)){ GLViewer::setState(val);}
	else { LOG_WARN("GLViewer configuration not found in display parameters, skipped."); }
}

void GLViewer::saveDisplayParameters(size_t n){
	LOG_DEBUG("Saving display parameters to #"<<n);
	vector<shared_ptr<DisplayParameters> >& dispParams=Omega::instance().getRootBody()->dispParams;
	if(dispParams.size()<=n){while(dispParams.size()<=n) dispParams.push_back(shared_ptr<DisplayParameters>(new DisplayParameters));} assert(n<dispParams.size());
	shared_ptr<DisplayParameters>& dp=dispParams[n];
	ostringstream oglre; IOFormatManager::saveToStream("XMLFormatManager",oglre,"renderer",renderer);
	dp->setValue("OpenGLRenderingEngine",oglre.str());
	dp->setValue("GLViewer",GLViewer::getState());
}

string GLViewer::getState(){
	QString origStateFileName=stateFileName();
	char tmpnam_str [L_tmpnam]; tmpnam(tmpnam_str);
	setStateFileName(tmpnam_str); saveStateToFile(); setStateFileName(origStateFileName);
	LOG_DEBUG("State saved to temp file "<<tmpnam_str);
	// read tmp file contents and return it as string
	// this will replace all whitespace by space (nowlines will disappear, which is what we want)
	ifstream in(tmpnam_str); string ret; while(!in.eof()){string ss; in>>ss; ret+=" "+ss;}; in.close();
	boost::filesystem::remove(boost::filesystem::path(tmpnam_str));
	return ret;
}

void GLViewer::setState(string state){
	char tmpnam_str [L_tmpnam]; tmpnam(tmpnam_str);
	std::ofstream out(tmpnam_str);
	if(!out.good()){ LOG_ERROR("Error opening temp file `"<<tmpnam_str<<"', loading aborted."); return; }
	out<<state; out.close();
	LOG_DEBUG("Will load state from temp file "<<tmpnam_str);
	QString origStateFileName=stateFileName(); setStateFileName(tmpnam_str); restoreStateFromFile(); setStateFileName(origStateFileName);
	boost::filesystem::remove(boost::filesystem::path(tmpnam_str));
}

void GLViewer::keyPressEvent(QKeyEvent *e)
{
	last_user_event = boost::posix_time::second_clock::local_time();

	if(false){}
	/* special keys: Escape and Space */
	else if(e->key()==Qt::Key_Escape){ resetManipulation(); displayMessage("Manipulating scene."); }
	else if(e->key()==Qt::Key_Space){
		if(manipulatedClipPlane>=0) {displayMessage("Clip plane #"+lexical_cast<string>(manipulatedClipPlane+1)+(renderer->clipPlaneActive[manipulatedClipPlane]?" de":" ")+"activated"); renderer->clipPlaneActive[manipulatedClipPlane]=!renderer->clipPlaneActive[manipulatedClipPlane]; }
	}
	/* function keys */
	else if(e->key()==Qt::Key_F1 || e->key()==Qt::Key_F2 || e->key()==Qt::Key_F3 /* || ... */ ){
		int n=0; if(e->key()==Qt::Key_F1) n=1; else if(e->key()==Qt::Key_F2) n=2; else if(e->key()==Qt::Key_F3) n=3; assert(n>0); int planeId=n-1;
		if(planeId>=renderer->clipPlaneNum) return;
		if(planeId!=manipulatedClipPlane) startClipPlaneManipulation(planeId);
	}
	/* numbers */
	else if(e->key()==Qt::Key_0 &&(e->state() & AltButton)) { boundClipPlanes.clear(); displayMessage("Cleared bound planes group.");}
	else if(e->key()==Qt::Key_1 || e->key()==Qt::Key_2 || e->key()==Qt::Key_3 /* || ... */ ){
		int n=0; if(e->key()==Qt::Key_1) n=1; else if(e->key()==Qt::Key_2) n=2; else if(e->key()==Qt::Key_3) n=3; assert(n>0); int planeId=n-1;
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
	}
	else if(e->key()==Qt::Key_7 || e->key()==Qt::Key_8 || e->key()==Qt::Key_9){
		int nn=-1; if(e->key()==Qt::Key_7)nn=0; else if(e->key()==Qt::Key_8)nn=1; else if(e->key()==Qt::Key_9)nn=2; assert(nn>=0); size_t n=(size_t)nn;
		if(e->state() & AltButton) saveDisplayParameters(n);
		else useDisplayParameters(n);
	}
	/* letters alphabetically */
	else if(e->key()==Qt::Key_C && selectedName() >= 0 && (*(Omega::instance().getRootBody()->bodies)).exists(selectedName())) setSceneCenter(manipulatedFrame()->position());
	else if(e->key()==Qt::Key_C && (e->state() & AltButton)){ displayMessage("Median centering"); centerMedianQuartile(); }
	//else if(e->key()==Qt::Key_D) wasDynamic=true;
	else if(e->key()==Qt::Key_D) {timeDispMask+=1; if(timeDispMask>(TIME_REAL|TIME_VIRT|TIME_ITER))timeDispMask=0; }
	else if(e->key()==Qt::Key_G) {bool anyDrawn=drawGridXYZ[0]||drawGridXYZ[1]||drawGridXYZ[2]; for(int i=0; i<3; i++)drawGridXYZ[i]=!anyDrawn; }
	else if (e->key()==Qt::Key_M && selectedName() >= 0){
		if(!(isMoving=!isMoving)){displayMessage("Moving done."); mouseMovesCamera();}
		else{ displayMessage("Moving selected object"); mouseMovesManipulatedFrame();}
	}
	else if (e->key() == Qt::Key_T) camera()->setType(camera()->type()==qglviewer::Camera::ORTHOGRAPHIC ? qglviewer::Camera::PERSPECTIVE : qglviewer::Camera::ORTHOGRAPHIC);
	else if(e->key()==Qt::Key_O) camera()->setFieldOfView(camera()->fieldOfView()*0.9);
	else if(e->key()==Qt::Key_P) camera()->setFieldOfView(camera()->fieldOfView()*1.1);
	else if(e->key()==Qt::Key_R){ // reverse the clipping plane; revolve around scene center if no clipping plane selected
		if(manipulatedClipPlane>=0 && manipulatedClipPlane<renderer->clipPlaneNum){
			//manipulatedFrame()->setOrientation(qglviewer::Quaternion(qglviewer::Vec(0,1,0),Mathr::PI)*manipulatedFrame()->orientation());
			renderer->clipPlaneSe3[manipulatedClipPlane].orientation=Quaternionr(Vector3r(0,1,0),Mathr::PI)*renderer->clipPlaneSe3[manipulatedClipPlane].orientation; 
			displayMessage("Plane #"+lexical_cast<string>(manipulatedClipPlane+1)+" reversed.");
		}
		else {
			camera()->setRevolveAroundPoint(sceneCenter());
		}
	}
	else if(e->key()==Qt::Key_S){
		LOG_INFO("Saving QGLViewer state to /tmp/qglviewerState.xml");
		setStateFileName("/tmp/qglviewerState.xml"); saveStateToFile(); setStateFileName(QString::null);
	}
	else if(e->key()==Qt::Key_X || e->key()==Qt::Key_Y || e->key()==Qt::Key_Z){
		int axisIdx=(e->key()==Qt::Key_X?0:(e->key()==Qt::Key_Y?1:2));
		if(manipulatedClipPlane<0){ drawGridXYZ[axisIdx]=!drawGridXYZ[axisIdx]; }
		else{ // align clipping plane with world axis
			// x: (0,1,0),pi/2; y: (0,0,1),pi/2; z: (1,0,0),0
			qglviewer::Vec axis(0,0,0); axis[(axisIdx+1)%3]=1;
			manipulatedFrame()->setOrientation(qglviewer::Quaternion(axis,axisIdx==2?0:Mathr::PI/2));
		}
	}


// FIXME BEGIN - arguments for GLDraw*ers should be from dialog box, not through Omega !!!
	else if(e->key()==Qt::Key_Delete) Omega::instance().isoValue-=0.05;
	else if(e->key()==Qt::Key_Insert) Omega::instance().isoValue+=0.05;
	else if(e->key()==Qt::Key_Next) Omega::instance().isoThick-=0.05;
	else if(e->key()==Qt::Key_Prior)	Omega::instance().isoThick+=0.05;
	else if(e->key()==Qt::Key_End) Omega::instance().isoSec=std::max(1, Omega::instance().isoSec-1);
	else if(e->key()==Qt::Key_Home) Omega::instance().isoSec+=1;
// FIXME END

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
	else if(e->key()!=Qt::Key_Escape && e->key()!=Qt::Key_Space) QGLViewer::keyPressEvent(e);
	updateGL();
}

/* Calculate medians for x, y and z coordinates of all bodies;
 *then set scene center to median position and scene radius to 2*inter-quartile distance.
 *
 * This function eliminates the effect of lonely bodies that went nuts and enlarge
 * the scene's AABB in such a way that fitting the scene to see the AABB makes the
 * "central" (where most bodies is) part very small or even invisible.
 */
void GLViewer::centerMedianQuartile(){
	long nBodies=Omega::instance().getRootBody()->bodies->size();
	if(nBodies<4) {
		LOG_INFO("Less than 4 bodies, median makes no sense; calling centerScene() instead.");
		return centerScene();
	}
	std::vector<Real> coords[3];
	for(int i=0;i<3;i++)coords[i].reserve(nBodies);
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		for(int i=0; i<3; i++) coords[i].push_back(b->physicalParameters->se3.position[i]);
	}
	Vector3r median,interQuart;
	for(int i=0;i<3;i++){
		sort(coords[i].begin(),coords[i].end());
		median[i]=*(coords[i].begin()+nBodies/2);
		interQuart[i]=*(coords[i].begin()+3*nBodies/4)-*(coords[i].begin()+nBodies/4);
	}
	LOG_DEBUG("Median position is"<<median<<", inter-quartile distance is "<<interQuart);
	setSceneCenter(qglviewer::Vec(median[0],median[1],median[2]));
	setSceneRadius(2*(interQuart[0]+interQuart[1]+interQuart[2])/3.);
	showEntireScene();
}

void GLViewer::centerScene(){
	MetaBody* rb=Omega::instance().getRootBody().get();
	if (!rb) return;

	if(rb->bodies->size()<500){LOG_INFO("Less than 500 bodies, moving possible. Select with shift, press 'm' to move.");}
	else{LOG_INFO("More than 500 bodies. Moving not possible.");}
	Vector3r min,max;	
	if(rb->boundingVolume){
		min=rb->boundingVolume->min; max=rb->boundingVolume->max;
		if(std::max(max[0]-min[0],std::max(max[1]-min[1],max[2]-min[2]))<=0){
			// AABB is not yet calculated...
			LOG_DEBUG("rootBody's boundingVolume not yet calculated or has one dimension zero, attempt get that from bodies' positions.");
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
	LOG_DEBUG("Selection is "<<selectedName());
	int selection = selectedName();
	if(selection<0){
		if(isMoving){
			displayMessage("Moving finished"); mouseMovesCamera(); isMoving=false;
		}
		return;
	}
	if(selection>=0 && (*(Omega::instance().getRootBody()->bodies)).exists(selection)){
		resetManipulation();
		if(Body::byId(body_id_t(selection))->isClumpMember()){ // select clump (invisible) instead of its member
			LOG_DEBUG("Clump member #"<<selection<<" selected, selecting clump instead.");
			selection=Body::byId(body_id_t(selection))->clumpId;
		}
		setSelectedName(selection);
		LOG_DEBUG("New selection "<<selection);
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

qglviewer::Vec GLViewer::displayedSceneCenter(){
	return camera()->unprojectedCoordinatesOf(qglviewer::Vec(width()/2 /* pixels */ ,height()/2 /* pixels */, /*middle between near plane and far plane*/ .5));
}

float GLViewer::displayedSceneRadius(){
	return (camera()->unprojectedCoordinatesOf(qglviewer::Vec(width()/2,height()/2,.5))-camera()->unprojectedCoordinatesOf(qglviewer::Vec(0,0,.5))).norm();
}

void GLViewer::postDraw(){
	Real wholeDiameter=QGLViewer::camera()->sceneRadius()*2;
	Real dispDiameter=min(wholeDiameter,max((Real)displayedSceneRadius()*2,wholeDiameter/1e3)); // limit to avoid drawing 1e5 lines with big zoom level
	//qglviewer::Vec center=QGLViewer::camera()->sceneCenter();
	Real gridStep=pow(10,(floor(log10(dispDiameter)-.7)));
	Real scaleStep=pow(10,(floor(log10(displayedSceneRadius()*2)-.7))); // unconstrained
	int nSegments=((int)(wholeDiameter/gridStep))+1;
	Real realSize=nSegments*gridStep;
	//LOG_DEBUG("nSegments="<<nSegments<<",gridStep="<<gridStep<<",realSize="<<realSize);
	glPushMatrix();

	// XYZ grids
	glLineWidth(.5);
	if(drawGridXYZ[0]) {glColor3f(0.6,0.3,0.3); glPushMatrix(); glRotated(90.,0.,1.,0.); QGLViewer::drawGrid(realSize,nSegments); glPopMatrix();}
	if(drawGridXYZ[1]) {glColor3f(0.3,0.6,0.3); glPushMatrix(); glRotated(90.,1.,0.,0.); QGLViewer::drawGrid(realSize,nSegments); glPopMatrix();}
	if(drawGridXYZ[2]) {glColor3f(0.3,0.3,0.6); glPushMatrix(); /*glRotated(90.,0.,1.,0.);*/ QGLViewer::drawGrid(realSize,nSegments); glPopMatrix();}
	
	// scale
	if(drawScale){
		Real segmentSize=scaleStep;
		qglviewer::Vec screenDxDy[3]; // dx,dy for x,y,z scale segments
		int extremalDxDy[2];
		for(int axis=0; axis<3; axis++){
			qglviewer::Vec delta(0,0,0); delta[axis]=segmentSize;
			qglviewer::Vec center=displayedSceneCenter();
			screenDxDy[axis]=camera()->projectedCoordinatesOf(center+delta)-camera()->projectedCoordinatesOf(center);
			for(int xy=0;xy<2;xy++)extremalDxDy[xy]=(axis>0 ? min(extremalDxDy[xy],(int)screenDxDy[axis][xy]) : screenDxDy[axis][xy]);
		}
		//LOG_DEBUG("Screen offsets for axes: "<<" x("<<screenDxDy[0][0]<<","<<screenDxDy[0][1]<<") y("<<screenDxDy[1][0]<<","<<screenDxDy[1][1]<<") z("<<screenDxDy[2][0]<<","<<screenDxDy[2][1]<<")");
		int margin=10; // screen pixels
		int scaleCenter[2]; scaleCenter[0]=abs(extremalDxDy[0])+margin; scaleCenter[1]=abs(extremalDxDy[1])+margin;
		//LOG_DEBUG("Center of scale "<<scaleCenter[0]<<","<<scaleCenter[1]);
		//displayMessage(QString().sprintf("displayed scene radius %g",displayedSceneRadius()));
		startScreenCoordinatesSystem();
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);
			glLineWidth(3.0);
			for(int axis=0; axis<3; axis++){
				Vector3r color(.4,.4,.4); color[axis]=.9;
				glColor3v(color);
				glBegin(GL_LINES);
				glVertex2f(scaleCenter[0],scaleCenter[1]);
				glVertex2f(scaleCenter[0]+screenDxDy[axis][0],scaleCenter[1]+screenDxDy[axis][1]);
				glEnd();
			}
			glLineWidth(1.);
			glEnable(GL_DEPTH_TEST);
			QGLViewer::drawText(scaleCenter[0],scaleCenter[1],QString().sprintf("%.3g",scaleStep));
		stopScreenCoordinatesSystem();
	}

	// cutting planes (should be moved to OpenGLRenderingEngine perhaps?)
	// TODO: transparent planes
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
			drawArrow(wholeDiameter/6);
		glPopMatrix();
	}

	#define _W3 setw(3)<<setfill('0')
	#define _W2 setw(2)<<setfill('0')
	if(timeDispMask!=0){
		const int lineHt=12;
		unsigned x=10,y=height()-20;
		glColor3v(Vector3r(1,1,1));
		if(timeDispMask & GLViewer::TIME_VIRT){
			ostringstream oss;
			Real t=Omega::instance().getSimulationTime();
			unsigned min=((unsigned)t/60),sec=(((unsigned)t)%60),msec=((unsigned)(1e3*t))%1000,usec=((unsigned long)(1e6*t))%1000,nsec=((unsigned long)(1e9*t))%1000;
			if(min>0) oss<<_W2<<min<<":"<<_W2<<sec<<"."<<_W3<<msec<<"m"<<_W3<<usec<<"u"<<_W3<<nsec<<"n";
			else if (sec>0) oss<<_W2<<sec<<"."<<_W3<<msec<<"m"<<_W3<<usec<<"u"<<_W3<<nsec<<"n";
			else if (msec>0) oss<<_W3<<msec<<"m"<<_W3<<usec<<"u"<<_W3<<nsec<<"n";
			else if (usec>0) oss<<_W3<<usec<<"u"<<_W3<<nsec<<"n";
			else oss<<_W3<<nsec<<"ns";
			QGLViewer::drawText(x,y,oss.str());
			y-=lineHt;
		}
		glColor3v(Vector3r(0,.5,.5));
		if(timeDispMask & GLViewer::TIME_REAL){
			QGLViewer::drawText(x,y,getRealTimeString() /* virtual, since player gets that from db */);
			y-=lineHt;
		}
		if(timeDispMask & GLViewer::TIME_ITER){
			ostringstream oss;
			oss<<"#"<<Omega::instance().getCurrentIteration()<<"\n";
			QGLViewer::drawText(x,y,oss.str());
			y-=lineHt;
		}
	}
	QGLViewer::postDraw();
}

string GLViewer::getRealTimeString(){
	ostringstream oss;
	time_duration t=Omega::instance().getComputationDuration();
	unsigned d=t.hours()/24,h=t.hours()%24,m=t.minutes(),s=t.seconds();
	oss<<"wall ";
	if(d>0) oss<<d<<"days "<<_W2<<h<<":"<<_W2<<m<<":"<<_W2<<s;
	else if(h>0) oss<<_W2<<h<<":"<<_W2<<m<<":"<<_W2<<s;
	else oss<<_W2<<m<<":"<<_W2<<s;
	return oss.str();
}
#undef _W2
#undef _W3

void GLViewer::closeEvent(QCloseEvent *e){
	//emit closeSignal(viewId);
	YadeQtMainWindow::self->closeView(this);
}

void GLViewer::mouseMoveEvent(QMouseEvent *e){
	last_user_event = boost::posix_time::second_clock::local_time();
	QGLViewer::mouseMoveEvent(e);
}

void GLViewer::mousePressEvent(QMouseEvent *e){
	last_user_event = boost::posix_time::second_clock::local_time();
	QGLViewer::mousePressEvent(e);
}

/* Handle double-click event; if clipping plane is manipulated, align it with the global coordinate system.
 * Otherwise pass the event to QGLViewer to handle it normally.
 *
 * mostly copied over from ManipulatedFrame::mouseDoubleClickEvent
 */
void GLViewer::mouseDoubleClickEvent(QMouseEvent *event){
	last_user_event = boost::posix_time::second_clock::local_time();

	if(manipulatedClipPlane<0) { /* LOG_DEBUG("Double click not on clipping plane"); */ QGLViewer::mouseDoubleClickEvent(event); return; }
#if QT_VERSION >= 0x040000
	if (event->modifiers() == Qt::NoModifier)
#else
	if (event->state() == Qt::NoButton)
#endif
	switch (event->button()){
		case Qt::LeftButton:  manipulatedFrame()->alignWithFrame(NULL,true); LOG_DEBUG("Aligning cutting plane"); break;
		// case Qt::RightButton: projectOnLine(camera->position(), camera->viewDirection()); break;
		default: break; // avoid warning
	}
}

void GLViewer::wheelEvent(QWheelEvent* event){
	last_user_event = boost::posix_time::second_clock::local_time();

	if(manipulatedClipPlane<0){ QGLViewer::wheelEvent(event); return; }
	assert(manipulatedClipPlane<renderer->clipPlaneNum);
	float distStep=1e-3*sceneRadius();
	//const float wheelSensitivityCoef = 8E-4f;
	//Vec trans(0.0, 0.0, -event->delta()*wheelSensitivity()*wheelSensitivityCoef*(camera->position()-position()).norm());
	float dist=-event->delta()*manipulatedFrame()->wheelSensitivity()*distStep;
	Vector3r normal=renderer->clipPlaneSe3[manipulatedClipPlane].orientation*Vector3r(0,0,1);
	qglviewer::Vec nnormal(normal[0],normal[1],normal[2]);
	manipulatedFrame()->setPosition(manipulatedFrame()->position()+nnormal*dist);
	/* in draw, bound cutting planes will be moved as well */
}

// cut&paste from QGLViewer::domElement documentation
QDomElement GLViewer::domElement(const QString& name, QDomDocument& document) const{
	QDomElement de=document.createElement("gridXYZ");
	string val; if(drawGridXYZ[0])val+="x"; if(drawGridXYZ[1])val+="y"; if(drawGridXYZ[2])val+="z";
	de.setAttribute("normals",val);
	QDomElement de2=document.createElement("timeDisplay"); de2.setAttribute("mask",timeDispMask);
	QDomElement res=QGLViewer::domElement(name,document);
	res.appendChild(de);
	res.appendChild(de2);
	return res;
}

// cut&paste from QGLViewer::initFromDomElement documentation
void GLViewer::initFromDOMElement(const QDomElement& element){
	QGLViewer::initFromDOMElement(element);
	QDomElement child=element.firstChild().toElement();
	while (!child.isNull()){
		if (child.tagName()=="gridXYZ" && child.hasAttribute("normals")){
			string val=child.attribute("normals").lower();
			drawGridXYZ[0]=false; drawGridXYZ[1]=false; drawGridXYZ[2]=false;
			if(val.find("x")!=string::npos)drawGridXYZ[0]=true; if(val.find("y")!=string::npos)drawGridXYZ[1]=true; if(val.find("z")!=string::npos)drawGridXYZ[2]=true;
		}
		if(child.tagName()=="timeDisplay" && child.hasAttribute("mask")) timeDispMask=atoi(child.attribute("mask").ascii());
		child = child.nextSibling().toElement();
	}
}

boost::posix_time::ptime GLViewer::getLastUserEvent(){return last_user_event;};

