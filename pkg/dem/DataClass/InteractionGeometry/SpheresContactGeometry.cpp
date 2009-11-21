// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#include "SpheresContactGeometry.hpp"
#include<yade/core/Omega.hpp>
YADE_PLUGIN((SpheresContactGeometry));
// At least one virtual method must be in the .cpp file (!!!)
SpheresContactGeometry::~SpheresContactGeometry(){};

#ifdef SCG_SHEAR
Vector3r SpheresContactGeometry::updateShear(const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting){

	Vector3r axis;
	Real angle;

	Vector3r shearIncrement(Vector3r::ZERO);

	// approximated rotations
		axis = prevNormal.Cross(normal); 
		shearIncrement -= shear.Cross(axis);
		angle = dt*0.5*normal.Dot(rbp1->angVel + rbp2->angVel);
		axis = angle*normal;
		shearIncrement -= (shear+shearIncrement).Cross(axis);
		
	// exact rotations (not adapted to shear/shearIncrement!)
	#if 0
		Quaternionr q;
		axis					= prevNormal.Cross(normal);
		angle					= acos(normal.Dot(prevNormal));
		q.FromAngleAxis(angle,axis);
		shearForce        = shearForce*q;
		angle             = dt*0.5*normal.dot(rbp1->angVel+rbp2->angVel);
		axis					= normal;
		q.FromAngleAxis(angle,axis);
		shearForce        = q*shearForce;
	#endif

	Vector3r& x = contactPoint;
	Vector3r c1x, c2x;

	if(avoidGranularRatcheting){
		/* The following definition of c1x and c2x is to avoid "granular ratcheting" 
		 *  (see F. ALONSO-MARROQUIN, R. GARCIA-ROJO, H.J. HERRMANN, 
		 *  "Micro-mechanical investigation of granular ratcheting, in Cyclic Behaviour of Soils and Liquefaction Phenomena",
		 *  ed. T. Triantafyllidis (Balklema, London, 2004), p. 3-10 - and a lot more papers from the same authors) */

		// FIXME: For sphere-facet contact this will give an erroneous value of relative velocity...
		c1x =   radius1*normal; 
		c2x =  -radius2*normal;
	}
	else {
		// FIXME: It is correct for sphere-sphere and sphere-facet contact
		c1x = (x - rbp1->pos);
		c2x = (x - rbp2->pos);
	}

	Vector3r relativeVelocity = (rbp2->vel+rbp2->angVel.Cross(c2x)) - (rbp1->vel+rbp1->angVel.Cross(c1x));
	Vector3r shearVelocity = relativeVelocity-normal.Dot(relativeVelocity)*normal;
	Vector3r shearDisplacement = shearVelocity*dt;
	shearIncrement -= shearDisplacement;

	shear+=shearIncrement;
	return shearIncrement;
}
#endif

void SpheresContactGeometry::updateShearForce(Vector3r& shearForce, Real ks, const Vector3r& prevNormal, const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting){

	Vector3r axis;
	Real angle;

	// approximated rotations
		axis = prevNormal.Cross(normal); 
		shearForce -= shearForce.Cross(axis);
		angle = dt*0.5*normal.Dot(rbp1->angVel + rbp2->angVel);
		axis = angle*normal;
		shearForce -= shearForce.Cross(axis);
		
	// exact rotations
	#if 0
		Quaternionr q;
		axis					= prevNormal.Cross(normal);
		angle					= acos(normal.Dot(prevNormal));
		q.FromAngleAxis(angle,axis);
		shearForce        = shearForce*q;
		angle             = dt*0.5*normal.dot(rbp1->angVel+rbp2->angVel);
		axis					= normal;
		q.FromAngleAxis(angle,axis);
		shearForce        = q*shearForce;
	#endif

	Vector3r& x = contactPoint;
	Vector3r c1x, c2x;

	if(avoidGranularRatcheting){
		/* The following definition of c1x and c2x is to avoid "granular ratcheting" 
		 *  (see F. ALONSO-MARROQUIN, R. GARCIA-ROJO, H.J. HERRMANN, 
		 *  "Micro-mechanical investigation of granular ratcheting, in Cyclic Behaviour of Soils and Liquefaction Phenomena",
		 *  ed. T. Triantafyllidis (Balklema, London, 2004), p. 3-10 - and a lot more papers from the same authors) */

		// FIXME: For sphere-facet contact this will give an erroneous value of relative velocity...
		c1x =   radius1*normal; 
		c2x =  -radius2*normal;
	}
	else {
		// FIXME: It is correct for sphere-sphere and sphere-facet contact
		c1x = (x - rbp1->pos);
		c2x = (x - rbp2->pos);
	}

	Vector3r relativeVelocity = (rbp2->vel+rbp2->angVel.Cross(c2x)) - (rbp1->vel+rbp1->angVel.Cross(c1x));
	Vector3r shearVelocity = relativeVelocity-normal.Dot(relativeVelocity)*normal;
	Vector3r shearDisplacement = shearVelocity*dt;
	shearForce -= ks*shearDisplacement;
}


/* keep this for reference; declarations in the header */
#if 0
	Vector3r SpheresContactGeometry::relRotVector() const{
		Quaternionr relOri12=ori1.Conjugate()*ori2;
		Quaternionr oriDiff=initRelOri12.Conjugate()*relOri12;
		Vector3r axis; Real angle;
		oriDiff.ToAxisAngle(axis,angle);
		if(angle>Mathr::PI)angle-=Mathr::TWO_PI;
		return angle*axis;
	}

	void SpheresContactGeometry::bendingTorsionAbs(Vector3r& bend, Real& tors){
		Vector3r relRot=relRotVector();
		tors=relRot.Dot(normal);
		bend=relRot-tors*normal;
	}
#endif

