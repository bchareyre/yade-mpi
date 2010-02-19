/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "TriaxialCompressionEngine.hpp"
#include<yade/core/Scene.hpp>
#include<yade/core/Omega.hpp>
#include<yade/lib-base/Math.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/lambda/lambda.hpp>
#include<yade/pkg-dem/Shop.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/FrictPhys.hpp>
#include<yade/pkg-common/ElastMat.hpp>

class CohesiveFrictionalRelationships;

CREATE_LOGGER(TriaxialCompressionEngine);
YADE_PLUGIN((TriaxialCompressionEngine));

TriaxialCompressionEngine::TriaxialCompressionEngine() : uniaxialEpsilonCurr(strain[1])
{
	translationAxis=TriaxialStressController::normal[wall_bottom_id];
	translationAxisx=Vector3r(1,0,0);
	translationAxisz=Vector3r(0,0,1);
	strainRate=0;
	currentStrainRate=0;
	StabilityCriterion=0.001;
	//Phase1=false;
	currentState=STATE_UNINITIALIZED;
	previousState=currentState;
	UnbalancedForce = 1;
	Key = "";
	noFiles=false;
	Phase1End = "Compacted";
	FinalIterationPhase1 = 0;
	Iteration = 0;
	testEquilibriumInterval = 20;

	autoUnload=true;
	autoCompressionActivation=true;
	autoStopSimulation=true;

	UnbalancedForce = 1;
	saveSimulation = false;
	firstRun=true;
	previousSigmaIso=sigma_iso;
	frictionAngleDegree = -1;
	epsilonMax = 0.5;

	isotropicCompaction=false;
 	boxVolume=0;
	maxStress =0;
}

TriaxialCompressionEngine::~TriaxialCompressionEngine()
{	
}

void TriaxialCompressionEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing) translationAxis.Normalize();
}

void TriaxialCompressionEngine::doStateTransition(Scene * body, stateNum nextState){

	if ( /* currentState==STATE_UNINITIALIZED && */ nextState==STATE_ISO_COMPACTION){
		sigma_iso=sigmaIsoCompaction;
		previousSigmaIso=sigma_iso;
	}
	else if((currentState==STATE_ISO_COMPACTION || currentState==STATE_ISO_UNLOADING || currentState==STATE_LIMBO) && nextState==STATE_TRIAX_LOADING){
		sigma_iso=sigmaLateralConfinement;
		previousSigmaIso=sigma_iso;		
		internalCompaction = false;
		if (frictionAngleDegree>0) setContactProperties(body, frictionAngleDegree);
		height0 = height; depth0 = depth; width0 = width;
		//compressionActivated = true;
		wall_bottom_activated=false; wall_top_activated=false;
		if(currentState==STATE_ISO_UNLOADING && !noFiles){ LOG_INFO("Speres -> /tmp/unloaded.spheres"); Shop::saveSpheresToFile("/tmp/unloaded.spheres"); }
		if(!firstRun && !noFiles) saveSimulation=true; // saving snapshot .xml will actually be done in ::applyCondition
		Phase1End = "Unloaded";
	}
	else if(currentState==STATE_ISO_COMPACTION && nextState==STATE_ISO_UNLOADING){
		sigma_iso=sigmaLateralConfinement;
		sigmaIsoCompaction = sigmaLateralConfinement;
		previousSigmaIso=sigma_iso;
		internalCompaction=false; // unloading will not change grain sizes
		if (frictionAngleDegree>0) setContactProperties(body, frictionAngleDegree);
		if(!firstRun && !noFiles) saveSimulation=true;
		Phase1End = "Compacted";
	}	
	else if ((currentState==STATE_ISO_COMPACTION || currentState==STATE_ISO_UNLOADING) && nextState==STATE_LIMBO) {
	//urrentState==STATE_DIE_COMPACTION
		internalCompaction = false;
		height0 = height; depth0 = depth; width0 = width;
		if(!noFiles) saveSimulation=true; // saving snapshot .xml will actually be done in ::applyCondition
		// stop simulation here, since nothing will happen from now on
		Phase1End = (currentState==STATE_ISO_COMPACTION ? "compacted" : "unloaded");
		if(!noFiles) Shop::saveSpheresToFile("/tmp/limbo.spheres");
		// Please keep this saving process intact, I'm tired of running 3 days simulations and getting nothing at the end!
		if(!firstRun && !noFiles) saveSimulation=true; // saving snapshot .xml will actually be done in ::applyCondition
	}
	else if( nextState==STATE_FIXED_POROSITY_COMPACTION){		
		internalCompaction = false;
		wall_bottom_activated=false; wall_top_activated=false;
		wall_front_activated=false; wall_back_activated=false;
		wall_right_activated=false; wall_left_activated=false;
	}	
	else { LOG_ERROR("Undefined transition from "<<stateName(currentState)<<" to "<<stateName(nextState)<<"! (ignored)"); return; }

	LOG_INFO("State transition from "<<stateName(currentState)<<" to "<<stateName(nextState)<<" done.");
	currentState=nextState;
	previousState=currentState; // should be always kept in sync, used to track manual changes to the .xml
}

