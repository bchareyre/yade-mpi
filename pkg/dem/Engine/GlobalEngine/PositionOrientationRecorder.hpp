/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/PeriodicEngines.hpp>
#include <string>
#include <fstream>
/*!
 * Engine recording positions and orientation to plain ASCII files, primarily intended for use with the Simulation Player.
 *
 * See http://yade.wikia.com/wiki/New:Making_videos for details .
 *
 */
class PositionOrientationRecorder : public PeriodicEngine
{
	public :
		//! basename of the output files; for example, /tmp/xyz will produce /tmp/xyz_000010, /tmp/xyz_000020 and so on (with interval==10)
		std::string outputFile;
		bool saveRgb;
	
		PositionOrientationRecorder ();
		virtual void action(Scene*);

	REGISTER_ATTRIBUTES(PeriodicEngine,(outputFile)(saveRgb));
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(PositionOrientationRecorder,PeriodicEngine);
};

REGISTER_SERIALIZABLE(PositionOrientationRecorder);


