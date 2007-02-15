/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CundallNonViscousMomentumDamping.hpp"
#include "RigidBodyParameters.hpp"
#include "Momentum.hpp"


CundallNonViscousMomentumDamping::CundallNonViscousMomentumDamping() : damping(0)
{
}


void CundallNonViscousMomentumDamping::registerAttributes()
{
	PhysicalActionDamperUnit::registerAttributes();
	REGISTER_ATTRIBUTE(damping);
}


// this is Cundall non-viscous local damping, applied to momentum (Momentum)

void CundallNonViscousMomentumDamping::go( 	  const shared_ptr<PhysicalAction>& a
						, const shared_ptr<PhysicalParameters>& b
						, const Body*)
{

	Momentum * am = static_cast<Momentum*>(a.get());
	RigidBodyParameters * rb = static_cast<RigidBodyParameters*>(b.get());
	
	Vector3r& m  = am->momentum;
	
	
	for (int i=0; i<3; ++i)
	{
		m[i] *= 1 - damping*Mathr::Sign(m[i]*rb->angularVelocity[i]);	
	}
	
		

}


