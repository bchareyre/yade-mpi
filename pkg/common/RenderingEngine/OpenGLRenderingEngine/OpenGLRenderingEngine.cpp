/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "OpenGLRenderingEngine.hpp"
#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>
CREATE_LOGGER(OpenGLRenderingEngine);

OpenGLRenderingEngine::OpenGLRenderingEngine() : RenderingEngine(), clipPlaneNum(3)
{

	Body_state = false;
	Body_bounding_volume = false;
	Body_interacting_geom = false;
	Body_geometrical_model = true;
	Cast_shadows = false;
	Shadow_volumes = false;
	Fast_shadow_volume = true;
	Body_wire = false;
	Interaction_wire = false;
	Draw_inside = true;
	needInit = true;
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
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawGeometricalModelFunctor")) addGeometricalModelFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawShadowVolumeFunctor")) addShadowVolumeFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawInteractionGeometryFunctor")) addInteractionGeometryFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawInteractionPhysicsFunctor")) addInteractionPhysicsFunctor((*di).first);
	}
	postProcessAttributes(true);
}

OpenGLRenderingEngine::~OpenGLRenderingEngine(){}
void OpenGLRenderingEngine::init(){ if (needInit)needInit = false; }

void OpenGLRenderingEngine::renderWithNames(const shared_ptr<MetaBody>& rootBody){
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(!b->geometricalModel) continue;
		glPushMatrix();
		Se3r& se3 = b->physicalParameters->se3;
		Real angle;
		Vector3r axis;	
		se3.orientation.ToAxisAngle(axis,angle);	
		glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
		glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
		if(b->geometricalModel->getClassName() != "LineSegment"){ // FIXME: a body needs to say: I am selectable ?!?!
			glPushName(b->getId());
			geometricalModelDispatcher(b->geometricalModel,b->physicalParameters,/* always solid, not wireframe */false);
			glPopName();
		}
		glPopMatrix();
	}
};

bool OpenGLRenderingEngine::pointClipped(const Vector3r& p){
	if(clipPlaneNum<1) return false;
	for(int i=0;i<clipPlaneNum;i++) if(clipPlaneActive[i]&&(p-clipPlaneSe3[i].position).Dot(clipPlaneNormals[i])<0) return true;
	return false;
}

void OpenGLRenderingEngine::setBodiesRefSe3(const shared_ptr<MetaBody>& rootBody){
	LOG_DEBUG("(re)initializing reference positions and orientations.");
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies) b->physicalParameters->refSe3=b->physicalParameters->se3;
	numBodiesWhenRefSe3LastSet=rootBody->bodies->size();
}

void OpenGLRenderingEngine::setBodiesDispSe3(const shared_ptr<MetaBody>& rootBody){
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		const Se3r& se3=b->physicalParameters->se3; const Se3r& refSe3=b->physicalParameters->refSe3; Se3r& dispSe3=b->physicalParameters->dispSe3;
		b->physicalParameters->isDisplayed=!pointClipped(se3.position);
		// if no scaling, return quickly
		if(!(scaleDisplacements||scaleRotations)){ b->physicalParameters->dispSe3=b->physicalParameters->se3; continue; }
		// apply scaling
		dispSe3.position=(scaleDisplacements ? diagMult(displacementScale,se3.position-refSe3.position)+refSe3.position : se3.position );
		if(scaleRotations){
			Quaternionr relRot=refSe3.orientation.Conjugate()*se3.orientation;
			Vector3r axis; Real angle; relRot.ToAxisAngle(axis,angle);
			angle*=rotationScale;
			dispSe3.orientation=refSe3.orientation*Quaternionr(axis,angle);
		} else {dispSe3.orientation=se3.orientation;}
	}
}

