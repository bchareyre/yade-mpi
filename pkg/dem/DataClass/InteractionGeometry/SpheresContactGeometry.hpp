// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/core/InteractionGeometry.hpp>
#include<yade/lib-base/yadeWm3.hpp>
/*! Class representing geometry of two spheres in contact.
 *
 * exactRot code
 * =============
 * At initial contact, each of the two spheres fixes a point on its surface relative to its own orientation.
 * It is therefore possible to derive at any later point how much slipping between the two spheres has taken
 * place since the first contact.
 *
 * The surface point is stored as quaternion.
 *
 * Function is provided to manipulate those points:
 *
 * (a) plastic slip, when we want to limit their maximum distance (in the projected plane)
 * (b) rolling, where those points must be relocated to not flip over the π angle from the current contact point
 *
 * TODO: add torsion code, that will (if a flag is on) compute torsion angle on the contact axis.
 *
 * TODO: add bending code, that will compute bending angle of the contact axis
 *
 *
 */
class SpheresContactGeometry: public InteractionGeometry{
	public:
		Vector3r normal, // unit vector in the direction from sphere1 center to sphere2 center
			contactPoint;
		Real radius1,radius2,penetrationDepth;

		bool hasShear; // whether the exact rotation code is being used -- following fields are needed for that
		//! positions and orientations of both spheres -- must be updated at every iteration
		Vector3r pos1, pos2; Quaternionr ori1, ori2;
		/*! Orientation of the contact point relative to each sphere-local coordinates.
		 * Those fields are almost constant, except for a few cases
		 * 	(a) plastic slip and 
		 * 	(b) spheres mutually rolling without slipping with big angle, when the contact point must be moved since quaternions
		 * 		describe only rotations up to π (if the rotation is bigger, then the shorter path from the other side is
		 * 		taken instead)
		 */
		Quaternionr cp1rel, cp2rel;
		// interaction "radii" and total length; this is _really_ constant throughout the interaction life
		// d1 is really distance from the sphere1 center to the contact plane, it may not correspond to the sphere radius!
		// therefore, d1+d2=d0 (distance at which the contact was created)
		Real d1, d2, d0;

		// auxiliary functions; the quaternion magic is all in here
		static Vector3r unrollSpherePtToPlane(const Quaternionr& fromXtoPtOri, const Real& radius, const Vector3r& normal);
		static Quaternionr rollPlanePtToSphere(const Vector3r& planePt, const Real& radius, const Vector3r& normal);

		void setTgPlanePts(Vector3r p1new, Vector3r p2new);

		Vector3r contPtInTgPlane1(){assert(hasShear); return unrollSpherePtToPlane(ori1*cp1rel,d1,normal);}
		Vector3r contPtInTgPlane2(){assert(hasShear); return unrollSpherePtToPlane(ori2*cp2rel,d2,-normal);}
		Vector3r contPt(){return contactPoint; /*pos1+(d1/d0)*(pos2-pos1)*/}

		Real displacementN(){assert(hasShear); return (pos2-pos1).Length()-d0;}
		Real epsN(){return displacementN()*(1./d0);}
		Vector3r displacementT(){ assert(hasShear);
			// enabling automatic relocation decreases overall simulation speed by about 3%
			// perhaps: bool largeStrains ... ?
			#if 0 
				Vector3r p1=contPtInTgPlane1(), p2=contPtInTgPlane2();
				relocateContactPoints(p1,p2);
				return p2-p1; // shear before relocation, but that should be OK
			#else
				return contPtInTgPlane2()-contPtInTgPlane1();
			#endif
		}
		Vector3r epsT(){return displacementT()*(1./d0);}
	
		Real slipToDisplacementTMax(Real displacementTMax);
		//! slip to epsTMax if current epsT is greater; return the relative slip magnitude
		Real slipToEpsTMax(Real epsTMax){ return (1/d0)*slipToDisplacementTMax(epsTMax*d0);}

		void relocateContactPoints();
		void relocateContactPoints(const Vector3r& tgPlanePt1, const Vector3r& tgPlanePt2);

		SpheresContactGeometry():contactPoint(Vector3r::ZERO),radius1(0),radius2(0),hasShear(false),pos1(Vector3r::ZERO),pos2(Vector3r::ZERO){createIndex();}
		virtual ~SpheresContactGeometry();
	protected :
		virtual void registerAttributes(){
			REGISTER_ATTRIBUTE(radius1);
			REGISTER_ATTRIBUTE(radius2);
			REGISTER_ATTRIBUTE(contactPoint); // to allow access from python
			// hasShear
			REGISTER_ATTRIBUTE(hasShear);
			REGISTER_ATTRIBUTE(pos1);
			REGISTER_ATTRIBUTE(pos2);
			REGISTER_ATTRIBUTE(ori1);
			REGISTER_ATTRIBUTE(ori2);
			REGISTER_ATTRIBUTE(cp1rel);
			REGISTER_ATTRIBUTE(cp2rel);
			REGISTER_ATTRIBUTE(d1);
			REGISTER_ATTRIBUTE(d2);
			REGISTER_ATTRIBUTE(d0);
		}
	REGISTER_CLASS_NAME(SpheresContactGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometry);
	REGISTER_CLASS_INDEX(SpheresContactGeometry,InteractionGeometry);
};

REGISTER_SERIALIZABLE(SpheresContactGeometry,false);

