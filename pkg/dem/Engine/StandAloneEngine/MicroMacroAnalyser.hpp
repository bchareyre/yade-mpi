/*************************************************************************
*  Copyright (C) 2008 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef MACRO_MICRO_ANALYSER_HPP
#define MACRO_MICRO_ANALYSER_HPP

#include<yade/core/StandAloneEngine.hpp>
#include<yade/pkg-dem/TriaxialCompressionEngine.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <fstream>

/*! \brief Compute quantities like fabric tensor, local porosity, local deformation, and other micromechanicaly defined quantities based on triangulation/tesselation of the packing
		
	This class is using a separate library built from extra/triangulation sources		
 */

class PhysicalAction;
class KinematicLocalisationAnalyser;

class MicroMacroAnalyser : public StandAloneEngine
{
/// Attributes
	private :
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;
		int actionForceIndex;
		int actionMomentumIndex;
		std::ofstream ofile;
		
		shared_ptr<TriaxialCompressionEngine> triaxialCompressionEngine;
		shared_ptr<KinematicLocalisationAnalyser> analyser;
		std::string	 outputFile;
		std::string	 stateFileName;
		
	public :
		MicroMacroAnalyser();
		void action(MetaBody*);
		void setState(MetaBody* ncb, unsigned int state, bool saveStates = false, bool computeIncrement = false);//Set current state as initial (state=1) or final (state=2) congiguration for later kinematic analysis on the increment; if requested : save snapshots (with specific format) - possibly including contact forces increments on the state1->state2 interval
		int interval;
		
	DECLARE_LOGGER;
	
	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(MicroMacroAnalyser);
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};

REGISTER_SERIALIZABLE(MicroMacroAnalyser);

#endif // MACRO_MICRO_ANALYZER_HPP

