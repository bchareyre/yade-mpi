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
	Body_interacting_geom = false;
	#ifdef YADE_SHAPE
		Body_geometrical_model = true;
	#endif
	Cast_shadows = false;
	Shadow_volumes = false;
	Fast_shadow_volume = true;
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
	numBodiesWhenRefSe3LastSet=0;

	for(int i=0; i<clipPlaneNum; i++){clipPlaneSe3.push_back(Se3r(Vector3r::ZERO,Quaternionr::IDENTITY)); clipPlaneActive.push_back(false); clipPlaneNormals.push_back(Vector3r(1,0,0));}
	
	map<string,DynlibDescriptor>::const_iterator di = Omega::instance().getDynlibsDescriptor().begin();
	map<string,DynlibDescriptor>::const_iterator diEnd = Omega::instance().getDynlibsDescriptor().end();
	for(;di!=diEnd;++di){
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawStateFunctor")) addStateFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawBoundingVolumeFunctor")) addBoundingVolumeFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawInteractingGeometryFunctor")) addInteractingGeometryFunctor((*di).first);
		#ifdef YADE_SHAPE
			if (Omega::instance().isInheritingFrom((*di).first,"GLDrawGeometricalModelFunctor")) addGeometricalModelFunctor((*di).first);
			if (Omega::instance().isInheritingFrom((*di).first,"GLDrawShadowVolumeFunctor")) addShadowVolumeFunctor((*di).first);
		#endif
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

void OpenGLRenderingEngine::initgl(){
	LOG_INFO("(re)initializing GL for gldraw methods.\n");
	BOOST_FOREACH(vector<string>& s,stateFunctorNames)
		(static_pointer_cast<GLDrawStateFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
	BOOST_FOREACH(vector<string>& s,boundingVolumeFunctorNames)
		(static_pointer_cast<GLDrawBoundingVolumeFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
	BOOST_FOREACH(vector<string>& s,interactingGeometryFunctorNames)
		(static_pointer_cast<GLDrawInteractingGeometryFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
	#ifdef YADE_SHAPE
		BOOST_FOREACH(vector<string>& s,geometricalModelFunctorNames)
			(static_pointer_cast<GLDrawGeometricalModelFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
	#endif
	BOOST_FOREACH(vector<string>& s,interactionGeometryFunctorNames)
		(static_pointer_cast<GLDrawInteractionGeometryFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
	BOOST_FOREACH(vector<string>& s,interactionPhysicsFunctorNames)
		(static_pointer_cast<GLDrawInteractionPhysicsFunctor>(ClassFactory::instance().createShared(s[1])))->initgl();
}

void OpenGLRenderingEngine::renderWithNames(const shared_ptr<MetaBody>& rootBody){
	#ifdef YADE_SHAPE
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b || !b->geometricalModel) continue;
		glPushMatrix();
		const Se3r& se3=b->physicalParameters->se3;
		Real angle; Vector3r axis;	se3.orientation.ToAxisAngle(axis,angle);	
		glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
		glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
		if(b->geometricalModel->getClassName() != "LineSegment"){ // FIXME: a body needs to say: I am selectable ?!?!
			glPushName(b->getId());
			geometricalModelDispatcher(b->geometricalModel,b->physicalParameters,/* always solid, not wireframe */false);
			glPopName();
		}
		glPopMatrix();
	}
	#endif
};

bool OpenGLRenderingEngine::pointClipped(const Vector3r& p){
	if(clipPlaneNum<1) return false;
	for(int i=0;i<clipPlaneNum;i++) if(clipPlaneActive[i]&&(p-clipPlaneSe3[i].position).Dot(clipPlaneNormals[i])<0) return true;
	return false;
}

void OpenGLRenderingEngine::setBodiesRefSe3(const shared_ptr<MetaBody>& rootBody){
	LOG_DEBUG("(re)initializing reference positions and orientations.");
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies) if(b && b->physicalParameters) b->physicalParameters->refSe3=b->physicalParameters->se3;
	numBodiesWhenRefSe3LastSet=rootBody->bodies->size();
	numIterWhenRefSe3LastSet=Omega::instance().getCurrentIteration();
}
/* mostly copied from PeriodicInsertionSortCollider
 	FIXME: common implementation somewhere */

Real OpenGLRenderingEngine::wrapCell(const Real x, const Real x0, const Real x1){
	Real xNorm=(x-x0)/(x1-x0);
	return x0+(xNorm-floor(xNorm))*(x1-x0);
}
Vector3r OpenGLRenderingEngine::wrapCellPt(const Vector3r& pt, MetaBody* rb){
	if(!rb->isPeriodic) return pt;
	return Vector3r(wrapCell(pt[0],rb->cellMin[0],rb->cellMax[0]),wrapCell(pt[1],rb->cellMin[1],rb->cellMax[1]),wrapCell(pt[2],rb->cellMin[2],rb->cellMax[2]));
}

void OpenGLRenderingEngine::setBodiesDispSe3(const shared_ptr<MetaBody>& rootBody){
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b || !b->physicalParameters) continue;
		const Se3r& se3=b->physicalParameters->se3; const Se3r& refSe3=b->physicalParameters->refSe3; Se3r& dispSe3=b->physicalParameters->dispSe3;
		Vector3r posCell=wrapCellPt(se3.position,rootBody.get());
		b->physicalParameters->isDisplayed=!pointClipped(posCell);
		// if no scaling, return quickly
		if(!(scaleDisplacements||scaleRotations||rootBody->isPeriodic)){ b->physicalParameters->dispSe3=b->physicalParameters->se3; continue; }
		// apply scaling
		dispSe3.position=(scaleDisplacements ? diagMult(displacementScale,se3.position-refSe3.position)+wrapCellPt(refSe3.position,rootBody.get()) : posCell );
		if(scaleRotations){
			Quaternionr relRot=refSe3.orientation.Conjugate()*se3.orientation;
			Vector3r axis; Real angle; relRot.ToAxisAngle(axis,angle);
			angle*=rotationScale;
			dispSe3.orientation=refSe3.orientation*Quaternionr(axis,angle);
		} else {dispSe3.orientation=se3.orientation;}
	}
}
// draw periodic cell, if active
void OpenGLRenderingEngine::drawPeriodicCell(MetaBody* rootBody){
	if(!rootBody->isPeriodic) return;
	glPushMatrix();
		glColor3v(Vector3r(1,1,0));
		Vector3r cent=.5*(rootBody->cellMin+rootBody->cellMax); Vector3r size=rootBody->cellMax-rootBody->cellMin;
		glTranslate(cent[0],cent[1],cent[2]); glScale(size[0],size[1],size[2]);
		glutWireCube(1);
	glPopMatrix();
}



