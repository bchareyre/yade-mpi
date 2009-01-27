/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/PhysicalParametersEngineUnit.hpp>

class LeapFrogOrientationIntegrator : public PhysicalParametersEngineUnit 
{
	private :
// 		vector<Vector3r> prevAngularVelocities;
// 		vector<bool> firsts;
	
	public :
		virtual void go( 	  const shared_ptr<PhysicalParameters>&
					, Body*);

	FUNCTOR1D(RigidBodyParameters);	
	REGISTER_CLASS_NAME(LeapFrogOrientationIntegrator);
	REGISTER_BASE_CLASS_NAME(PhysicalParametersEngineUnit);
};

REGISTER_SERIALIZABLE(LeapFrogOrientationIntegrator);


