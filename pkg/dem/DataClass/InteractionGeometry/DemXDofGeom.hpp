// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/InteractionGeometry.hpp>

/*! Abstract class that unites ScGeom and Dem3DofGeom,
	created for the purposes of GlobalStiffnessTimeStepper.
	It might be removed in the future. */
class GenericSpheresContact: public InteractionGeometry{
	public:
		Vector3r normal;
		Real refR1, refR2;
	YADE_CLASS_BASE_DOC_ATTRS(GenericSpheresContact,InteractionGeometry,
		"Class uniting ScGeom and Dem3DofGeom, for the purposes of GlobalSittnessTimeStepper. (It might be removed inthe future). Do not use this class directly.",
		((normal,"Unit vector oriented along the interaction. |yupdate|"))
		((refR1,"Reference radius of particle #1. |ycomp|"))
		((refR2,"Reference radius of particle #2. |ycomp|"))
	);
	virtual ~GenericSpheresContact(); // vtable
};
REGISTER_SERIALIZABLE(GenericSpheresContact);

/*! Abstract base class for representing contact geometry of 2 elements that has 3 degrees of freedom:
 *  normal (1 component) and shear (Vector3r, but in plane perpendicular to the normal)
 */
class Dem3DofGeom: public GenericSpheresContact{
	public:
		//! some length used to convert displacements to strains
		Real refLength;
		// inherited from GenericSpheresContact: Vector3r& normal; Real& refR1, refR2;
		//! some reference point for the interaction
		Vector3r contactPoint;
		//! make strain go to -∞ for length going to zero
		bool logCompression;
		// copies of bodies' se3
		Se3r se31, se32;

		Dem3DofGeom(): logCompression(false) { createIndex(); }
		virtual ~Dem3DofGeom();

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

		YADE_CLASS_BASE_DOC_ATTRS(Dem3DofGeom,GenericSpheresContact,
			"Abstract base class for representing contact geometry of 2 elements that has 3 degrees of freedom: normal (1 component) and shear (Vector3r, but in plane perpendicular to the normal).",
			((refLength,"some length used to convert displacements to strains. |ycomp|"))
			((contactPoint,"some reference point for the interaction (usually in the middle). |ycomp|"))
			((logCompression,"make strain go to -∞ for length going to zero (false by default)."))
			((se31,"Copy of body #1 se3 (needed to compute torque from the contact, strains etc). |yupdate|"))
			((se31,"Copy of body #2 se3. |yupdate|"))
		)
		REGISTER_CLASS_INDEX(Dem3DofGeom,InteractionGeometry);
};
REGISTER_SERIALIZABLE(Dem3DofGeom);

#if 1
/*! Abstract class for providing torsion and bending, in addition to inherited normal and shear strains. */
class Dem6DofGeom: public Dem3DofGeom {
	public:
		//! rotations perpendicular to the normal (bending; in global coords) and parallel with the normal (torsion)
		virtual void bendTwistAbs(Vector3r& bend, Real& twist) {throw std::logic_error("bendTwistAbs not overridden in derived class.");};
		void bendTwistRel(Vector3r& bend, Real& twist){ bendTwistAbs(bend,twist); bend/=refLength; twist/=refLength;}
		virtual ~Dem6DofGeom();
	YADE_CLASS_BASE_DOC_ATTRS(Dem6DofGeom,Dem3DofGeom,"Abstract class for providing torsion and bending, in addition to inherited normal and shear strains.", /* no attrs */);
};
REGISTER_SERIALIZABLE(Dem6DofGeom);
#endif