void OpenGLRenderingEngine::render(const shared_ptr<MetaBody>& rootBody, body_id_t selection	/*FIXME: not sure. maybe a list of selections, or maybe bodies themselves should remember if they are selected? */) {

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

	// if scaling positions or orientations, save reference values if not already done; if # of bodies changes, we have to reset those; remember iteration when this was done to detect (at least in most cases) that the simulation was reloaded
	if((scaleDisplacements || scaleRotations) && (rootBody->bodies->size()!=numBodiesWhenRefSe3LastSet||Omega::instance().getCurrentIteration()<=numIterWhenRefSe3LastSet)){setBodiesRefSe3(rootBody);}
	
	// set displayed Se3 of body (scaling) and isDisplayed (clipping)
	setBodiesDispSe3(rootBody);

	drawPeriodicCell(rootBody.get());

	if (Show_DOF || Show_ID) renderDOF_ID(rootBody);
	if (Body_geometrical_model){
		if (Cast_shadows){	
			if (Fast_shadow_volume) renderSceneUsingFastShadowVolumes(rootBody,Light_position);
			else renderSceneUsingShadowVolumes(rootBody,Light_position);
			// draw transparent shadow volume
			if (Shadow_volumes) {
				glAlphaFunc(GL_GREATER, 1.0f/255.0f);
				glEnable(GL_ALPHA_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_BLEND);	
			
				glColor4f(0.86,0.058,0.9,0.3);
				glEnable(GL_LIGHTING);
				
				glEnable(GL_CULL_FACE);
			
				glCullFace(GL_FRONT);
				renderShadowVolumes(rootBody,Light_position);

				glCullFace(GL_BACK);
				renderShadowVolumes(rootBody,Light_position);

				glEnable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
			}
		} else{
			glEnable(GL_CULL_FACE);
			glEnable(GL_NORMALIZE);
			#ifdef YADE_SHAPE
				renderGeometricalModel(rootBody);
			#endif
		}
	}
	if (Body_state) renderState(rootBody);
	if (Body_bounding_volume) renderBoundingVolume(rootBody);
	if (Body_interacting_geom){
		glEnable(GL_LIGHTING);
		glEnable(GL_CULL_FACE);
		renderInteractingGeometry(rootBody);
	}
	if (Interaction_geometry) renderInteractionGeometry(rootBody);
	if (Interaction_physics) renderInteractionPhysics(rootBody);
}


