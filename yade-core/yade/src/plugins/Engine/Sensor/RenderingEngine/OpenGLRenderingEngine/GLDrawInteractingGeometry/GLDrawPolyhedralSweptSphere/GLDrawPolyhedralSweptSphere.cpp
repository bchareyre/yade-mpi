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

#include "GLDrawPolyhedralSweptSphere.hpp"

#include <yade-common/PolyhedralSweptSphere.hpp>
#include <yade-lib-opengl/OpenGLWrapper.hpp>
#include <yade-lib-opengl/OpenGLWrapper.hpp>

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLDrawPolyhedralSweptSphere::go(const shared_ptr<InteractingGeometry>& cg, const shared_ptr<PhysicalParameters>&)
{
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cg->diffuseColor);
	glColor3v(cg->diffuseColor);
	
	PolyhedralSweptSphere * pss = static_cast<PolyhedralSweptSphere*>(cg.get());
	
	const vector<Vector3r>& vertices = pss->vertices;
	const vector<vector<int> >& faces = pss->faces;
	const vector<pair<int,int> >& edges = pss->edges;
	Real radius = pss->radius;
	
	for(unsigned int i=0 ; i<vertices.size() ; i++)
	{
		glPushMatrix();
		glTranslatef(vertices[i][0],vertices[i][1],vertices[i][2]);
		glutSolidSphere(radius,30,30);
		glPopMatrix();
	}

	for(unsigned int i=0 ; i<edges.size() ; i++)
	{
		glPushMatrix();
		Vector3r v1 = vertices[edges[i].first];
		Vector3r v2 = vertices[edges[i].second];
		
		Quaternionr q;
		Vector3r dir = (v2-v1);
		dir.normalize();
		Vector3r initDir = Vector3r(0,0,1);
		initDir.normalize();
		q.align(initDir,dir);
		Vector3r axis;
		Real angle;
		q.toAxisAngle(axis,angle);
		
		glTranslatef(v1[0],v1[1],v1[2]);
		glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
		
		Real height = (v1-v2).length();
		
		glutSolidCylinder( radius, height, 30, 30);
		
		glPopMatrix();
	}
	
	for(unsigned int i=0 ; i<faces.size() ; i++)
	{	
		Vector3r v1 = vertices[faces[i][0]];
		Vector3r v2 = vertices[faces[i][1]];
		Vector3r v3 = vertices[faces[i][2]];
		Vector3r n = (v1-v2).unitCross(v1-v3);
		glNormal3f(n[0],n[1],n[2]);
		n *= radius;

		glBegin(GL_POLYGON);
		for(unsigned int j=0 ; j<faces[i].size() ; j++)
			glVertex3fv(vertices[faces[i][j]]+n);
		glEnd();
	}




}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

