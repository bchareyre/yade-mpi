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
	if (Omega::instance().getRootBody()) // FIXME - would it contain the loop in the private variables, this check would be unnecessary
	{
		Omega::instance().getRootBody()->moveToNextTimeStep();
		Omega::instance().incrementCurrentIteration();
		Omega::instance().incrementSimulationTime();
		if(Omega::instance().stopAtIteration>0 && Omega::instance().getCurrentIteration()==Omega::instance().stopAtIteration){
			cerr<<"PAUSE at iteration #"<<Omega::instance().getCurrentIteration()<<" as requested."<<endl;
			setTerminate(true);
			return;
		}
	}
};