void OpenGLRenderingEngine::renderSceneUsingShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r Light_position)
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_NORMALIZE);
	#ifdef YADE_SHAPE
		renderGeometricalModel(rootBody);	
	#endif
	
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);
	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glCullFace(GL_BACK);  /* increment using front face of shadow volume */
	renderShadowVolumes(rootBody,Light_position);
	
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);  /* increment using front face of shadow volume */
	renderShadowVolumes(rootBody,Light_position);	
			
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		
	glStencilFunc(GL_EQUAL, 1, 1);  /* draw shadowed part */
	glStencilFunc(GL_NOTEQUAL, 0, (GLuint)(-1));
	glDisable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	#ifdef YADE_SHAPE
		renderGeometricalModel(rootBody);	
	#endif
	
	glStencilFunc(GL_EQUAL, 0, 1);  /* draw lit part */
	glStencilFunc(GL_EQUAL, 0, (GLuint)(-1));
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	#ifdef YADE_SHAPE
		renderGeometricalModel(rootBody);		
	#endif
	
	glDepthFunc(GL_LESS);
	glDisable(GL_STENCIL_TEST);

}


void OpenGLRenderingEngine::renderSceneUsingFastShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r Light_position)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	#ifdef YADE_SHAPE
		renderGeometricalModel(rootBody);	
	#endif

	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);
	glStencilFunc(GL_ALWAYS, 0, 0);	
	
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glCullFace(GL_BACK);  /* increment using front face of shadow volume */
	renderShadowVolumes(rootBody,Light_position);
	
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);  /* increment using front face of shadow volume */
	renderShadowVolumes(rootBody,Light_position);	
	
	// Need to do that to remove shadow that are not on object but if glClear is 0
/*	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);	
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
	Real clearDepthValue=0;
	glGetDoublev(GL_DEPTH_CLEAR_VALUE,&clearDepthValue);
	glDepthFunc(GL_EQUAL);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0.0, -clearDepthValue);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	glColor3f(1,0,0);
	glBegin(GL_QUADS);
		glVertex3f(0,0,clearDepthValue);
		glVertex3f(0,1,clearDepthValue);
		glVertex3f(1,1,clearDepthValue);
		glVertex3f(1,0,clearDepthValue);
	glEnd();
	
	glMatrixMode(GL_PROJECTION); 
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix(); */
			
	//glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


	glStencilFunc(GL_NOTEQUAL, 0, (GLuint)(-1));
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0.0, -1.0);	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glAlphaFunc(GL_GREATER, 1.0f/255.0f);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);	
	glDisable(GL_LIGHTING);	
	glColor4f(0,0,0,0.5);
	glBegin(GL_QUADS);
		glVertex2f(0,0);
		glVertex2f(0,1);
		glVertex2f(1,1);
		glVertex2f(1,0);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glMatrixMode(GL_PROJECTION); 
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix(); 

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDisable(GL_STENCIL_TEST);
}	


void OpenGLRenderingEngine::renderShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r Light_position){	
	#ifdef YADE_SHAPE
	if (!rootBody->geometricalModel){
		FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
			if(!b || !b->physicalParameters->isDisplayed) continue;
			if (b->geometricalModel->shadowCaster && ( (b->getGroupMask() & Draw_mask) || b->getGroupMask()==0 ))
				shadowVolumeDispatcher(b->geometricalModel,b->physicalParameters,Light_position);
		}
	}
	else shadowVolumeDispatcher(rootBody->geometricalModel,rootBody->physicalParameters,Light_position);
