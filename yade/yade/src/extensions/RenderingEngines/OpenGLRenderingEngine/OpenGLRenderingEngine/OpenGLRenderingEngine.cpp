/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "OpenGLRenderingEngine.hpp"
#include "Sphere.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

OpenGLRenderingEngine::OpenGLRenderingEngine() : RenderingEngine()
{

	drawBoundingVolume = false;
	drawCollisionGeometry = false;
	drawGeometricalModel = true;
	castShadow = false;
	drawShadowVolumes = false;
	useFastShadowVolume = true;
	needInit = true;
	lightPos = Vector3r(75.0,130.0,0.0);
	
	addBoundingVolumeFunctor();
	addCollisionGeometryFunctor();
	addGeometricalModelFunctor();
	addShadowVolumeFunctor();
	
	
}

OpenGLRenderingEngine::~OpenGLRenderingEngine()
{

}

void OpenGLRenderingEngine::init()
{
	if (needInit)
	{
		shared_ptr<GeometricalModel> gm = dynamic_pointer_cast<GeometricalModel>(ClassFactory::instance().createShared("Sphere"));
		gm->buildDisplayList();
		needInit = false;
	}
}

void OpenGLRenderingEngine::render(shared_ptr<ComplexBody> rootBody)
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
			rootBody->glDrawGeometricalModel();
		}
	}
	
	if (drawBoundingVolume)
		rootBody->glDrawBoundingVolume();
	
	if (drawCollisionGeometry)
		rootBody->glDrawCollisionGeometry();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void OpenGLRenderingEngine::renderSceneUsingShadowVolumes(shared_ptr<ComplexBody> rootBody,Vector3r lightPos)
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_NORMALIZE);
	rootBody->glDrawGeometricalModel();	
	
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
	rootBody->glDrawGeometricalModel();	
	
	glStencilFunc(GL_EQUAL, 0, 1);  /* draw lit part */
	glStencilFunc(GL_EQUAL, 0, (GLuint)(-1));
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	rootBody->glDrawGeometricalModel();	
	
	glDepthFunc(GL_LESS);
	glDisable(GL_STENCIL_TEST);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void OpenGLRenderingEngine::renderSceneUsingFastShadowVolumes(shared_ptr<ComplexBody> rootBody,Vector3r lightPos)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	rootBody->glDrawGeometricalModel();	

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
	double clearDepthValue=0;
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void OpenGLRenderingEngine::renderShadowVolumes(shared_ptr<ComplexBody> rootBody,Vector3r lightPos)
{
	shared_ptr<ComplexBody> ncb = dynamic_pointer_cast<ComplexBody>(rootBody);
	
	for( ncb->bodies->gotoFirst() ; ncb->bodies->notAtEnd() ; ncb->bodies->gotoNext() )
	{
		shared_ptr<Body> b = ncb->bodies->getCurrent();
		
		if (b->gm->shadowCaster)
		{
			b->gm->renderShadowVolumes(b->se3,lightPos);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void OpenGLRenderingEngine::registerAttributes()
{	
	REGISTER_ATTRIBUTE(lightPos);
	REGISTER_ATTRIBUTE(drawBoundingVolume);
	REGISTER_ATTRIBUTE(drawCollisionGeometry);
	REGISTER_ATTRIBUTE(drawGeometricalModel);
	REGISTER_ATTRIBUTE(castShadow);
	REGISTER_ATTRIBUTE(drawShadowVolumes);
	REGISTER_ATTRIBUTE(useFastShadowVolume);	
	
	//REGISTER_ATTRIBUTE(boundingVolumeFunctorNames);
	//REGISTER_ATTRIBUTE(collisionGeometryFunctorNames);
	//REGISTER_ATTRIBUTE(geometricalModelFunctorNames);
	//REGISTER_ATTRIBUTE(shadowVolumeFunctorNames);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void OpenGLRenderingEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		for(unsigned int i=0;i<boundingVolumeFunctorNames.size();i++)
			boundingVolumeDispatcher.add(boundingVolumeFunctorNames[i][0],boundingVolumeFunctorNames[i][1]);
			
		for(unsigned int i=0;i<collisionGeometryFunctorNames.size();i++)
			interactionGeometryDispatcher.add(collisionGeometryFunctorNames[i][0],collisionGeometryFunctorNames[i][1]);
			
		for(unsigned int i=0;i<geometricalModelFunctorNames.size();i++)
			geometricalModelDispatcher.add(geometricalModelFunctorNames[i][0],geometricalModelFunctorNames[i][1]);
		
		for(unsigned int i=0;i<shadowVolumeFunctorNames.size();i++)
			shadowVolumeDispatcher.add(shadowVolumeFunctorNames[i][0],shadowVolumeFunctorNames[i][1]);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void addBoundingVolumeFunctor(const string& str1,const string& str2)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	boundingVolumeFunctorNames.push_back(v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void addCollisionGeometryFunctor(const string& str1,const string& str2)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	collisionGeometryFunctorNames.push_back(v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void addGeometricalModelFunctor(const string& str1,const string& str2)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	geometricalModelFunctorNames.push_back(v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void addShadowVolumeFunctor(const string& str1,const string& str2)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	shadowVolumeFunctorNames.push_back(v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