void TriaxialCompressionEngine::updateParameters ( Scene * ncb )
{
	UnbalancedForce=ComputeUnbalancedForce ( ncb );

	if ( currentState==STATE_ISO_COMPACTION || currentState==STATE_ISO_UNLOADING || currentState==STATE_FIXED_POROSITY_COMPACTION || autoCompressionActivation)
	{
		// FIXME: do we need this?? it makes sense to activate compression only during compaction!: || autoCompressionActivation)
		//ANSWER TO FIXME : yes we need that because we want to start compression from LIMBO most of the time
		if ( UnbalancedForce<=StabilityCriterion && abs ( ( meanStress-sigma_iso ) /sigma_iso ) <0.005 && isotropicCompaction==false )
		{
			// only go to UNLOADING if it is needed (hard float comparison... :-| )
			if ( currentState==STATE_ISO_COMPACTION && autoUnload && sigmaLateralConfinement!=sigmaIsoCompaction ) {
				doStateTransition (ncb, STATE_ISO_UNLOADING );
				computeStressStrain ( ncb ); // update stress and strain
			}
			// Preserve transition from LIMBO to something, I need that! (BC)
			else if((currentState==STATE_ISO_COMPACTION || currentState==STATE_ISO_UNLOADING || currentState==STATE_LIMBO) && autoCompressionActivation){
				doStateTransition (ncb, STATE_TRIAX_LOADING );
				computeStressStrain ( ncb ); // update stress and strain
			}
			// stop simulation if unloaded and compression is not activate automatically
			else if (currentState==STATE_ISO_UNLOADING && !autoCompressionActivation){
				Omega::instance().stopSimulationLoop();
			}
			// huh?! this will never happen, because of the first condition...
			else 
			{ 
			doStateTransition (ncb, STATE_LIMBO );
			}
		}
		else if ( porosity<=fixedPorosity && currentState==STATE_FIXED_POROSITY_COMPACTION )
		{
			Omega::instance().stopSimulationLoop();
			return;
		}
	}
}


