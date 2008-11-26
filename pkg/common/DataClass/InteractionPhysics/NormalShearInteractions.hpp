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
		NormalInteraction(){createIndex(); }
		virtual ~NormalInteraction();
	protected:
		virtual void registerAttributes(){
			REGISTER_ATTRIBUTE(kn);
			REGISTER_ATTRIBUTE(normalForce);
		}
	REGISTER_CLASS_NAME(NormalInteraction);
	REGISTER_BASE_CLASS_NAME(InteractionPhysics);
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
		NormalShearInteraction(){ createIndex(); }
		virtual ~NormalShearInteraction();
	protected:
		virtual void registerAttributes(){	
			NormalInteraction::registerAttributes();
			REGISTER_ATTRIBUTE(ks);
			REGISTER_ATTRIBUTE(shearForce);
		}
	REGISTER_CLASS_NAME(NormalShearInteraction);
	REGISTER_BASE_CLASS_NAME(NormalInteraction);
	REGISTER_CLASS_INDEX(NormalShearInteraction,NormalInteraction);
};
REGISTER_SERIALIZABLE(NormalShearInteraction);
