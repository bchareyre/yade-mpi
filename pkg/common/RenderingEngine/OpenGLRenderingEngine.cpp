// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#include"OpenGLRenderingEngine.hpp"
#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/lib-opengl/GLUtils.hpp>
#include<yade/core/Timing.hpp>

#ifdef __APPLE__
#  include <OpenGL/glu.h>
#  include <OpenGL/gl.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glu.h>
#  include <GL/gl.h>
#  include <GL/glut.h>
#endif

YADE_PLUGIN((OpenGLRenderingEngine));
YADE_REQUIRE_FEATURE(OPENGL)
CREATE_LOGGER(OpenGLRenderingEngine);

bool OpenGLRenderingEngine::glutInitDone=false;
size_t OpenGLRenderingEngine::selectBodyLimit=500;

OpenGLRenderingEngine::OpenGLRenderingEngine() : RenderingEngine(), clipPlaneNum(3){
	Show_DOF = false;
	Show_ID = false;
	Body_state = false;
	Body_bounding_volume = false;
	Body_interacting_geom = true;
	Body_wire = false;
	Interaction_wire = false;
	Draw_inside = true;
	Draw_mask = ~0;
	Light_position = Vector3r(75.0,130.0,0.0);
	Background_color = Vector3r(0.2,0.2,0.2);
	Interaction_geometry = false;
	Interaction_physics = false;

	scaleDisplacements=false; scaleRotations=false;
	displacementScale=Vector3r(1,1,1); rotationScale=1;

	for(int i=0; i<clipPlaneNum; i++){clipPlaneSe3.push_back(Se3r(Vector3r::ZERO,Quaternionr::IDENTITY)); clipPlaneActive.push_back(false); clipPlaneNormals.push_back(Vector3r(1,0,0));}
	
	map<string,DynlibDescriptor>::const_iterator di = Omega::instance().getDynlibsDescriptor().begin();
	map<string,DynlibDescriptor>::const_iterator diEnd = Omega::instance().getDynlibsDescriptor().end();
	for(;di!=diEnd;++di){
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawStateFunctor")) addStateFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawBoundingVolumeFunctor")) addBoundingVolumeFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawInteractingGeometryFunctor")) addInteractingGeometryFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawInteractionGeometryFunctor")) addInteractionGeometryFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawInteractionPhysicsFunctor")) addInteractionPhysicsFunctor((*di).first);
	}
	postProcessAttributes(true);
}

OpenGLRenderingEngine::~OpenGLRenderingEngine(){}

void OpenGLRenderingEngine::init(){
	if (glutInitDone) return;
	glutInit(&Omega::instance().origArgc,Omega::instance().origArgv);
	/* transparent spheres (still not working): glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_ALPHA); glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE); */
	glutInitDone=true;
}

void OpenGLRenderingEngine::setBodiesRefSe3(const shared_ptr<World>& rootBody){
	LOG_DEBUG("(re)initializing reference positions and orientations.");
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies) if(b && b->state) { b->state->refPos=b->state->pos; b->state->refOri=b->state->ori; }
}


