// © 2007 Janek Kozicki <cosurgi@mail.berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/core/InteractionPhysics.hpp>

/* Abstract class for interactions that have normal stiffness. */
class NormalInteraction:public InteractionPhysics {
	public:
		//! normal stiffness
		Real kn;
		//! normal force
		Vector3r normalForce;
		NormalInteraction(): normalForce(Vector3r::ZERO) {createIndex(); }
		virtual ~NormalInteraction();
	REGISTER_ATTRIBUTES(/*no base class attributes*/,(kn)(normalForce));
	REGISTER_CLASS_AND_BASE(NormalInteraction,InteractionPhysics);
	REGISTER_CLASS_INDEX(NormalInteraction,InteractionPhysics);
};
REGISTER_SERIALIZABLE(NormalInteraction);

/* Abstract class for interactions that have shear stiffnesses, in addition to normal stiffness.
 *
 * This class is used in the PFC3d-style timestepper. */
class NormalShearInteraction: public NormalInteraction{
	public:
		//! shear stiffness
		Real ks;
		//! shear force
		Vector3r shearForce;
		NormalShearInteraction(): shearForce(Vector3r::ZERO){ createIndex(); }
		virtual ~NormalShearInteraction();
	REGISTER_ATTRIBUTES(NormalInteraction,(ks)(shearForce));
	REGISTER_CLASS_AND_BASE(NormalShearInteraction,NormalInteraction);
	REGISTER_CLASS_INDEX(NormalShearInteraction,NormalInteraction);
};
REGISTER_SERIALIZABLE(NormalShearInteraction);
