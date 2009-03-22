// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#include "SpheresContactGeometry.hpp"
#include<yade/core/Omega.hpp>
YADE_PLUGIN("SpheresContactGeometry");

// At least one virtual method must be in the .cpp file (!!!)
SpheresContactGeometry::~SpheresContactGeometry(){};

#ifdef SCG_SHEAR
void SpheresContactGeometry::updateShear(const RigidBodyParameters* rbp1, const RigidBodyParameters* rbp2, Real dt, bool avoidGranularRatcheting){

	Vector3r axis;
	Real angle;

	shearIncrement=Vector3r::ZERO;

	// approximated rotations
		axis = prevNormal.Cross(normal); 
		shearIncrement -= shear.Cross(axis);
		angle = dt*0.5*normal.Dot(rbp1->angularVelocity + rbp2->angularVelocity);
		axis = angle*normal;
		shearIncrement -= (shear+shearIncrement).Cross(axis);
		
	// exact rotations (not adapted to shear/shearIncrement!)
	#if 0
		Quaternionr q;
		axis					= prevNormal.Cross(normal);
		angle					= acos(normal.Dot(prevNormal));
		q.FromAngleAxis(angle,axis);
		shearForce        = shearForce*q;
		angle             = dt*0.5*normal.dot(rbp1->angularVelocity+rbp2->angularVelocity);
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
		c1x = (x - rbp1->se3.position);
		c2x = (x - rbp2->se3.position);
	}

	Vector3r relativeVelocity = (rbp2->velocity+rbp2->angularVelocity.Cross(c2x)) - (rbp1->velocity+rbp1->angularVelocity.Cross(c1x));
	Vector3r shearVelocity = relativeVelocity-normal.Dot(relativeVelocity)*normal;
	Vector3r shearDisplacement = shearVelocity*dt;
	shearIncrement -= shearDisplacement;

	shear+=shearIncrement;
	shearUpdateIter=Omega::instance().getCurrentIteration();
}
#endif

void SpheresContactGeometry::updateShearForce(Vector3r& shearForce, Real ks, const Vector3r& prevNormal, const RigidBodyParameters* rbp1, const RigidBodyParameters* rbp2, Real dt, bool avoidGranularRatcheting){

	Vector3r axis;
	Real angle;

	// approximated rotations
		axis = prevNormal.Cross(normal); 
		shearForce -= shearForce.Cross(axis);
		angle = dt*0.5*normal.Dot(rbp1->angularVelocity + rbp2->angularVelocity);
		axis = angle*normal;
		shearForce -= shearForce.Cross(axis);
		
	// exact rotations
	#if 0
		Quaternionr q;
		axis					= prevNormal.Cross(normal);
		angle					= acos(normal.Dot(prevNormal));
		q.FromAngleAxis(angle,axis);
		shearForce        = shearForce*q;
		angle             = dt*0.5*normal.dot(rbp1->angularVelocity+rbp2->angularVelocity);
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
		c1x = (x - rbp1->se3.position);
		c2x = (x - rbp2->se3.position);
	}

	Vector3r relativeVelocity = (rbp2->velocity+rbp2->angularVelocity.Cross(c2x)) - (rbp1->velocity+rbp1->angularVelocity.Cross(c1x));
	Vector3r shearVelocity = relativeVelocity-normal.Dot(relativeVelocity)*normal;
	Vector3r shearDisplacement = shearVelocity*dt;
	shearForce -= ks*shearDisplacement;
}




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

/* Set contact points on both spheres such that their projection is the one given
 * (should be on the plane passing through origin and oriented with normal; not checked!)
 */
void SpheresContactGeometry::setTgPlanePts(Vector3r p1new, Vector3r p2new){
	assert(hasShear);
	cp1rel=ori1.Conjugate()*rollPlanePtToSphere(p1new,d1,normal);
	cp2rel=ori2.Conjugate()*rollPlanePtToSphere(p2new,d2,-normal);
}


/* Move contact point on both spheres in such way that their relative position (displacementT) is the same;
 * this should be done regularly to ensure that the angle doesn't go over π, since then quaternion would
 * flip axis and the point would project on other side of the tangent plane piece.
 */
