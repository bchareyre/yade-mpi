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
	const GLfloat pos[4]	= {75.0,75.0,0.0,1.0};
	Vector3r lightPos(pos[0],pos[1],pos[2]);
	const GLfloat lightColor[4]	= {1.0,1.0,1.0,1.0};
	const GLfloat ambientColor[4]	= {0.5,0.5,0.5,1.0};
	//glLightfv(GL_LIGHT0, GL_POSITION, pos);
	//glDisable(GL_LIGHT0);
	
	
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
	
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	renderRootBody(rootBody);  /* render scene in depth buffer */
	
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
	
	//glStencilFunc(GL_EQUAL, 1, 1);  /* draw shadowed part */
	glStencilFunc(GL_NOTEQUAL, 0, (GLuint)(-1));
	glDisable(GL_LIGHT0);
	renderRootBody(rootBody);  
	
	//glStencilFunc(GL_EQUAL, 0, 1);  /* draw lit part */
	glStencilFunc(GL_EQUAL, 0, (GLuint)(-1));
	glEnable(GL_LIGHT0);
	renderRootBody(rootBody);  
	
	glDepthFunc(GL_LESS);
	glDisable(GL_STENCIL_TEST);
	    

//	renderShadowVolumes(rootBody,lightPos);
	  

}

void OpenGLRenderingEngine::renderRootBody(shared_ptr<NonConnexBody> rootBody)
{
	glEnable(GL_NORMALIZE);
//	glEnable(GL_CULL_FACE);

	rootBody->glDraw();
}

void OpenGLRenderingEngine::renderShadowVolumes(shared_ptr<NonConnexBody> rootBody,Vector3r& lightPos)
{
	shared_ptr<NonConnexBody> ncb = dynamic_pointer_cast<NonConnexBody>(rootBody);
	
	for( ncb->bodies->gotoFirst() ; ncb->bodies->notAtEnd() ; ncb->bodies->gotoNext() )
	{
		shared_ptr<Body> b = ncb->bodies->getCurrent();
		float p[4];
      		glGetLightfv(GL_LIGHT0, GL_POSITION, p);
		if (b->gm->shadowCaster)
		{
			shared_ptr<Sphere> s = dynamic_pointer_cast<Sphere>(b->gm);
			
			Vector3r center = b->se3.translation;
			Vector3r dir = lightPos-center;
			Vector3r normalDir(-dir[1],dir[0],0);
			normalDir.normalize();
			normalDir *= s->radius+0.001;
			Vector3r biNormalDir = normalDir.unitCross(dir)*(s->radius+0.001);
			
			glColor3f(1.0,1.0,1.0);
			int nbSegments = 100;
// 			glLineWidth(3);
// 			glBegin(GL_LINE_LOOP);
// 			for(int i=0;i<nbSegments;i++)
// 			{
// 				Vector3r p = center;
// 				float angle = Mathr::TWO_PI/(float)nbSegments*i;
// 				p += sin(angle)*normalDir+cos(angle)*biNormalDir;
// 				glVertex3fv(p);
// 			}
// 			glEnd();
			
			glDisable(GL_LIGHTING);	
			
			glBegin(GL_QUADS);
			for(int i=0;i<nbSegments;i++)
			{
				float angle = Mathr::TWO_PI/(float)nbSegments*i;
				Vector3r p1 = center+sin(angle)*normalDir+cos(angle)*biNormalDir;
				
				angle = Mathr::TWO_PI/(float)nbSegments*(i+1);
				Vector3r p4 = center+sin(angle)*normalDir+cos(angle)*biNormalDir;
				Vector3r p2 = p1 + (p1-lightPos);
				Vector3r p3 = p4 + (p4-lightPos);
				Vector3r n = (p2-p1).unitCross(p4-p1);
//				glNormal3fv(n);
				glVertex3fv(p1);
				glVertex3fv(p2);
				glVertex3fv(p3);
				glVertex3fv(p4);
			}
			glEnd();
			glEnable(GL_LIGHTING);	
		}
	}
}