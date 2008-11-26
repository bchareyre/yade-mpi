/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/




#ifndef RESULTANT_FORCE_ENGINE_HPP
#define RESULTANT_FORCE_ENGINE_HPP

#include<yade/core/DeusExMachina.hpp>
#include <Wm3Math.h>
#include<yade/lib-base/yadeWm3.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class PhysicalAction;

class ResultantForceEngine : public DeusExMachina 
{
	private :
		shared_ptr<PhysicalAction> actionParameterGlobalStiffness;
		shared_ptr<PhysicalAction> actionParameterForce;
		
			
	public :
		//! CAUTION : interval must be equal to the interval of the StiffnessCounter, it is used here to check if stiffness has been computed 
		unsigned int interval;
		//! Defines the prescibed resultant force 
		Vector3r		force;	
		//! Stores the value of the translation at the previous time step
		Vector3r 		previoustranslation;
		//! The value of stiffness (updated according to interval) 
		Vector3r		stiffness;
		//! damping coefficient - damping=1 implies a "perfect" control of the resultant force
		Real			damping;
		//! maximum velocity (usefull to prevent explosions when stiffness is very low...) 
		Real			max_vel;

		ResultantForceEngine();
		virtual ~ResultantForceEngine();
	
		virtual void applyCondition(MetaBody*);
		
	
	protected :
		virtual void registerAttributes();
	NEEDS_BEX("Force","GlobalStiffness");
	REGISTER_CLASS_NAME(ResultantForceEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(ResultantForceEngine);

#endif // FORCE_ENGINE_HPP

