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
#include<boost/lexical_cast.hpp>
#include<boost/lambda/lambda.hpp>
#include<yade/extra/Shop.hpp>


class CohesiveFrictionalRelationships;

CREATE_LOGGER(TriaxialCompressionEngine);

TriaxialCompressionEngine::TriaxialCompressionEngine() : actionForce(new Force)
{
	translationAxis=TriaxialStressController::normal[wall_bottom_id];
	strainRate=0;
	currentStrainRate=0;
	StabilityCriterion=0.001;
	//Phase1=false;
	currentState=STATE_UNINITIALIZED;
	previousState=currentState;
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
	REGISTER_ATTRIBUTE(sigmaIsoCompaction);
	REGISTER_ATTRIBUTE(sigmaLateralConfinement);
}

void TriaxialCompressionEngine::doStateTransition(stateNum nextState){
	if ( /* currentState==STATE_UNINITIALIZED && */ nextState==STATE_ISO_COMPACTION){
		sigma_iso=sigmaIsoCompaction;
	}
	else if((currentState==STATE_ISO_COMPACTION || currentState==STATE_ISO_UNLOADING || currentState==STATE_LIMBO) && nextState==STATE_TRIAX_LOADING){
		sigma_iso=sigmaLateralConfinement;
		internalCompaction = false;
		height0 = height; depth0 = depth; width0 = width;
		//compressionActivated = true;
		wall_bottom_activated=false;
		wall_top_activated=false;
		if(currentState==STATE_ISO_UNLOADING){ LOG_INFO("Speres -> /tmp/unloaded.spheres"); Shop::saveSpheresToFile("/tmp/unloaded.spheres"); }
		if(1){
			max_vel*=20;
			Shop::createCohesion(1e-4,1e-4,0); // (boost::lambda::_1 %2==0) && (boost::lambda::_2%2==0));
			/*shared_ptr<MetaBody> rootBody=Omega::instance().getRootBody();
			for(vector<shared_ptr<Engine> >::iterator I=rootBody->engines.begin(); I!=rootBody->engines.end(); ++I){
				if((*I)->getClassName()=="PersistentSAPCollider") {
					rootBody->engines.erase(I);
					LOG_DEBUG("Removed PersistentSAPCollider engine.");
					break;
				}
			}*/
		}
		if(!firstRun) saveSimulation=true; // saving snapshot .xml will actually be done in ::applyCondition
	}
	else if(currentState==STATE_ISO_COMPACTION && nextState==STATE_ISO_UNLOADING){
		sigma_iso=sigmaLateralConfinement;
		internalCompaction=false; // unloading will not change grain sizes
	}
	else if(currentState==STATE_ISO_COMPACTION && nextState==STATE_LIMBO){
		internalCompaction = false;
		height0 = height; depth0 = depth; width0 = width;
		saveSimulation=true; // saving snapshot .xml will actually be done in ::applyCondition
		// stop simulation here, since nothing will happen from now on
		Omega::instance().stopSimulationLoop();
	}
	else goto undefinedTransition;

	LOG_INFO("State transition from "<<stateName(currentState)<<" to "<<stateName(nextState)<<" done.");
	currentState=nextState;
	previousState=currentState; // should be always kept in sync, used to track manual changes to the .xml
	return;

	undefinedTransition:
		LOG_ERROR("Undefined transition from "<<stateName(currentState)<<" to "<<stateName(nextState)<<"! (ignored)");
}

void TriaxialCompressionEngine::updateParameters(Body * body)
{

	UnbalancedForce=ComputeUnbalancedForce(body); // calculated at every iteration
	MetaBody * ncb = static_cast<MetaBody*>(body);

	if (Omega::instance().getCurrentIteration() % 100 == 0) {
		LOG_INFO("UnbalancedForce="<< UnbalancedForce);
		cerr << "UnbalancedForce=" << UnbalancedForce << endl;
		/* TRVAR1(meanStress);*/ /* TRVAR2(stateName(currentState),sigma_iso); */
	}

	if(currentState==STATE_ISO_COMPACTION || currentState==STATE_ISO_UNLOADING){
		// FIXME: do we need this?? it makes sense to activate compression only during compaction!: || autoCompressionActivation)
		if ((Omega::instance().getCurrentIteration() % computeStressStrainInterval) == 0){
			computeStressStrain(ncb);
			//TRVAR5(UnbalancedForce,StabilityCriterion,meanStress,sigma_iso,abs((meanStress-sigma_iso)/sigma_iso));
		}
		if ( UnbalancedForce<=StabilityCriterion && abs((meanStress-sigma_iso)/sigma_iso)<0.02 ) {
			if(currentState==STATE_ISO_COMPACTION && autoCompressionActivation){
				doStateTransition(STATE_ISO_UNLOADING); /*update stress and strain here*/ computeStressStrain(ncb);
			}
			else if(currentState==STATE_ISO_UNLOADING && autoCompressionActivation) {
				doStateTransition(STATE_TRIAX_LOADING); computeStressStrain(ncb);
			}
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
		LOG_INFO("First run, will initialize!");
		//sigma_iso was changed, we need to rerun compaction
		if(sigma_iso!=previousSigmaIso || currentState==STATE_UNINITIALIZED) doStateTransition(STATE_ISO_COMPACTION);
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
				fileName="../data/"+Phase1End+"_"+lexical_cast<string>(Omega::instance().getCurrentIteration())+".spheres";
				LOG_INFO("saving spheres: "<<fileName);
				Shop::saveSpheresToFile(fileName);
            saveSimulation = false;
        }
    }
	 if (currentState==STATE_TRIAX_LOADING) {
	 if (Omega::instance().getCurrentIteration() % 100 == 0) {
            cerr << "Compression started" << endl;
            }
        // if (Omega::instance().getCurrentIteration() % 100 == 0) LOG_DEBUG("Compression active.");
        Real dt = Omega::instance().getTimeStep();

        if (currentStrainRate < strainRate) currentStrainRate += strainRate*0.0003;	// !!! if unloading (?)
        else currentStrainRate = strainRate;

		  /* Move top and bottom wall according to strain rate */
        PhysicalParameters* p=static_cast<PhysicalParameters*>(Body::byId(wall_bottom_id)->physicalParameters.get());
        p->se3.position += 0.5*strainRate*height*translationAxis*dt;
        p = static_cast<PhysicalParameters*>(Body::byId(wall_top_id)->physicalParameters.get());
        p->se3.position -= 0.5*strainRate*height*translationAxis*dt;
    }
}


YADE_PLUGIN();
