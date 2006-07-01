/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMULATION_LOOP_HPP
#define SIMULATION_LOOP_HPP

/*! 
\brief	SimulationFlow contains information about tasks to be performed when
	the simulation is ran. It counts time of the simulation ran.

	FIXME - it is currently calling rootBody->actors 

 */

class SimulationFlow  // perhaps simulation steps, or stage? as it is a single stage of the simulation, that consists of several steps
{
	public :
		void singleLoop();
};

#endif

