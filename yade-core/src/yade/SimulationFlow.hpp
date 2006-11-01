/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMULATION_FLOW_HPP
#define SIMULATION_FLOW_HPP

#include "ThreadWorker.hpp"

class SimulationFlow // FIXME ; bad name
	: public ThreadWorker
{
	public:
		virtual void	singleAction();
};

#endif


