#include "Mesh2D.hpp"
#include <iostream>


Mesh2D::Mesh2D () : CollisionModel()
{	
}

Mesh2D::~Mesh2D ()
{

}

void Mesh2D::processAttributes()
{
	CollisionModel::processAttributes();
}

void Mesh2D::registerAttributes()
{
	CollisionModel::registerAttributes();
	REGISTER_ATTRIBUTE(vertices);
	REGISTER_ATTRIBUTE(edges);
}

bool Mesh2D::collideWith(CollisionModel* )
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
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
		for(int i=0;i<edges.size();i++)
		{
			glVertex3fv(vertices[edges[i].first]);
			glVertex3fv(vertices[edges[i].second]);			
		}	
	glEnd();
		
}
