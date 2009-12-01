/*************************************************************************
*  Copyright (C) 2006 by Luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SAMPLE_CAPILLARY_PRESSURE_ENGINE_HPP
#define SAMPLE_CAPILLARY_PRESSURE_ENGINE_HPP

#include<yade/core/DeusExMachina.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/pkg-dem/TriaxialStressController.hpp>
#include <string>

/*! \brief Isotropic compression + suction variation test */

class CapillaryCohesiveLaw;
class PhysicalAction;


class SampleCapillaryPressureEngine : public TriaxialStressController
{
	private :
		//shared_ptr<PhysicalAction> actionForce;
	
	public :
		SampleCapillaryPressureEngine();
		virtual ~SampleCapillaryPressureEngine();
		
		unsigned int	 interval, VariationInterval;
		
		//! Max ratio of resultant forces on mean contact force
		Real UnbalancedForce;
		//! Value of UnbalancedForce for which the system is considered
		Real StabilityCriterion, SigmaPrecision;
		//! is isotropicInternalCompactionFinished?
		bool Phase1;
		int Iteration, pressureIntervalRec;
		std::string Phase1End;
		//! pressure affectation
		Real Pressure;
		Real PressureVariation;
		//! Is pressure variation currently activated?
		bool pressureVariationActivated, fusionDetection, binaryFusion;
			
		//shared_ptr<CapillaryCohesiveLaw>  capillaryCohesiveLaw;
		CapillaryCohesiveLaw* capillaryCohesiveLaw;
			
		virtual void applyCondition(World * ncb);
		void updateParameters(World * ncb);
		
	REGISTER_ATTRIBUTES(TriaxialStressController,(PressureVariation)(Pressure)(UnbalancedForce)(StabilityCriterion)(SigmaPrecision)(pressureVariationActivated)(fusionDetection)(binaryFusion)(pressureIntervalRec));
	REGISTER_CLASS_NAME(SampleCapillaryPressureEngine);
	REGISTER_BASE_CLASS_NAME(TriaxialStressController);
};

REGISTER_SERIALIZABLE(SampleCapillaryPressureEngine);

#endif //  SAMPLECAPILLARYPRESSUREENGINE_HPP

