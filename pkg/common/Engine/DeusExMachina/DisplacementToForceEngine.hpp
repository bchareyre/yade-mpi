/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef DISPLACEMENT_FORCE_ENGINE_HPP
#define DISPLACEMENT_FORCE_ENGINE_HPP

#include<yade/core/DeusExMachina.hpp>
#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class Force;

class DisplacementToForceEngine : public DeusExMachina
{
	private :
		shared_ptr<Force>	actionParameterForce;
		Real			target_length_sq,direction,old_direction,oscillations;
	public :
		Vector3r		targetForce,targetForceMask;
		Real			displacement;
		Vector3r		translationAxis;

		DisplacementToForceEngine();
		virtual ~DisplacementToForceEngine();

		virtual void applyCondition(Body * body);

	protected :
		virtual void postProcessAttributes(bool);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(DisplacementToForceEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(DisplacementToForceEngine,false);

#endif //  DISPLACEMENTENGINE_HPP

