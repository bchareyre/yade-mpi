/*************************************************************************
*  Copyright (C) 2009 by Luc Sibille                                     *
*  luc.sibille@univ-nantes.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/lib/base/Math.hpp>
#include<yade/pkg/dem/TriaxialStressController.hpp>
#include<string>



/** \brief Class for controlling in stress or in strain with respect to each spatial direction a cubical assembly of particles.
 *
 * The engine perform a triaxial compression with a control in direction "i" in stress "if (stressControl_i)" else in strain.
 * For a stress control the imposed stress is specified by "sigma_i" with a "max_veli" depending on "strainRatei". To obtain the same strain rate in stress control than in strain control you need to set "wallDamping = 0.8".
 * For a strain control the imposed strain is specified by "strainRatei".
 * With this engine you can perform internal compaction by growing the size of particles by using TriaxialStressController::controlInternalStress . For that, just switch on 'internalCompaction=1' and fix sigma_iso=value of mean pressure that you want at the end of the internal compaction.
 *
 */

class ThreeDTriaxialEngine : public TriaxialStressController
{
	public :
// 		ThreeDTriaxialEngine();
		virtual ~ThreeDTriaxialEngine();
				
		
		Vector3r translationAxisy;
		Vector3r translationAxisx;
		Vector3r translationAxisz;

		//! is this the beginning of the simulation, after reading the scene? -> it is the first time that Yade passes trought the engine ThreeDTriaxialEngine
		bool firstRun;
				
		virtual void action();

		
		///Change physical properties of interactions and/or bodies in the middle of a simulation (change only friction for the moment, complete this function to set cohesion and others before compression test)
		void setContactProperties(Real frictionDegree);

		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(
		ThreeDTriaxialEngine,TriaxialStressController,
		"The engine perform a triaxial compression with a control in direction 'i' in stress (if stressControl_i) else in strain.\n\n"
		"For a stress control the imposed stress is specified by 'sigma_i' with a 'max_veli' depending on 'strainRatei'. To obtain the same strain rate in stress control than in strain control you need to set 'wallDamping = 0.8'.\n"
		"For a strain control the imposed strain is specified by 'strainRatei'.\n"
		"With this engine you can also perform internal compaction by growing the size of particles by using ``TriaxialStressController::controlInternalStress``. For that, just switch on 'internalCompaction=1' and fix sigma_iso=value of mean pressure that you want at the end of the internal compaction.\n"
		,
		((Real, strainRate1,0,,"target strain rate in direction 1 (./s)"))
		((Real, currentStrainRate1,0,,"current strain rate in direction 1 - converging to :yref:'ThreeDTriaxialEngine::strainRate1' (./s)"))
		((Real, strainRate2,0,,"target strain rate in direction 2 (./s)"))
		((Real, currentStrainRate2,0,,"current strain rate in direction 2 - converging to :yref:'ThreeDTriaxialEngine::strainRate2' (./s)"))
		((Real, strainRate3,0,,"target strain rate in direction 3 (./s)"))
		((Real, currentStrainRate3,0,,"current strain rate in direction 3 - converging to :yref:'ThreeDTriaxialEngine::strainRate3' (./s)"))
		((Real, UnbalancedForce,1,,"mean resultant forces divided by mean contact force"))
		((Real, frictionAngleDegree,-1,,"Value of friction used in the simulation if (updateFrictionAngle)"))
		((bool, updateFrictionAngle,false,,"Switch to activate the update of the intergranular frictionto the value :yref:'ThreeDTriaxialEngine::frictionAngleDegree")) 
		((bool, stressControl_1,true,,"Switch to choose a stress or a strain control in directions 1"))
		((bool, stressControl_2,true,,"Switch to choose a stress or a strain control in directions 2"))
		((bool, stressControl_3,true,,"Switch to choose a stress or a strain control in directions 3"))
		((std::string,Key,"",,"A string appended at the end of all files, use it to name simulations."))
		,
		translationAxisy=Vector3r(0,1,0);
		translationAxisx=Vector3r(1,0,0);
		translationAxisz=Vector3r(0,0,1);
		firstRun=true;
		boxVolume=0;
		,
		.def("setContactProperties",&ThreeDTriaxialEngine::setContactProperties,"Assign a new friction angle (degrees) to dynamic bodies and relative interactions")
		)

	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(ThreeDTriaxialEngine);