void OpenGLRenderingEngine::render(
		const shared_ptr<MetaBody>& rootBody, 
		body_id_t selection	// FIXME: not sure. maybe a list of selections, 
					// or maybe bodies themselves should remember if they are selected?
		)
{	// FIXME - make a compile time flag for that. So yade can compile with different versions.
#ifndef NO_GLUTINIT
	static bool initDone=false;
	if(!initDone) // FIXME - this is a quick hack for newest version of libglut
	{
		int a=1;
		char* b=(char*)"./yade";
		glutInit(&a,&b);
// transparent spheres (still not working)
//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_ALPHA);
//glEnable(GL_BLEND);
//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		initDone=true;
	}
#endif
	current_selection = selection;

	const GLfloat pos[4]	= {Light_position[0],Light_position[1],Light_position[2],1.0};
	const GLfloat ambientColor[4]	= {0.5,0.5,0.5,1.0};	

	glClearColor(Background_color[0],Background_color[1],Background_color[2],1.0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColor);
	glEnable(GL_LIGHT0);
	
	glDisable(GL_LIGHTING);
	
	glPushMatrix();
	glTranslatef(Light_position[0],Light_position[1],Light_position[2]);
	glColor3f(1.0,1.0,1.0);
	glutSolidSphere(3,10,10);
	glPopMatrix();	

	// TODO: do this only if clipping is effective (i.e. if clipPlaneActive[i] AND'ed together); dtto for pointClipped
	for(int i=0;i<clipPlaneNum; i++){ clipPlaneNormals[i]=clipPlaneSe3[i].orientation*Vector3r(0,0,1); /* glBegin(GL_LINES);glVertex3v(clipPlaneSe3[i].position);glVertex3v(clipPlaneSe3[i].position+clipPlaneNormals[i]);glEnd(); */ }

	// if scaling positions or orientations, save reference values if not already done; if # of bodies changes, we have to reset those
	if((scaleDisplacements || scaleRotations) && rootBody->bodies->size()!=numBodiesWhenRefSe3LastSet){setBodiesRefSe3(rootBody);}
	
	// set displayed Se3 of body (scaling) and isDisplayed (clipping)
	setBodiesDispSe3(rootBody);

	// debugging only: show line between spatial and scaled body position
	#if 0
		if(scaleDisplacements){
			glColor3d(1,1,0); glBegin(GL_LINES); 
			FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
				Se3r se3=renderedSe3(b);
				glVertex3v(b->physicalParameters->se3.position); glVertex3v(se3.position); }
			glEnd();
		}
	#endif

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
			renderGeometricalModel(rootBody);
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
	renderGeometricalModel(rootBody);	
	
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
	renderGeometricalModel(rootBody);	
	
	glStencilFunc(GL_EQUAL, 0, 1);  /* draw lit part */
	glStencilFunc(GL_EQUAL, 0, (GLuint)(-1));
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	renderGeometricalModel(rootBody);	
	
	glDepthFunc(GL_LESS);
	glDisable(GL_STENCIL_TEST);

}


void OpenGLRenderingEngine::renderSceneUsingFastShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r Light_position)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	renderGeometricalModel(rootBody);	

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
	if (!rootBody->geometricalModel){
		FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
			if(pointClipped(b->physicalParameters->se3.position)) continue;
			if (b->geometricalModel->shadowCaster && ( (b->getGroupMask() & Draw_mask) || b->getGroupMask()==0 ))
				shadowVolumeDispatcher(b->geometricalModel,b->physicalParameters,Light_position);
		}
	}
	else shadowVolumeDispatcher(rootBody->geometricalModel,rootBody->physicalParameters,Light_position);
}

void OpenGLRenderingEngine::renderGeometricalModel(const shared_ptr<MetaBody>& rootBody){	
	const GLfloat ambientColorSelected[4]={10.0,0.0,0.0,1.0};	
	const GLfloat ambientColorUnselected[4]={0.5,0.5,0.5,1.0};	
	if((rootBody->geometricalModel || Draw_inside) && Draw_inside) {
		FOREACH(const shared_ptr<Body> b, *rootBody->bodies){
			if(b->geometricalModel && ((b->getGroupMask() & Draw_mask) || b->getGroupMask()==0)){
				if(!b->physicalParameters->isDisplayed) continue;
				const Se3r& se3=b->physicalParameters->dispSe3;
				glPushMatrix();
				Real angle; Vector3r axis;	se3.orientation.ToAxisAngle(axis,angle);	
				glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
				glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
				if(current_selection==b->getId()){glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColorSelected);}
				geometricalModelDispatcher(b->geometricalModel,b->physicalParameters,Body_wire);
				if(current_selection == b->getId()){glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColorUnselected);}
				glPopMatrix();
			}
		}
	}
	if(rootBody->geometricalModel) geometricalModelDispatcher(rootBody->geometricalModel,rootBody->physicalParameters,Body_wire);
}


