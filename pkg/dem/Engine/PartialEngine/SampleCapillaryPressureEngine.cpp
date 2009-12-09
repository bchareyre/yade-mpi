/*************************************************************************
*  Copyright (C) 2006 by Luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SampleCapillaryPressureEngine.hpp"
#include <yade/pkg-dem/CapillaryCohesiveLaw.hpp>
#include<yade/core/Scene.hpp>
#include<yade/core/Omega.hpp>
//#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

SampleCapillaryPressureEngine::SampleCapillaryPressureEngine()
{	
	//cerr << "constructeur de SamplePressureEngine" << endl;
	
	capillaryCohesiveLaw = new CapillaryCohesiveLaw;
	capillaryCohesiveLaw->CapillaryPressure= 0;
	capillaryCohesiveLaw->sdecGroupMask = 2;
	
	StabilityCriterion=0.01;
	SigmaPrecision = 0.001;
	Phase1=false;
	Phase1End = "Phase1End";
	Iteration = 0;
	UnbalancedForce = 0.01;

	pressureVariationActivated=false;
	Pressure = 0;
	PressureVariation = 1000;
	fusionDetection = true;
	binaryFusion = true;
	
	pressureIntervalRec = 10000;
	
	//cerr << "fin du constructeur de SamplePressureEngine" << endl;
	
}

SampleCapillaryPressureEngine::~SampleCapillaryPressureEngine()
{
}


void SampleCapillaryPressureEngine::updateParameters(Scene * ncb)
{
	//cerr << "updateParameters" << endl;
// 	Scene * ncb = static_cast<Scene*>(body);

	UnbalancedForce = ComputeUnbalancedForce(ncb);

	if (Omega::instance().getCurrentIteration() % 100 == 0) cerr << "UnbalancedForce=" << UnbalancedForce << endl;

	if (!Phase1 && UnbalancedForce<=StabilityCriterion && !pressureVariationActivated)
	{	
		internalCompaction = false;
		Phase1 = true;
	}
	
	if ( Phase1 && UnbalancedForce<=StabilityCriterion && !pressureVariationActivated)
	
	{
		Real S = meanStress; // revoir ici comment acceder à computeStress(ncb);
		//abs ( (meanStress-sigma_iso ) /sigma_iso ) <0.005

		cerr << "Smoy = " << meanStress << endl;
		if ((S > (sigma_iso - (sigma_iso*SigmaPrecision))) && (S < (sigma_iso + (sigma_iso*SigmaPrecision))))
		{
			//Iteration = Omega::instance().getCurrentIteration();

			// saving snapshot.xml
			string fileName = "../data/" + Phase1End + "_" + 
			lexical_cast<string>(Omega::instance().getCurrentIteration()) + ".xml";
			cerr << "saving snapshot: " << fileName << " ...";
			Omega::instance().saveSimulation(fileName);
			
			//recordStructure(ncb, Omega::instance().getCurrentIteration());

			pressureVariationActivated = true;
		}
	
	}

// 	if (pressureVariationActivated)
// 	
// 	{
// 		autoCompressionActivation = true;
// 	}

	//cerr << "fin updateParameters" << endl;
}
	
void SampleCapillaryPressureEngine::applyCondition(Scene * ncb)
{	
	//cerr << "applyConditionSampleCapillaryPressure" << endl;
// 	Scene* ncb = static_cast<Scene*>(body);

	updateParameters(ncb);
	
	TriaxialStressController::applyCondition(ncb);

	//cerr << "1" << endl;

		if (pressureVariationActivated)
		
		{
			//if ((Omega::instance().getCurrentIteration()) % pressureIntervalRec == 0) recordStructure(ncb, Omega::instance().getCurrentIteration());

			if (Omega::instance().getCurrentIteration() % 100 == 0) cerr << "pressure variation!!" << endl;
		
			if ((Pressure>=0) && (Pressure<=1000000000)) Pressure += PressureVariation;
			capillaryCohesiveLaw->CapillaryPressure = Pressure;

 			capillaryCohesiveLaw->fusionDetection = fusionDetection;
			capillaryCohesiveLaw->binaryFusion = binaryFusion;
		
		}
		
		else { capillaryCohesiveLaw->CapillaryPressure = Pressure;
		       capillaryCohesiveLaw->fusionDetection = fusionDetection;
		       capillaryCohesiveLaw->binaryFusion = binaryFusion;}

		if (Omega::instance().getCurrentIteration() % 100 == 0) cerr << "capillary pressure = " << Pressure << endl;
		
		//cerr << "3" << endl;
		capillaryCohesiveLaw->action(ncb);
		//cerr << "4" << endl;	
		
		UnbalancedForce = ComputeUnbalancedForce(ncb);
} 

//YADE_REQUIRE_FEATURE(PHYSPAR);

