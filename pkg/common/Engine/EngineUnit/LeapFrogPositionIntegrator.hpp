/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/StateEngineUnit.hpp>

class LeapFrogPositionIntegrator : public StateEngineUnit
{
	private :
//		vector<Vector3r> prevVelocities;
//		vector<bool> firsts;
	
	public :
		virtual void go(	  const shared_ptr<State>&
					, Body*, BexContainer&);

	FUNCTOR1D(ParticleParameters);	
	REGISTER_CLASS_NAME(LeapFrogPositionIntegrator);
	REGISTER_BASE_CLASS_NAME(StateEngineUnit);
};

REGISTER_SERIALIZABLE(LeapFrogPositionIntegrator);



