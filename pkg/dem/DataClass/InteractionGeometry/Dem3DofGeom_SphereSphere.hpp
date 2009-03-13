#pragma once
#include<yade/pkg-dem/DemXDofGeom.hpp>

class Dem3DofGeom_SphereSphere: public Dem3DofGeom{
	public:
		// auxiliary functions; the quaternion magic is all in here
		static Vector3r unrollSpherePtToPlane(const Quaternionr& fromXtoPtOri, const Real& radius, const Vector3r& normal);
		static Quaternionr rollPlanePtToSphere(const Vector3r& planePt, const Real& radius, const Vector3r& normal);
	private:
		Vector3r contPtInTgPlane1() const { return unrollSpherePtToPlane(ori1*cp1rel,effR1,normal); }
		Vector3r contPtInTgPlane2() const { return unrollSpherePtToPlane(ori2*cp2rel,effR2,-normal);}
		void setTgPlanePts(Vector3r p1new, Vector3r p2new);
		void relocateContactPoints();
		void relocateContactPoints(const Vector3r& tgPlanePt1, const Vector3r& tgPlanePt2);
	public:
		//! Positions and orientations of both spheres; must be updated at every iteration by the geom functor
		Se3r se31, se32;
		const Vector3r& pos1, pos2; const Quaternionr& ori1, ori2;
		Quaternionr cp1rel, cp2rel;
		Dem3DofGeom_SphereSphere(): pos1(se31.position), pos2(se32.position), ori1(se31.orientation), ori2(se32.orientation){}
		~Dem3DofGeom_SphereSphere();
		//! effective radii of spheres for this interaction; can be smaller/larger than actual radii, but quasi-constant throughout the interaction life
		Real effR1, effR2;
		
		/********* API **********/
		Real displacementN(){ return (pos2-pos2).Length()-refLength; }
		Vector3r displacementT() {
			// enabling automatic relocation decreases overall simulation speed by about 3%; perhaps: bool largeStrains ... ?
			#if 1 
				Vector3r p1=contPtInTgPlane1(), p2=contPtInTgPlane2(); relocateContactPoints(p1,p2); return p2-p1; // shear before relocation, but that is OK
			#else
				return contPtInTgPlane2()-contPtInTgPlane1();
			#endif
		}
		Real slipToDisplacementTMax(Real displacementTMax);
		/********* end API ***********/

	REGISTER_ATTRIBUTES(Dem3DofGeom,(se31)(se32)(effR1)(effR2));
};
REGISTER_SERIALIZABLE(Dem3DofGeom_SphereSphere);