#endif
}

void OpenGLRenderingEngine::renderDOF_ID(const shared_ptr<MetaBody>& rootBody){	
	#ifdef YADE_SHAPE
	const GLfloat ambientColorSelected[4]={10.0,0.0,0.0,1.0};	
	const GLfloat ambientColorUnselected[4]={0.5,0.5,0.5,1.0};	
	if((rootBody->geometricalModel || Draw_inside) && Draw_inside) {
		FOREACH(const shared_ptr<Body> b, *rootBody->bodies){
			if(!b) continue;
			if(b->geometricalModel && ((b->getGroupMask() & Draw_mask) || b->getGroupMask()==0)){
				if(b->physicalParameters && !b->physicalParameters->isDisplayed) continue;
				if(!Show_ID && b->physicalParameters->blockedDOFs==0) continue;
				const Se3r& se3=b->physicalParameters->dispSe3;
				glPushMatrix();
				glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
				if(current_selection==b->getId()){glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColorSelected);}
				{ // write text
					glColor3f(1.0-Background_color[0],1.0-Background_color[1],1.0-Background_color[2]);
					unsigned DOF = b->physicalParameters->blockedDOFs;
					std::string dof = std::string("") 
					                  + (((DOF & PhysicalParameters::DOF_X )!=0)?"X":" ")
					                  + (((DOF & PhysicalParameters::DOF_Y )!=0)?"Y":" ")
					                  + (((DOF & PhysicalParameters::DOF_Z )!=0)?"Z":" ")
					                  + (((DOF & PhysicalParameters::DOF_RX)!=0)?"RX":"  ")
					                  + (((DOF & PhysicalParameters::DOF_RY)!=0)?"RY":"  ")
					                  + (((DOF & PhysicalParameters::DOF_RZ)!=0)?"RZ":"  ");
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
	}
	if(rootBody->geometricalModel) geometricalModelDispatcher(rootBody->geometricalModel,rootBody->physicalParameters,Body_wire);
	#endif
}
#ifdef YADE_SHAPE
void OpenGLRenderingEngine::renderGeometricalModel(const shared_ptr<MetaBody>& rootBody){
	const GLfloat ambientColorSelected[4]={10.0,0.0,0.0,1.0};	
	const GLfloat ambientColorUnselected[4]={0.5,0.5,0.5,1.0};
	if(rootBody->geometricalModel) geometricalModelDispatcher(rootBody->geometricalModel,rootBody->physicalParameters,Body_wire);
	if(!Draw_inside) return;
	FOREACH(const shared_ptr<Body> b, *rootBody->bodies){
		if(!b) continue;
		if(!b->geometricalModel || (!((b->getGroupMask() & Draw_mask) || b->getGroupMask()==0))) continue;
		if(b->physicalParameters && !b->physicalParameters->isDisplayed) continue;
		const Se3r& se3=b->physicalParameters->dispSe3;
		glPushMatrix();
		Real angle; Vector3r axis;	se3.orientation.ToAxisAngle(axis,angle);	
		glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
		glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
		if(current_selection==b->getId() || b->geometricalModel->highlight){
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColorSelected);
			glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
			const Vector3r& h(current_selection==b->getId() ? highlightEmission0 : highlightEmission1);
			glColor4(h[0],h[1],h[2],.2);
			glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);

			geometricalModelDispatcher(b->geometricalModel,b->physicalParameters,Body_wire);

			glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColorUnselected);
			glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
			glColor3v(Vector3r::ZERO);
			glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
		} else {
			geometricalModelDispatcher(b->geometricalModel,b->physicalParameters,Body_wire);
		}
		glPopMatrix();
		if(current_selection==b->getId() || b->geometricalModel->highlight){
			if(!b->boundingVolume || Body_wire || b->geometricalModel->wire) GLUtils::GLDrawInt(b->getId(),se3.position);
			else {
				// move the label towards the camera by the bounding box so that it is not hidden inside the body
				const Vector3r& mn=b->boundingVolume->min; const Vector3r& mx=b->boundingVolume->max; const Vector3r& p=se3.position;
				Vector3r ext(viewDirection[0]>0?p[0]-mn[0]:p[0]-mx[0],viewDirection[1]>0?p[1]-mn[1]:p[1]-mx[1],viewDirection[2]>0?p[2]-mn[2]:p[2]-mx[2]); // signed extents towards the camera
				Vector3r dr=-1.01*(viewDirection.Dot(ext)*viewDirection);
				GLUtils::GLDrawInt(b->getId(),se3.position+dr,Vector3r::ONE);
			}
		}
		// if the body goes over the cell margin, draw it in all other positions with wire
		if(b->boundingVolume && rootBody->isPeriodic){
			const Vector3r& cellMin(rootBody->cellMin); const Vector3r& cellMax(rootBody->cellMax); Vector3r cellSize=cellMax-cellMin;
			Vector3<int> bodyPer,minPer,maxPer;
			for(int i=0; i<3; i++){
				bodyPer[i]=(int)floor((b->physicalParameters->se3.position[i]-cellMin[i])/cellSize[i]);
				minPer[i]=(int)floor((b->boundingVolume->min[i]-cellMin[i])/cellSize[i]);
				maxPer[i]=(int)floor((b->boundingVolume->max[i]-cellMin[i])/cellSize[i]);
				//assert(bodyPer[i]<=maxPer[i]); assert(bodyPer[i]>=minPer[i]);
			}
			/* m is bitmask from 3 couples (0…64=2^6) */
			for(int m=0; m<64; m++){
				// any mask containing 00 couple is invalid
				if((!(m&1) && (!(m&2))) || (!(m&4) && (!(m&8))) || (!(m&16) && (!(m&32)))) continue;
				Vector3r pt(se3.position);
				bool isInside=false;
				for(int j=0; j<3; j++){
					if(m&(1<<(2*j))) {
						if(m&(1<<(2*j+1))) { if(bodyPer[j]>=maxPer[j]) {isInside=true; break; } pt[j]-=cellSize[j]; }
						else { if(bodyPer[j]<=minPer[j]){ isInside=true; break; } pt[j]+=cellSize[j]; }
					}
				}
				if(isInside) continue;
				if(pt==se3.position) continue; // shouldn't happen, but it happens :-(
				glPushMatrix();
					glTranslatev(pt);
					glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
					geometricalModelDispatcher(b->geometricalModel,b->physicalParameters,/*Body_wire*/ true);
				glPopMatrix();
			}
		}
	}
}
#endif

