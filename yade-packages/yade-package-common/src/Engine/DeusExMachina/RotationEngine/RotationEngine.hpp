/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ROTATIONENGINE_HPP
#define ROTATIONENGINE_HPP

#include <yade/yade-core/DeusExMachina.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>

class RotationEngine : public DeusExMachina
{
	public :
		RotationEngine();

		Real angularVelocity;
		Vector3r rotationAxis;
		bool rotateAroundZero;

		void applyCondition(Body * body);

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(RotationEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(RotationEngine,false);

#endif // ROTATIONENGINE_HPP