void OpenGLRenderingEngine::renderInteractionGeometry(const shared_ptr<MetaBody>& rootBody){	
	{
		boost::mutex::scoped_lock lock(rootBody->persistentInteractions->drawloopmutex);

		InteractionContainer::iterator bi    = rootBody->persistentInteractions->begin();
		InteractionContainer::iterator biEnd = rootBody->persistentInteractions->end();
		for( ; bi!=biEnd ; ++bi)
		{
			if((*bi)->interactionGeometry){
				const shared_ptr<Body>& b1=Body::byId((*bi)->getId1(),rootBody), b2=Body::byId((*bi)->getId2(),rootBody);
				if(pointClipped(b1->physicalParameters->se3.position)&&pointClipped(b2->physicalParameters->se3.position)) continue;
				glPushMatrix();
					interactionGeometryDispatcher((*bi)->interactionGeometry,(*bi),(*(rootBody->bodies))[(*bi)->getId1()],(*(rootBody->bodies))[(*bi)->getId2()],Interaction_wire);
				glPopMatrix();
			}
		}
	}

	{
		boost::mutex::scoped_lock lock(rootBody->transientInteractions->drawloopmutex);

		InteractionContainer::iterator bi    = rootBody->transientInteractions->begin();
		InteractionContainer::iterator biEnd = rootBody->transientInteractions->end();
		for( ; bi!=biEnd ; ++bi)
		{
			glPushMatrix();
			if((*bi)->interactionGeometry)
				interactionGeometryDispatcher((*bi)->interactionGeometry,(*bi),(*(rootBody->bodies))[(*bi)->getId1()],(*(rootBody->bodies))[(*bi)->getId2()],Interaction_wire);
			glPopMatrix();
		}
	}
}


void OpenGLRenderingEngine::renderInteractionPhysics(const shared_ptr<MetaBody>& rootBody)
{	
	{
		boost::mutex::scoped_lock lock(rootBody->persistentInteractions->drawloopmutex);

		InteractionContainer::iterator bi    = rootBody->persistentInteractions->begin();
		InteractionContainer::iterator biEnd = rootBody->persistentInteractions->end();
		for( ; bi!=biEnd ; ++bi)
		{
			glPushMatrix();
			if((*bi)->interactionPhysics)
				interactionPhysicsDispatcher((*bi)->interactionPhysics,(*bi),(*(rootBody->bodies))[(*bi)->getId1()],(*(rootBody->bodies))[(*bi)->getId2()],Interaction_wire);
			glPopMatrix();
		}
	}

	{
		boost::mutex::scoped_lock lock(rootBody->transientInteractions->drawloopmutex);

		InteractionContainer::iterator bi    = rootBody->transientInteractions->begin();
		InteractionContainer::iterator biEnd = rootBody->transientInteractions->end();
		for( ; bi!=biEnd ; ++bi)
		{
			if((*bi)->interactionPhysics){
				const shared_ptr<Body>& b1=Body::byId((*bi)->getId1(),rootBody), b2=Body::byId((*bi)->getId2(),rootBody);
				if(pointClipped(b1->physicalParameters->se3.position)&&pointClipped(b2->physicalParameters->se3.position)) continue;
				glPushMatrix();
					interactionPhysicsDispatcher((*bi)->interactionPhysics,(*bi),(*(rootBody->bodies))[(*bi)->getId1()],(*(rootBody->bodies))[(*bi)->getId2()],Interaction_wire);
				glPopMatrix();
			}
		}
	}
}


void OpenGLRenderingEngine::renderState(const shared_ptr<MetaBody>& rootBody)
{	
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for( ; bi!=biEnd ; ++bi)
	{
		if(pointClipped((*bi)->physicalParameters->se3.position)) continue;
		glPushMatrix();
		if((*bi)->physicalParameters && ( ((*bi)->getGroupMask() & Draw_mask ) || (*bi)->getGroupMask()==0 ))
			stateDispatcher((*bi)->physicalParameters);
		glPopMatrix();
	}
	
	glPushMatrix();
	if(rootBody->physicalParameters)
		stateDispatcher(rootBody->physicalParameters);
	glPopMatrix();
}


void OpenGLRenderingEngine::renderBoundingVolume(const shared_ptr<MetaBody>& rootBody)
{	
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for( ; bi!=biEnd ; ++bi)
	{		
		if(pointClipped((*bi)->physicalParameters->se3.position)) continue;
		glPushMatrix();
		if((*bi)->boundingVolume && ( ((*bi)->getGroupMask() & Draw_mask) || (*bi)->getGroupMask()==0 ))
			boundingVolumeDispatcher((*bi)->boundingVolume);
		glPopMatrix();
	}
	
	glPushMatrix();
	if(rootBody->boundingVolume)
		boundingVolumeDispatcher(rootBody->boundingVolume);
	glPopMatrix();
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
		if(!b->physicalParameters->isDisplayed) continue;
		const Se3r& se3=b->physicalParameters->dispSe3;
		glPushMatrix();
			Real angle;	Vector3r axis;	se3.orientation.ToAxisAngle(axis,angle);	
			glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
			glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
			if(b->interactingGeometry && ( (b->getGroupMask() & Draw_mask) || b->getGroupMask()==0 ))
				interactingGeometryDispatcher(b->interactingGeometry,b->physicalParameters,Body_wire);
		glPopMatrix();
	}
	glPushMatrix();
		if(rootBody->interactingGeometry) interactingGeometryDispatcher(rootBody->interactingGeometry,rootBody->physicalParameters,Body_wire);
	glPopMatrix();
}


