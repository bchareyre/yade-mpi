// © 2007 Janek Kozicki <cosurgi@mail.berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/core/InteractionPhysics.hpp>

class NormPhys:public InteractionPhysics {
	public:
		virtual ~NormPhys();
	YADE_CLASS_BASE_DOC_ATTRDECL_CTOR_PY(NormPhys,InteractionPhysics,"Abstract class for interactions that have normal stiffness.",
		((Real,kn,NaN,"Normal stiffness"))
		((Vector3r,normalForce,Vector3r::ZERO,"Normal force after previous step.")),
		createIndex(),
	);
	REGISTER_CLASS_INDEX(NormPhys,InteractionPhysics);
};
REGISTER_SERIALIZABLE(NormPhys);

class NormShearPhys: public NormPhys{
	public:
		virtual ~NormShearPhys();
	YADE_CLASS_BASE_DOC_ATTRDECL_CTOR_PY(NormShearPhys,NormPhys,
		"Abstract class for interactions that have shear stiffnesses, in addition to normal stiffness. This class is used in the PFC3d-style stiffness timestepper.",
		((Real,ks,NaN,"Shear stiffness"))
		((Vector3r,shearForce,Vector3r::ZERO,"Shear force after previous step")),
		createIndex(),
	);
	REGISTER_CLASS_INDEX(NormShearPhys,NormPhys);
};
REGISTER_SERIALIZABLE(NormShearPhys);
