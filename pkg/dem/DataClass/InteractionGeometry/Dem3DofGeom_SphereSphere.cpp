#include "Dem3DofGeom_SphereSphere.hpp"

#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/core/Omega.hpp>
YADE_PLUGIN((Dem3DofGeom_SphereSphere)
	#ifdef YADE_OPENGL
		(GLDraw_Dem3DofGeom_SphereSphere)
	#endif
	(ef2_Sphere_Sphere_Dem3DofGeom));


Dem3DofGeom_SphereSphere::~Dem3DofGeom_SphereSphere(){}

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
Vector3r Dem3DofGeom_SphereSphere::unrollSpherePtToPlane(const Quaternionr& fromXtoPtOri, const Real& radius, const Vector3r& planeNormal){
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
 * @param planeNormal _unit_ vector pointing away from sphere center
 * @returns orientation that transforms +x axis to the vector between sphere center and point on the sphere that corresponds to planePt.
 *
 * @note It is not checked whether planePt relly lies on the tangent plane. If not, result will be incorrect.
 */
Quaternionr Dem3DofGeom_SphereSphere::rollPlanePtToSphere(const Vector3r& planePt, const Real& radius, const Vector3r& planeNormal){
	Vector3r axis=planeNormal.Cross(planePt); axis.Normalize();
	Real angle=planePt.Length()/radius;
	Quaternionr normal2pt(axis,angle);
	Quaternionr ret; ret.Align(Vector3r::UNIT_X,normal2pt*planeNormal);
	return ret;
}



/* Set contact points on both spheres such that their projection is the one given
 * (should be on the plane passing through origin and oriented with normal; not checked!)
 */
void Dem3DofGeom_SphereSphere::setTgPlanePts(Vector3r p1new, Vector3r p2new){
	cp1rel=ori1.Conjugate()*rollPlanePtToSphere(p1new,effR1,normal);
	cp2rel=ori2.Conjugate()*rollPlanePtToSphere(p2new,effR2,-normal);
}



/*! Perform slip of the projected contact points so that their distance becomes equal (or remains smaller) than the given one.
 * The slipped distance is returned.
 */
Real Dem3DofGeom_SphereSphere::slipToDisplacementTMax(Real displacementTMax){
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


/* Move contact point on both spheres in such way that their relative position (displacementT) is the same;
 * this should be done regularly to ensure that the angle doesn't go over π, since then quaternion would
 * flip axis and the point would project on other side of the tangent plane piece. */
void Dem3DofGeom_SphereSphere::relocateContactPoints(){
	relocateContactPoints(contPtInTgPlane1(),contPtInTgPlane2());
}

/*! Like Dem3DofGeom_SphereSphere::relocateContactPoints(), but use already computed tangent plane points. */
void Dem3DofGeom_SphereSphere::relocateContactPoints(const Vector3r& p1, const Vector3r& p2){
	Vector3r midPt=(effR1/(effR1+effR2))*(p1+p2); // proportionally to radii, so that angle would be the same
	if((p1.SquaredLength()>pow(effR1,2) || p2.SquaredLength()>pow(effR2,2)) && midPt.SquaredLength()>pow(min(effR1,effR2),2)){
		//cerr<<"RELOCATION with displacementT="<<displacementT(); // should be the same before and after relocation
		setTgPlanePts(p1-midPt,p2-midPt);
		//cerr<<" → "<<displacementT()<<endl;
	}
}

#ifdef YADE_OPENGL
	#include<yade/lib-opengl/OpenGLWrapper.hpp>
	#include<yade/lib-opengl/GLUtils.hpp>
	bool GLDraw_Dem3DofGeom_SphereSphere::normal=false;
	bool GLDraw_Dem3DofGeom_SphereSphere::rolledPoints=false;
	bool GLDraw_Dem3DofGeom_SphereSphere::unrolledPoints=false;
	bool GLDraw_Dem3DofGeom_SphereSphere::shear=false;
	bool GLDraw_Dem3DofGeom_SphereSphere::shearLabel=false;

	void GLDraw_Dem3DofGeom_SphereSphere::go(const shared_ptr<InteractionGeometry>& ig, const shared_ptr<Interaction>& ip, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
		Dem3DofGeom_SphereSphere* ss = static_cast<Dem3DofGeom_SphereSphere*>(ig.get());
		//const Se3r& se31=b1->physicalParameters->dispSe3,se32=b2->physicalParameters->dispSe3;
		const Se3r& se31=b1->state->se3,se32=b2->state->se3;
		const Vector3r& pos1=se31.position,pos2=se32.position;
		Vector3r& contPt=ss->contactPoint;
		
		if(normal){
			GLUtils::GLDrawArrow(contPt,contPt+ss->normal*.5*ss->refLength); // normal of the contact
		}
		#if 0
			// never used, since bending/torsion not used
			//Vector3r contPt=se31.position+(ss->effR1/ss->refLength)*(se32.position-se31.position); // must be recalculated to not be unscaled if scaling displacements ...
			GLUtils::GLDrawLine(pos1,pos2,Vector3r(.5,.5,.5));
			Vector3r bend; Real tors;
			ss->bendingTorsionRel(bend,tors);
			GLUtils::GLDrawLine(contPt,contPt+10*ss->radius1*(bend+ss->normal*tors),Vector3r(1,0,0));
			#if 0
				GLUtils::GLDrawNum(bend[0],contPt-.2*ss->normal*ss->radius1,Vector3r(1,0,0));
				GLUtils::GLDrawNum(bend[1],contPt,Vector3r(0,1,0));
				GLUtils::GLDrawNum(bend[2],contPt+.2*ss->normal*ss->radius1,Vector3r(0,0,1));
				GLUtils::GLDrawNum(tors,contPt+.5*ss->normal*ss->radius2,Vector3r(1,1,0));
			#endif
		#endif
		// sphere center to point on the sphere
		if(rolledPoints){
			GLUtils::GLDrawLine(pos1,pos1+(ss->ori1*ss->cp1rel*Vector3r::UNIT_X*ss->effR1),Vector3r(0,.5,1));
			GLUtils::GLDrawLine(pos2,pos2+(ss->ori2*ss->cp2rel*Vector3r::UNIT_X*ss->effR2),Vector3r(0,1,.5));
		}
		//TRVAR4(pos1,ss->ori1,pos2,ss->ori2);
		//TRVAR2(ss->cp2rel,pos2+(ss->ori2*ss->cp2rel*Vector3r::UNIT_X*ss->effR2));
		// contact point to projected points
		if(unrolledPoints||shear){
			Vector3r ptTg1=ss->contPtInTgPlane1(), ptTg2=ss->contPtInTgPlane2();
			if(unrolledPoints){
				//TRVAR3(ptTg1,ptTg2,ss->normal)
				GLUtils::GLDrawLine(contPt,contPt+ptTg1,Vector3r(0,.5,1)); GLUtils::GLDrawLine(pos1,contPt+ptTg1,Vector3r(0,.5,1));
				GLUtils::GLDrawLine(contPt,contPt+ptTg2,Vector3r(0,1,.5)); GLUtils::GLDrawLine(pos2,contPt+ptTg2,Vector3r(0,1,.5));
			}
			if(shear){
				GLUtils::GLDrawLine(contPt+ptTg1,contPt+ptTg2,Vector3r(1,1,1));
				if(shearLabel) GLUtils::GLDrawNum(ss->displacementT().Length(),contPt,Vector3r(1,1,1));
			}
		}
	}
	CREATE_LOGGER(ef2_Sphere_Sphere_Dem3DofGeom);
#endif

bool ef2_Sphere_Sphere_Dem3DofGeom::go(const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const shared_ptr<Interaction>& c){
	InteractingSphere *s1=static_cast<InteractingSphere*>(cm1.get()), *s2=static_cast<InteractingSphere*>(cm2.get());
	Vector3r normal=(state2.pos+shift2)-state1.pos;
	Real penetrationDepthSq=pow((distFactor>0?distFactor:1.)*(s1->radius+s2->radius),2)-normal.SquaredLength();
	if (penetrationDepthSq<0 && !c->isReal()){
		return false;
	}

	Real dist=normal.Normalize(); /* Normalize() works in-place and returns length before normalization; from here, normal is unit vector */
	shared_ptr<Dem3DofGeom_SphereSphere> ss;
	if(c->interactionGeometry) ss=YADE_PTR_CAST<Dem3DofGeom_SphereSphere>(c->interactionGeometry);
	else {
		ss=shared_ptr<Dem3DofGeom_SphereSphere>(new Dem3DofGeom_SphereSphere());
		c->interactionGeometry=ss;
		// constants
		if(distFactor>0) ss->refLength=dist;
		else ss->refLength=s1->radius+s2->radius;
		ss->refR1=s1->radius; ss->refR2=s2->radius;
		Real penetrationDepth=s1->radius+s2->radius-ss->refLength;
		if(Omega::instance().getCurrentIteration()<=10){
			ss->effR1=s1->radius-.5*penetrationDepth; ss->effR2=s2->radius-.5*penetrationDepth;
		} else {ss->effR1=s1->radius; ss->effR2=s2->radius;}
		// for bending only: ss->initRelOri12=state1.ori.Conjugate()*state2.ori;
		// quasi-constants
		ss->cp1rel.Align(Vector3r::UNIT_X,state1.ori.Conjugate()*normal);
		ss->cp2rel.Align(Vector3r::UNIT_X,state2.ori.Conjugate()*(-normal));
		ss->cp1rel.Normalize(); ss->cp2rel.Normalize();
	}
	ss->normal=normal;
	ss->contactPoint=state1.pos+(ss->effR1-.5*(ss->refLength-dist))*ss->normal;
	ss->se31=state1.se3; ss->se32=state2.se3;
	return true;
}

