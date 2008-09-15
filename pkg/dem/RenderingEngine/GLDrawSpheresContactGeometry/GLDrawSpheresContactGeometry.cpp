/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"GLDrawSpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>

#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/lib-opengl/GLUtils.hpp>

YADE_PLUGIN("GLDrawSpheresContactGeometry");

void GLDrawSpheresContactGeometry::go(
		const shared_ptr<InteractionGeometry>& ig,
		const shared_ptr<Interaction>& ip,
		const shared_ptr<Body>& b1,
		const shared_ptr<Body>& b2,
		bool wireFrame)
{
	SpheresContactGeometry*    sc = static_cast<SpheresContactGeometry*>(ig.get());


	if(wireFrame)
	{
		midMax=0;
		forceMax=0;
		glPushMatrix();
			glTranslatev(sc->contactPoint);
			glBegin(GL_LINES);
				glColor3(0.0,1.0,0.0);
				glVertex3(0.0,0.0,0.0);
				glVertex3v(-1.0*(sc->normal*sc->radius1));
				glVertex3(0.0,0.0,0.0);
				glVertex3v( 1.0*(sc->normal*sc->radius2));
			glEnd();
		glPopMatrix();
	}
	else
	{
		#if 0
		Quaternionr q;
		q.Align(Vector3r(1.0,0.0,0.0),sc->normal);
		Vector3r axis;	
		Real angle;
		q.ToAxisAngle(axis,angle);

		Real mid = (sc->radius1 + sc->radius2);
		Real dif = (sc->radius1 - sc->radius2)*0.5;
		midMax = std::max(mid,midMax);
		glPushMatrix();
			glTranslatev(sc->contactPoint-(sc->normal*dif));
			glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
			// BUG: would crash with anything else than ElasticContactInteraction; use GLDrawInteractionPhysics for such things
			/// ElasticContactInteraction* el = static_cast<ElasticContactInteraction*>(ip->interactionPhysics.get());
			// FIXME - we need a way to give parameters from outside, again.... so curerntly this scale is hardcoded here
			if( (!ip->isNew) && ip->isReal && ip->interactionPhysics){
				Real force = el->normalForce.Length()/600;
				forceMax = std::max(force,forceMax);
				Real scale = midMax*(force/forceMax)*0.3;
				glScalef(mid,scale,scale);
			}
			else 
				glScalef(mid,mid*0.05,mid*0.05);
			glColor3(0.5,0.5,0.5);
	 
			glEnable(GL_LIGHTING);
			glutSolidCube(1.0);
		#endif
	}

	if(sc->exactRot){
		//GLUtils::GLDrawLine(sc->pos1,sc->pos2,Vector3r(1,1,1));
		// sphere center to point on the sphere
		GLUtils::GLDrawText("[1]",sc->pos1,Vector3r(1,1,1)); GLUtils::GLDrawText("[2]",sc->pos2,Vector3r(1,1,1));
		GLUtils::GLDrawLine(sc->pos1,sc->pos1+(sc->ori1*sc->cp1rel*Vector3r::UNIT_X),Vector3r(0,.5,1));
		GLUtils::GLDrawLine(sc->pos2,sc->pos2+(sc->ori2*sc->cp2rel*Vector3r::UNIT_X),Vector3r(0,1,.5));
		//cerr<<"=== cp1rel="<<sc->cp1rel[0]<<";"<<sc->cp1rel[1]<<";"<<sc->cp1rel[2]<<";"<<sc->cp1rel[3]<<endl;
		//cerr<<"=== ori1="<<sc->ori1[0]<<";"<<sc->ori1[1]<<";"<<sc->ori1[2]<<";"<<sc->ori1[3]<<endl;
		//cerr<<"+++ cp1rel="<<sc->cp1rel<<", ori1="<<sc->ori1<<", cp1rel*ori1="<<sc->cp1rel*sc->ori1<<endl;
		//<<", *UNIT_X="<<sc->cp1rel*sc->ori1*Vector3r::UNIT_X<<", +pos1="<<sc->pos1+(sc->cp1rel*sc->ori1*Vector3r::UNIT_X)<<endl;
		// contact point to projected points
		Vector3r ptTg1=sc->contPtInTgPlane1(), ptTg2=sc->contPtInTgPlane2();
		GLUtils::GLDrawLine(sc->contPt(),sc->contPt()+ptTg1,Vector3r(0,.5,1));
		GLUtils::GLDrawLine(sc->contPt(),sc->contPt()+ptTg2,Vector3r(0,1,.5));
		// projected shear
		GLUtils::GLDrawLine(sc->contPt()+ptTg1,sc->contPt()+ptTg2,Vector3r(.7,.7,.7));
		// TODO: crosshair to show contact plane (?)
	}



}

