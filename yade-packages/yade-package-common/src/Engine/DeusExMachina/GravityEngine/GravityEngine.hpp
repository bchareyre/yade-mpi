/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GRAVITY_FORCE_FUNCTOR_HPP
#define GRAVITY_FORCE_FUNCTOR_HPP 

#include <yade/yade-core/DeusExMachina.hpp>

class Force;

class GravityEngine : public DeusExMachina 
{
	private	:
		shared_ptr<Force> actionParameterForce;

	public :
		Vector3r gravity;
		GravityEngine();
		virtual ~GravityEngine();
	
		virtual void applyCondition(Body*);
	
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(GravityEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(GravityEngine,false);

#endif // GRAVITY_FORCE_FUNCTOR_HPP 

