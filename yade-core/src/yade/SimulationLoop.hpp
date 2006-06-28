/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMULATIONLOOP_HPP
#define SIMULATIONLOOP_HPP

#include <yade/yade-lib-threads/Threadable.hpp>

class SimulationLoop// : public Threadable<SimulationLoop>
{
	public :
		SimulationLoop();
		~SimulationLoop();
	
	//	bool notEnd();
		void doOneLoop();
		void start();
		void stop();
		bool isStopped();
};

#endif // SIMULATIONLOOP_HPP