void OpenGLRenderingEngine::registerAttributes()
{
	REGISTER_ATTRIBUTE(scaleDisplacements);
	REGISTER_ATTRIBUTE(displacementScale);
	REGISTER_ATTRIBUTE(scaleRotations);
	REGISTER_ATTRIBUTE(rotationScale);

	REGISTER_ATTRIBUTE(Light_position);
	REGISTER_ATTRIBUTE(Background_color);
	
	REGISTER_ATTRIBUTE(Body_wire);

	REGISTER_ATTRIBUTE(Body_state);
	REGISTER_ATTRIBUTE(Body_bounding_volume);
	REGISTER_ATTRIBUTE(Body_interacting_geom);
	REGISTER_ATTRIBUTE(Body_geometrical_model);
	
	REGISTER_ATTRIBUTE(Interaction_wire);
	REGISTER_ATTRIBUTE(Interaction_geometry);
	REGISTER_ATTRIBUTE(Interaction_physics);
	
	REGISTER_ATTRIBUTE(Draw_mask);
	REGISTER_ATTRIBUTE(Draw_inside);

	REGISTER_ATTRIBUTE(Cast_shadows);
	REGISTER_ATTRIBUTE(Shadow_volumes);
	REGISTER_ATTRIBUTE(Fast_shadow_volume);

	REGISTER_ATTRIBUTE(clipPlaneSe3);
	REGISTER_ATTRIBUTE(clipPlaneActive);
}


void OpenGLRenderingEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		for(unsigned int i=0;i<stateFunctorNames.size();i++)
			stateDispatcher.add1DEntry(stateFunctorNames[i][0],stateFunctorNames[i][1]);
			
		for(unsigned int i=0;i<boundingVolumeFunctorNames.size();i++)
			boundingVolumeDispatcher.add1DEntry(boundingVolumeFunctorNames[i][0],boundingVolumeFunctorNames[i][1]);
			
		for(unsigned int i=0;i<interactingGeometryFunctorNames.size();i++)
			interactingGeometryDispatcher.add1DEntry(interactingGeometryFunctorNames[i][0],interactingGeometryFunctorNames[i][1]);
			
		for(unsigned int i=0;i<geometricalModelFunctorNames.size();i++)
			geometricalModelDispatcher.add1DEntry(geometricalModelFunctorNames[i][0],geometricalModelFunctorNames[i][1]);
		
		for(unsigned int i=0;i<shadowVolumeFunctorNames.size();i++)
			shadowVolumeDispatcher.add1DEntry(shadowVolumeFunctorNames[i][0],shadowVolumeFunctorNames[i][1]);
		
		for(unsigned int i=0;i<interactionGeometryFunctorNames.size();i++)
			interactionGeometryDispatcher.add1DEntry(interactionGeometryFunctorNames[i][0],interactionGeometryFunctorNames[i][1]);
			
		for(unsigned int i=0;i<interactionPhysicsFunctorNames.size();i++)
			interactionPhysicsDispatcher.add1DEntry(interactionPhysicsFunctorNames[i][0],interactionPhysicsFunctorNames[i][1]);	
	}
}


void OpenGLRenderingEngine::addInteractionGeometryFunctor(const string& str2)
{
	string str1 = (static_pointer_cast<GLDrawInteractionGeometryFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	interactionGeometryFunctorNames.push_back(v);
}

void OpenGLRenderingEngine::addInteractionPhysicsFunctor(const string& str2)
{
	string str1 = (static_pointer_cast<GLDrawInteractionPhysicsFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	interactionPhysicsFunctorNames.push_back(v);
}

void OpenGLRenderingEngine::addStateFunctor(const string& str2)
{
	string str1 = (static_pointer_cast<GLDrawStateFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	stateFunctorNames.push_back(v);
}


void OpenGLRenderingEngine::addBoundingVolumeFunctor(const string& str2)
{
	string str1 = (static_pointer_cast<GLDrawBoundingVolumeFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	boundingVolumeFunctorNames.push_back(v);
}


void OpenGLRenderingEngine::addInteractingGeometryFunctor(const string& str2)
{
	string str1 = (static_pointer_cast<GLDrawInteractingGeometryFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	interactingGeometryFunctorNames.push_back(v);
}


void OpenGLRenderingEngine::addGeometricalModelFunctor(const string& str2)
{
	string str1 = (static_pointer_cast<GLDrawGeometricalModelFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	geometricalModelFunctorNames.push_back(v);
}


void OpenGLRenderingEngine::addShadowVolumeFunctor(const string& str2)
{
	string str1 = (static_pointer_cast<GLDrawShadowVolumeFunctor>(ClassFactory::instance().createShared(str2)))->renders();
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	shadowVolumeFunctorNames.push_back(v);
}

YADE_PLUGIN();
