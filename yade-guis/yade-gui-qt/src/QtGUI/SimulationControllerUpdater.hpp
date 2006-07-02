/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMULATIONCONTROLLERUPDATER_HPP
#define SIMULATIONCONTROLLERUPDATER_HPP


class SimulationController;

class SimulationControllerUpdater
{	
	private :
		SimulationController * controller;

	public :
		SimulationControllerUpdater(SimulationController * sc);
		virtual ~SimulationControllerUpdater();
		void oneLoop();
		bool notEnd();
};

#endif // SIMULATIONCONTROLLERUPDATER_HPP

