/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CundallNonViscousForceDamping.hpp"
#include "ParticleParameters.hpp"
#include "Force.hpp"


CundallNonViscousForceDamping::CundallNonViscousForceDamping() : damping(0)
{
}


void CundallNonViscousForceDamping::registerAttributes()
{
	PhysicalActionDamperUnit::registerAttributes();
	REGISTER_ATTRIBUTE(damping);
}


// this is Cundall non-viscous local damping, applied to force (Force)

void CundallNonViscousForceDamping::go(    const shared_ptr<PhysicalAction>& a
						, const shared_ptr<PhysicalParameters>& b
						, const Body* body)
{
	if(body->isClump()) return;
	Force * af = static_cast<Force*>(a.get());
	ParticleParameters * p = static_cast<ParticleParameters*>(b.get());
		
	
	for (int i=0; i<3; ++i)
	{
		af->force[i] *= 1 - damping*Mathr::Sign(af->force[i]*p->velocity[i]);	
	}
	
}


YADE_PLUGIN();
