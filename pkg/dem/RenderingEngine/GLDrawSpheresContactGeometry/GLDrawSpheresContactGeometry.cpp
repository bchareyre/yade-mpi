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
CREATE_LOGGER(GLDrawSpheresContactGeometry);

void GLDrawSpheresContactGeometry::go(
		const shared_ptr<InteractionGeometry>& ig,
		const shared_ptr<Interaction>& ip,
		const shared_ptr<Body>& b1,
		const shared_ptr<Body>& b2,
		bool wireFrame)
{
	SpheresContactGeometry* sc = static_cast<SpheresContactGeometry*>(ig.get());

	const Se3r& se31=b1->physicalParameters->dispSe3,se32=b2->physicalParameters->dispSe3;
	const Vector3r& pos1=se31.position,pos2=se32.position;

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

	if(sc->hasShear){
		Vector3r pos1=sc->pos1, pos2=sc->pos2, contPt=sc->contPt();
		//Vector3r contPt=se31.position+(sc->d1/sc->d0)*(se32.position-se31.position); // must be recalculated to not be unscaled if scaling displacements ...
		GLUtils::GLDrawLine(pos1,pos2,Vector3r(.5,.5,.5));
		Vector3r bend; Real tors;
		sc->bendingTorsionRel(bend,tors);
		GLUtils::GLDrawLine(contPt,contPt+10*sc->radius1*(bend+sc->normal*tors),Vector3r(1,0,0));
		#if 0
		GLUtils::GLDrawNum(bend[0],contPt-.2*sc->normal*sc->radius1,Vector3r(1,0,0));
		GLUtils::GLDrawNum(bend[1],contPt,Vector3r(0,1,0));
		GLUtils::GLDrawNum(bend[2],contPt+.2*sc->normal*sc->radius1,Vector3r(0,0,1));
		GLUtils::GLDrawNum(tors,contPt+.5*sc->normal*sc->radius2,Vector3r(1,1,0));
		#endif
		// sphere center to point on the sphere
		//GLUtils::GLDrawLine(pos1,pos1+(sc->ori1*sc->cp1rel*Vector3r::UNIT_X*sc->d1),Vector3r(0,.5,1));
		//GLUtils::GLDrawLine(pos2,pos2+(sc->ori2*sc->cp2rel*Vector3r::UNIT_X*sc->d2),Vector3r(0,1,.5));
		//TRVAR4(pos1,sc->ori1,pos2,sc->ori2);
		//TRVAR2(sc->cp2rel,pos2+(sc->ori2*sc->cp2rel*Vector3r::UNIT_X*sc->d2));
		// contact point to projected points
		Vector3r ptTg1=sc->contPtInTgPlane1(), ptTg2=sc->contPtInTgPlane2();
		TRVAR3(ptTg1,ptTg2,sc->normal)
		//GLUtils::GLDrawLine(contPt,contPt+ptTg1,Vector3r(0,.5,1)); GLUtils::GLDrawLine(pos1,contPt+ptTg1,Vector3r(0,.5,1));
		//GLUtils::GLDrawLine(contPt,contPt+ptTg2,Vector3r(0,1,.5)); GLUtils::GLDrawLine(pos2,contPt+ptTg2,Vector3r(0,1,.5));
		// projected shear
		GLUtils::GLDrawLine(contPt+ptTg1,contPt+ptTg2,Vector3r(1,1,1));
		// 
		//GLUtils::GLDrawNum(sc->epsN(),contPt,Vector3r(1,1,1));
	}



}