void OpenGLRenderingEngine::renderInteractionGeometry(const shared_ptr<MetaBody>& rootBody){	
	{
		boost::mutex::scoped_lock lock(rootBody->transientInteractions->drawloopmutex);
		FOREACH(const shared_ptr<Interaction>& I, *rootBody->transientInteractions){
			if(!I->interactionGeometry) continue;
			const shared_ptr<Body>& b1=Body::byId(I->getId1(),rootBody), b2=Body::byId(I->getId2(),rootBody);
			if(!(b1->physicalParameters->isDisplayed||b2->physicalParameters->isDisplayed)) continue;
			glPushMatrix(); interactionGeometryDispatcher(I->interactionGeometry,I,b1,b2,Interaction_wire); glPopMatrix();
		}
	}
}


void OpenGLRenderingEngine::renderInteractionPhysics(const shared_ptr<MetaBody>& rootBody){	
	{
		boost::mutex::scoped_lock lock(rootBody->transientInteractions->drawloopmutex);
		FOREACH(const shared_ptr<Interaction>& I, *rootBody->transientInteractions){
			if(!I->interactionPhysics) continue;
			const shared_ptr<Body>& b1=Body::byId(I->getId1(),rootBody), b2=Body::byId(I->getId2(),rootBody);
			//if(!b1->physicalParameters||!b2->physicalParameters) continue;
			if(!(b1->physicalParameters->isDisplayed||b2->physicalParameters->isDisplayed)) continue;
			glPushMatrix(); interactionPhysicsDispatcher(I->interactionPhysics,I,b1,b2,Interaction_wire); glPopMatrix();
		}
	}
}


