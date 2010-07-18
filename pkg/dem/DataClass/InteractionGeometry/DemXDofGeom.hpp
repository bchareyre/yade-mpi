// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/InteractionGeometry.hpp>

/*! Abstract class that unites ScGeom and Dem3DofGeom,
	created for the purposes of GlobalStiffnessTimeStepper.
	It might be removed in the future. */
class GenericSpheresContact: public InteractionGeometry{
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(GenericSpheresContact,InteractionGeometry,
		"Class uniting :yref:`ScGeom` and :yref:`Dem3DofGeom`, for the purposes of :yref:`GlobalStiffnessTimeStepper`. (It might be removed inthe future). Do not use this class directly.",
		((Vector3r,normal,,"Unit vector oriented along the interaction. |yupdate|"))
		((Real,refR1,,"Reference radius of particle #1. |ycomp|"))
		((Real,refR2,,"Reference radius of particle #2. |ycomp|")),
		createIndex();
	);
	REGISTER_CLASS_INDEX(GenericSpheresContact,InteractionGeometry);

	virtual ~GenericSpheresContact(); // vtable
};
REGISTER_SERIALIZABLE(GenericSpheresContact);

/*! Abstract base class for representing contact geometry of 2 elements that has 3 degrees of freedom:
 *  normal (1 component) and shear (Vector3r, but in plane perpendicular to the normal)
 */
class Dem3DofGeom: public GenericSpheresContact{
	public:
		virtual ~Dem3DofGeom();

		// API that needs to be implemented in derived classes
		virtual Real displacementN();
		virtual Vector3r displacementT(){throw;}
		virtual Real slipToDisplacementTMax(Real displacementTMax){throw;}; // plasticity
		virtual Vector3r scaleDisplacementT(Real multiplier){throw;}; // plasticity (variant using multiplier dispMax/disp)
		// reference radii, for contact stiffness computation; set to negative for nonsense values
		// end API

		Real strainN(){
			//if(!logCompression)
			return displacementN()/refLength;
			//else{Real dn=displacementN(); }
		}
		Vector3r strainT(){return displacementT()/refLength;}
		Real slipToStrainTMax(Real strainTMax){return slipToDisplacementTMax(strainTMax*refLength)/refLength;}

		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Dem3DofGeom,GenericSpheresContact,
			"Abstract base class for representing contact geometry of 2 elements that has 3 degrees of freedom: normal (1 component) and shear (Vector3r, but in plane perpendicular to the normal).",
			((Real,refLength,,"some length used to convert displacements to strains. |ycomp|"))
			((Vector3r,contactPoint,,"some reference point for the interaction (usually in the middle). |ycomp|"))
			((bool,logCompression,false,"make strain go to -∞ for length going to zero (false by default)."))
			((Se3r,se31,,"Copy of body #1 se3 (needed to compute torque from the contact, strains etc). |yupdate|"))
			((Se3r,se32,,"Copy of body #2 se3. |yupdate|")),
			createIndex();,
			.def("displacementN",&Dem3DofGeom::displacementN)
			.def("displacementT",&Dem3DofGeom::displacementT)
			.def("strainN",&Dem3DofGeom::strainN)
			.def("strainT",&Dem3DofGeom::strainT)
			.def("slipToDisplacementTMax",&Dem3DofGeom::slipToDisplacementTMax)
			.def("slipToStrainTMax",&Dem3DofGeom::slipToStrainTMax)
		);
		REGISTER_CLASS_INDEX(Dem3DofGeom,InteractionGeometry);
};
REGISTER_SERIALIZABLE(Dem3DofGeom);


