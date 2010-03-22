/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/PeriodicEngines.hpp>
#include<string>
#include<fstream>

class PositionOrientationRecorder: public PeriodicEngine{
	public :
	virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(PositionOrientationRecorder,PeriodicEngine,"Engine recording positions and orientation to bz2-compressed plain text files for later use by external programs. Each line contains 7 components of position (3) and orientation (4), one body per line.\n\n.. warning::\n\tThis class is deprecated. It used to be used with Simulation Player (https://yade-dem.org/index.php/New:Making_videos), which doesn't exist anymore. Use :yref:`VTKRecorder` instead.\n\n.. note::\n\t:yref:`iterPeriod<PeriodicEngine::iterPeriod>` is initialized to 50.\n",
		((std::string,outputFile,"positionorientation","Basename for output, will iteratrion number appended: ``/tmp/xyz`` will produce ``/tmp/xyz_000010.bz2``, ``/tmp/xyz_000020.bz2`` and so on (with interPeriod==10)."))
		((bool,saveRgb,false,"Additionally save files with colors of bodies, creating files like ``/tmp/xyz_000010.rgz.bz2`` etc.")),
		iterPeriod=50
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(PositionOrientationRecorder);


