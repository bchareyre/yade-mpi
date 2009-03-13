// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/InteractionGeometry.hpp>

/*! Abstract base class for representing contact geometry of 2 elements that has 3 degrees of freedom:
 *  normal (1 component) and shear (Vector3r, but in plane perpendicular to the normal)
 */
class Dem3DofGeom: public InteractionGeometry {
	public:
		//! some length used to convert displacements to strains
		Real refLength;
		//! some unit reference vector (in the direction of the interaction)
		Vector3r normal;
		//! some reference point for the interaction
		Vector3r contactPoint;

		// API that needs to be implemented in derived classes
		virtual Real displacementN()=0;
		virtual Vector3r displacementT()=0;
		virtual Real slipToDisplacementTMax(Real displacementTMax)=0; // plasticity
		// end API

		Real strainN(){return displacementN()/refLength;}
		Vector3r strainT(){return displacementT()/refLength;}
		Real slipToStrainTMax(Real strainTMax){return slipToDisplacementTMax(strainTMax*refLength)/refLength;}

		REGISTER_CLASS_AND_BASE(Dem3DofGeom,InteractionGeometry);
		REGISTER_ATTRIBUTES(InteractionGeometry,(refLength)(normal)(contactPoint));
};
//REGISTER_SERIALIZABLE(Dem3DofGeom);

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

