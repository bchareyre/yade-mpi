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

#include "Sphere.hpp"
#include "OpenGLWrapper.hpp"

int Sphere::glSphereList=-1;
int Sphere::glWiredSphereList=-1;

vector<Vector3r> Sphere::vertices;
vector<Vector3r> Sphere::faces;


Sphere::Sphere (float r) : CollisionGeometry()
{
	radius = r;
	Sphere::Sphere();
}

Sphere::Sphere () : CollisionGeometry()
{

	float X = 0.525731112119133606;
	float Z = 0.850650808352039932;
	vertices.push_back(Vector3r(-X,0,Z));
	vertices.push_back(Vector3r(X,0,Z));
	vertices.push_back(Vector3r(-X,0,-Z));
	vertices.push_back(Vector3r(X,0,-Z));
	vertices.push_back(Vector3r(0,Z,X));
	vertices.push_back(Vector3r(0,Z,-X));
	vertices.push_back(Vector3r(0,-Z,X));
	vertices.push_back(Vector3r(0,-Z,-X));
	vertices.push_back(Vector3r(Z,X,0));
	vertices.push_back(Vector3r(-Z,X,0));
	vertices.push_back(Vector3r(Z,-X,0));
	vertices.push_back(Vector3r(-Z,-X,0));

	faces.push_back(Vector3r(0,4,1));
	faces.push_back(Vector3r(0,9,4));
	faces.push_back(Vector3r(9,5,4));
	faces.push_back(Vector3r(4,5,8));
	faces.push_back(Vector3r(4,8,1));
	faces.push_back(Vector3r(8,10,1));
	faces.push_back(Vector3r(8,3,10));
	faces.push_back(Vector3r(5,3,8));
	faces.push_back(Vector3r(5,2,3));
	faces.push_back(Vector3r(2,7,3));
	faces.push_back(Vector3r(7,10,3));
	faces.push_back(Vector3r(7,6,10));
	faces.push_back(Vector3r(7,11,6));
	faces.push_back(Vector3r(11,0,6));
	faces.push_back(Vector3r(0,1,6));
	faces.push_back(Vector3r(6,1,10));
	faces.push_back(Vector3r(9,0,11));
	faces.push_back(Vector3r(9,11,2));
	faces.push_back(Vector3r(9,2,5));
	faces.push_back(Vector3r(7,2,11));
}

Sphere::~Sphere ()
{

}

void Sphere::postProcessAttributes(bool deserializing)
{
	CollisionGeometry::postProcessAttributes(deserializing);
}

void Sphere::registerAttributes()
{
	CollisionGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(radius);
}

bool Sphere::collideWith(CollisionGeometry* )
{
	return true;
}

/* FIXME - do this
template<typename TT>
glNormal3v(TT tt)
{
}

template<>
glNormal3v<float>(TT tt)
{
	glNormal3v(tt);
}
*/

void Sphere::subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth)
{
	Vector3r v12,v23,v31;

	if (depth==0)
	{
		Vector3r v = (v1+v2+v3)/3.0;
		float angle = atan(v[2]/v[0])/v.length();

		GLfloat matAmbient[4];

		if (angle>-Mathr::PI/6.0 && angle<=Mathr::PI/6.0)
		{
			matAmbient[0] = 0.2;
			matAmbient[1] = 0.2;
			matAmbient[2] = 0.2;
			matAmbient[3] = 0.0;
		}
		else
		{
			matAmbient[0] = 0.0;
			matAmbient[1] = 0.0;
			matAmbient[2] = 0.0;
			matAmbient[3] = 0.0;
		}

		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,matAmbient);
		
		glBegin(GL_TRIANGLES);
			glNormal3v(v3);
			glVertex3v(v3);
			glNormal3v(v2);
			glVertex3v(v2);
			glNormal3v(v1);
			glVertex3v(v1);
		glEnd();

		return;
	}
	v12 = v1+v2;
	v23 = v2+v3;
	v31 = v3+v1;
	v12.normalize();
	v23.normalize();
	v31.normalize();
	subdivideTriangle(v1,v12,v31,depth-1);
	subdivideTriangle(v2,v23,v12,depth-1);
	subdivideTriangle(v3,v31,v23,depth-1);
	subdivideTriangle(v12,v23,v31,depth-1);
}

void Sphere::drawSphere(int depth)
{
	glShadeModel(GL_SMOOTH);
	GLfloat matAmbientAndDiffuse[] = { 1.0,1.0,1.0,1.0};

	number=0;
	for(int i=0;i<20;i++)
		subdivideTriangle(vertices[(unsigned int)faces[i][0]],vertices[(unsigned int)faces[i][1]],vertices[(unsigned int)faces[i][2]],depth);
}

void Sphere::buildDisplayList()
{
	//if (glSphereList==-1)
	//{
		glWiredSphereList = glGenLists(1);
		glNewList(glWiredSphereList,GL_COMPILE);
			glDisable(GL_LIGHTING);
			drawSphere(1);
		glEndList();
		glSphereList = glGenLists(1);
		glNewList(glSphereList,GL_COMPILE);
			glEnable(GL_LIGHTING); // FIXME : remove enable/disable lighting from object
			drawSphere(1);
		glEndList();
	//}
}

void Sphere::glDraw()
{
	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuseColor);
	glColor3v(diffuseColor);
	if (wire)
	{
		glScalef(radius,radius,radius);
		glCallList(glWiredSphereList);
	}
	else
	{
		glScalef(radius,radius,radius);
		glCallList(glSphereList);
	}
}


void Sphere::renderShadowVolumes(const Se3r& se3, const Vector3r& lightPos)
{
	Vector3r center = se3.translation;
	Vector3r dir = lightPos-center;
	Vector3r normalDir(-dir[1],dir[0],0);
	normalDir.normalize();
	normalDir *= radius;
	Vector3r biNormalDir = normalDir.unitCross(dir)*radius;
	
	int nbSegments = 15;
	
	Vector3r p1,p2;
	glBegin(GL_QUAD_STRIP);
		p1 = center+biNormalDir;
		p2 = p1 + (p1-lightPos)*10;
		glVertex3v(p1);
		glVertex3v(p2);
		for(int i=1;i<=nbSegments;i++)
		{
			float angle = Mathr::TWO_PI/(float)nbSegments*i;
			p1 = center+sin(angle)*normalDir+cos(angle)*biNormalDir;
			p2 = p1 + (p1-lightPos)*10;
			glVertex3v(p1);
			glVertex3v(p2);
		}
	glEnd();
			
	// closing shadow volumes ??
// 	glColor3f(0,1,0);
// 	glBegin(GL_POLYGON);
// 	for(int i=0;i<nbSegments;i++)
// 	{
// 		float angle = Mathr::TWO_PI/(float)nbSegments*i;
// 		p1 = center+sin(angle)*normalDir+cos(angle)*biNormalDir;
// 		p2 = p1 + (p1-lightPos)*2;
// 		glVertex3fv(p2);
// 	}
// 	glEnd();
}