void OpenGLRenderingEngine::initgl(){
	LOG_INFO("(re)initializing GL for gldraw methods.\n");
	BOOST_FOREACH(vector<string>& s,stateFunctorNames)
		(static_pointer_cast<GLDrawStateFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
	BOOST_FOREACH(vector<string>& s,boundingVolumeFunctorNames)
		(static_pointer_cast<GLDrawBoundingVolumeFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
	BOOST_FOREACH(vector<string>& s,interactingGeometryFunctorNames)
		(static_pointer_cast<GLDrawInteractingGeometryFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
	BOOST_FOREACH(vector<string>& s,interactionGeometryFunctorNames)
		(static_pointer_cast<GLDrawInteractionGeometryFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
	BOOST_FOREACH(vector<string>& s,interactionPhysicsFunctorNames)
		(static_pointer_cast<GLDrawInteractionPhysicsFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
}

void OpenGLRenderingEngine::renderWithNames(const shared_ptr<World>& rootBody){
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b || !b->interactingGeometry) continue;
		glPushMatrix();
		const Se3r& se3=b->state->se3;
		Real angle; Vector3r axis;	se3.orientation.ToAxisAngle(axis,angle);	
		glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
		glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
		//if(b->interactingGeometry->getClassName() != "LineSegment"){ // FIXME: a body needs to say: I am selectable ?!?!
			glPushName(b->getId());
			interactingGeometryDispatcher(b->interactingGeometry,b->state,Body_wire || b->interactingGeometry->wire);
			glPopName();
		//}
		glPopMatrix();
	}
};

bool OpenGLRenderingEngine::pointClipped(const Vector3r& p){
	if(clipPlaneNum<1) return false;
	for(int i=0;i<clipPlaneNum;i++) if(clipPlaneActive[i]&&(p-clipPlaneSe3[i].position).Dot(clipPlaneNormals[i])<0) return true;
	return false;
}

/* mostly copied from PeriodicInsertionSortCollider
 	FIXME: common implementation somewhere */

Real OpenGLRenderingEngine::wrapCell(const Real x, const Real x0, const Real x1){
	Real xNorm=(x-x0)/(x1-x0);
	return x0+(xNorm-floor(xNorm))*(x1-x0);
}
Vector3r OpenGLRenderingEngine::wrapCellPt(const Vector3r& pt, World* rb){
	if(!rb->isPeriodic) return pt;
	return Vector3r(wrapCell(pt[0],rb->cellMin[0],rb->cellMax[0]),wrapCell(pt[1],rb->cellMin[1],rb->cellMax[1]),wrapCell(pt[2],rb->cellMin[2],rb->cellMax[2]));
}

void OpenGLRenderingEngine::setBodiesDispInfo(const shared_ptr<World>& rootBody){
	if(rootBody->bodies->size()!=bodyDisp.size()) bodyDisp.resize(rootBody->bodies->size());
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b || !b->state) continue;
		size_t id=b->getId();
		const Vector3r& pos=b->state->pos; const Vector3r& refPos=b->state->refPos;
		const Quaternionr& ori=b->state->ori; const Quaternionr& refOri=b->state->refOri;
		Vector3r posCell=wrapCellPt(pos,rootBody.get());
		bodyDisp[id].isDisplayed=!pointClipped(posCell);	
		// if no scaling and no periodic, return quickly
		if(!(scaleDisplacements||scaleRotations||rootBody->isPeriodic)){ bodyDisp[id].pos=pos; bodyDisp[id].ori=ori; continue; }
		// apply scaling
		bodyDisp[id].pos=(scaleDisplacements ? diagMult(displacementScale,pos-refPos)+wrapCellPt(refPos,rootBody.get()) : posCell );
		if(!scaleRotations) bodyDisp[id].ori=ori;
		else{
			Quaternionr relRot=refOri.Conjugate()*ori;
			Vector3r axis; Real angle; relRot.ToAxisAngle(axis,angle);
			angle*=rotationScale;
			bodyDisp[id].ori=refOri*Quaternionr(axis,angle);
		}
	}
}

// draw periodic cell, if active
void OpenGLRenderingEngine::drawPeriodicCell(World* rootBody){
	if(!rootBody->isPeriodic) return;
	glPushMatrix();
		glColor3v(Vector3r(1,1,0));
		Vector3r cent=.5*(rootBody->cellMin+rootBody->cellMax); Vector3r size=rootBody->cellMax-rootBody->cellMin;
		glTranslate(cent[0],cent[1],cent[2]); glScale(size[0],size[1],size[2]);
		glutWireCube(1);
	glPopMatrix();
}



void OpenGLRenderingEngine::render(const shared_ptr<World>& rootBody, body_id_t selection	/*FIXME: not sure. maybe a list of selections, or maybe bodies themselves should remember if they are selected? */) {

	assert(glutInitDone);
	current_selection = selection;

	// recompute emissive light colors for highlighted bodies
	Real now=TimingInfo::getNow(/*even if timing is disabled*/true)*1e-9;
	highlightEmission0[0]=highlightEmission0[1]=highlightEmission0[2]=.8*normSquare(now,1);
	highlightEmission1[0]=highlightEmission1[1]=highlightEmission0[2]=.5*normSaw(now,2);
		

	// Draw light source
	const GLfloat pos[4]	= {Light_position[0],Light_position[1],Light_position[2],1.0};
	const GLfloat ambientColor[4]={0.5,0.5,0.5,1.0};	
	//const GLfloat specularColor[4]={0.5,0.5,0.5,1.0};	
	glClearColor(Background_color[0],Background_color[1],Background_color[2],1.0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColor);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,1); // important: do lighting calculations on both sides of polygons
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glPushMatrix();
		glTranslatef(Light_position[0],Light_position[1],Light_position[2]);
		glColor3f(1.0,1.0,1.0);
		glutSolidSphere(3,10,10);
	glPopMatrix();	

	// clipping
	assert(clipPlaneNormals.size()==(size_t)clipPlaneNum);
	for(size_t i=0;i<(size_t)clipPlaneNum; i++){
		// someone could have modified those from python and truncate the vectors; fill those here in that case
		if(i==clipPlaneSe3.size()) clipPlaneSe3.push_back(Se3r(Vector3r::ZERO,Quaternionr::IDENTITY));
		if(i==clipPlaneActive.size()) clipPlaneActive.push_back(false);
		if(i==clipPlaneNormals.size()) clipPlaneNormals.push_back(Vector3r::UNIT_X);
		// end filling stuff modified from python
		if(clipPlaneActive[i]) clipPlaneNormals[i]=clipPlaneSe3[i].orientation*Vector3r(0,0,1);
		/* glBegin(GL_LINES);glVertex3v(clipPlaneSe3[i].position);glVertex3v(clipPlaneSe3[i].position+clipPlaneNormals[i]);glEnd(); */
	}

	// set displayed Se3 of body (scaling) and isDisplayed (clipping)
	setBodiesDispInfo(rootBody);

	drawPeriodicCell(rootBody.get());

	if (Show_DOF || Show_ID) renderDOF_ID(rootBody);
	#ifdef YADE_PHYSPAR
		if (Body_state) renderState(rootBody);
	#endif
	if (Body_bounding_volume) renderBoundingVolume(rootBody);
	if (Body_interacting_geom){
		glEnable(GL_LIGHTING);
		glEnable(GL_CULL_FACE);
		renderInteractingGeometry(rootBody);
	}
	if (Interaction_geometry) renderInteractionGeometry(rootBody);
	if (Interaction_physics) renderInteractionPhysics(rootBody);
}

