/*************************************************************************
*  Copyright (C) 2012 by François Kneib   francois.kneib@gmail.com       *
*  Copyright (C) 2012 by Bruno Chareyre   bruno.chareyre@hmg.inpg.fr     *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Grid.hpp"
#ifdef YADE_OPENGL
	#include<lib/opengl/OpenGLWrapper.hpp>

//!##################	Rendering   #####################

bool Gl1_GridConnection::wire;
bool Gl1_GridConnection::glutNormalize;
int  Gl1_GridConnection::glutSlices;
int  Gl1_GridConnection::glutStacks;

void Gl1_GridConnection::out( Quaternionr q )
{
	AngleAxisr aa(q);
	std::cout << " axis: " <<  aa.axis()[0] << " " << aa.axis()[1] << " " << aa.axis()[2] << ", angle: " << aa.angle() << " | ";
}

void Gl1_GridConnection::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& st ,bool wire2, const GLViewInfo&)
{	
	GridConnection *GC=static_cast<GridConnection*>(cm.get());
	Real r=GC->radius;
	Real length=GC->getLength();
	const shared_ptr<Interaction> intr = scene->interactions->find((int)GC->node1->getId(),(int)GC->node2->getId());
	Vector3r segt = GC->node2->state->pos - GC->node1->state->pos;
	if (scene->isPeriodic && intr) segt+=scene->cell->intrShiftPos(intr->cellDist);
	//glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(cm->color[0],cm->color[1],cm->color[2]));

	glColor3v(cm->color);
	if(glutNormalize) glPushAttrib(GL_NORMALIZE);
// 	glPushMatrix();
	Quaternionr shift;
	shift.setFromTwoVectors(Vector3r::UnitZ(),segt);
	
	st->ori = Quaternionr::Identity();// Otherwise clumped connexions get rotated by the clump motion and the view is messed up (note that orientation is never used in mechanical calculations in the case of connexions and pfacets).
	
	if(intr){drawCylinder(wire || wire2, r,length,shift);}
// 	if (intr && scene->isPeriodic) { glTranslatef(-segt[0],-segt[1],-segt[2]); drawCylinder(wire || wire2, r,length,-shift);}
	if(glutNormalize) glPopAttrib();
// 	glPopMatrix();
	return;
}

void Gl1_GridConnection::drawCylinder(bool wire, Real radius, Real length, const Quaternionr& shift)
{
   glPushMatrix();
   GLUquadricObj *quadObj = gluNewQuadric();
   gluQuadricDrawStyle(quadObj, (GLenum) (wire ? GLU_SILHOUETTE : GLU_FILL));
   gluQuadricNormals(quadObj, (GLenum) GLU_SMOOTH);
   gluQuadricOrientation(quadObj, (GLenum) GLU_OUTSIDE);
   AngleAxisr aa(shift);
   glRotatef(aa.angle()*180.0/Mathr::PI,aa.axis()[0],aa.axis()[1],aa.axis()[2]);
   gluCylinder(quadObj, radius, radius, length, glutSlices,glutStacks);
   gluQuadricOrientation(quadObj, (GLenum) GLU_INSIDE);
   //glutSolidSphere(radius,glutSlices,glutStacks);
   glTranslatef(0.0,0.0,length);

   //glutSolidSphere(radius,glutSlices,glutStacks);
//    gluDisk(quadObj,0.0,radius,glutSlices,_loops);
   gluDeleteQuadric(quadObj);
   glPopMatrix();
}
YADE_PLUGIN((Gl1_GridConnection));
#endif
