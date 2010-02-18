/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/core/TimeStepper.hpp>
#include<yade/core/GlobalEngine.hpp>
#include<yade/core/Scene.hpp>

// TimeStepper::TimeStepper() : GlobalEngine()
// {	
// 	active = true;
// 	timeStepUpdateInterval = 1;
// }


bool TimeStepper::isActivated(Scene* mb)
{
	return (active && (mb->currentIteration % timeStepUpdateInterval == 0));
}



void TimeStepper::setActive(bool a, int nb)
{
	active = a; 
	if (nb>0)
		timeStepUpdateInterval = (unsigned int)nb;
}