void OpenGLRenderingEngine::renderDOF_ID(const shared_ptr<World>& rootBody){	
	const GLfloat ambientColorSelected[4]={10.0,0.0,0.0,1.0};	
	const GLfloat ambientColorUnselected[4]={0.5,0.5,0.5,1.0};	
	FOREACH(const shared_ptr<Body> b, *rootBody->bodies){
		if(!b) continue;
		if(b->interactingGeometry && ((b->getGroupMask() & Draw_mask) || b->getGroupMask()==0)){
			if(b->state /* && FIXME: !b->physicalParameters->isDisplayed */) continue;
			if(!Show_ID && b->state->blockedDOFs==0) continue;
			const Se3r& se3=b->state->se3; // FIXME: should be dispSe3
			glPushMatrix();
			glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
			if(current_selection==b->getId()){glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColorSelected);}
			{ // write text
				glColor3f(1.0-Background_color[0],1.0-Background_color[1],1.0-Background_color[2]);
				unsigned DOF = b->state->blockedDOFs;
				std::string dof = std::string("") 
										+ (((DOF & State::DOF_X )!=0)?"X":" ")
										+ (((DOF & State::DOF_Y )!=0)?"Y":" ")
										+ (((DOF & State::DOF_Z )!=0)?"Z":" ")
										+ (((DOF & State::DOF_RX)!=0)?"RX":"  ")
										+ (((DOF & State::DOF_RY)!=0)?"RY":"  ")
										+ (((DOF & State::DOF_RZ)!=0)?"RZ":"  ");
				std::string id = boost::lexical_cast<std::string>(b->getId());
				std::string str("");
				if(Show_DOF && Show_ID) id += " ";
				if(Show_ID) str += id;
				if(Show_DOF) str += dof;
				glPushMatrix();
				glRasterPos2i(0,0);
				for(unsigned int i=0;i<str.length();i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
				glPopMatrix();
			}
			if(current_selection == b->getId()){glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColorUnselected);}
			glPopMatrix();
		}
	}
	if(rootBody->interactingGeometry) interactingGeometryDispatcher(rootBody->interactingGeometry,rootBody->state,Body_wire);
}

void OpenGLRenderingEngine::renderInteractionGeometry(const shared_ptr<World>& rootBody){	
	{
		boost::mutex::scoped_lock lock(rootBody->interactions->drawloopmutex);
		FOREACH(const shared_ptr<Interaction>& I, *rootBody->interactions){
			if(!I->interactionGeometry) continue;
			const shared_ptr<Body>& b1=Body::byId(I->getId1(),rootBody), b2=Body::byId(I->getId2(),rootBody);
			if(!(bodyDisp[I->getId1()].isDisplayed||bodyDisp[I->getId2()].isDisplayed)) continue;
			glPushMatrix(); interactionGeometryDispatcher(I->interactionGeometry,I,b1,b2,Interaction_wire); glPopMatrix();
		}
	}
}


