/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimulationFlow.hpp"
#include "MetaBody.hpp"
#include "Omega.hpp"

void SimulationFlow::singleAction()
{
	Omega& OO=Omega::instance();
	if (OO.getRootBody()) // FIXME - would it contain the loop in the private variables, this check would be unnecessary
	{
		OO.getRootBody()->moveToNextTimeStep();
		OO.incrementCurrentIteration();
		OO.incrementSimulationTime();
		if(OO.getRootBody()->stopAtIteration>0 && OO.getCurrentIteration()==OO.getRootBody()->stopAtIteration){
			setTerminate(true);
		}
	}
};