void TriaxialCompressionEngine::applyCondition ( Scene * ncb )
{
	// here, we make sure to get consistent parameters, in case someone fiddled with the scene .xml manually
	if ( firstRun )
	{
		LOG_INFO ( "First run, will initialize!" );
		/* FIXME: are these three if's mutually exclusive and are partition of all possibilities? */
		//sigma_iso was changed, we need to rerun compaction

		if ( (sigmaIsoCompaction!=previousSigmaIso || currentState==STATE_UNINITIALIZED || currentState== STATE_LIMBO) && currentState!=STATE_TRIAX_LOADING && isotropicCompaction == false) doStateTransition (ncb, STATE_ISO_COMPACTION );
		if ( previousState==STATE_LIMBO && currentState==STATE_TRIAX_LOADING && isotropicCompaction == false ) doStateTransition (ncb, STATE_TRIAX_LOADING );
		if ( fixedPorosity<1 && currentState==STATE_UNINITIALIZED && isotropicCompaction!=false ) doStateTransition (ncb, STATE_FIXED_POROSITY_COMPACTION );
		previousState=currentState;
		previousSigmaIso=sigma_iso;
		firstRun=false; // change this only _after_ state transitions
	}
	if ( Omega::instance().getCurrentIteration() % testEquilibriumInterval == 0 )
	{
		updateParameters ( ncb );
		maxStress = max(maxStress,stress[wall_top][1]);
		LOG_INFO("UnbalancedForce="<< UnbalancedForce<<", rel stress "<< abs ( ( meanStress-sigma_iso ) /sigma_iso ));
	}	
	if ( saveSimulation )
	{
		if(!noFiles){
			string fileName = "./"+ Key + "_" + Phase1End + "_" +
							  lexical_cast<string> ( Omega::instance().getCurrentIteration() ) + "_" +
							  lexical_cast<string> ( currentState ) + ".xml";
			LOG_INFO ( "saving snapshot: "<<fileName );
			Omega::instance().saveSimulation ( fileName );
			fileName="./"+ Key + "_"+Phase1End+"_"+lexical_cast<string> ( Omega::instance().getCurrentIteration() ) + "_" +
					 lexical_cast<string> ( currentState ) +".spheres";
			LOG_INFO ( "saving spheres: "<<fileName );
			Shop::saveSpheresToFile ( fileName );
		}
		saveSimulation = false;
	}
	if ( currentState==STATE_LIMBO && autoStopSimulation )
	{		
		Omega::instance().stopSimulationLoop();
		return;
	}
	TriaxialStressController::applyCondition ( ncb );

	if ( currentState==STATE_TRIAX_LOADING )
	{
		if ( Omega::instance().getCurrentIteration() % 100 == 0 )
		{
			LOG_INFO ("Triax Compression started");
		}
		if (Omega::instance().getCurrentIteration() % 100 == 0) LOG_DEBUG("Compression active.");
		Real dt = Omega::instance().getTimeStep();
		 
		if (abs(epsilonMax) > abs(strain[1])) {
			if ( currentStrainRate != strainRate ) currentStrainRate += ( strainRate-currentStrainRate ) *0.0003;
			/* Move top and bottom wall according to strain rate */
			State* p_bottom=Body::byId(wall_bottom_id,ncb)->state.get();
			p_bottom->pos += 0.5*currentStrainRate*height*translationAxis*dt;
			State* p_top=Body::byId(wall_top_id,ncb)->state.get();
			p_top->pos -= 0.5*currentStrainRate*height*translationAxis*dt;
		} else {
			Omega::instance().stopSimulationLoop();
		}
	}
	if ( currentState==STATE_FIXED_POROSITY_COMPACTION )
	{
		if ( Omega::instance().getCurrentIteration() % 100 == 0 )
		{
			LOG_INFO ("Compression started");
		}		
		Real dt = Omega::instance().getTimeStep();
		State* p_bottom=Body::byId(wall_bottom_id,ncb)->state.get();
		State* p_top=Body::byId(wall_top_id,ncb)->state.get();
		State* p_left=Body::byId(wall_left_id,ncb)->state.get();
		State* p_right=Body::byId(wall_right_id,ncb)->state.get();
		State* p_front=Body::byId(wall_front_id,ncb)->state.get();
		State* p_back=Body::byId(wall_back_id,ncb)->state.get();

		/* Move top and bottom wall according to strain rate */
		p_bottom->pos += 0.5*strainRate*height*translationAxis*dt;
		p_top->pos -= 0.5*strainRate*height*translationAxis*dt;
		p_back->pos += 0.5*strainRate*depth*translationAxisz*dt;
		p_front->pos -= 0.5*strainRate*depth*translationAxisz*dt;
		p_left->pos += 0.5*strainRate*width*translationAxisx*dt;
		p_right->pos -= 0.5*strainRate*width*translationAxisx*dt;
	}
 
}

void TriaxialCompressionEngine::setContactProperties(Scene * ncb, Real frictionDegree)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	BodyContainer::iterator bi = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for ( ; bi!=biEnd; ++bi)	
	{	
		shared_ptr<Body> b = *bi;
		if (b->isDynamic)
		YADE_PTR_CAST<FrictMat> (b->material)->frictionAngle = frictionDegree * Mathr::PI/180.0;
	}
	InteractionContainer::iterator ii    = ncb->interactions->begin();
	InteractionContainer::iterator iiEnd = ncb->interactions->end(); 
	for(  ; ii!=iiEnd ; ++ii ) {
		if (!(*ii)->isReal()) continue;
		const shared_ptr<FrictMat>& sdec1 = YADE_PTR_CAST<FrictMat>((*bodies)[(body_id_t) ((*ii)->getId1())]->material);
		const shared_ptr<FrictMat>& sdec2 = YADE_PTR_CAST<FrictMat>((*bodies)[(body_id_t) ((*ii)->getId2())]->material);		
		//FIXME - why dynamic_cast fails here?
		//const shared_ptr<FrictPhys>& contactPhysics = YADE_PTR_CAST<FrictPhys>((*ii)->interactionPhysics);
		const shared_ptr<FrictPhys>& contactPhysics = static_pointer_cast<FrictPhys>((*ii)->interactionPhysics);

		Real fa 	= sdec1->frictionAngle;
		Real fb 	= sdec2->frictionAngle;
		contactPhysics->frictionAngle			= std::min(fa,fb);
		contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle);
	}
} 

