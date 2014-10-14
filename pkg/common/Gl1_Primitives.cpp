/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  © 2008 Václav Šmilauer <eudoxos@arcig.cz>                             *
*                                                                        *
*  © 2008 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifdef YADE_OPENGL

#include "Gl1_Primitives.hpp"
#include <lib/opengl/OpenGLWrapper.hpp>
#include <core/Scene.hpp>

YADE_PLUGIN((Gl1_Aabb)(Gl1_Box)(Gl1_Facet));
YADE_PLUGIN((GlBoundFunctor)(GlShapeFunctor)(GlIGeomFunctor)(GlIPhysFunctor)(GlStateFunctor)
            (GlBoundDispatcher)(GlShapeDispatcher)(GlIGeomDispatcher)(GlIPhysDispatcher)
            (GlStateDispatcher));


void Gl1_Aabb::go(const shared_ptr<Bound>& bv, Scene* scene){
	Aabb* aabb = static_cast<Aabb*>(bv.get());
	glColor3v(bv->color);
	if(!scene->isPeriodic){
		glTranslatev(Vector3r(.5*(aabb->min+aabb->max)));
		glScalev(Vector3r(aabb->max-aabb->min));
	} else {
		glTranslatev(Vector3r(scene->cell->shearPt(scene->cell->wrapPt(.5*(aabb->min+aabb->max)))));
		glMultMatrixd(scene->cell->getGlShearTrsfMatrix());
		glScalev(Vector3r(aabb->max-aabb->min));
	}
	glutWireCube(1);
}

void Gl1_Box::go(const shared_ptr<Shape>& cg, const shared_ptr<State>&,bool wire,const GLViewInfo&)
{
	glColor3v(cg->color);
	Vector3r &extents = (static_cast<Box*>(cg.get()))->extents;
	glScalef(2*extents[0],2*extents[1],2*extents[2]);
	if (wire) glutWireCube(1);
	else glutSolidCube(1);
}


bool Gl1_Facet::normals=false;

void Gl1_Facet::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire,const GLViewInfo&)
{   
	Facet* facet = static_cast<Facet*>(cm.get());
	const vector<Vector3r>& vertices = facet->vertices;
	const Vector3r* ne = facet->ne;
	const Real& icr = facet->icr;

	if(cm->wire || wire){
		// facet
		glBegin(GL_LINE_LOOP);
			glColor3v(normals ? Vector3r(1,0,0): cm->color);
		   glVertex3v(vertices[0]);
		   glVertex3v(vertices[1]);
		   glVertex3v(vertices[2]);
	    glEnd();
		if(!normals) return;
		// facet's normal 
		glBegin(GL_LINES);
			glColor3(0.0,0.0,1.0); 
			glVertex3(0.0,0.0,0.0);
			glVertex3v(facet->normal);
		glEnd();
		// normal of edges
		glColor3(0.0,0.0,1.0); 
		glBegin(GL_LINES);
			glVertex3(0.0,0.0,0.0); glVertex3v(Vector3r(icr*ne[0]));
			glVertex3(0.0,0.0,0.0);	glVertex3v(Vector3r(icr*ne[1]));
			glVertex3(0.0,0.0,0.0);	glVertex3v(Vector3r(icr*ne[2]));
		glEnd();
	} else {
		glDisable(GL_CULL_FACE); 
		Vector3r normal=(facet->vertices[1]-facet->vertices[0]).cross(facet->vertices[2]-facet->vertices[1]); normal.normalize();
		glColor3v(cm->color);
		glBegin(GL_TRIANGLES);
			glNormal3v(normal); // this makes every triangle different WRT the light direction; important!
			glVertex3v(facet->vertices[0]);
			glVertex3v(facet->vertices[1]);
			glVertex3v(facet->vertices[2]);
		glEnd();
	}
}

// Spheres==========================================

bool Gl1_Sphere::wire;
bool Gl1_Sphere::stripes;
int  Gl1_Sphere::glutSlices;
int  Gl1_Sphere::glutStacks;
Real  Gl1_Sphere::quality;
bool  Gl1_Sphere::localSpecView;
vector<Vector3r> Gl1_Sphere::vertices, Gl1_Sphere::faces;
int Gl1_Sphere::glStripedSphereList=-1;
int Gl1_Sphere::glGlutSphereList=-1;
Real  Gl1_Sphere::prevQuality=0;

