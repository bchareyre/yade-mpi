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

CREATE_LOGGER(TriaxialCompressionEngine);

TriaxialCompressionEngine::TriaxialCompressionEngine() : actionForce(new Force)
{
	translationAxis=TriaxialStressController::normal[wall_bottom_id];
	strainRate=0;
	currentStrainRate=0;
	StabilityCriterion=0.001;
	//Phase1=false;
	currentState=STATE_ISO_COMPACTION;
	UnbalancedForce = 1;
	Phase1End = "Compacted";
	FinalIterationPhase1 = 0;
	Iteration = 0;
	testEquilibriumInterval = 20;
	//compressionActivated=false;
	autoCompressionActivation=true;
	UnbalancedForce = 1;
	saveSimulation = false;
	firstRun=true;
}

TriaxialCompressionEngine::~TriaxialCompressionEngine()
{	
}


void TriaxialCompressionEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing) translationAxis.Normalize();
}


void TriaxialCompressionEngine::registerAttributes()
{
	TriaxialStressController::registerAttributes();
	REGISTER_ATTRIBUTE(strainRate);
	REGISTER_ATTRIBUTE(currentStrainRate);
	//REGISTER_ATTRIBUTE(Phase1);
	REGISTER_ATTRIBUTE(UnbalancedForce);
	REGISTER_ATTRIBUTE(StabilityCriterion);
	REGISTER_ATTRIBUTE(translationAxis);
	//REGISTER_ATTRIBUTE(compressionActivated);
	REGISTER_ATTRIBUTE(autoCompressionActivation);
	REGISTER_ATTRIBUTE(testEquilibriumInterval);
	REGISTER_ATTRIBUTE(currentState);
	REGISTER_ATTRIBUTE(previousState);
}

void TriaxialCompressionEngine::doStateTransition(stateNum nextState){
	if (nextState==STATE_ISO_COMPACTION){
		currentState=nextState;
		LOG_INFO("State transition to STATE_ISO_COMPACTION done.");
	}
	if(nextState==STATE_TRIAX_LOADING){
		assert(currentState==STATE_ISO_COMPACTION || currentState==STATE_LIMBO);
		internalCompaction = false;
		height0 = height; depth0 = depth; width0 = width;
		//compressionActivated = true;
		wall_bottom_activated=false;
		wall_top_activated=false;
		autoCompressionActivation = false; // FIXME: this can be removed, since it will not be used anymore
		if(!firstRun) saveSimulation=true; // saving snapshot .xml will actually be done in ::applyCondition
		currentState=nextState;
		LOG_INFO("State transition from STATE_ISO_COMPACTION to STATE_TRIAX_LOADING done.");
	}
	if(nextState==STATE_LIMBO){
		assert(currentState==STATE_ISO_COMPACTION);
		internalCompaction = false;
		height0 = height; depth0 = depth; width0 = width;
		saveSimulation=true; // saving snapshot .xml will actually be done in ::applyCondition
		currentState=nextState;
		LOG_INFO("State transition from STATE_ISO_COMPACTION to STATE_LIMBO done.");
		// stop simulation here, since nothing will happen from now on
		// Omega::instance().stopSimulationLoop();
	}
	previousState=currentState; // should be always kept in sync, used to track manual changes to the .xml
}

void TriaxialCompressionEngine::updateParameters(Body * body)
{

	UnbalancedForce=ComputeUnbalancedForce(body);
	MetaBody * ncb = static_cast<MetaBody*>(body);
	if (Omega::instance().getCurrentIteration() % 100 == 0) LOG_DEBUG("UnbalancedForce="<<UnbalancedForce);

	if(currentState==STATE_ISO_COMPACTION){ // FIXME: do we need this?? it makes sense to activate compression only during compaction!: || autoCompressionActivation){
		if ((Omega::instance().getCurrentIteration() % computeStressStrainInterval) == 0) computeStressStrain(ncb);
		TRVAR5(UnbalancedForce,StabilityCriterion,meanStress,sigma_iso,abs((meanStress-sigma_iso)/sigma_iso));

		if ( UnbalancedForce<=StabilityCriterion && abs((meanStress-sigma_iso)/sigma_iso)<0.02 ) {
			if(autoCompressionActivation) doStateTransition(STATE_TRIAX_LOADING);
			else doStateTransition(STATE_LIMBO);
		}
#if 0
        //This is a hack in order to allow subsequent run without activating compression - like for the YADE-COMSOL coupling
        if (!compressionActivated)
        {
				// 		vector<shared_ptr<Engine> >::iterator itFirst = ncb->engines.begin();
				// 		vector<shared_ptr<Engine> >::iterator itLast = ncb->engines.end();
				// 		for (;itFirst!=itLast; ++itFirst) {
				// 			if ((*itFirst)->getClassName() == "CohesiveFrictionalRelationships")
				// 				(static_cast<CohesiveFrictionalRelationships*> ( (*itFirst).get()))->setCohesionNow = true;
				// 		}
            internalCompaction = false;
            Phase1 = true;
            string fileName = "../data/" + Phase1End + "_" +
                              lexical_cast<string>(Omega::instance().getCurrentIteration()) + ".xml";
            cerr << "saving snapshot: " << fileName << " ...";
            Omega::instance().saveSimulation(fileName);
            Omega::instance().stopSimulationLoop();
        }
#endif
    }
}


void TriaxialCompressionEngine::applyCondition(Body * body)
{
	// here, we make sure to get consistent parameters, in case someone fiddled with the scene .xml manually
	if(firstRun){
		//sigma_iso was changed, we need to rerun compaction
		if(sigma_iso!=previousSigmaIso) doStateTransition(STATE_ISO_COMPACTION);
		if(previousState==STATE_LIMBO && currentState==STATE_TRIAX_LOADING) doStateTransition(STATE_TRIAX_LOADING);
		previousState=currentState;
		previousSigmaIso=sigma_iso;
		firstRun=false; // change this only _after_ state transitions
	}
	if(currentState==STATE_LIMBO) return;

    TriaxialStressController::applyCondition(body);
    if (Omega::instance().getCurrentIteration() % testEquilibriumInterval == 0) {
        updateParameters(body);
        if (saveSimulation) {
            string fileName = "../data/" + Phase1End + "_" +
                              lexical_cast<string>(Omega::instance().getCurrentIteration()) + ".xml";
            LOG_INFO("saving snapshot: "<<fileName);
            Omega::instance().saveSimulation(fileName);
            saveSimulation = false;
        }
    }
	 if (currentState==STATE_TRIAX_LOADING) {
        if (Omega::instance().getCurrentIteration() % 100 == 0) LOG_DEBUG("Compression active.");
        Real dt = Omega::instance().getTimeStep();
        MetaBody * ncb = static_cast<MetaBody*>(body);
        shared_ptr<BodyContainer>& bodies = ncb->bodies;

        if (currentStrainRate < strainRate) currentStrainRate += strainRate*0.0003;	// !!! if unloading (?)
        else currentStrainRate = strainRate;

		  /* Move top and bottom wall according to strain rate */
        PhysicalParameters* p = static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->physicalParameters. get());
        p->se3.position += 0.5*strainRate*height*translationAxis*dt;
        p = static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->physicalParameters.get( ));
        p->se3.position -= 0.5*strainRate*height*translationAxis*dt;
    }
}


