// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/core/InteractionGeometry.hpp>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
/*! Class representing geometry of two spheres in contact.
 *
 * The code under SCG_SHEAR is experimental and is used only if ElasticContactLaw::useShear is explicitly true
 */

#define SCG_SHEAR

class SpheresContactGeometry: public InteractionGeometry{
	public:
		Vector3r normal, // unit vector in the direction from sphere1 center to sphere2 center
			contactPoint;
		Real radius1,radius2,penetrationDepth;

		#ifdef SCG_SHEAR
			//! Total value of the current shear. Update the value using updateShear.
			Vector3r shear;
			//! Normal of the contact in the previous step
			Vector3r prevNormal;
			//! update shear on this contact given dynamic parameters of bodies. Should be called from constitutive law, exactly once per iteration. Returns shear increment in this update, which is already added to shear.
			Vector3r updateShear(const RigidBodyParameters* rbp1, const RigidBodyParameters* rbp2, Real dt, bool avoidGranularRatcheting=true);
		#endif
		
		// all the rest here will hopefully disappear at some point...

		// begin abusive storage
		//! Whether the original contact was on the positive (1) or negative (-1) facet side; this is to permit repulsion to the right side even if the sphere passes, under extreme pressure, geometrically to the other facet's side. This is used only in InteractingFacet2IteractingSphere4SpheresContactGeometry. If at any point the contact is with the edge or vertex instead of face, this attribute is reset so that contact with the other face is possible.
		int facetContactFace;
		// end abusive storage

		bool hasShear; // whether the exact rotation code is being used -- following fields are needed for that
		//! positions and orientations of both spheres -- must be updated at every iteration
		Vector3r pos1, pos2; Quaternionr ori1, ori2;
		/*! Orientation of the contact point relative to each sphere-local coordinates.+
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
		// d0fixup is added to d0 when computing normal strain; it should fix problems with sphere-facet interactions never getting enough compressed
		Real d1, d2, d0, d0fixup;
		// initial relative orientation, used for bending and torsion computation
		Quaternionr initRelOri12;

		// auxiliary functions; the quaternion magic is all in here
		static Vector3r unrollSpherePtToPlane(const Quaternionr& fromXtoPtOri, const Real& radius, const Vector3r& normal);
		static Quaternionr rollPlanePtToSphere(const Vector3r& planePt, const Real& radius, const Vector3r& normal);

		void setTgPlanePts(Vector3r p1new, Vector3r p2new);
		
		Vector3r contPtInTgPlane1() const {assert(hasShear); return unrollSpherePtToPlane(ori1*cp1rel,d1,normal);}
		Vector3r contPtInTgPlane2() const {assert(hasShear); return unrollSpherePtToPlane(ori2*cp2rel,d2,-normal);}
		Vector3r contPt() const {return contactPoint; /*pos1+(d1/d0)*(pos2-pos1)*/}

		Real displacementN() const {assert(hasShear); return (pos2-pos1).Length()-d0;}
		Real epsN() const {return displacementN()*(1./(d0+d0fixup));}
		Vector3r displacementT() { assert(hasShear);
			// enabling automatic relocation decreases overall simulation speed by about 3%
			// perhaps: bool largeStrains ... ?
			#if 1 
				Vector3r p1=contPtInTgPlane1(), p2=contPtInTgPlane2();
				relocateContactPoints(p1,p2);
				return p2-p1; // shear before relocation, but that is OK
			#else
				return contPtInTgPlane2()-contPtInTgPlane1();
			#endif
		}
		Vector3r epsT() {return displacementT()*(1./(d0+d0fixup));}
	
		Real slipToDisplacementTMax(Real displacementTMax);
		//! slip to epsTMax if current epsT is greater; return the relative slip magnitude
		Real slipToStrainTMax(Real epsTMax){ return (1/d0)*slipToDisplacementTMax(epsTMax*d0);}

		void relocateContactPoints();
		void relocateContactPoints(const Vector3r& tgPlanePt1, const Vector3r& tgPlanePt2);

		void bendingTorsionAbs(Vector3r& bend, Real& tors);
		void bendingTorsionRel(Vector3r& bend, Real& tors){ bendingTorsionAbs(bend,tors); bend/=d0; tors/=d0;}

		Vector3r relRotVector() const;

		SpheresContactGeometry():contactPoint(Vector3r::ZERO),radius1(0),radius2(0),facetContactFace(0.),hasShear(false),pos1(Vector3r::ZERO),pos2(Vector3r::ZERO),ori1(Quaternionr::IDENTITY),ori2(Quaternionr::IDENTITY),cp1rel(Quaternionr::IDENTITY),cp2rel(Quaternionr::IDENTITY),d1(0),d2(0),d0(0),d0fixup(0),initRelOri12(Quaternionr::IDENTITY){createIndex();
		#ifdef SCG_SHEAR
			shear=Vector3r::ZERO; prevNormal=Vector3r::ZERO /*initialized to proper value by geom functor*/;
		#endif	
		}
		virtual ~SpheresContactGeometry();

		void updateShearForce(Vector3r& shearForce, Real ks, const Vector3r& prevNormal, const RigidBodyParameters* rbp1, const RigidBodyParameters* rbp2, Real dt, bool avoidGranularRatcheting=true);

	REGISTER_ATTRIBUTES(/* no attributes from base class */,
			(normal)
			(contactPoint)
			(radius1)
			(radius2)
			#ifdef SCG_SHEAR
				(shear)
				(prevNormal)
			#endif
			(facetContactFace)
			// hasShear
			(hasShear)
			(pos1)
			(pos2)
			(ori1)
			(ori2)
			(cp1rel)
			(cp2rel)
			(d1)
			(d2)
			(d0)
			(d0fixup)
			(initRelOri12));
	REGISTER_CLASS_AND_BASE(SpheresContactGeometry,InteractionGeometry);
	REGISTER_CLASS_INDEX(SpheresContactGeometry,InteractionGeometry);
};

REGISTER_SERIALIZABLE(SpheresContactGeometry);

