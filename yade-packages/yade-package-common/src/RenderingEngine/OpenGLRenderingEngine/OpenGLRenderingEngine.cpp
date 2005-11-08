/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "OpenGLRenderingEngine.hpp"
#include <yade/yade-lib-opengl/OpenGLWrapper.hpp>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>


OpenGLRenderingEngine::OpenGLRenderingEngine() : RenderingEngine()
{

	drawBoundingVolume = false;
	drawInteractionGeometry = false;
	drawGeometricalModel = true;
	castShadow = false;
	drawShadowVolumes = false;
	useFastShadowVolume = true;
	drawWireFrame = false;
	drawInside = true;
	needInit = true;
	lightPos = Vector3r(75.0,130.0,0.0);
	
	addBoundingVolumeFunctor("AABB","GLDrawAABB");
	addBoundingVolumeFunctor("BoundingSphere","GLDrawBoundingSphere");
	
	addInteractionGeometryFunctor("InteractingSphere","GLDrawInteractionSphere");
	addInteractionGeometryFunctor("InteractingBox","GLDrawInteractionBox");
	
	addInteractionGeometryFunctor("PolyhedralSweptSphere","GLDrawPolyhedralSweptSphere");
		
	addGeometricalModelFunctor("Box","GLDrawBox");
	addGeometricalModelFunctor("Sphere","GLDrawSphere");
	addGeometricalModelFunctor("Mesh2D","GLDrawMesh2D");
	addGeometricalModelFunctor("LineSegment","GLDrawLineSegment");
	addGeometricalModelFunctor("Tetrahedron","GLDrawTetrahedron");
	
	addShadowVolumeFunctor("Box","GLDrawBoxShadowVolume");
	addShadowVolumeFunctor("Sphere","GLDrawSphereShadowVolume");
	
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

void OpenGLRenderingEngine::render(const shared_ptr<MetaBody>& rootBody)
{		
	const GLfloat pos[4]	= {lightPos[0],lightPos[1],lightPos[2],1.0};
	const GLfloat ambientColor[4]	= {0.5,0.5,0.5,1.0};	
	
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColor);
	glEnable(GL_LIGHT0);
	
	glDisable(GL_LIGHTING);
	
	glPushMatrix();
	glTranslatef(lightPos[0],lightPos[1],lightPos[2]);
	glColor3f(1.0,1.0,1.0);
	glutSolidSphere(3,10,10);
	glPopMatrix();	
	
	if (drawGeometricalModel)
	{
		if (castShadow)
		{	
			
			if (useFastShadowVolume)
				renderSceneUsingFastShadowVolumes(rootBody,lightPos);
			else
				renderSceneUsingShadowVolumes(rootBody,lightPos);
				
			// draw transparent shadow volume
			if (drawShadowVolumes)
			{
				glAlphaFunc(GL_GREATER, 1.0f/255.0f);
				glEnable(GL_ALPHA_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_BLEND);	
			
				glColor4f(0.86,0.058,0.9,0.3);
				glEnable(GL_LIGHTING);
				
				glEnable(GL_CULL_FACE);
			
				glCullFace(GL_FRONT);
				renderShadowVolumes(rootBody,lightPos);
				
				glCullFace(GL_BACK);
				renderShadowVolumes(rootBody,lightPos);
				
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
	
	if (drawBoundingVolume)
		renderBoundingVolume(rootBody);
	
	if (drawInteractionGeometry)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_CULL_FACE);
		renderInteractionGeometry(rootBody);
	}


/*	shared_ptr<BodyContainer> bodies = rootBody->bodies;
	shared_ptr<InteractionContainer>& collisions = rootBody->volatileInteractions;
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


void OpenGLRenderingEngine::renderSceneUsingShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r lightPos)
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
	renderShadowVolumes(rootBody,lightPos);
	
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);  /* increment using front face of shadow volume */
	renderShadowVolumes(rootBody,lightPos);	
			
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


void OpenGLRenderingEngine::renderSceneUsingFastShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r lightPos)
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
	renderShadowVolumes(rootBody,lightPos);
	
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);  /* increment using front face of shadow volume */
	renderShadowVolumes(rootBody,lightPos);	
	
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


void OpenGLRenderingEngine::renderShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r lightPos)
{	
	if (!rootBody->geometricalModel)
	{
		BodyContainer::iterator bi    = rootBody->bodies->begin();
		BodyContainer::iterator biEnd = rootBody->bodies->end();
		for(; bi!=biEnd ; ++bi )
		{
			shared_ptr<Body> b = *bi;
			if (b->geometricalModel->shadowCaster)
				shadowVolumeDispatcher(b->geometricalModel,b->physicalParameters,lightPos);
		}
	}
	else
		shadowVolumeDispatcher(rootBody->geometricalModel,rootBody->physicalParameters,lightPos);
}


