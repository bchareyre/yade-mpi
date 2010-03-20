/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/pkg-common/Recorder.hpp>

#include <string>
#include <fstream>


class CapillaryRecorder : public Recorder
{
	private :
		std::ofstream ofile; 

		bool changed;
	
	public :
		std::string	 outputFile;
		unsigned int	 interval;

		int		bigBallId;

		CapillaryRecorder ();

		virtual void action();
		virtual bool isActivated();
	REGISTER_ATTRIBUTES(Recorder,(outputFile)(interval)(bigBallId));
	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(CapillaryRecorder);
	REGISTER_BASE_CLASS_NAME(Recorder);
};

REGISTER_SERIALIZABLE(CapillaryRecorder);


