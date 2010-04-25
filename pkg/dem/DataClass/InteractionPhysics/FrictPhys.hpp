// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
#pragma once

#include<yade/pkg-common/NormShearPhys.hpp>

class FrictPhys: public NormShearPhys
{
	public :
		// kn,ks,normal inherited from NormShearPhys
		Real initialKn			// initial normal elastic constant.
				,initialKs			// initial shear elastic constant.
				//Those two attributes were removed. If you need them, copy those two lines in your class inheriting from FrictPhys
// 				,equilibriumDistance		// equilibrium distance
// 				,initialEquilibriumDistance	// initial equilibrium distance
				,frictionAngle 			// angle of friction, according to Coulumb criterion
				;	
		virtual ~FrictPhys();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(FrictPhys,NormShearPhys,"Interaction with friction",
		((Vector3r,prevNormal,Vector3r::Zero(),"unit normal of the contact plane in previous step"))
		((Real,tangensOfFrictionAngle,NaN,"tan of angle of friction")),
		createIndex()
	);
	REGISTER_CLASS_INDEX(FrictPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(FrictPhys);