void OpenGLRenderingEngine::renderGeometricalModel(const shared_ptr<MetaBody>& rootBody)
{	
	shared_ptr<BodyContainer>& bodies = rootBody->bodies;

	if((rootBody->geometricalModel || drawInside) && drawInside)
	{
		BodyContainer::iterator bi    = bodies->begin();
		BodyContainer::iterator biEnd = bodies->end();
		for( ; bi!=biEnd ; ++bi)
		{

			shared_ptr<Body> b = *bi;
			if(b->geometricalModel)
			{
				glPushMatrix();
				Se3r& se3 = b->physicalParameters->se3;
				Real angle;
				Vector3r axis;	
				se3.orientation.toAxisAngle(axis,angle);	
				glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
				glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
				geometricalModelDispatcher(b->geometricalModel,b->physicalParameters,drawWireFrame);
				glPopMatrix();
			}
		}
	}
	
	if(rootBody->geometricalModel)
		geometricalModelDispatcher(rootBody->geometricalModel,rootBody->physicalParameters,drawWireFrame);
}


void OpenGLRenderingEngine::renderBoundingVolume(const shared_ptr<MetaBody>& rootBody)
{	
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for( ; bi!=biEnd ; ++bi)
	{
		shared_ptr<Body> b = *bi;
		glPushMatrix();
		if(b->boundingVolume)
			(b->boundingVolume);
		glPopMatrix();
	}
	
	glPushMatrix();
	if(rootBody->boundingVolume)
		boundingVolumeDispatcher(rootBody->boundingVolume);
	glPopMatrix();
}



void OpenGLRenderingEngine::renderInteractionGeometry(const shared_ptr<MetaBody>& rootBody)
{
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for( ; bi!=biEnd ; ++bi)
	{
		shared_ptr<Body> b = *bi;
		glPushMatrix();
		Se3r& se3 = b->physicalParameters->se3;
		Real angle;
		Vector3r axis;	
		se3.orientation.toAxisAngle(axis,angle);	
		glTranslatef(se3.position[0],se3.position[1],se3.position[2]);
		glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
		if(b->interactionGeometry)
			interactionGeometryDispatcher(b->interactionGeometry,b->physicalParameters);
		glPopMatrix();
	}
	
	glPushMatrix();
	if(rootBody->interactionGeometry)
		interactionGeometryDispatcher(rootBody->interactionGeometry,rootBody->physicalParameters);
	glPopMatrix();
}


void OpenGLRenderingEngine::registerAttributes()
{	
	REGISTER_ATTRIBUTE(lightPos);
	REGISTER_ATTRIBUTE(drawBoundingVolume);
	REGISTER_ATTRIBUTE(drawInteractionGeometry);
	REGISTER_ATTRIBUTE(drawGeometricalModel);
	REGISTER_ATTRIBUTE(castShadow);
	REGISTER_ATTRIBUTE(drawShadowVolumes);
	REGISTER_ATTRIBUTE(useFastShadowVolume);	
	REGISTER_ATTRIBUTE(drawWireFrame);
	REGISTER_ATTRIBUTE(drawInside);
	
	//REGISTER_ATTRIBUTE(boundingVolumeFunctorNames);
	//REGISTER_ATTRIBUTE(interactionGeometryFunctorNames);
	//REGISTER_ATTRIBUTE(geometricalModelFunctorNames);
	//REGISTER_ATTRIBUTE(shadowVolumeFunctorNames);
}


void OpenGLRenderingEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		for(unsigned int i=0;i<boundingVolumeFunctorNames.size();i++)
			boundingVolumeDispatcher.add1DEntry(boundingVolumeFunctorNames[i][0],boundingVolumeFunctorNames[i][1]);
			
		for(unsigned int i=0;i<interactionGeometryFunctorNames.size();i++)
			interactionGeometryDispatcher.add1DEntry(interactionGeometryFunctorNames[i][0],interactionGeometryFunctorNames[i][1]);
			
		for(unsigned int i=0;i<geometricalModelFunctorNames.size();i++)
			geometricalModelDispatcher.add1DEntry(geometricalModelFunctorNames[i][0],geometricalModelFunctorNames[i][1]);
		
		for(unsigned int i=0;i<shadowVolumeFunctorNames.size();i++)
			shadowVolumeDispatcher.add1DEntry(shadowVolumeFunctorNames[i][0],shadowVolumeFunctorNames[i][1]);
	}
}


void OpenGLRenderingEngine::addBoundingVolumeFunctor(const string& str1,const string& str2)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	boundingVolumeFunctorNames.push_back(v);
}


void OpenGLRenderingEngine::addInteractionGeometryFunctor(const string& str1,const string& str2)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	interactionGeometryFunctorNames.push_back(v);
}


void OpenGLRenderingEngine::addGeometricalModelFunctor(const string& str1,const string& str2)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	geometricalModelFunctorNames.push_back(v);
}


void OpenGLRenderingEngine::addShadowVolumeFunctor(const string& str1,const string& str2)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	shadowVolumeFunctorNames.push_back(v);
}

