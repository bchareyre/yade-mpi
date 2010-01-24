// © 2007 Janek Kozicki <cosurgi@mail.berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/core/InteractionPhysics.hpp>

class NormPhys:public InteractionPhysics {
	public:
		Real kn;
		Vector3r normalForce;
		NormPhys(): normalForce(Vector3r::ZERO) {createIndex(); }
		virtual ~NormPhys();
	YADE_CLASS_BASE_DOC_ATTRS(NormPhys,InteractionPhysics,"Abstract class for interactions that have normal stiffness.",
		((kn,"Normal stiffness"))
		((normalForce,"Normal force after previous step."))
	);
	REGISTER_CLASS_INDEX(NormPhys,InteractionPhysics);
};
REGISTER_SERIALIZABLE(NormPhys);

class NormShearPhys: public NormPhys{
	public:
		Real ks;
		Vector3r shearForce;
		NormShearPhys(): shearForce(Vector3r::ZERO){ createIndex(); }
		virtual ~NormShearPhys();
	YADE_CLASS_BASE_DOC_ATTRS(NormShearPhys,NormPhys,"Abstract class for interactions that have shear stiffnesses, in addition to normal stiffness. This class is used in the PFC3d-style stiffness timestepper.",
		((ks,"Shear stiffness"))
		((shearForce,"Shear force after previous step"))
	);
	REGISTER_CLASS_INDEX(NormShearPhys,NormPhys);
};
REGISTER_SERIALIZABLE(NormShearPhys);
