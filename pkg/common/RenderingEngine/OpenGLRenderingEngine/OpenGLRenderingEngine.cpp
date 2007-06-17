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


OpenGLRenderingEngine::OpenGLRenderingEngine() : RenderingEngine()
{

	Body_state		= false;
	Body_bounding_volume	= false;
	Body_interacting_geom	= false;
	Body_geometrical_model	= true;
	Cast_shadows		= false;
	Shadow_volumes	= false;
	Fast_shadow_volume	= true;
	Body_wire		= false;
	Interaction_wire	= false;
	Draw_inside		= true;
	needInit		= true;
	Draw_mask		= ~0;
	Light_position		= Vector3r(75.0,130.0,0.0);
	Background_color		= Vector3r(0.2,0.2,0.2);
	
	Interaction_geometry	= false;
	Interaction_physics	= false;
	
	map<string,DynlibDescriptor>::const_iterator di    = Omega::instance().getDynlibsDescriptor().begin();
	map<string,DynlibDescriptor>::const_iterator diEnd = Omega::instance().getDynlibsDescriptor().end();
	for(;di!=diEnd;++di)
	{
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawStateFunctor"))
			addStateFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawBoundingVolumeFunctor"))
			addBoundingVolumeFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawInteractingGeometryFunctor"))
			addInteractingGeometryFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawGeometricalModelFunctor"))
			addGeometricalModelFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawShadowVolumeFunctor"))
			addShadowVolumeFunctor((*di).first);

	//	InteractionGeometry
	//	InteractionPhysics
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawInteractionGeometryFunctor"))
			addInteractionGeometryFunctor((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"GLDrawInteractionPhysicsFunctor"))
			addInteractionPhysicsFunctor((*di).first);
	}

	postProcessAttributes(true);
}

OpenGLRenderingEngine::~OpenGLRenderingEngine()
{

}

void OpenGLRenderingEngine::init()
{
	if (needInit)
	{
		// FIXME : how to build display list now ??
		//shared_ptr<GeometricalModel> gm = dynamic_pointer_cast<GeometricalModel>(ClassFactory::instance().createShared("Sphere"));
		//gm->buildDisplayList();
		needInit = false;
	}
}

void OpenGLRenderingEngine::renderWithNames(const shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<BodyContainer>& bodies = rootBody->bodies;
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi)
	{
		if((*bi)->geometricalModel)
		{
			glPushMatrix();
			Se3r& se3 = (*bi)->physicalParameters->se3;
			Real angle;
			Vector3r axis;	
			se3.orientation.ToAxisAngle(axis,angle);	
			glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
			glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
			if((*bi)->geometricalModel->getClassName() != "LineSegment") // FIXME FIXME !! - so a body needs to say: I am selectable ?!?!
			{
				glPushName( (*bi)->getId() );
				geometricalModelDispatcher((*bi)->geometricalModel,(*bi)->physicalParameters,/* always solid, not wireframe */false);
				glPopName();
			}
			glPopMatrix();
		}
	}
};

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
		char* b="./yade";
		glutInit(&a,&b);
		initDone=true;
	}
#endif
	current_selection = selection;
	//
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
	
	if (Body_geometrical_model)
	{
		if (Cast_shadows)
		{	
			
			if (Fast_shadow_volume)
				renderSceneUsingFastShadowVolumes(rootBody,Light_position);
			else
				renderSceneUsingShadowVolumes(rootBody,Light_position);
				
			// draw transparent shadow volume
			if (Shadow_volumes)
			{
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
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glEnable(GL_NORMALIZE);
			renderGeometricalModel(rootBody);
		}
	}
	
	if (Body_state)
		renderState(rootBody);
	
	if (Body_bounding_volume)
		renderBoundingVolume(rootBody);
	
	if (Body_interacting_geom)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_CULL_FACE);
		renderInteractingGeometry(rootBody);
	}

	if (Interaction_geometry)
		renderInteractionGeometry(rootBody);
	
	if (Interaction_physics)
		renderInteractionPhysics(rootBody);
	

/*	shared_ptr<BodyContainer> bodies = rootBody->bodies;
	shared_ptr<InteractionContainer>& collisions = rootBody->transientInteractions;
	for( collisions->gotoFirst() ; collisions->notAtEnd() ; collisions->gotoNext())
	{
		const shared_ptr<Interaction>& col = collisions->getCurrent();

		shared_ptr<Body>& b1 = (*bodies)[col->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[col->getId2()];

		SpheresContactGeometry * mmcg = static_cast<SpheresContactGeometry*>(col->interactionGeometry.get());

		Vector3r v1 = mmcg->contactPoint+mmcg->normal*mmcg->penetrationDepth*0.5;
		Vector3r v2 = mmcg->contactPoint-mmcg->normal*mmcg->penetrationDepth*0.5;
		glBegin(GL_LINES);
			glVertex3v(v1);
			glVertex3v(v2);
		glEnd();
		glPushMatrix();
			glColor3v(b1->geometricalModel->diffuseColor);
			glTranslate(v1[0],v1[1],v1[2]);
			glutSolidSphere(1,10,10);
		glPopMatrix();
		glPushMatrix();
			glColor3v(b2->geometricalModel->diffuseColor);
			glTranslate(v2[0],v2[1],v2[2]);
			glutSolidSphere(1,10,10);
		glPopMatrix();
	}*/
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


