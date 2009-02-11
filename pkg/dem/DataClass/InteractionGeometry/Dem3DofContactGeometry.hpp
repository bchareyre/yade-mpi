// 2009 © Václav Šmilauer <eudoxos@arcig.cz>

/*! Abstract class for geometry providing normal and shear strains (or displacements) */
class Dem3DofContactGeometry: public InteractionGeometry {
	public:
		//! reference length of the contact (usually initial length)
		Real refLength;
		//! unit vector in the interaction direction (normal to the contact plane), always from id1 towards id2 
		Vector3r normal;
		//! contact point
		Vector3r contactPoint;
		//! Absolute displacement in the normal direction
		virtual Real displacementN()=0;
		//! Strain in the normal direction
		virtual Real epsN(){ return displacementN()/refLength; }
		//! Absolute displacement in the shear direction (in global coordinates)
		virtual Vector3r displacementT()=0;
		//! Strain in the shear direction (in global coordinates)
		virtual Vector3r epsT(){ return displacementT/refLength; }
	REGISTER_CLASS_AND_BASE(Dem3DofContactGeometry,InteractionGeometry);
	REGISTER_ATTRIBUTES(InteractionGeometry,(refLength)(normal)(contactPoint));
};
REGISTER_SERIALIZABLE(Dem3DofContactGeometry);

#if 0
/*! Abstract class for providing torsion and bending, in addition to inherited normal and shear strains. */
class Dem6DofContactGeometry: public Dem3DofContactGeometry {
	public:
		//! Absolute value of rotation along the interaction normal
		virtual Real rotationN()=0;
		//! Relative rotation along the contact normal
		virtual Real torsion(){ return rotationN()/refLength; }
		//! Absolute value of bending (in global coordinates)
		virtual Vector3r rotationT()=0;
		//! Relative rotation perpendicular to contact normal (in global coordinates)
		virtual Vector3r bending(){ return rotationT()/refLength; }
	REGISTER_CLASS_AND_BASE(Dem6DofContactGeometry,Dem3DofContactGeometry);
	REGISTER_ATTRIBUTES(Dem3DofContactGeometry,);
};
REGISTER_SERIALIZABLE(Dem6DofContactGeometry);
#endif 
