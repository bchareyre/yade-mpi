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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Mesh2D.hpp"
#include "OpenGLWrapper.hpp"
#include <iostream>


Mesh2D::Mesh2D () : CollisionGeometry()
{		
	createIndex();
}

Mesh2D::~Mesh2D ()
{

}

void Mesh2D::postProcessAttributes(bool deserializing)
{
	CollisionGeometry::postProcessAttributes(deserializing);
	
	if(deserializing)
	{
	
		fNormals.resize(faces.size());
		vNormals.resize(vertices.size());
		triPerVertices.resize(vertices.size());
		for(unsigned int i=0;i<faces.size();i++)
		{
			triPerVertices[faces[i][0]].push_back(i);
			triPerVertices[faces[i][1]].push_back(i);
			triPerVertices[faces[i][2]].push_back(i);
		}
	}
}

void Mesh2D::registerAttributes()
{
	CollisionGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(vertices);
	REGISTER_ATTRIBUTE(edges);
	REGISTER_ATTRIBUTE(faces);
	REGISTER_ATTRIBUTE(width);
	REGISTER_ATTRIBUTE(height);
}



void Mesh2D::glDraw()
{
	glColor3v(diffuseColor);

	//wire=true;
	if (wire)
	{
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			for(unsigned int i=0;i<edges.size();i++)
			{
				glVertex3v(vertices[edges[i].first]);
				glVertex3v(vertices[edges[i].second]);			
			}	
		glEnd();
	}
	else
	{
			glShadeModel(GL_SMOOTH);
	GLfloat matSpecular[] = { 1.0,1.0,1.0,1.0};
	GLfloat matShininess[] = { 50.0};
	glMaterialfv(GL_FRONT,GL_SPECULAR,matSpecular);
	glMaterialfv(GL_FRONT,GL_SHININESS,matShininess);

		glEnable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		computeNormals();
		glBegin(GL_TRIANGLES);
			for(unsigned int i=0;i<faces.size();i++)
			{
				int v1 = faces[i][0];
				int v2 = faces[i][1];
				int v3 = faces[i][2];
				glNormal3v(vNormals[v1]);
				glVertex3v(vertices[v1]);
				glNormal3v(vNormals[v2]);
				glVertex3v(vertices[v2]);
				glNormal3v(vNormals[v3]);
				glVertex3v(vertices[v3]);
			}
		glEnd();
	}
}

void Mesh2D::computeNormals()
{
	for(unsigned int i=0;i<faces.size();i++)
	{
		Vector3r v1 = vertices[faces[i][0]];
		Vector3r v2 = vertices[faces[i][1]];
		Vector3r v3 = vertices[faces[i][2]];
		fNormals[i] = -(v2-v1).cross(v3-v1);
	}
	for(unsigned int i=0;i<vertices.size();i++)
	{
		int size = triPerVertices[i].size();
		vNormals[i] = fNormals[triPerVertices[i][0]];
		for(int j=1;j<size;j++)
			vNormals[i] += fNormals[triPerVertices[i][j]];
		vNormals[i] /= size;
	}
}