void OpenGLRenderingEngine::renderShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r Light_position)
{	
	if (!rootBody->geometricalModel)
	{
		BodyContainer::iterator bi    = rootBody->bodies->begin();
		BodyContainer::iterator biEnd = rootBody->bodies->end();
		for(; bi!=biEnd ; ++bi )
		{
			if ((*bi)->geometricalModel->shadowCaster && ( ((*bi)->getGroupMask() & Draw_mask) || (*bi)->getGroupMask()==0 ))
				shadowVolumeDispatcher((*bi)->geometricalModel,(*bi)->physicalParameters,Light_position);
		}
	}
	else
		shadowVolumeDispatcher(rootBody->geometricalModel,rootBody->physicalParameters,Light_position);
}


void OpenGLRenderingEngine::renderGeometricalModel(const shared_ptr<MetaBody>& rootBody)
{	
	shared_ptr<BodyContainer>& bodies = rootBody->bodies;
	bool done=false;

	if((rootBody->geometricalModel || Draw_inside) && Draw_inside)
	{
		BodyContainer::iterator bi    = bodies->begin();
		BodyContainer::iterator biEnd = bodies->end();
		for( ; bi!=biEnd ; ++bi)
		{
			if((*bi)->geometricalModel && ( ((*bi)->getGroupMask() & Draw_mask) || (*bi)->getGroupMask()==0 ))
			{
				glPushMatrix();
				Se3r& se3 = (*bi)->physicalParameters->se3;
				Real angle;
				Vector3r axis;	
				se3.orientation.ToAxisAngle(axis,angle);	
				glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
				glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
				if(current_selection == (*bi)->getId())
				{
					const GLfloat ambientColor[4]	= {10.0,0.0,0.0,1.0};	
					glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColor);
					
				} else if (done) {
					done = false;
					const GLfloat ambientColor[4]	= {0.5,0.5,0.5,1.0};	
					glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColor);
				}
				// FIXME FIXME - in fact it is a 1D dispatcher
				geometricalModelDispatcher((*bi)->geometricalModel,(*bi)->physicalParameters,Body_wire);
				if(current_selection == (*bi)->getId())
					done = true;
				glPopMatrix();
			}
		}
	}
	
	if(rootBody->geometricalModel)
		geometricalModelDispatcher(rootBody->geometricalModel,rootBody->physicalParameters,Body_wire);
}


void OpenGLRenderingEngine::renderInteractionGeometry(const shared_ptr<MetaBody>& rootBody)
{	
	{
		boost::mutex::scoped_lock lock(rootBody->persistentInteractions->drawloopmutex);

		InteractionContainer::iterator bi    = rootBody->persistentInteractions->begin();
		InteractionContainer::iterator biEnd = rootBody->persistentInteractions->end();
		for( ; bi!=biEnd ; ++bi)
		{
			glPushMatrix();
			if((*bi)->interactionGeometry)
				interactionGeometryDispatcher((*bi)->interactionGeometry,(*bi),(*(rootBody->bodies))[(*bi)->getId1()],(*(rootBody->bodies))[(*bi)->getId2()],Interaction_wire);
			glPopMatrix();
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
			glPushMatrix();
			if((*bi)->interactionPhysics)
				interactionPhysicsDispatcher((*bi)->interactionPhysics,(*bi),(*(rootBody->bodies))[(*bi)->getId1()],(*(rootBody->bodies))[(*bi)->getId2()],Interaction_wire);
			glPopMatrix();
		}
	}
}


void OpenGLRenderingEngine::renderState(const shared_ptr<MetaBody>& rootBody)
{	
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for( ; bi!=biEnd ; ++bi)
	{
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

	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for( ; bi!=biEnd ; ++bi)
	{
		glPushMatrix();
		Se3r& se3 = (*bi)->physicalParameters->se3;
		Real angle;
		Vector3r axis;	
		se3.orientation.ToAxisAngle(axis,angle);	
		glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
		glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
		if((*bi)->interactingGeometry && ( ((*bi)->getGroupMask() & Draw_mask) || (*bi)->getGroupMask()==0 ))
			interactingGeometryDispatcher((*bi)->interactingGeometry,(*bi)->physicalParameters,Body_wire);
		glPopMatrix();
	}
	
	glPushMatrix();
	if(rootBody->interactingGeometry)
		interactingGeometryDispatcher(rootBody->interactingGeometry,rootBody->physicalParameters,Body_wire);
	glPopMatrix();
}


void OpenGLRenderingEngine::registerAttributes()
{
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

