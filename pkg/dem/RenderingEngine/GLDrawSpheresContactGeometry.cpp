/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"GLDrawSpheresContactGeometry.hpp"
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>

#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/lib-opengl/GLUtils.hpp>

YADE_PLUGIN((GLDrawSpheresContactGeometry));
YADE_REQUIRE_FEATURE(OPENGL)
CREATE_LOGGER(GLDrawSpheresContactGeometry);

void GLDrawSpheresContactGeometry::go(
		const shared_ptr<InteractionGeometry>& ig,
		const shared_ptr<Interaction>& ip,
		const shared_ptr<Body>& b1,
		const shared_ptr<Body>& b2,
		bool wireFrame)
{
	ScGeom* sc = static_cast<ScGeom*>(ig.get());

	#if 0
		const Se3r& se31=b1->physicalParameters->dispSe3,se32=b2->physicalParameters->dispSe3;
		const Vector3r& pos1=se31.position,pos2=se32.position;
	#endif

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
			if( (!ip->isNew) && ip->isReal() && ip->interactionPhysics){
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

}


YADE_REQUIRE_FEATURE(PHYSPAR);

