/*************************************************************************
*  Copyright (C) 2009 by Luc Sibille                                     *
*  luc.sibille@univ-nantes.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ThreeDTriaxialEngine.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/core/Omega.hpp>
//#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/lambda/lambda.hpp>
#include<yade/extra/Shop.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>

class CohesiveFrictionalRelationships;

CREATE_LOGGER(ThreeDTriaxialEngine);

//ThreeDTriaxialEngine::ThreeDTriaxialEngine() : actionForce(new Force), uniaxialEpsilonCurr(strain[1])  //Luc?????
ThreeDTriaxialEngine::ThreeDTriaxialEngine() : uniaxialEpsilonCurr(strain[1])  //Luc?????
{
	//translationAxis=TriaxialStressController::normal[wall_bottom_id];

	translationAxisy=Vector3r(0,1,0);
	translationAxisx=Vector3r(1,0,0);
	translationAxisz=Vector3r(0,0,1);
	strainRate1=0;
	currentStrainRate1=0;
	strainRate2=0;
	currentStrainRate2=0;
	strainRate2=0;
	currentStrainRate2=0;
	StabilityCriterion=0.001;
	//Phase1=false;
	//currentState=STATE_UNINITIALIZED;
	//previousState=currentState;
	UnbalancedForce = 1;
	Key = "";
	//Phase1End = "Compacted";
	//FinalIterationPhase1 = 0;
	Iteration = 0;
	testEquilibriumInterval = 20;

	//autoUnload=true;
	//autoCompressionActivation=true;
	//autoStopSimulation=true;

	//UnbalancedForce = 1;
	//saveSimulation = false;
	firstRun=true;
	//previousSigmaIso=sigma_iso;
	frictionAngleDegree = -1;
	//epsilonMax = 0.5;
	updateFrictionAngle=false;

	stressControl_1=false;
	stressControl_2=false;
	stressControl_3=false;

	//isotropicCompaction=false;
 	boxVolume=0;

//	calculatedPorosity=1.1;	

}

ThreeDTriaxialEngine::~ThreeDTriaxialEngine()
{	
}


//void ThreeDTriaxialEngine::postProcessAttributes(bool deserializing)  // luc quelle est l'utilité???
//{
//	if(deserializing) translationAxis.Normalize();
//}


void ThreeDTriaxialEngine::registerAttributes()
{
	TriaxialStressController::registerAttributes();
	REGISTER_ATTRIBUTE(strainRate1);
	REGISTER_ATTRIBUTE(currentStrainRate1);
	REGISTER_ATTRIBUTE(strainRate2);
	REGISTER_ATTRIBUTE(currentStrainRate2);
	REGISTER_ATTRIBUTE(strainRate3);
	REGISTER_ATTRIBUTE(currentStrainRate3);
	//REGISTER_ATTRIBUTE(Phase1);
	REGISTER_ATTRIBUTE(UnbalancedForce);
	REGISTER_ATTRIBUTE(StabilityCriterion);
	//REGISTER_ATTRIBUTE(translationAxis);
	//REGISTER_ATTRIBUTE(compressionActivated);
	//REGISTER_ATTRIBUTE(autoCompressionActivation);
	//REGISTER_ATTRIBUTE(autoStopSimulation);
	REGISTER_ATTRIBUTE(testEquilibriumInterval);
	//REGISTER_ATTRIBUTE(currentState);
	//REGISTER_ATTRIBUTE(previousState);
	//REGISTER_ATTRIBUTE(sigmaIsoCompaction);
	//REGISTER_ATTRIBUTE(previousSigmaIso);
	//REGISTER_ATTRIBUTE(sigmaLateralConfinement);
	REGISTER_ATTRIBUTE(Key);
	REGISTER_ATTRIBUTE(frictionAngleDegree);
	REGISTER_ATTRIBUTE(updateFrictionAngle);
	//REGISTER_ATTRIBUTE(epsilonMax);
	REGISTER_ATTRIBUTE(uniaxialEpsilonCurr);
 	//REGISTER_ATTRIBUTE(isotropicCompaction);
 	REGISTER_ATTRIBUTE(spheresVolume);
 	//REGISTER_ATTRIBUTE(fixedPorosity);
	REGISTER_ATTRIBUTE(stressControl_1);
	REGISTER_ATTRIBUTE(stressControl_2);
	REGISTER_ATTRIBUTE(stressControl_3);
	REGISTER_ATTRIBUTE(sigma1);
	REGISTER_ATTRIBUTE(sigma2);
	REGISTER_ATTRIBUTE(sigma3);
}



void ThreeDTriaxialEngine::applyCondition ( MetaBody * ncb )
{

	if ( firstRun )
	{
		LOG_INFO ( "First run, will initialize!" );

		if (updateFrictionAngle) setContactProperties(ncb, frictionAngleDegree);
		
		height0 = height; depth0 = depth; width0 = width;

		if (stressControl_1){
		wall_right_activated=true; wall_left_activated=true; //are the right walls for direction 1?
		} else {
		wall_right_activated=false; wall_left_activated=false;
		}

		if (stressControl_2){
		wall_bottom_activated=true; wall_top_activated=true;
		} else {
		wall_bottom_activated=false; wall_top_activated=false;
		}

		if (stressControl_3){
		wall_front_activated=true; wall_back_activated=true; //are the right walls for direction 3?
		} else {
		wall_front_activated=false; wall_back_activated=false;
		}

		//sigma_iso=sigmaLateralConfinement;

		internalCompaction=false;  //is needed to avoid a control for internal compaction by the TriaxialStressController engine

		isTriaxialCompression=false; //is needed to avoid a stress control according the parameter sigma_iso (but according to sigma1, sigma2 and sigma3)

		firstRun=false;
	}

	
	if ( Omega::instance().getCurrentIteration() % testEquilibriumInterval == 0 )
	{
		//updateParameters ( ncb );
		computeStressStrain ( ncb );
		UnbalancedForce=ComputeUnbalancedForce ( ncb );
		LOG_INFO("UnbalancedForce="<< UnbalancedForce);
	}
	

	TriaxialStressController::applyCondition ( ncb ); // this function is called to perform the external stress control

	Real dt = Omega::instance().getTimeStep();

	if(!stressControl_1)  // control in strain if wanted
	{
		if ( currentStrainRate1 != strainRate1 ) currentStrainRate1 += ( strainRate1-currentStrainRate1 ) *0.0003;

		PhysicalParameters* p1=static_cast<PhysicalParameters*> ( Body::byId ( wall_left_id )->physicalParameters.get() );
		p1->se3.position += 0.5*currentStrainRate1*width*translationAxisx*dt;
		p1 = static_cast<PhysicalParameters*> ( Body::byId ( wall_right_id )->physicalParameters.get() );
		p1->se3.position -= 0.5*currentStrainRate1*width*translationAxisx*dt;
	} else {

		if ( currentStrainRate1 != strainRate1 ) currentStrainRate1 += ( strainRate1-currentStrainRate1 ) *0.0003;
		max_vel1 = 0.5*currentStrainRate1*width;
	}

	
	if(!stressControl_2)  // control in strain if wanted
	{
		if ( currentStrainRate2 != strainRate2 ) currentStrainRate2 += ( strainRate2-currentStrainRate2 ) *0.0003;

		PhysicalParameters* p2=static_cast<PhysicalParameters*> ( Body::byId ( wall_bottom_id )->physicalParameters.get() );
		p2->se3.position += 0.5*currentStrainRate2*height*translationAxisy*dt;
		p2 = static_cast<PhysicalParameters*> ( Body::byId ( wall_top_id )->physicalParameters.get() );
		p2->se3.position -= 0.5*currentStrainRate2*height*translationAxisy*dt;
	} else {

		if ( currentStrainRate2 != strainRate2 ) currentStrainRate2 += ( strainRate2-currentStrainRate2 ) *0.0003;
		max_vel2 = 0.5*currentStrainRate2*height;
	}


	if(!stressControl_3)  // control in strain if wanted
	{
		if ( currentStrainRate3 != strainRate3 ) currentStrainRate3 += ( strainRate3-currentStrainRate3 ) *0.0003;

		PhysicalParameters* p3=static_cast<PhysicalParameters*> ( Body::byId ( wall_back_id )->physicalParameters.get() );
		p3->se3.position += 0.5*currentStrainRate3*depth*translationAxisz*dt;
		p3 = static_cast<PhysicalParameters*> ( Body::byId ( wall_front_id )->physicalParameters.get() );
		p3->se3.position -= 0.5*currentStrainRate3*depth*translationAxisz*dt;
	} else {

		if ( currentStrainRate3 != strainRate3 ) currentStrainRate3 += ( strainRate3-currentStrainRate3 ) *0.0003;
		max_vel3 = 0.5*currentStrainRate3*depth;
	}

}

void ThreeDTriaxialEngine::setContactProperties(MetaBody * ncb, Real frictionDegree)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
			
	BodyContainer::iterator bi = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	
	for ( ; bi!=biEnd; ++bi)	
	{	
		shared_ptr<Body> b = *bi;
		if (b->isDynamic)
		YADE_PTR_CAST<BodyMacroParameters> (b->physicalParameters)->frictionAngle = frictionDegree * Mathr::PI/180.0;
	}
		
	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end(); 
	for(  ; ii!=iiEnd ; ++ii ) {
		if (!(*ii)->isReal) continue;
		const shared_ptr<BodyMacroParameters>& sdec1 = YADE_PTR_CAST<BodyMacroParameters>((*bodies)[(body_id_t) ((*ii)->getId1())]->physicalParameters);
		const shared_ptr<BodyMacroParameters>& sdec2 = YADE_PTR_CAST<BodyMacroParameters>((*bodies)[(body_id_t) ((*ii)->getId2())]->physicalParameters);		
		//FIXME - why dynamic_cast fails here?
		//const shared_ptr<ElasticContactInteraction>& contactPhysics = YADE_PTR_CAST<ElasticContactInteraction>((*ii)->interactionPhysics);
		const shared_ptr<ElasticContactInteraction>& contactPhysics = static_pointer_cast<ElasticContactInteraction>((*ii)->interactionPhysics);

		Real fa 	= sdec1->frictionAngle;
		Real fb 	= sdec2->frictionAngle;

		contactPhysics->frictionAngle			= std::min(fa,fb);
		contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle); 
	}
} 

 

YADE_PLUGIN();

