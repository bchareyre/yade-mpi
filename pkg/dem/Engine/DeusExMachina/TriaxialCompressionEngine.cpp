/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "TriaxialCompressionEngine.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/core/Omega.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include <boost/lexical_cast.hpp>

class CohesiveFrictionalRelationships;

TriaxialCompressionEngine::TriaxialCompressionEngine() : actionForce(new Force)
{
	translationAxis=TriaxialStressController::normal[wall_bottom_id];
	strainRate=0;
	currentStrainRate=0;
	StabilityCriterion=0.001;
	Phase1=false;
	UnbalancedForce = 1;
	Phase1End = "Compacted";
	FinalIterationPhase1 = 0;
	Iteration = 0;
	testEquilibriumInterval = 20;
	compressionActivated=false;
	autoCompressionActivation=true;
	UnbalancedForce = 1;
	previousSigmaIso = 0;
	//for (int i=0; i<3; ++i) strain[i]=0;
	saveSimulation = false;
	
	
}

TriaxialCompressionEngine::~TriaxialCompressionEngine()
{	
}


void TriaxialCompressionEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
        translationAxis.Normalize();
}


void TriaxialCompressionEngine::registerAttributes()
{
	TriaxialStressController::registerAttributes();
	REGISTER_ATTRIBUTE(strainRate);
	REGISTER_ATTRIBUTE(currentStrainRate);
	REGISTER_ATTRIBUTE(Phase1);
	//REGISTER_ATTRIBUTE(strain);
	REGISTER_ATTRIBUTE(UnbalancedForce);
	REGISTER_ATTRIBUTE(StabilityCriterion);
	REGISTER_ATTRIBUTE(translationAxis);
	REGISTER_ATTRIBUTE(compressionActivated);
	REGISTER_ATTRIBUTE(autoCompressionActivation);
	REGISTER_ATTRIBUTE(testEquilibriumInterval);
	REGISTER_ATTRIBUTE(previousSigmaIso);
		
}



void TriaxialCompressionEngine::updateParameters(Body * body)
{

    UnbalancedForce=ComputeUnbalancedForce(body);
    MetaBody * ncb = static_cast<MetaBody*>(body);
    if (Omega::instance().getCurrentIteration() % 100 == 0) cerr << "UnbalancedForce=" << UnbalancedForce << endl;
    //Real S = computeStressStrain(ncb);
    if (sigma_iso != previousSigmaIso) {
    	previousSigmaIso = sigma_iso;
    	Phase1 = false;
    }

    if ((!Phase1) || (autoCompressionActivation))
    {
    	if (!(Omega::instance().getCurrentIteration() % computeStressStrainInterval == 0)) computeStressStrain(ncb);
    	//cerr << "UnbalancedForce" << UnbalancedForce << "StabilityCriterion" << StabilityCriterion << "meanStress"<< meanStress<< "sigma_iso"<<sigma_iso<< endl;
        if (UnbalancedForce<=StabilityCriterion && abs((meanStress-sigma_iso)/sigma_iso) < 0.02)
        {
       // cerr << "go to phase1" << endl;
            Phase1 = true;
            internalCompaction = false;
            height0 = height;
            depth0 = depth;
            width0 = width;

// 			FinalIterationPhase1 =
// 			Omega::instance().getCurrentIteration();


            //if ((S-sigma_iso)/sigma_iso < 0.005)
            //{
            Iteration = Omega::instance().getCurrentIteration();

            //if ((Omega::instance().getCurrentIteration()) >= (Iteration + 1000));

            // saving snapshot.xml
            saveSimulation = true;
//             string fileName = "../data/" + Phase1End + "_" +
//                               lexical_cast<string>(Omega::instance().getCurrentIteration()) + ".xml";
//             cerr << "saving snapshot: " << fileName << " ...";
//             Omega::instance().saveSimulation(fileName);
            if (autoCompressionActivation)
            {
                compressionActivated = true;
                wall_bottom_activated=false;
                wall_top_activated=false;
                autoCompressionActivation = false;
            }
           // else Omega::instance().stopSimulationLoop();//FIXME : will not stop the nullgui runs


        }

// 	if (compressionActivated)
// 	{
// 		wall_bottom_activated=false;
// 		wall_top_activated=false;
// 		internalCompaction = false;
// 		autoCompressionActivation = false;
// 	}


        //This is a hack in order to allow subsequent run without activating compression - like for the YADE-COMSOL coupling
//         if (!compressionActivated)
//         {
// // 		vector<shared_ptr<Engine> >::iterator itFirst = ncb->engines.begin();
// // 		vector<shared_ptr<Engine> >::iterator itLast = ncb->engines.end();
// // 		for (;itFirst!=itLast; ++itFirst) {
// // 			if ((*itFirst)->getClassName() == "CohesiveFrictionalRelationships")
// // 				(static_cast<CohesiveFrictionalRelationships*> ( (*itFirst).get()))->setCohesionNow = true;
// // 		}
//             internalCompaction = false;
//             Phase1 = true;
//             string fileName = "../data/" + Phase1End + "_" +
//                               lexical_cast<string>(Omega::instance().getCurrentIteration()) + ".xml";
//             cerr << "saving snapshot: " << fileName << " ...";
//             Omega::instance().saveSimulation(fileName);
//             Omega::instance().stopSimulationLoop();
//         }
    }
    if (compressionActivated && !Phase1)
    {
        wall_bottom_activated=false;
        wall_top_activated=false;
        internalCompaction = false;
        Phase1 = true;
        autoCompressionActivation = false;
        height0 = height;
        depth0 = depth;
        width0 = width;
    }
}


void TriaxialCompressionEngine::applyCondition(Body * body)
{
    //cerr << "TriaxialCompressionEngine::applyCondition(Body * body)" << endl;
    TriaxialStressController::applyCondition(body);
    if (Omega::instance().getCurrentIteration() % testEquilibriumInterval == 0)
    {
        updateParameters(body);
        if (saveSimulation)
        {
            string fileName = "../data/" + Phase1End + "_" +
                              lexical_cast<string>(Omega::instance().getCurrentIteration()) + ".xml";
            cerr << "saving snapshot: " << fileName << " ...";
            Omega::instance().saveSimulation(fileName);
            saveSimulation = false;
        }
    }


//        MetaBody * ncb2 = static_cast<MetaBody*>(body);
    //cerr << computeStress(ncb2)  << endl;

    if (compressionActivated)
    {
        if (Omega::instance().getCurrentIteration() % 100 == 0)
            cerr << "Compression started!!" << endl;
        Real dt = Omega::instance().getTimeStep();
        MetaBody * ncb = static_cast<MetaBody*>(body);
        shared_ptr<BodyContainer>& bodies = ncb->bodies;

        if (currentStrainRate < strainRate) currentStrainRate
            += strainRate*0.0003;	// !!! si decharge
        else currentStrainRate = strainRate;

        PhysicalParameters* p =
            static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->
                                             physicalParameters. get());
        p->se3.position += 0.5*strainRate*height*translationAxis*dt;
//                 cerr << "deplacmt = " <<
// 		0.5*strainRate*height*translationAxis*dt << endl;
//                 cerr << "wall_bottom : p->se3.position = " << p->se3.position
// 		<< endl;

        p =
            static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->
                                             physicalParameters.get( ));
        p->se3.position -= 0.5*strainRate*height*translationAxis*dt;
//                 cerr << "wall_top : p->se3.position = " << p->se3.position
// 		<< endl;

    }
}


