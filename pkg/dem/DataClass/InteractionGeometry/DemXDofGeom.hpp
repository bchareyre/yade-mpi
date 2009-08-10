// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/InteractionGeometry.hpp>

/*! Abstract class that unites SpheresContactGeometry and Dem3DofGeom,
	created for the purposes of GlobalStiffnessTimeStepper.
	It exists purely on the c++, i.e. registers no attributes (the derived classes do register
	their attributes that are initialized as references here) and doesn't exist in the yade
	hierarchy. */
class GenericSpheresContact: public InteractionGeometry{
	public:
		Vector3r normal;
		Real refR1, refR2;
	virtual ~GenericSpheresContact(); // vtable
};

/*! Abstract base class for representing contact geometry of 2 elements that has 3 degrees of freedom:
 *  normal (1 component) and shear (Vector3r, but in plane perpendicular to the normal)
 */
class Dem3DofGeom: public GenericSpheresContact{
	public:
		//! some length used to convert displacements to strains
		Real refLength;
		//! some unit reference vector (in the direction of the interaction)
		Vector3r &normal;
		//! some reference point for the interaction
		Vector3r contactPoint;
		//! make strain go to -∞ for length going to zero
		bool logCompression;
		//! se3 of both bodies (needed to compute torque from the contact, strains etc). Must be updated at every step.
		Se3r se31, se32;
		//! reference radii of particles (for initial contact stiffness computation)
		Real &refR1, &refR2;

		Dem3DofGeom(): normal(GenericSpheresContact::normal), refR1(GenericSpheresContact::refR1), refR2(GenericSpheresContact::refR2) {}

		// API that needs to be implemented in derived classes
		virtual Real displacementN();
		virtual Vector3r displacementT(){throw;}
		virtual Real slipToDisplacementTMax(Real displacementTMax){throw;}; // plasticity
		// reference radii, for contact stiffness computation; set to negative for nonsense values
		// end API

		Real strainN(){
			//if(!logCompression)
			return displacementN()/refLength;
			//else{Real dn=displacementN(); }
		}
		Vector3r strainT(){return displacementT()/refLength;}
		Real slipToStrainTMax(Real strainTMax){return slipToDisplacementTMax(strainTMax*refLength)/refLength;}

		REGISTER_CLASS_AND_BASE(Dem3DofGeom,InteractionGeometry);
		REGISTER_ATTRIBUTES(InteractionGeometry,(refLength)(normal)(contactPoint)(se31)(se32)(refR1)(refR2));
};
REGISTER_SERIALIZABLE(Dem3DofGeom);

#if 0
/*! Abstract class for providing torsion and bending, in addition to inherited normal and shear strains. */
class Dem6DofGeom: public Dem3DofGeom {
	public:
		//! rotations perpendicular to the normal (bending; in global coords) and parallel with the normal (torsion)
		void bendingTorsionAbs(Vector3r& bend, Real& tors)=0;
		void bendingTorsionRel(Vector3r& bend, Real& tors){ bendingTorsionAbs(bend,tors); bend/=refLength; tors/=refLength;}
	REGISTER_CLASS_AND_BASE(Dem6DofGeom,Dem3DofGeom);
	REGISTER_ATTRIBUTES(Dem3DofGeom, /*nothing*/);
};
REGISTER_SERIALIZABLE(Dem6DofGeom);
#endif

