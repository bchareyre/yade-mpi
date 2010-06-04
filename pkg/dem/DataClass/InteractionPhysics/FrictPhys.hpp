/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include<yade/pkg-common/NormShearPhys.hpp>

class FrictPhys: public NormShearPhys
{
	public :
		// kn,ks,normal inherited from NormShearPhys
		Real frictionAngle 			// angle of friction, according to Coulumb criterion
				//Those two attributes were removed. If you need them, copy those two lines in your class inheriting from FrictPhys
// 				,equilibriumDistance		// equilibrium distance
// 				,initialEquilibriumDistance	// initial equilibrium distance
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

