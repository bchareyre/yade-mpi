/*************************************************************************
*  Copyright (C) 2009 by Luc Sibille                                     *
*  luc.sibille@univ-nantes.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/pkg-dem/TriaxialStressController.hpp>
#include<string>



class PhysicalAction;



/** \brief Class for controlling in stress or in strain with respect to each spatial direction a cubical assembly of particles.
 *
 * The engine perform a triaxial compression with a control in direction "i" in stress "if (stressControl_i)" else in strain.
 * For a stress control the imposed stress is specified by "sigma_i".
 * For a strain control the imposed strain is specified by "strainRatei".
 *
 */

class ThreeDTriaxialEngine : public TriaxialStressController
{
	private :
		shared_ptr<PhysicalAction> actionForce;
		//std::string Phase1End;//used to name output files based on current state
				
	public :
		ThreeDTriaxialEngine();
		virtual ~ThreeDTriaxialEngine();
				
		//! Strain velocity (./s)
		Real strainRate1;
		Real currentStrainRate1;
		Real strainRate2;
		Real currentStrainRate2;
		Real strainRate3;
		Real currentStrainRate3;
		//! Max ratio of resultant forces on mean contact force
		Real UnbalancedForce;
		//! Value of UnbalancedForce for which the system is considered stable
		Real StabilityCriterion;
		//! Value of axial deformation for which the simulation must stop
		//Real epsilonMax;
		//! Current value of axial deformation during confined loading (is reference to strain[1])
		Real& uniaxialEpsilonCurr;
		//! Value of friction to use in the test "if (updateFrictionAngle)"
		Real frictionAngleDegree;
		//! Swith to activate if an update of the intergranular friction angle is required
		bool updateFrictionAngle;

		//! switches to choose a stress or a strain control in directions 1, 2 or 3
		bool stressControl_1;
		bool stressControl_2;
		bool stressControl_3;
		

		Vector3r translationAxisy;
		Vector3r translationAxisx;
		Vector3r translationAxisz;

		//! is this the beginning of the simulation, after reading the scene?
		bool firstRun;
		//int FinalIterationPhase1, Iteration, testEquilibriumInterval;
		int Iteration, testEquilibriumInterval;
		
		std::string Key;//A code that is appended to file names to help distinguish between different simulations
				
		virtual void action();
		//void updateParameters();
		
		///Change physical properties of interactions and/or bodies in the middle of a simulation (change only friction for the moment, complete this function to set cohesion and others before compression test)
		void setContactProperties(Scene * body, Real frictionAngle);


		DECLARE_LOGGER;
		

	REGISTER_ATTRIBUTES(TriaxialStressController,(strainRate1)(currentStrainRate1)(strainRate2)(currentStrainRate2)(strainRate3)(currentStrainRate3)/*(Phase1)*/(UnbalancedForce)(StabilityCriterion)/*(translationAxis)(compressionActivated)(autoCompressionActivation)(autoStopSimulation)*/(testEquilibriumInterval)/*(currentState)(previousState)(sigmaIsoCompaction)(previousSigmaIso)(sigmaLateralConfinement)*/(Key)(frictionAngleDegree)(updateFrictionAngle)	/*(epsilonMax)*/ (uniaxialEpsilonCurr)/*(isotropicCompaction)*/(spheresVolume)/*(fixedPorosity)*/(stressControl_1)(stressControl_2)(stressControl_3)(sigma1)(sigma2)(sigma3));
	REGISTER_CLASS_NAME(ThreeDTriaxialEngine);
	REGISTER_BASE_CLASS_NAME(TriaxialStressController);
};

REGISTER_SERIALIZABLE(ThreeDTriaxialEngine);


