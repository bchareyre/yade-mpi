/*************************************************************************
*  © 2004 Olivier Galizzi  <olivier.galizzi@imag.fr>                     *
*  © 2008 Václav Šmilauer <eudoxos@arcig.cz>                             *
*  © 2008 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifdef YADE_OPENGL

#include "Gl1_Sphere.hpp"
#include<yade/pkg/common/Sphere.hpp>
#include<yade/lib/opengl/OpenGLWrapper.hpp>

bool Gl1_Sphere::wire;
bool Gl1_Sphere::stripes;
int  Gl1_Sphere::glutSlices;
int  Gl1_Sphere::glutStacks;
Real  Gl1_Sphere::quality;
bool  Gl1_Sphere::localSpecView;
vector<Vector3r> Gl1_Sphere::vertices, Gl1_Sphere::faces;
int Gl1_Sphere::glSphereList=-1;
int Gl1_Sphere::glGlutSphereList=-1;
Real  Gl1_Sphere::prevQuality=0;

void Gl1_Sphere::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire2, const GLViewInfo&)
{
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);
	GLfloat glutMatSpecular[4]={0.5,0.5,0.5,1.0};
	GLfloat glutMatEmit[4]={0.2,0.2,0.2,1.0};
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,glutMatSpecular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,glutMatEmit);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 80);
	Real r=(static_cast<Sphere*>(cm.get()))->radius;

	glColor3v(cm->color);
	if (wire || wire2) glutWireSphere(r,glutSlices,glutStacks);
	else {
		initGlLists();
		glScalef(r,r,r);
		if(stripes) glCallList(glSphereList);
		else glCallList(glGlutSphereList);
	}
	return;
}
YADE_PLUGIN((Gl1_Sphere));

void Gl1_Sphere::subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth){
	if (depth==0){
		Vector3r v = (v1+v2+v3)/3.0;
		Real angle = atan(v[2]/v[0])/v.norm();
		GLfloat matEmit[4];
		if (angle>-Mathr::PI/6.0 && angle<=Mathr::PI/6.0){
			matEmit[0] = 0.4;
			matEmit[1] = 0.4;
			matEmit[2] = 0.4;
			matEmit[3] = 1.f;
		}else{
			matEmit[0] = 0.2;
			matEmit[1] = 0.2;
			matEmit[2] = 0.2;
			matEmit[3] = 1.f;
		}
 		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmit);
		glBegin(GL_TRIANGLES);
			glNormal3v(v3); glVertex3v(v3);
			glNormal3v(v2); glVertex3v(v2);
			glNormal3v(v1); glVertex3v(v1);
		glEnd();
		return;
	}
	Vector3r v12 = v1+v2;
	Vector3r v23 = v2+v3;
	Vector3r v31 = v3+v1;
	v12.normalize();
	v23.normalize();
	v31.normalize();
	subdivideTriangle(v1,v12,v31,depth-1);
	subdivideTriangle(v2,v23,v12,depth-1);
	subdivideTriangle(v3,v31,v23,depth-1);
	subdivideTriangle(v12,v23,v31,depth-1);
}

void Gl1_Sphere::initGlLists(){
	//Generate the "stripes" dislpay list, only once
	if (glSphereList<0) {
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
		glEndList();
	}
	//Generate the "no-stripes" display list, each time quality is modified
	if (glGlutSphereList<0 || abs(quality-prevQuality)>0.001 /*detect any meaningfull change in quality to regenerate GL primitives*/) {
		glGlutSphereList = glGenLists(1);
		glNewList(glGlutSphereList,GL_COMPILE);
			glEnable(GL_LIGHTING);
			glShadeModel(GL_SMOOTH);
			glutSolidSphere(1.0,quality*glutSlices,quality*glutStacks);
		glEndList();
		prevQuality=quality;
	}
}

#endif /* YADE_OPENGL */
