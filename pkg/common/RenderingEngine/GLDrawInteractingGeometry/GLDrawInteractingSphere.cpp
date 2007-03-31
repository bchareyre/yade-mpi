/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawInteractingSphere.hpp"
#include "InteractingSphere.hpp"
#include<yade/lib-opengl/OpenGLWrapper.hpp>


int GLDrawInteractingSphere::glSphereList=-1;
int GLDrawInteractingSphere::glWiredSphereList=-1;

vector<Vector3r> GLDrawInteractingSphere::vertices;
vector<Vector3r> GLDrawInteractingSphere::faces;


void GLDrawInteractingSphere::go(const shared_ptr<InteractingGeometry>& cm, const shared_ptr<PhysicalParameters>& ,bool)
{
	static bool first=true;
	
	if (first)
	{
		Real X = 0.525731112119133606;
		Real Z = 0.850650808352039932;
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

		
		first = false;
	}
	
	Real radius = (static_cast<InteractingSphere*>(cm.get()))->radius;
	
	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(cm->diffuseColor[0],cm->diffuseColor[1],cm->diffuseColor[2]));
	glColor3v(cm->diffuseColor);
// 	if (cm->wire)
// 	{
// 		glScalef(radius,radius,radius);
// 		glCallList(glWiredSphereList);
// 	}
// 	else
// 	{
		glScalef(radius,radius,radius);
		glCallList(glSphereList);
//	}
}


void GLDrawInteractingSphere::subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth)
{
	Vector3r v12,v23,v31;

	if (depth==0)
	{
		Vector3r v = (v1+v2+v3)/3.0;
		Real angle = atan(v[2]/v[0])/v.Length();

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
	v12.Normalize();
	v23.Normalize();
	v31.Normalize();
	subdivideTriangle(v1,v12,v31,depth-1);
	subdivideTriangle(v2,v23,v12,depth-1);
	subdivideTriangle(v3,v31,v23,depth-1);
	subdivideTriangle(v12,v23,v31,depth-1);
}


void GLDrawInteractingSphere::drawSphere(int depth)
{
	glShadeModel(GL_SMOOTH);
	
	for(int i=0;i<20;i++)
		subdivideTriangle(vertices[(unsigned int)faces[i][0]],vertices[(unsigned int)faces[i][1]],vertices[(unsigned int)faces[i][2]],depth);
}

