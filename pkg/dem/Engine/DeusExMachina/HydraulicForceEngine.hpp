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
class Momentum;

class HydraulicForceEngine : public DeusExMachina 
{
	private	:
		shared_ptr<Force> actionParameterForce;
		shared_ptr<Momentum> actionParameterMomentum;

	public :
		Vector3r gravity;
		bool isActivated;
		bool dummyParameter;
		HydraulicForceEngine();
		virtual ~HydraulicForceEngine();
	
		virtual void applyCondition(MetaBody*);
	
	protected :
		virtual void registerAttributes();
	NEEDS_BEX("Force","Momentum");
	REGISTER_CLASS_NAME(HydraulicForceEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(HydraulicForceEngine,false);

#endif // GRAVITY_FORCE_FUNCTOR_HPP 

