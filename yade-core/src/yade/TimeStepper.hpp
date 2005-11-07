/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TIMESTEPPER_HPP
#define TIMESTEPPER_HPP

#include <list>
#include <vector>
#include "Interaction.hpp"
#include "Engine.hpp"

class Body;

class TimeStepper : public Engine
{
	public :
		bool active;
		unsigned int timeStepUpdateInterval;

		TimeStepper();
		virtual void computeTimeStep(Body*  ) { throw; };
		virtual bool isActivated();
		virtual void action(Body* b) { computeTimeStep(b);} ;
		void setActive(bool a, int nb=-1);

	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(TimeStepper);
	REGISTER_BASE_CLASS_NAME(Engine);
};

REGISTER_SERIALIZABLE(TimeStepper,false);

#endif // TIMESTEPPER_HPP

