#include "Mesh2D.hpp"
#include <iostream>


Mesh2D::Mesh2D () : CollisionGeometry()
{	
}

Mesh2D::~Mesh2D ()
{

}

void Mesh2D::processAttributes()
{
	CollisionGeometry::processAttributes();

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

void Mesh2D::registerAttributes()
{
	CollisionGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(vertices);
	REGISTER_ATTRIBUTE(edges);
	REGISTER_ATTRIBUTE(faces);
	REGISTER_ATTRIBUTE(width);
	REGISTER_ATTRIBUTE(height);
}

bool Mesh2D::collideWith(CollisionGeometry* )
{
	return true;

}

bool Mesh2D::loadFromFile(char * )
{
	return false;
}

void Mesh2D::glDraw()
{
	glColor3fv(diffuseColor);

	//wire=true;
	if (wire)
	{
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			for(unsigned int i=0;i<edges.size();i++)
			{
				glVertex3fv(vertices[edges[i].first]);
				glVertex3fv(vertices[edges[i].second]);			
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
				glNormal3fv(vNormals[v1]);
				glVertex3fv(vertices[v1]);
				glNormal3fv(vNormals[v2]);
				glVertex3fv(vertices[v2]);
				glNormal3fv(vNormals[v3]);
				glVertex3fv(vertices[v3]);
			}
		glEnd();
	}
}

void Mesh2D::computeNormals()
{
	for(unsigned int i=0;i<faces.size();i++)
	{
		Vector3 v1 = vertices[faces[i][0]];
		Vector3 v2 = vertices[faces[i][1]];
		Vector3 v3 = vertices[faces[i][2]];
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
