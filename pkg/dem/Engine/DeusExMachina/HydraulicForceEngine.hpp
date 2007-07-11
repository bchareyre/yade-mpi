/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef HYDRAULIC_FORCE_FUNCTOR_HPP
#define HYDRAULIC_FORCE_FUNCTOR_HPP 

#include<yade/core/DeusExMachina.hpp>

class Force;

class HydraulicForceEngine : public DeusExMachina 
{
	private	:
		shared_ptr<Force> actionParameterForce;

	public :
		Vector3r gravity;
		bool isActivated;
		HydraulicForceEngine();
		virtual ~HydraulicForceEngine();
	
		virtual void applyCondition(Body*);
	
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(HydraulicForceEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(HydraulicForceEngine,false);

#endif // GRAVITY_FORCE_FUNCTOR_HPP 

