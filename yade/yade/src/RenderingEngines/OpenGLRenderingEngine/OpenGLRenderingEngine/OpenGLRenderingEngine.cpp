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

}

OpenGLRenderingEngine::~OpenGLRenderingEngine()
{

}
	
void OpenGLRenderingEngine::render(shared_ptr<NonConnexBody> rootBody)
{
	const GLfloat pos[4]	= {75.0,130.0,0.0,1.0};
	Vector3r lightPos(pos[0],pos[1],pos[2]);
	const GLfloat ambientColor[4]	= {0.5,0.5,0.5,1.0};	
	
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColor);
	glEnable(GL_LIGHT0);

	float sceneRadius = 100;
	
	glDisable(GL_LIGHTING);
	
	glPushMatrix();
	glTranslatef(lightPos[0],lightPos[1],lightPos[2]);
	glColor3f(1.0,1.0,1.0);
	glutSolidSphere(3,10,10);
	glPopMatrix();	
	
	glEnable(GL_CULL_FACE);

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Switch between faster/slower method
///////////////////////////////////////////////////////////////////////////////////////////////////
	
// 	renderRootBody(rootBody);  /* render scene in depth buffer */	
// 	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

///////////////////////////////////////////////////////////////////////////////////////////////////
	
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	renderRootBody(rootBody);  /* render scene in depth buffer */

///////////////////////////////////////////////////////////////////////////////////////////////////
/// End Switch between faster/slower method
///////////////////////////////////////////////////////////////////////////////////////////////////
	
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
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// End Switch between faster/slower method
///////////////////////////////////////////////////////////////////////////////////////////////////

	// Need to do that to remove shadow that are not on object
// 	glCullFace(GL_BACK);
// 	glDepthMask(GL_LESS);
// 	
// 	glMatrixMode(GL_PROJECTION);
// 	glPushMatrix();
// 	glLoadIdentity();
// 	glOrtho(0, 1, 1, 0, 0.0, -1.0);
// 	
// 	glMatrixMode(GL_MODELVIEW);
// 	glPushMatrix();
// 	glLoadIdentity();
// 	
// 	glBegin(GL_QUADS);
// 		glVertex3f(0,0,0.99);
// 		glVertex3f(0,1,0.99);
// 		glVertex3f(1,1,0.99);
// 		glVertex3f(1,0,0.99);
// 	glEnd();
// 		
// 	glMatrixMode(GL_PROJECTION); 
// 	glPopMatrix();
// 	
// 	glMatrixMode(GL_MODELVIEW);
// 	glPopMatrix(); 
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// End Switch between faster/slower method
///////////////////////////////////////////////////////////////////////////////////////////////////
		
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Switch between faster/slower method
///////////////////////////////////////////////////////////////////////////////////////////////////

// 	glStencilFunc(GL_NOTEQUAL, 0, (GLuint)(-1));
// 	glMatrixMode(GL_PROJECTION);
// 	glPushMatrix();
// 	glLoadIdentity();
// 	glOrtho(0, 1, 1, 0, 0.0, -1.0);
// 	
// 	glMatrixMode(GL_MODELVIEW);
// 	glPushMatrix();
// 	glLoadIdentity();
// 	
// 	glDisable(GL_CULL_FACE);
// 	glAlphaFunc(GL_GREATER, 1.0f/255.0f);
// 	glEnable(GL_ALPHA_TEST);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	glEnable(GL_BLEND);	
// 		
// 	glColor4f(0.3,0.3,0.3,0.4);
// 	glBegin(GL_QUADS);
// 		glVertex2f(0,0);
// 		glVertex2f(0,1);
// 		glVertex2f(1,1);
// 		glVertex2f(1,0);
// 	glEnd();
// 	
// 	glEnable(GL_DEPTH_TEST);
// 	glDisable(GL_BLEND);
// 	glDisable(GL_ALPHA_TEST);
// 	glEnable(GL_CULL_FACE);
// 	
// 	glMatrixMode(GL_PROJECTION); 
// 	glPopMatrix();
// 	
// 	glMatrixMode(GL_MODELVIEW);
// 	glPopMatrix(); 

///////////////////////////////////////////////////////////////////////////////////////////////////
		
		glStencilFunc(GL_EQUAL, 1, 1);  /* draw shadowed part */
		glStencilFunc(GL_NOTEQUAL, 0, (GLuint)(-1));
		glDisable(GL_LIGHT0);
		renderRootBody(rootBody);
		
		glStencilFunc(GL_EQUAL, 0, 1);  /* draw lit part */
		glStencilFunc(GL_EQUAL, 0, (GLuint)(-1));
		glEnable(GL_LIGHT0);
		renderRootBody(rootBody);  
		
///////////////////////////////////////////////////////////////////////////////////////////////////
/// End Switch between faster/slower method
///////////////////////////////////////////////////////////////////////////////////////////////////

	glDepthFunc(GL_LESS);
	glDisable(GL_STENCIL_TEST);

	// draw transparent shadow volume
// 	glAlphaFunc(GL_GREATER, 1.0f/255.0f);
// 	glEnable(GL_ALPHA_TEST);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	glEnable(GL_BLEND);	
// 	glEnable(GL_LIGHTING);
// 	
// 	glEnable(GL_CULL_FACE);
// 	glCullFace(GL_FRONT);
// 	renderShadowVolumes(rootBody);
// 	
// 	glCullFace(GL_BACK);
// 	renderShadowVolumes(rootBody,lightPos);
// 	
// 	glEnable(GL_DEPTH_TEST);
// 	glDisable(GL_BLEND);
// 	glDisable(GL_ALPHA_TEST);
}

void OpenGLRenderingEngine::renderRootBody(shared_ptr<NonConnexBody> rootBody)
{
	glEnable(GL_NORMALIZE);

	rootBody->glDraw();
}

void OpenGLRenderingEngine::renderShadowVolumes(shared_ptr<NonConnexBody> rootBody,Vector3r lightPos)
{
	shared_ptr<NonConnexBody> ncb = dynamic_pointer_cast<NonConnexBody>(rootBody);
	
	for( ncb->bodies->gotoFirst() ; ncb->bodies->notAtEnd() ; ncb->bodies->gotoNext() )
	{
		shared_ptr<Body> b = ncb->bodies->getCurrent();
		
		if (b->gm->shadowCaster)
		{
			shared_ptr<Sphere> s = dynamic_pointer_cast<Sphere>(b->gm);
			
			Vector3r center = b->se3.translation;
			Vector3r dir = lightPos-center;
			Vector3r normalDir(-dir[1],dir[0],0);
			normalDir.normalize();
			normalDir *= s->radius+0.001;
			Vector3r biNormalDir = normalDir.unitCross(dir)*(s->radius+0.001);
			
			glColor4f(0.86,0.058,0.9,0.3);
			int nbSegments = 50;
			
			Vector3r p1,p2;
			glBegin(GL_QUAD_STRIP);
				p1 = center+biNormalDir;
				p2 = p1 + (p1-lightPos)*10;
				glVertex3fv(p1);
				glVertex3fv(p2);
				for(int i=1;i<=nbSegments;i++)
				{
					float angle = Mathr::TWO_PI/(float)nbSegments*i;			
					p1 = center+sin(angle)*normalDir+cos(angle)*biNormalDir;
					p2 = p1 + (p1-lightPos)*10;
					glVertex3fv(p1);
					glVertex3fv(p2);
				}
			glEnd();
		}
	}
}