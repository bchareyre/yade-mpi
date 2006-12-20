/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMULATIONCONTROLLERUPDATER_HPP
#define SIMULATIONCONTROLLERUPDATER_HPP

#include<boost/date_time/posix_time/posix_time.hpp>
#include<yade/yade-lib-base/Logging.hpp>


class SimulationController;

class SimulationControllerUpdater
{	
	private :
		SimulationController * controller;
		const int iterPerSec_TTL_ms;
		long  iterPerSec_LastIter;
		boost::posix_time::ptime iterPerSec_LastLocalTime;

	public :
		SimulationControllerUpdater(SimulationController * sc);
		virtual ~SimulationControllerUpdater();
		void oneLoop();
		DECLARE_LOGGER;
};

#endif // SIMULATIONCONTROLLERUPDATER_HPP

