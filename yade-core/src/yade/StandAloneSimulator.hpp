/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef STANDALONESIMULATOR_HPP
#define STANDALONESIMULATOR_HPP

#include <yade/yade-lib-serialization/Serializable.hpp>
#include <yade/yade-lib-wm3-math/Math.hpp>

class StandAloneSimulator : public Serializable
{
	protected :
		bool	 record;

		int	 interval
			,paddle;

		string	 outputDirectory
			,outputBaseName;

	public : 
		StandAloneSimulator();
		virtual ~StandAloneSimulator() {};

		virtual void setTimeStep(Real dt) {};
		virtual void doOneIteration() {};
		virtual void run(int nbIterations) {};
		virtual void loadConfigurationFile(const string& fileName) {};

		void setRecording(bool record);
		void setRecordingProperties(int interval, const string& outputDirectory,const string& outputBaseName,int paddle);

	REGISTER_CLASS_NAME(StandAloneSimulator);
	REGISTER_BASE_CLASS_NAME(Serializable);
};

REGISTER_SERIALIZABLE(StandAloneSimulator,false);

#endif // STANDALONESIMULATOR_HPP

