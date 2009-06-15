/*************************************************************************
 Copyright (C) 2008 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr      					 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include<yade/core/DeusExMachina.hpp>
#include<Wm3Vector3.h>

/*! An engine that can replace the usual series of engines used for integrating the laws of motion.

This engine is faster because it uses less loops and less dispatching 

The result is almost the same as with :
-NewtonsForceLaw
-NewtonsMomentumLaw
-LeapFrogPositionIntegrator
-LeapFrogOrientationIntegrator
-CundallNonViscousForceDamping
-CundallNonViscousMomentumDamping

...but the implementation of damping is slightly different compared to CundallNonViscousForceDamping+CundallNonViscousMomentumDamping. Here, damping is dependent on predicted (undamped) velocity at t+dt/2, while the other engines use velocity at time t.
 
Requirements :
-All dynamic bodies must have physical parameters of type (or inheriting from) BodyMacroParameters
-Physical actions must include forces and moments
 
NOTE: Cundall damping affected dynamic simulation! See examples/dynamic_simulation_tests
 
 */


class NewtonsDampedLaw : public DeusExMachina
{
	inline void cundallDamp(const Real& dt, const Vector3r& f, const Vector3r& velocity, Vector3r& acceleration, const Vector3r& m, const Vector3r& angularVelocity, Vector3r& angularAcceleration);
	public :
		///damping coefficient for Cundall's non viscous damping
		Real damping;
	public :
		virtual	void applyCondition(MetaBody *);		
		NewtonsDampedLaw();
	
	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(NewtonsDampedLaw);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(NewtonsDampedLaw);

