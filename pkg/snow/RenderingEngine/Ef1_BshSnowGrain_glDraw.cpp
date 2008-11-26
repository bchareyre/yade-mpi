/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ef1_BshSnowGrain_glDraw.hpp"
#include<yade/pkg-snow/BshSnowGrain.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>


void Ef1_BshSnowGrain_glDraw::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>&,bool wire)
{
	bool surface = !wire;
	BshSnowGrain* gr = static_cast<BshSnowGrain*>(gm.get());
	Real LEN=(gr->start - gr->end).Length();

//	glTranslatef(gr->center[0],gr->center[1],gr->center[2]);
	glColor3f(0.5,0.5,1.0);
	glutSolidCube(LEN*0.1);
	glTranslatef(-gr->center[0],-gr->center[1],-gr->center[2]);

  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(gm->diffuseColor[0],gm->diffuseColor[1],gm->diffuseColor[2]));
	glColor3v(gm->diffuseColor);

//	std::cerr << gr->slices.size() << " ===========  \n";

	if(surface)
	{
		glDisable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		//glShadeModel(GL_SMOOTH);
		//glColor3f(gr->color[0],gr->color[1],gr->color[2]);
		for(int i=0;i<gr->slices.size()-1;++i)
		{
			glBegin(GL_QUAD_STRIP);
				for(int j=0;j<gr->slices[i].size()-1;++j)
				{
					Vector3r n=1.0*((gr->slices[i  ][j] - gr->slices[i+1][j]).Cross(gr->slices[i  ][j+1] - gr->slices[i][j]));
					n.Normalize();
					glNormal3f(n[0],n[1],n[2]);

					glVertex3d(gr->slices[i  ][j][0],gr->slices[i  ][j][1],gr->slices[i  ][j][2]);
					glVertex3d(gr->slices[i+1][j][0],gr->slices[i+1][j][1],gr->slices[i+1][j][2]);
				}
				glVertex3d(gr->slices[i  ][0][0],gr->slices[i  ][0][1],gr->slices[i  ][0][2]);
				glVertex3d(gr->slices[i+1][0][0],gr->slices[i+1][0][1],gr->slices[i+1][0][2]);
			glEnd();
		}
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glBegin(GL_LINE_STRIP);
			glColor3f(0,0,0);
			glVertex3d(gr->start[0],gr->start[1],gr->start[2]);
			glVertex3d(gr->end[0]  ,gr->end[1]  ,gr->end[2]);
		glEnd();
			glColor3v(gm->diffuseColor);
			for(int i=0;i < gr->slices.size();++i)
			{
				glBegin(GL_LINE_STRIP);
					for(int j=0 ; j < gr->slices[i].size() ; ++j)
						glVertex3d(gr->slices[i][j][0],gr->slices[i][j][1],gr->slices[i][j][2]);
					glVertex3d(gr->slices[i][0][0],gr->slices[i][0][1],gr->slices[i][0][2]);
				glEnd();
			}
		glEnable(GL_LIGHTING);
	}
	glTranslatef(gr->center[0],gr->center[1],gr->center[2]);
/*	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(gm->diffuseColor[0],gm->diffuseColor[1],gm->diffuseColor[2]));
	glColor3v(gm->diffuseColor);
	
	Vector3r &extents = (static_cast<Box*>(gm.get()))->extents;
	
	glScalef(2*extents[0],2*extents[1],2*extents[2]);

	if (gm->wire || wire)
	{
		glDisable(GL_LIGHTING);
		glutWireCube(1);
	}
	else
	{
		glEnable(GL_LIGHTING);
		glutSolidCube(1);
	}
*/
}


YADE_PLUGIN();