void SpheresContactGeometry::relocateContactPoints(){
	assert(hasShear);
	relocateContactPoints(contPtInTgPlane1(),contPtInTgPlane2());
}

/* Like SpheresContactGeometry::relocateContactPoints(), but use already computed tangent plane points.
 *
 *
 */
void SpheresContactGeometry::relocateContactPoints(const Vector3r& p1, const Vector3r& p2){
	Vector3r midPt=(d1/(d1+d2))*(p1+p2); // proportionally to radii, so that angle would be the same
	if((p1.SquaredLength()>pow(d1,2) || p2.SquaredLength()>pow(d2,2)) && midPt.SquaredLength()>pow(min(d1,d2),2)){
		//cerr<<"RELOCATION with displacementT="<<displacementT(); // should be the same before and after relocation
		setTgPlanePts(p1-midPt,p2-midPt);
		//cerr<<" → "<<displacementT()<<endl;
	}
}


/*! Perform slip of the projected contact points so that their distance becomes equal (or remains smaller) than the given one.
 * The slipped distance is returned.
 */
Real SpheresContactGeometry::slipToDisplacementTMax(Real displacementTMax){
	assert(hasShear);
	// very close, reset shear
	if(displacementTMax<=0.){ setTgPlanePts(Vector3r(0,0,0),Vector3r(0,0,0)); return displacementTMax;}
	// otherwise
	Vector3r p1=contPtInTgPlane1(), p2=contPtInTgPlane2();
	Real currDistSq=(p2-p1).SquaredLength();
	if(currDistSq<pow(displacementTMax,2)) return 0; // close enough, no slip needed
	Vector3r diff=.5*(sqrt(currDistSq)/displacementTMax-1)*(p2-p1);
	setTgPlanePts(p1+diff,p2-diff);
	return 2*diff.Length();
}

/*! Project point from sphere surface to tangent plane,
 * such that the angle of shortest arc from (1,0,0) pt on the sphere to the point itself is the same
 * as the angle of segment of the same length on the tangent plane.
 *
 * This function is (or should be) inverse of SpheresContactGeometry::rollPlanePtToSphere.
 * 
 * @param fromXtoPtOri gives orientation of the vector from sphere center to the sphere point from the global +x axis.
 * @param radius the distance from sphere center to the contact plane
 * @param planeNormal unit vector pointing away from the sphere center, determining plane orientation on which the projected point lies.
 * @returns The projected point coordinates (with origin at the contact point).
 */
Vector3r SpheresContactGeometry::unrollSpherePtToPlane(const Quaternionr& fromXtoPtOri, const Real& radius, const Vector3r& planeNormal){
	Quaternionr normal2pt; normal2pt.Align(planeNormal,fromXtoPtOri*Vector3r::UNIT_X);
	Vector3r axis; Real angle; normal2pt.ToAxisAngle(axis,angle);
	return (angle*radius) /* length */ *(axis.Cross(planeNormal)) /* direction: both are unit vectors */;
}

/*! Project point from tangent plane to the sphere.
 *
 * This function is (or should be) inverse of SpheresContactGeometry::unrollSpherePtToPlane.
 *
 * @param planePt point on the tangent plane, with origin at the contact point (i.e. at sphere center + normal*radius)
 * @param radius sphere radius
 * @param planeNorma _unit_ vector pointing away from sphere center
 * @returns orientation that transforms +x axis to the vector between sphere center and point on the sphere that corresponds to planePt.
 *
 * @note It is not checked whether planePt relly lies on the tangent plane. If not, result will be incorrect.
 */
Quaternionr SpheresContactGeometry::rollPlanePtToSphere(const Vector3r& planePt, const Real& radius, const Vector3r& planeNormal){
		Vector3r axis=planeNormal.Cross(planePt); axis.Normalize();
		Real angle=planePt.Length()/radius;
		Quaternionr normal2pt(axis,angle);
		Quaternionr ret; ret.Align(Vector3r::UNIT_X,normal2pt*planeNormal);
		return ret;
}


