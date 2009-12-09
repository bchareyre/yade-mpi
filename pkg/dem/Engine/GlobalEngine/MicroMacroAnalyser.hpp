/*************************************************************************
*  Copyright (C) 2008 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg-dem/TriaxialCompressionEngine.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <fstream>

/*! \brief Compute quantities like fabric tensor, local porosity, local deformation, and other micromechanicaly defined quantities based on triangulation/tesselation of the packing
		
	This class is using a separate library built from lib/triangulation sources		
 */

class KinematicLocalisationAnalyser;

class MicroMacroAnalyser : public GlobalEngine
{
/// Attributes
	private :
		std::ofstream ofile;
		
		shared_ptr<TriaxialCompressionEngine> triaxialCompressionEngine;
		shared_ptr<KinematicLocalisationAnalyser> analyser;
		std::string	 outputFile;
		std::string	 stateFileName;
		
	public :
		MicroMacroAnalyser();
		void action(Scene*);
		void setState(Scene* ncb, unsigned int state, bool saveStates = false, bool computeIncrement = false);//Set current state as initial (state=1) or final (state=2) congiguration for later kinematic analysis on the increment; if requested : save snapshots (with specific format) - possibly including contact forces increments on the state1->state2 interval
		int interval;
		
	DECLARE_LOGGER;
	
	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(GlobalEngine,(interval)(outputFile));
	REGISTER_CLASS_NAME(MicroMacroAnalyser);
	REGISTER_BASE_CLASS_NAME(GlobalEngine);
};

REGISTER_SERIALIZABLE(MicroMacroAnalyser);