void OpenGLRenderingEngine::renderInteractionPhysics(const shared_ptr<World>& rootBody){	
	{
		boost::mutex::scoped_lock lock(rootBody->interactions->drawloopmutex);
		FOREACH(const shared_ptr<Interaction>& I, *rootBody->interactions){
			if(!I->interactionPhysics) continue;
			const shared_ptr<Body>& b1=Body::byId(I->getId1(),rootBody), b2=Body::byId(I->getId2(),rootBody);
			body_id_t id1=I->getId1(), id2=I->getId2();
			if(!(bodyDisp[id1].isDisplayed||bodyDisp[id2].isDisplayed)) continue;
			glPushMatrix(); interactionPhysicsDispatcher(I->interactionPhysics,I,b1,b2,Interaction_wire); glPopMatrix();
		}
	}
}

#ifdef YADE_PHYSPAR
void OpenGLRenderingEngine::renderState(const shared_ptr<World>& rootBody){	
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b) continue;
		if(b->state /* && !b->state->isDisplayed*/ ) continue;
		if(b->state && ((b->getGroupMask()&Draw_mask) || b->getGroupMask()==0)){
			glPushMatrix(); stateDispatcher(b->state); glPopMatrix();
		}
	}
	if(rootBody->state){ glPushMatrix(); stateDispatcher(rootBody->state); glPopMatrix();}
}
#endif

void OpenGLRenderingEngine::renderBoundingVolume(const shared_ptr<World>& rootBody){	
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b || !b->boundingVolume) continue;
		if(!bodyDisp[b->getId()].isDisplayed) continue;
		if(b->boundingVolume && ((b->getGroupMask()&Draw_mask) || b->getGroupMask()==0)){
			glPushMatrix(); boundingVolumeDispatcher(b->boundingVolume); glPopMatrix();
		}
	}
	if(rootBody->boundingVolume){ glPushMatrix(); boundingVolumeDispatcher(rootBody->boundingVolume); glPopMatrix(); }
}


void OpenGLRenderingEngine::renderInteractingGeometry(const shared_ptr<World>& rootBody)
{
	// Additional clipping planes: http://fly.srk.fer.hr/~unreal/theredbook/chapter03.html
	#if 0
		GLdouble clip0[4]={.0,1.,0.,0.}; // y<0
		glClipPlane(GL_CLIP_PLANE0,clip0);
		glEnable(GL_CLIP_PLANE0);
	#endif

	const GLfloat ambientColorSelected[4]={10.0,0.0,0.0,1.0};	
	const GLfloat ambientColorUnselected[4]={0.5,0.5,0.5,1.0};

	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b || !b->interactingGeometry) continue;
		if(!bodyDisp[b->getId()].isDisplayed) continue;
		Vector3r pos=bodyDisp[b->getId()].pos;
		Quaternionr ori=bodyDisp[b->getId()].ori;
		if(!b->interactingGeometry || !((b->getGroupMask()&Draw_mask) || b->getGroupMask()==0)) continue;
		glPushMatrix();
			Real angle;	Vector3r axis;	ori.ToAxisAngle(axis,angle);	
			glTranslatef(pos[0],pos[1],pos[2]);
			glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
			if(current_selection==b->getId() || b->interactingGeometry->highlight){
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColorSelected);
				glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
				const Vector3r& h(current_selection==b->getId() ? highlightEmission0 : highlightEmission1);
				glColor4(h[0],h[1],h[2],.2);
				glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
				///
				interactingGeometryDispatcher(b->interactingGeometry,b->state,Body_wire || b->interactingGeometry->wire,viewInfo);
				///
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColorUnselected);
				glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
				glColor3v(Vector3r::ZERO);
				glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
			} else {
				interactingGeometryDispatcher(b->interactingGeometry,b->state,Body_wire || b->interactingGeometry->wire,viewInfo);
			}
		glPopMatrix();
		if(current_selection==b->getId() || b->interactingGeometry->highlight){
			if(!b->boundingVolume || Body_wire || b->interactingGeometry->wire) GLUtils::GLDrawInt(b->getId(),pos);
			else {
				// move the label towards the camera by the bounding box so that it is not hidden inside the body
				const Vector3r& mn=b->boundingVolume->min; const Vector3r& mx=b->boundingVolume->max; const Vector3r& p=pos;
				Vector3r ext(viewDirection[0]>0?p[0]-mn[0]:p[0]-mx[0],viewDirection[1]>0?p[1]-mn[1]:p[1]-mx[1],viewDirection[2]>0?p[2]-mn[2]:p[2]-mx[2]); // signed extents towards the camera
				Vector3r dr=-1.01*(viewDirection.Dot(ext)*viewDirection);
				GLUtils::GLDrawInt(b->getId(),pos+dr,Vector3r::ONE);
			}
		}
		// if the body goes over the cell margin, draw it in positions where the bbox overlaps with the cell in wire
		// precondition: pos is inside the cell.
		if(b->boundingVolume && rootBody->isPeriodic){
			const Vector3r& cellMin(rootBody->cellMin); const Vector3r& cellMax(rootBody->cellMax); Vector3r cellSize=cellMax-cellMin;
			// traverse all periodic cells around the body, to see if any of them touches
			Vector3r halfSize=b->boundingVolume->max-b->boundingVolume->min; halfSize*=.5;
			Vector3r pmin,pmax;
			Vector3<int> i;
			for(i[0]=-1; i[0]<=1; i[0]++) for(i[1]=-1;i[1]<=1; i[1]++) for(i[2]=-1; i[2]<=1; i[2]++){
				if(i[0]==0 && i[1]==0 && i[2]==0) continue; // middle; already rendered above
				Vector3r pt=pos+Vector3r(cellSize[0]*i[0],cellSize[1]*i[1],cellSize[2]*i[2]);
				pmin=pt-halfSize; pmax=pt+halfSize;
				if(pmin[0]<=cellMax[0] && pmax[0]>=cellMin[0] &&
					pmin[1]<=cellMax[1] && pmax[1]>=cellMin[1] &&
					pmin[2]<=cellMax[2] && pmax[2]>=cellMin[2]) {
					glPushMatrix();
						glTranslatev(pt);
						glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
						interactingGeometryDispatcher(b->interactingGeometry,b->state,/*Body_wire*/ true, viewInfo);
					glPopMatrix();
				}
			}
		}
	}
	if(rootBody->interactingGeometry){ glPushMatrix(); interactingGeometryDispatcher(rootBody->interactingGeometry,rootBody->state,Body_wire,viewInfo); glPopMatrix(); }
}



