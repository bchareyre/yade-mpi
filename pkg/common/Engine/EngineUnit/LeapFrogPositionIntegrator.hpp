/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/PhysicalParametersEngineUnit.hpp>

class LeapFrogPositionIntegrator : public PhysicalParametersEngineUnit
{
	private :
//		vector<Vector3r> prevVelocities;
//		vector<bool> firsts;
	
	public :
		virtual void go(	  const shared_ptr<PhysicalParameters>&
					, Body*, BexContainer&);

	FUNCTOR1D(ParticleParameters);	
	REGISTER_CLASS_NAME(LeapFrogPositionIntegrator);
	REGISTER_BASE_CLASS_NAME(PhysicalParametersEngineUnit);
};

REGISTER_SERIALIZABLE(LeapFrogPositionIntegrator);



