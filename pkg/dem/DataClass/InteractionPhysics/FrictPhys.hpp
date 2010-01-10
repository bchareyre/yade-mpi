// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
#pragma once

#include<yade/pkg-common/NormShearPhys.hpp>

class FrictPhys: public NormShearPhys
{
	public :
		// kn,ks,normal inherited from NormShearPhys
		Real initialKn			// initial normal elastic constant.
				,initialKs			// initial shear elastic constant.
				,equilibriumDistance		// equilibrium distance
				,initialEquilibriumDistance	// initial equilibrium distance
				,frictionAngle 			// angle of friction, according to Coulumb criterion
				,tangensOfFrictionAngle
				;	
		Vector3r	prevNormal;			// unit normal of the contact plane.

		FrictPhys(){ createIndex(); }
		virtual ~FrictPhys();
	REGISTER_ATTRIBUTES(NormShearPhys,(prevNormal)/*(initialKn)(initialKs)*/(tangensOfFrictionAngle));
	REGISTER_CLASS_NAME(FrictPhys);
	REGISTER_BASE_CLASS_NAME(NormShearPhys);
	REGISTER_CLASS_INDEX(FrictPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(FrictPhys);