void OpenGLRenderingEngine::renderState(const shared_ptr<MetaBody>& rootBody){	
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b) continue;
		if(b->physicalParameters && !b->physicalParameters->isDisplayed) continue;
		if(b->physicalParameters && ((b->getGroupMask()&Draw_mask) || b->getGroupMask()==0)){
			glPushMatrix(); stateDispatcher(b->physicalParameters); glPopMatrix();
		}
	}
	if(rootBody->physicalParameters){ glPushMatrix(); stateDispatcher(rootBody->physicalParameters); glPopMatrix();}
}


void OpenGLRenderingEngine::renderBoundingVolume(const shared_ptr<MetaBody>& rootBody){	
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b) continue;
		if(b->physicalParameters && !b->physicalParameters->isDisplayed) continue;
		if(b->boundingVolume && ((b->getGroupMask()&Draw_mask) || b->getGroupMask()==0)){
			glPushMatrix(); boundingVolumeDispatcher(b->boundingVolume); glPopMatrix();
		}
	}
	if(rootBody->boundingVolume){ glPushMatrix(); boundingVolumeDispatcher(rootBody->boundingVolume); glPopMatrix(); }
}


void OpenGLRenderingEngine::renderInteractingGeometry(const shared_ptr<MetaBody>& rootBody)
{
	// Additional clipping planes: http://fly.srk.fer.hr/~unreal/theredbook/chapter03.html
	#if 0
		GLdouble clip0[4]={.0,1.,0.,0.}; // y<0
		glClipPlane(GL_CLIP_PLANE0,clip0);
		glEnable(GL_CLIP_PLANE0);
	#endif
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b) continue;
		if(b->physicalParameters && !b->physicalParameters->isDisplayed) continue;
		const Se3r& se3=b->physicalParameters->dispSe3;
		if(b->interactingGeometry && ((b->getGroupMask()&Draw_mask) || b->getGroupMask()==0)){
			glPushMatrix();
				Real angle;	Vector3r axis;	se3.orientation.ToAxisAngle(axis,angle);	
				glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
				glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
				interactingGeometryDispatcher(b->interactingGeometry,b->physicalParameters,Body_wire,viewInfo);
			glPopMatrix();
		}
	}
	if(rootBody->interactingGeometry){ glPushMatrix(); interactingGeometryDispatcher(rootBody->interactingGeometry,rootBody->physicalParameters,Body_wire,viewInfo); glPopMatrix(); }
}



void OpenGLRenderingEngine::postProcessAttributes(bool deserializing){
	if(!deserializing) return;
	for(unsigned int i=0;i<stateFunctorNames.size();i++) stateDispatcher.add1DEntry(stateFunctorNames[i][0],stateFunctorNames[i][1]);
	for(unsigned int i=0;i<boundingVolumeFunctorNames.size();i++) boundingVolumeDispatcher.add1DEntry(boundingVolumeFunctorNames[i][0],boundingVolumeFunctorNames[i][1]);
	for(unsigned int i=0;i<interactingGeometryFunctorNames.size();i++) interactingGeometryDispatcher.add1DEntry(interactingGeometryFunctorNames[i][0],interactingGeometryFunctorNames[i][1]);
	#ifdef YADE_SHAPE
		for(unsigned int i=0;i<geometricalModelFunctorNames.size();i++) geometricalModelDispatcher.add1DEntry(geometricalModelFunctorNames[i][0],geometricalModelFunctorNames[i][1]);
	#endif
	for(unsigned int i=0;i<shadowVolumeFunctorNames.size();i++) shadowVolumeDispatcher.add1DEntry(shadowVolumeFunctorNames[i][0],shadowVolumeFunctorNames[i][1]);
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
#ifdef YADE_SHAPE
void OpenGLRenderingEngine::addGeometricalModelFunctor(const string& str2){
	string str1 = (static_pointer_cast<GLDrawGeometricalModelFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v; v.push_back(str1); v.push_back(str2); geometricalModelFunctorNames.push_back(v);
}
#endif
void OpenGLRenderingEngine::addShadowVolumeFunctor(const string& str2){
	string str1 = (static_pointer_cast<GLDrawShadowVolumeFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v; v.push_back(str1); v.push_back(str2); shadowVolumeFunctorNames.push_back(v);
}

