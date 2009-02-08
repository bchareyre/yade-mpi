// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#include "SpheresContactGeometry.hpp"
#include<yade/core/Omega.hpp>
YADE_PLUGIN("SpheresContactGeometry");

// At least one virtual method must be in the .cpp file (!!!)
SpheresContactGeometry::~SpheresContactGeometry(){};

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
	if(displacementTMax<=Mathr::ZERO_TOLERANCE){ setTgPlanePts(Vector3r(0,0,0),Vector3r(0,0,0)); return displacementTMax;}
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


