/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimulationLoop.hpp"
#include "MetaBody.hpp"
#include "Omega.hpp"
#include <yade/yade-lib-threads/ThreadSynchronizer.hpp>

SimulationLoop::SimulationLoop() : Threadable<SimulationLoop>(Omega::instance().getSynchronizer())
{
	createThread();
}


SimulationLoop::~SimulationLoop()
{

}


bool SimulationLoop::notEnd()
{
	return true;
}


void SimulationLoop::oneLoop()
{
	LOCK(Omega::instance().getRootBodyMutex());
		
	if (Omega::instance().getRootBody())
	{
		Omega::instance().getRootBody()->moveToNextTimeStep();
		Omega::instance().incrementCurrentIteration();
		Omega::instance().incrementSimulationTime();
	}
}

