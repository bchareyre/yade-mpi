/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v[1] or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawTetrahedron.hpp"
#include<yade/pkg-common/Tetrahedron.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/lib-base/yadeWm3.hpp>


void GLDrawTetrahedron::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>&,bool wire)
{
	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(gm->diffuseColor[0],gm->diffuseColor[1],gm->diffuseColor[2]));
	glColor3v(gm->diffuseColor);
	
	Tetrahedron* tet = static_cast<Tetrahedron*>(gm.get());
	
	if (gm->wire || wire) // FIXME - draws only wire
	{
		glBegin(GL_LINES);
			glDisable(GL_LIGHTING);
			
			// FIXME - stupid method to draw lines of tetrahedron
			glVertex3v(tet->v[0]);
			glVertex3v(tet->v[1]);
			
			glVertex3v(tet->v[0]);
			glVertex3v(tet->v[2]);
			
			glVertex3v(tet->v[0]);
			glVertex3v(tet->v[3]);
			
			glVertex3v(tet->v[2]);
			glVertex3v(tet->v[3]);
			
			glVertex3v(tet->v[1]);
			glVertex3v(tet->v[3]);
			
			glVertex3v(tet->v[1]);
			glVertex3v(tet->v[2]);
			
		glEnd();
	}
	else
	{
// 		glBegin(GL_TRIANGLE_STRIP);
// 			glEnable(GL_LIGHTING); 
// 			
// 			glNormal3v (tet->v[3]);
// 			glVertex3fv(tet->v[3]);
// 			
// 			glNormal3v (tet->v[0]);
// 			glVertex3fv(tet->v[0]);
// 			
// 			glNormal3v (tet->v[1]);
// 			glVertex3fv(tet->v[1]);
// 			
// 			
// 			glNormal3v (tet->v[2]);
// 			glVertex3fv(tet->v[2]);
// 			
// 			glNormal3v (tet->v[3]);
// 			glVertex3fv(tet->v[3]);
// 			
// 			glNormal3v (tet->v[0]);
// 			glVertex3fv(tet->v[0]);
// 		glEnd();

/*		glDisable(GL_CULL_FACE);
		glBegin(GL_TRIANGLE_STRIP);
			glEnable(GL_LIGHTING); 
		

				glVertex3fv(tet->v[3]);
			
			
				glVertex3fv(tet->v[0]);			

			
			Vector3r n = (tet->v[0]-tet->v[3]).unitCross(tet->v[1]-tet->v[3]);
			glNormal3v (n);
				glVertex3fv(tet->v[1]);
			
			n = (tet->v[1]-tet->v[0]).unitCross(tet->v[2]-tet->v[0]);
			glNormal3v (n);
			
				glVertex3fv(tet->v[2]);
			n = (tet->v[2]-tet->v[1]).unitCross(tet->v[3]-tet->v[1]);
			glNormal3v (n);
			
				glVertex3fv(tet->v[3]);
			n = (tet->v[3]-tet->v[2]).unitCross(tet->v[0]-tet->v[2]);
			glNormal3v (n);
				glVertex3fv(tet->v[0]);
		glEnd();*/
		glDisable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);

		Vector3r center = (tet->v[0]+tet->v[1]+tet->v[2]+tet->v[3])*.25;
		
		glBegin(GL_TRIANGLES);
			Vector3r n = (tet->v[0]-tet->v[3]).UnitCross(tet->v[1]-tet->v[3]);
			Vector3r faceCenter = (tet->v[3]+tet->v[0]+tet->v[1])/3.0;
			if ((faceCenter-center).Dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3v(tet->v[3]);
			glVertex3v(tet->v[0]);
			glVertex3v(tet->v[1]);
			
			n = (tet->v[1]-tet->v[0]).UnitCross(tet->v[2]-tet->v[0]);
			faceCenter = (tet->v[0]+tet->v[1]+tet->v[2])/3.0;
			if ((faceCenter-center).Dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3v(tet->v[0]);
			glVertex3v(tet->v[1]);
			glVertex3v(tet->v[2]);

			n = (tet->v[2]-tet->v[1]).UnitCross(tet->v[3]-tet->v[1]);
			faceCenter = (tet->v[1]+tet->v[2]+tet->v[3])/3.0;
			if ((faceCenter-center).Dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3v(tet->v[1]);
			glVertex3v(tet->v[2]);
			glVertex3v(tet->v[3]);
			
			n = (tet->v[3]-tet->v[2]).UnitCross(tet->v[0]-tet->v[2]);
			faceCenter = (tet->v[2]+tet->v[3]+tet->v[0])/3.0;
			if ((faceCenter-center).Dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3v(tet->v[2]);
			glVertex3v(tet->v[3]);
			glVertex3v(tet->v[0]);
		glEnd();
	}
}

YADE_PLUGIN("GLDrawTetrahedron");
YADE_REQUIRE_FEATURE(OPENGL)
