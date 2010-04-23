/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Gl1_Sphere.hpp"
#include<yade/pkg-common/Sphere.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>

bool Gl1_Sphere::wire;
bool Gl1_Sphere::stripes;
bool Gl1_Sphere::glutNormalize;
int  Gl1_Sphere::glutSlices;
int  Gl1_Sphere::glutStacks;

vector<Vector3r> Gl1_Sphere::vertices, Gl1_Sphere::faces;
int Gl1_Sphere::glSphereList=-1;

void Gl1_Sphere::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire2, const GLViewInfo&)
{
	Real r=(static_cast<Sphere*>(cm.get()))->radius;
	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(cm->color[0],cm->color[1],cm->color[2]));
	glColor3v(cm->color);
	if(glutNormalize)	glPushAttrib(GL_NORMALIZE); // as per http://lists.apple.com/archives/Mac-opengl/2002/Jul/msg00085.html
		if (wire || wire2) glutWireSphere(r,glutSlices,glutStacks);
		else {
 			if(stripes) { glScalef(r,r,r); drawSphere();}
			else {
// 				if(glSphereList<0) {
// 					glSphereList = glGenLists(1);
// 					glNewList(glSphereList,GL_COMPILE);
// 					//glShadeModel(GL_SMOOTH); glDisable(GL_LIGHTING);
// 					glutSolidSphere(1,glutSlices,glutStacks);
// 					glEndList();}
// 				
// 				glScalef(r,r,r);
// 				glCallList(glSphereList);}
				glutSolidSphere(r,glutSlices,glutStacks);}
		}
	if(glutNormalize) glPopAttrib();
	return;
}
YADE_PLUGIN((Gl1_Sphere));
YADE_REQUIRE_FEATURE(OPENGL)


/***************************** WARNING *********************
***** The following code was written by Olivier Galizzi ****
************ and hasn't been properly reviewed *************/

// https://blueprints.launchpad.net/yade/+spec/sphere-gl-stripes

void Gl1_Sphere::drawSphere(){
	if(glSphereList<0) initGlLists();
	glShadeModel(GL_SMOOTH);
	//glScalef(radius,radius,radius);
	glCallList(glSphereList);
}

void Gl1_Sphere::subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth){
	Vector3r v12,v23,v31;
	if (depth==0){
		Vector3r v = (v1+v2+v3)/3.0;
		Real angle = atan(v[2]/v[0])/v.Length();
		GLfloat matAmbient[4];
		if (angle>-Mathr::PI/6.0 && angle<=Mathr::PI/6.0){
			matAmbient[0] = 0.2;
			matAmbient[1] = 0.2;
			matAmbient[2] = 0.2;
			matAmbient[3] = 0.0;
		}else{
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

void Gl1_Sphere::initGlLists(void){
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
	
	glSphereList = glGenLists(1);
	glNewList(glSphereList,GL_COMPILE);
		glEnable(GL_LIGHTING);
		glShadeModel(GL_SMOOTH);	
		// render the sphere now
		for(int i=0;i<20;i++)
			subdivideTriangle(vertices[(unsigned int)faces[i][0]],vertices[(unsigned int)faces[i][1]],vertices[(unsigned int)faces[i][2]],1);
		//drawSphere();
	glEndList();
}
