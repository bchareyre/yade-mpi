/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <list>
#include <vector>
#include "Interaction.hpp"
#include "GlobalEngine.hpp"

class Body;

class TimeStepper : public GlobalEngine
{
	public :
		bool active;
		unsigned int timeStepUpdateInterval;

		TimeStepper();
		virtual void computeTimeStep(Scene* ) { throw; };
		virtual bool isActivated(Scene*);
		virtual void action(Scene* b) { computeTimeStep(b);} ;
		void setActive(bool a, int nb=-1);

	REGISTER_ATTRIBUTES(GlobalEngine,(active)(timeStepUpdateInterval));
	REGISTER_CLASS_AND_BASE(TimeStepper,GlobalEngine);
};

REGISTER_SERIALIZABLE(TimeStepper);


