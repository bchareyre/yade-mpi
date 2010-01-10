// © 2007 Janek Kozicki <cosurgi@mail.berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/core/InteractionPhysics.hpp>

/* Abstract class for interactions that have normal stiffness. */
class NormPhys:public InteractionPhysics {
	public:
		//! normal stiffness
		Real kn;
		//! normal force
		Vector3r normalForce;
		NormPhys(): normalForce(Vector3r::ZERO) {createIndex(); }
		virtual ~NormPhys();
	REGISTER_ATTRIBUTES(Serializable,(kn)(normalForce));
	REGISTER_CLASS_AND_BASE(NormPhys,InteractionPhysics);
	REGISTER_CLASS_INDEX(NormPhys,InteractionPhysics);
};
REGISTER_SERIALIZABLE(NormPhys);

/* Abstract class for interactions that have shear stiffnesses, in addition to normal stiffness.
 *
 * This class is used in the PFC3d-style timestepper. */
class NormShearPhys: public NormPhys{
	public:
		//! shear stiffness
		Real ks;
		//! shear force
		Vector3r shearForce;
		NormShearPhys(): shearForce(Vector3r::ZERO){ createIndex(); }
		virtual ~NormShearPhys();
	REGISTER_ATTRIBUTES(NormPhys,(ks)(shearForce));
	REGISTER_CLASS_AND_BASE(NormShearPhys,NormPhys);
	REGISTER_CLASS_INDEX(NormShearPhys,NormPhys);
};
REGISTER_SERIALIZABLE(NormShearPhys);
