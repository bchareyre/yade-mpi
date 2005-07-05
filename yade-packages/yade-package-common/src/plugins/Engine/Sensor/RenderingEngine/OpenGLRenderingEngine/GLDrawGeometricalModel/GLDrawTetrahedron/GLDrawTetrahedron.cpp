/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#include "GLDrawTetrahedron.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-common/Tetrahedron.hpp>
#include <yade/yade-lib-opengl/OpenGLWrapper.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLDrawTetrahedron::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>&,bool wire)
{
	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gm->diffuseColor);
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

		Vector3r center = 0.25*(tet->v1+tet->v2+tet->v3+tet->v4);
		
		glBegin(GL_TRIANGLES);
			Vector3r n = (tet->v1-tet->v4).unitCross(tet->v2-tet->v4);
			Vector3r faceCenter = (tet->v4+tet->v1+tet->v2)/3.0;
			if ((faceCenter-center).dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3fv(tet->v4);
			glVertex3fv(tet->v1);
			glVertex3fv(tet->v2);
			
			n = (tet->v2-tet->v1).unitCross(tet->v3-tet->v1);
			faceCenter = (tet->v1+tet->v2+tet->v3)/3.0;
			if ((faceCenter-center).dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3fv(tet->v1);
			glVertex3fv(tet->v2);
			glVertex3fv(tet->v3);

			n = (tet->v3-tet->v2).unitCross(tet->v4-tet->v2);
			faceCenter = (tet->v2+tet->v3+tet->v4)/3.0;
			if ((faceCenter-center).dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3fv(tet->v2);
			glVertex3fv(tet->v3);
			glVertex3fv(tet->v4);
			
			n = (tet->v4-tet->v3).unitCross(tet->v1-tet->v3);
			faceCenter = (tet->v3+tet->v4+tet->v1)/3.0;
			if ((faceCenter-center).dot(n)<0)
				n = -n;
			glNormal3v (n);
			glVertex3fv(tet->v3);
			glVertex3fv(tet->v4);
			glVertex3fv(tet->v1);
		glEnd();
	}
}

