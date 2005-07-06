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

#include "GLDrawMesh2D.hpp"
#include "Mesh2D.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-opengl/OpenGLWrapper.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLDrawMesh2D::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>&,bool wire)
{
	glColor3v(gm->diffuseColor);

	if (gm->wire || wire)
	{
		vector<Edge>& edges 	   = (static_cast<Mesh2D*>(gm.get()))->edges;
		vector<Vector3r>& vertices = (static_cast<Mesh2D*>(gm.get()))->vertices;
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
		Mesh2D * mesh2d = static_cast<Mesh2D*>(gm.get());
		vector<Vector3r>& vertices 		= mesh2d->vertices;
		vector<vector<int> >& faces 		= mesh2d->faces;
		vector<Vector3r>& fNormals  		= mesh2d->fNormals;
		vector<Vector3r>& vNormals  		= mesh2d->vNormals;
 		vector<vector<int> >& triPerVertices	= mesh2d->triPerVertices;
		
		glShadeModel(GL_SMOOTH);
		GLfloat matSpecular[] = { 1.0,1.0,1.0,1.0};
		GLfloat matShininess[] = { 50.0};
		glMaterialfv(GL_FRONT,GL_SPECULAR,matSpecular);
		glMaterialfv(GL_FRONT,GL_SHININESS,matShininess);
		
		glEnable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		
		// Computing Normals
		// FIXME : should we do that here or inside geometricalModelUpdator?
		// add flag isDeformable so that is is useless to recompute normal
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