void OpenGLRenderingEngine::postProcessAttributes(bool deserializing){
	if(!deserializing) return;
	#ifdef YADE_PHYSPAR
		for(unsigned int i=0;i<stateFunctorNames.size();i++) stateDispatcher.add1DEntry(stateFunctorNames[i][0],stateFunctorNames[i][1]);
	#endif
	for(unsigned int i=0;i<boundingVolumeFunctorNames.size();i++) boundingVolumeDispatcher.add1DEntry(boundingVolumeFunctorNames[i][0],boundingVolumeFunctorNames[i][1]);
	for(unsigned int i=0;i<interactingGeometryFunctorNames.size();i++) interactingGeometryDispatcher.add1DEntry(interactingGeometryFunctorNames[i][0],interactingGeometryFunctorNames[i][1]);
	for(unsigned int i=0;i<interactionGeometryFunctorNames.size();i++) interactionGeometryDispatcher.add1DEntry(interactionGeometryFunctorNames[i][0],interactionGeometryFunctorNames[i][1]);
	for(unsigned int i=0;i<interactionPhysicsFunctorNames.size();i++) interactionPhysicsDispatcher.add1DEntry(interactionPhysicsFunctorNames[i][0],interactionPhysicsFunctorNames[i][1]);	
}
void OpenGLRenderingEngine::addInteractionGeometryFunctor(const string& str2){
	string str1 = (static_pointer_cast<GLDrawInteractionGeometryFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v; v.push_back(str1); v.push_back(str2); interactionGeometryFunctorNames.push_back(v);
}
void OpenGLRenderingEngine::addInteractionPhysicsFunctor(const string& str2){
	string str1 = (static_pointer_cast<GLDrawInteractionPhysicsFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v; v.push_back(str1); v.push_back(str2); interactionPhysicsFunctorNames.push_back(v);
}
void OpenGLRenderingEngine::addStateFunctor(const string& str2){
	string str1 = (static_pointer_cast<GLDrawStateFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v; v.push_back(str1); v.push_back(str2); stateFunctorNames.push_back(v);
}
void OpenGLRenderingEngine::addBoundingVolumeFunctor(const string& str2){
	string str1 = (static_pointer_cast<GLDrawBoundingVolumeFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v; v.push_back(str1); v.push_back(str2); boundingVolumeFunctorNames.push_back(v);
}
void OpenGLRenderingEngine::addInteractingGeometryFunctor(const string& str2){
	string str1 = (static_pointer_cast<GLDrawInteractingGeometryFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v; v.push_back(str1); v.push_back(str2); interactingGeometryFunctorNames.push_back(v);
}