void Gl1_Sphere::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire2, const GLViewInfo&)
{
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);

	Real r=(static_cast<Sphere*>(cm.get()))->radius;
	glColor3v(cm->color);
	if (wire || wire2) glutWireSphere(r,quality*glutSlices,quality*glutStacks);
	else {
		//Check if quality has been modified or if previous lists are invalidated (e.g. by creating a new qt view), then regenerate lists
		bool somethingChanged = (std::abs(quality-prevQuality)>0.001 || glIsList(glStripedSphereList)!=GL_TRUE);
		if (somethingChanged) {initStripedGlList(); initGlutGlList(); prevQuality=quality;}
		glScalef(r,r,r);
		if(stripes) glCallList(glStripedSphereList);
		else glCallList(glGlutSphereList);
	}
	return;
}
YADE_PLUGIN((Gl1_Sphere));

void Gl1_Sphere::subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth){
	Vector3r v;
	//Change color only at the appropriate level, i.e. 8 times in total, since we draw 8 mono-color sectors one after another
	if (depth==int(quality) || quality<=0){
		v = (v1+v2+v3)/3.0;
		GLfloat matEmit[4];
		if (v[1]*v[0]*v[2]>0){
			matEmit[0] = 0.3;
			matEmit[1] = 0.3;
			matEmit[2] = 0.3;
			matEmit[3] = 1.f;
		}else{
			matEmit[0] = 0.15;
			matEmit[1] = 0.15;
			matEmit[2] = 0.15;
			matEmit[3] = 0.2;
		}
 		glMaterialfv(GL_FRONT, GL_EMISSION, matEmit);
	}
	if (depth==1){//Then display 4 triangles
		Vector3r v12 = v1+v2;
		Vector3r v23 = v2+v3;
		Vector3r v31 = v3+v1;
		v12.normalize();
		v23.normalize();
		v31.normalize();
		//Use TRIANGLE_STRIP for faster display of adjacent facets
		glBegin(GL_TRIANGLE_STRIP);
			glNormal3v(v1); glVertex3v(v1);
			glNormal3v(v31); glVertex3v(v31);
			glNormal3v(v12); glVertex3v(v12);
			glNormal3v(v23); glVertex3v(v23);
			glNormal3v(v2); glVertex3v(v2);
		glEnd();
		//terminate with this triangle left behind
		glBegin(GL_TRIANGLES);
			glNormal3v(v3); glVertex3v(v3);
			glNormal3v(v23); glVertex3v(v23);
			glNormal3v(v31); glVertex3v(v31);
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

void Gl1_Sphere::initStripedGlList() {
	if (!vertices.size()){//Fill vectors with vertices and facets
		//Define 6 points for +/- coordinates
		vertices.push_back(Vector3r(-1,0,0));//0
		vertices.push_back(Vector3r(1,0,0));//1
		vertices.push_back(Vector3r(0,-1,0));//2
		vertices.push_back(Vector3r(0,1,0));//3
		vertices.push_back(Vector3r(0,0,-1));//4
		vertices.push_back(Vector3r(0,0,1));//5
		//Define 8 sectors of the sphere
		faces.push_back(Vector3r(3,4,1));
		faces.push_back(Vector3r(3,0,4));
		faces.push_back(Vector3r(3,5,0));
		faces.push_back(Vector3r(3,1,5));
		faces.push_back(Vector3r(2,1,4));
		faces.push_back(Vector3r(2,4,0));
		faces.push_back(Vector3r(2,0,5));
		faces.push_back(Vector3r(2,5,1));
	}
	//Generate the list. Only once for each qtView, or more if quality is modified.
	glDeleteLists(glStripedSphereList,1);
	glStripedSphereList = glGenLists(1);
	glNewList(glStripedSphereList,GL_COMPILE);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	// render the sphere now
	for (int i=0;i<8;i++)
		subdivideTriangle(vertices[(unsigned int)faces[i][0]],vertices[(unsigned int)faces[i][1]],vertices[(unsigned int)faces[i][2]],1+ (int) quality);
	glEndList();

}

void Gl1_Sphere::initGlutGlList(){
	//Generate the "no-stripes" display list, each time quality is modified
	glDeleteLists(glGlutSphereList,1);
	glGlutSphereList = glGenLists(1);
	glNewList(glGlutSphereList,GL_COMPILE);
		glEnable(GL_LIGHTING);
		glShadeModel(GL_SMOOTH);
		glutSolidSphere(1.0,max(quality*glutSlices,(Real)2.),max(quality*glutStacks,(Real)3.));
	glEndList();
}
#endif /* YADE_OPENGL */
