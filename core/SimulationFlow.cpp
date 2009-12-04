/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimulationFlow.hpp"
#include "Scene.hpp"
#include "Omega.hpp"

void SimulationFlow::singleAction()
{
	Omega& OO=Omega::instance();
	if (OO.getScene()) // FIXME - would it contain the loop in the private variables, this check would be unnecessary
	{
		OO.getScene()->moveToNextTimeStep();
		if(OO.getScene()->stopAtIteration>0 && OO.getCurrentIteration()==OO.getScene()->stopAtIteration){
			setTerminate(true);
		}
	}
};

