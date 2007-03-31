/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawTetrahedron.hpp"
#include "Tetrahedron.hpp"
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
			glVertex3v(tet->v1);
			glVertex3v(tet->v2);
			
			glVertex3v(tet->v1);
			glVertex3v(tet->v3);
			
			glVertex3v(tet->v1);
			glVertex3v(tet->v4);
			
			glVertex3v(tet->v3);
			glVertex3v(tet->v4);
			
			glVertex3v(tet->v2);
			glVertex3v(tet->v4);
			
			glVertex3v(tet->v2);
			glVertex3v(tet->v3);
			
		glEnd();
	}
	else
	{
// 		glBegin(GL_TRIANGLE_STRIP);
// 			glEnable(GL_LIGHTING); 
// 			
// 			glNormal3v (tet->v4);
// 			glVertex3fv(tet->v4);
// 			
// 			glNormal3v (tet->v1);
// 			glVertex3fv(tet->v1);
// 			
// 			glNormal3v (tet->v2);
// 			glVertex3fv(tet->v2);
// 			
// 			
// 			glNormal3v (tet->v3);
// 			glVertex3fv(tet->v3);
// 			
// 			glNormal3v (tet->v4);
// 			glVertex3fv(tet->v4);
// 			
// 			glNormal3v (tet->v1);
// 			glVertex3fv(tet->v1);
// 		glEnd();

/*		glDisable(GL_CULL_FACE);
		glBegin(GL_TRIANGLE_STRIP);
			glEnable(GL_LIGHTING); 
		

				glVertex3fv(tet->v4);
			
			
				glVertex3fv(tet->v1);			

			
			Vector3r n = (tet->v1-tet->v4).unitCross(tet->v2-tet->v4);
			glNormal3v (n);
				glVertex3fv(tet->v2);
			
			n = (tet->v2-tet->v1).unitCross(tet->v3-tet->v1);
			glNormal3v (n);
			
				glVertex3fv(tet->v3);
			n = (tet->v3-tet->v2).unitCross(tet->v4-tet->v2);
			glNormal3v (n);
			
				glVertex3fv(tet->v4);
			n = (tet->v4-tet->v3).unitCross(tet->v1-tet->v3);
			glNormal3v (n);
				glVertex3fv(tet->v1);
		glEnd();*/
		glDisable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);

		Vector3r center = (tet->v1+tet->v2+tet->v3+tet->v4)*.25;
		
		glBegin(GL_TRIANGLES);
			Vector3r n = (tet->v1-tet->v4).UnitCross(tet->v2-tet->v4);
			Vector3r faceCenter = (tet->v4+tet->v1+tet->v2)/3.0;
			if ((faceCenter-center).Dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3v(tet->v4);
			glVertex3v(tet->v1);
			glVertex3v(tet->v2);
			
			n = (tet->v2-tet->v1).UnitCross(tet->v3-tet->v1);
			faceCenter = (tet->v1+tet->v2+tet->v3)/3.0;
			if ((faceCenter-center).Dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3v(tet->v1);
			glVertex3v(tet->v2);
			glVertex3v(tet->v3);

			n = (tet->v3-tet->v2).UnitCross(tet->v4-tet->v2);
			faceCenter = (tet->v2+tet->v3+tet->v4)/3.0;
			if ((faceCenter-center).Dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3v(tet->v2);
			glVertex3v(tet->v3);
			glVertex3v(tet->v4);
			
			n = (tet->v4-tet->v3).UnitCross(tet->v1-tet->v3);
			faceCenter = (tet->v3+tet->v4+tet->v1)/3.0;
			if ((faceCenter-center).Dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3v(tet->v3);
			glVertex3v(tet->v4);
			glVertex3v(tet->v1);
		glEnd();
	}
}

