/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include "TriaxialTestWater.hpp"

#include <yade/pkg-dem/ElasticContactLaw.hpp>
#include <yade/pkg-dem/CapillaryCohesiveLaw.hpp>

#include <yade/pkg-dem/MacroMicroElasticRelationshipsWater.hpp>
#include <yade/pkg-dem/BodyMacroParameters.hpp>

#include <yade/pkg-dem/GlobalStiffnessCounter.hpp>
#include <yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>

#include <yade/pkg-dem/AveragePositionRecorder.hpp>
#include <yade/pkg-dem/ForceRecorder.hpp>
#include <yade/pkg-dem/VelocityRecorder.hpp>
#include <yade/pkg-dem/TriaxialStateRecorder.hpp>
#include <yade/pkg-dem/CapillaryStressRecorder.hpp>
#include <yade/pkg-dem/ContactStressRecorder.hpp>
#include <yade/pkg-dem/TriaxialStressController.hpp>
#include <yade/pkg-dem/TriaxialCompressionEngine.hpp>

#include <yade/pkg-common/Box.hpp>
#include <yade/pkg-common/AABB.hpp>
#include <yade/pkg-common/Sphere.hpp>
#include <yade/core/MetaBody.hpp>
#include <yade/pkg-common/SAPCollider.hpp>
#include <yade/pkg-common/PersistentSAPCollider.hpp>
#include <yade/lib-serialization/IOFormatManager.hpp>
#include <yade/core/Interaction.hpp>
#include <yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include <yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include <yade/pkg-common/MetaInteractingGeometry.hpp>

#include <yade/pkg-common/GravityEngine.hpp>
#include <yade/pkg-common/PhysicalActionApplier.hpp>
#include <yade/pkg-common/PhysicalActionDamper.hpp>
#include <yade/pkg-common/CundallNonViscousForceDamping.hpp>
#include <yade/pkg-common/CundallNonViscousMomentumDamping.hpp>

#include <yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include <yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include <yade/core/Body.hpp>
#include <yade/pkg-common/InteractingBox.hpp>
#include <yade/pkg-common/InteractingSphere.hpp>

#include <yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include <yade/pkg-common/PhysicalActionContainerInitializer.hpp>

#include <yade/pkg-common/PhysicalParametersMetaEngine.hpp>

#include <yade/pkg-common/BodyRedirectionVector.hpp>
#include <yade/pkg-common/InteractionVecSet.hpp>
#include <yade/pkg-common/InteractionHashMap.hpp>
#include <yade/pkg-common/PhysicalActionVectorVector.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>

// random
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>



using namespace boost;
using namespace std;


typedef pair<Vector3r, Real> BasicSphere;
//! make a list of spheres non-overlapping sphere
string GenerateCloud(vector<BasicSphere>& sphere_list, Vector3r lowerCorner, Vector3r upperCorner, long number, Real rad_std_dev, Real porosity);


TriaxialTestWater::TriaxialTestWater () : FileGenerator()
{
	lowerCorner 		= Vector3r(0,0,0);
	upperCorner 		= Vector3r(0.0065,0.0065,0.0065);
	thickness 		= 0.00001;
	importFilename 		= "./small.sdec.xyz";
	outputFileName 		= "./TriaxialTestWater.xml";
	//nlayers = 1;
	wall_top 		= true;
	wall_bottom 		= true;
	wall_1			= true;
	wall_2			= true;
	wall_3			= true;
	wall_4			= true;
	wall_top_wire 		= true;
	wall_bottom_wire	= true;
	wall_1_wire		= true;
	wall_2_wire		= true;
	wall_3_wire		= true;
	wall_4_wire		= true;
	spheresColor		= Vector3r(0.8,0.3,0.3);
	spheresRandomColor	= false;
	//recordBottomForce	= true;
	//forceRecordFile		= "./force";
	//recordAveragePositions	= true;
	//positionRecordFile	= "./position";
	recordIntervalIter	= 100;
	//velocityRecordFile 	= "./velocities";
	wallStressRecordFile	= "./wallStress";
	capillaryStressRecordFile	= "./capillaryStress";
	contactStressRecordFile	= "./contactStress";
	rotationBlocked = false;
	
//	boxWalls 		= false;
	boxWalls 		= true;
	internalCompaction	=false;

//	bigBall 		= true;
//	bigBall 		= false;
	bigBallRadius		= 0.075;
	bigBallPoissonRatio 	= 0.3;
	bigBallYoungModulus 	= 10000000.0;
	bigBallFrictDeg 	= 60;
	bigBallDensity		= 7800;
	bigBallDropTimeSeconds	= 30;
	bigBallDropHeight 	= 3.04776;
	
	dampingForce = 0.2;
	dampingMomentum = 0.2;
	
	defaultDt = 0.000001;
	timeStepUpdateInterval = 50;
	timeStepOutputInterval = 50;
	wallStiffnessUpdateInterval = 10;
	numberOfGrains = 1000;
	max_vel = 1;
	strainRate = 0.1;
	StabilityCriterion = 0.01;
	autoCompressionActivation = true;
	maxMultiplier = 1.01;
	finalMaxMultiplier = 1.001;
	
	sphereYoungModulus  = 150000000.0;
	spherePoissonRatio  = 0.2;
	sphereFrictionDeg   = 18.0;
	density		    = 2600;
	Rdispersion 	    = 0.4;
	
	boxYoungModulus   = 150000000.0;
	boxPoissonRatio  = 0.2;
	boxFrictionDeg   = 0;
	gravity 	= Vector3r(0,-9.81,0);
	
	sigma_iso = 50000;
	water = true;
	CapillaryPressure = 0;
	
}


TriaxialTestWater::~TriaxialTestWater ()
{

}


void TriaxialTestWater::registerAttributes()
{
	REGISTER_ATTRIBUTE(lowerCorner);
	REGISTER_ATTRIBUTE(upperCorner);
	REGISTER_ATTRIBUTE(thickness);
	REGISTER_ATTRIBUTE(importFilename);
	REGISTER_ATTRIBUTE(internalCompaction);
	REGISTER_ATTRIBUTE(maxMultiplier);
	REGISTER_ATTRIBUTE(finalMaxMultiplier);

	REGISTER_ATTRIBUTE(sphereYoungModulus);
	REGISTER_ATTRIBUTE(spherePoissonRatio);
	REGISTER_ATTRIBUTE(sphereFrictionDeg);

	REGISTER_ATTRIBUTE(boxYoungModulus);
	REGISTER_ATTRIBUTE(boxPoissonRatio);
	REGISTER_ATTRIBUTE(boxFrictionDeg);

	REGISTER_ATTRIBUTE(density);
	REGISTER_ATTRIBUTE(Rdispersion);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	REGISTER_ATTRIBUTE(rotationBlocked);
	REGISTER_ATTRIBUTE(defaultDt);
	REGISTER_ATTRIBUTE(timeStepUpdateInterval);
	REGISTER_ATTRIBUTE(timeStepOutputInterval);
	REGISTER_ATTRIBUTE(wallStiffnessUpdateInterval);
	REGISTER_ATTRIBUTE(numberOfGrains);
	REGISTER_ATTRIBUTE(max_vel)
	
	REGISTER_ATTRIBUTE(strainRate);
	REGISTER_ATTRIBUTE(StabilityCriterion);
	REGISTER_ATTRIBUTE(autoCompressionActivation);

	//REGISTER_ATTRIBUTE(recordBottomForce);
	//REGISTER_ATTRIBUTE(forceRecordFile);
// 	REGISTER_ATTRIBUTE(recordAveragePositions);
	//REGISTER_ATTRIBUTE(positionRecordFile);
	//REGISTER_ATTRIBUTE(velocityRecordFile)
	REGISTER_ATTRIBUTE(recordIntervalIter);
	REGISTER_ATTRIBUTE(wallStressRecordFile);
	REGISTER_ATTRIBUTE(capillaryStressRecordFile);
	REGISTER_ATTRIBUTE(contactStressRecordFile);

	REGISTER_ATTRIBUTE(sigma_iso);
	REGISTER_ATTRIBUTE(water);
	REGISTER_ATTRIBUTE(CapillaryPressure);

}


bool TriaxialTestWater::generate()
{
	//unsigned int startId=boost::numeric::bounds<unsigned int>::highest(), endId=0; // record forces from group 2
	
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

	rootBody->persistentInteractions = 
	shared_ptr<InteractionContainer>(new InteractionHashMap);
	rootBody->transientInteractions		=
	shared_ptr<InteractionContainer>(new InteractionHashMap);
	rootBody->physicalActions		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

	shared_ptr<Body> body;
	
	vector<BasicSphere> sphere_list;
	message = GenerateCloud(sphere_list, lowerCorner, upperCorner,
	numberOfGrains, Rdispersion, 0.75);
	
	vector<BasicSphere>::iterator it = sphere_list.begin();
	vector<BasicSphere>::iterator it_end = sphere_list.end();
			
	for (;it!=it_end; ++it)
	{
		//cerr << "sphere (" << it->first << " " << it->second << endl;
		createSphere(body,it->first,it->second,false,true);
		rootBody->bodies->insert(body);
	}
	

//	forcerec->bigBallReleaseTime = 0;

	if(boxWalls)
	{
	// bottom box
	 	Vector3r center		= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						lowerCorner[1]-thickness/2.0,
	 						(lowerCorner[2]+upperCorner[2])/2);
	 	Vector3r halfSize	= Vector3r(
	 						fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
							thickness/2.0,
	 						fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
		createBox(body,center,halfSize,wall_bottom_wire);
	 	if(wall_bottom) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_bottom_id
			= body->getId();
//			forcerec->startId = body->getId();
//			forcerec->endId   = body->getId();
			//triaxialStateRecorder->wall_bottom_id = body->getId();
			capillaryStressRecorder->wall_bottom_id = body->getId();
			contactStressRecorder->wall_bottom_id = body->getId();
			}
			
	// top box
	 	center			= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						upperCorner[1]+thickness/2.0,
	 						(lowerCorner[2]+upperCorner[2])/2);
	 	halfSize		= Vector3r(
	 						fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						thickness/2.0,
	 						fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
		createBox(body,center,halfSize,wall_top_wire);
	 	if(wall_top) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_top_id = body->getId();
			//triaxialStateRecorder->wall_top_id = body->getId();
			capillaryStressRecorder->wall_top_id = body->getId();
			contactStressRecorder->wall_top_id = body->getId();
			}
	// box 1
	
	 	center			= Vector3r(
	 						lowerCorner[0]-thickness/2.0,
	 						(lowerCorner[1]+upperCorner[1])/2,
	 						(lowerCorner[2]+upperCorner[2])/2);
		halfSize		= Vector3r(
							thickness/2.0,
	 						fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
		createBox(body,center,halfSize,wall_1_wire);
	 	if(wall_1) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_left_id = body->getId();
			//triaxialStateRecorder->wall_left_id = body->getId();
			capillaryStressRecorder->wall_left_id = body->getId();
			contactStressRecorder->wall_left_id = body->getId();
			}
	// box 2
	 	center			= Vector3r(
	 						upperCorner[0]+thickness/2.0,
	 						(lowerCorner[1]+upperCorner[1])/2,
							(lowerCorner[2]+upperCorner[2])/2);
	 	halfSize		= Vector3r(
	 						thickness/2.0,
	 						fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	 	
		createBox(body,center,halfSize,wall_2_wire);
	 	if(wall_2) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_right_id = body->getId();
			//triaxialStateRecorder->wall_right_id = body->getId();
			capillaryStressRecorder->wall_right_id = body->getId();
			contactStressRecorder->wall_right_id = body->getId();
			}
	// box 3
	 	center			= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						(lowerCorner[1]+upperCorner[1])/2,
	 						lowerCorner[2]-thickness/2.0);
	 	halfSize		= Vector3r(
	 						fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						thickness/2.0);
		createBox(body,center,halfSize,wall_3_wire);
	 	if(wall_3) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_back_id = body->getId();
			//triaxialStateRecorder->wall_back_id = body->getId();
			capillaryStressRecorder->wall_back_id = body->getId();
			contactStressRecorder->wall_back_id = body->getId();
			}
	
	// box 4
	 	center			= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						(lowerCorner[1]+upperCorner[1])/2,
	 						upperCorner[2]+thickness/2.0);
	 	halfSize		= Vector3r(
	 						fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						thickness/2.0);
		createBox(body,center,halfSize,wall_3_wire);
	 	if(wall_4) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_front_id = body->getId();
			//triaxialStateRecorder->wall_front_id = body->getId();
			capillaryStressRecorder->wall_front_id = body->getId();
			contactStressRecorder->wall_front_id = body->getId();
			}
			 
	}
	
	message=std::string("ATTN: this test will not work without data file for capillary law\n\n")+message;
	return true;
//  	return "Generated a sample inside box of dimensions: (" 
//  		+ lexical_cast<string>(lowerCorner[0]) + "," 
//  		+ lexical_cast<string>(lowerCorner[1]) + "," 
//  		+ lexical_cast<string>(lowerCorner[2]) + ") and (" 
//  		+ lexical_cast<string>(upperCorner[0]) + "," 
//  		+ lexical_cast<string>(upperCorner[1]) + "," 
//  		+ lexical_cast<string>(upperCorner[2]) + ").";

}


void TriaxialTestWater::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius, bool big, bool dynamic )
{
	body = shared_ptr<Body>(new Body(0,2));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	body->isDynamic			= dynamic;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*(big ? bigBallDensity : density);
	
	physics->inertia		= Vector3r( 	2.0/5.0*physics->mass*radius*radius,
							2.0/5.0*physics->mass*radius*radius,
							2.0/5.0*physics->mass*radius*radius);
	physics->se3			= Se3r(position,q);
	physics->young			= big ? bigBallYoungModulus : sphereYoungModulus;
	physics->poisson		= big ? bigBallPoissonRatio : spherePoissonRatio;
	physics->frictionAngle		= (big ? bigBallFrictDeg : sphereFrictionDeg ) * Mathr::PI/180.0;

	if((!big) && (!dynamic) && (!boxWalls))
	{
		physics->young			= boxYoungModulus;
		physics->poisson		= boxPoissonRatio;
		physics->frictionAngle		= boxFrictionDeg * Mathr::PI/180.0;
	}
	
	aabb->diffuseColor		= Vector3r(0,1,0);


	gSphere->radius			= radius;
	gSphere->diffuseColor		= spheresColor;
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(Mathf::UnitRandom(),Mathf::UnitRandom(),Mathf::UnitRandom());

	body->interactingGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}


void TriaxialTestWater::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents, bool wire)
{
	body = shared_ptr<Body>(new Body(0,2));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 0; 
	//physics->mass			= extents[0]*extents[1]*extents[2]*density*2; 
	physics->inertia		= Vector3r(
							  physics->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
							, physics->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
							, physics->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3
						);
//	physics->mass			= 0;
//	physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);

	physics->young			= boxYoungModulus;
	physics->poisson		= boxPoissonRatio;
	physics->frictionAngle		= boxFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3r(1,1,1);
	gBox->wire			= wire;
	gBox->visible			= true;
	gBox->shadowCaster		= false;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3r(1,1,1);

	body->boundingVolume		= aabb;
	body->interactingGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

void TriaxialTestWater::createActors(shared_ptr<MetaBody>& rootBody)
{


// recording global stress
	triaxialStateRecorder = shared_ptr<TriaxialStateRecorder>(new
	TriaxialStateRecorder);
	triaxialStateRecorder-> outputFile 	= wallStressRecordFile;
	triaxialStateRecorder-> interval 		= recordIntervalIter;
	//triaxialStateRecorder-> thickness 		= thickness;
	
// recording capillary stress
	capillaryStressRecorder = shared_ptr<CapillaryStressRecorder>(new
	CapillaryStressRecorder);
	capillaryStressRecorder -> outputFile 	= capillaryStressRecordFile;
 	capillaryStressRecorder -> interval 	= recordIntervalIter;
 	capillaryStressRecorder-> thickness 	= thickness;
//  	capillaryStressRecorder-> upperCorner 	= upperCorner;
//  	capillaryStressRecorder-> lowerCorner 	= lowerCorner;

//recording contact stress
	contactStressRecorder = shared_ptr<ContactStressRecorder>(new
	ContactStressRecorder);
	contactStressRecorder -> outputFile 	= contactStressRecordFile;
 	contactStressRecorder -> interval 	= recordIntervalIter;
 	contactStressRecorder-> thickness 	= thickness;
 	contactStressRecorder-> upperCorner 	= upperCorner;
 	contactStressRecorder-> lowerCorner 	= lowerCorner;

	shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
	physicalActionInitializer->physicalActionNames.push_back("Force");
	physicalActionInitializer->physicalActionNames.push_back("Momentum");
	//physicalActionInitializer->physicalActionNames.push_back("StiffnessMatrix");
	physicalActionInitializer->physicalActionNames.push_back("GlobalStiffness");
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere2InteractingSphere4SpheresContactGeometryWater");
	interactionGeometryDispatcher->add("InteractingBox2InteractingSphere4SpheresContactGeometry");

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add("MacroMicroElasticRelationshipsWater");
		
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere2AABBwater");
	boundingVolumeDispatcher->add("InteractingBox2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry2AABB");

	

		
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
	actionDampingDispatcher->add(actionForceDamping);
	actionDampingDispatcher->add(actionMomentumDamping);
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("NewtonsForceLaw");
	applyActionDispatcher->add("NewtonsMomentumLaw");
		
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("LeapFrogPositionIntegrator");
	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("LeapFrogOrientationIntegrator");


	shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper(new
	GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->sdecGroupMask = 2;
	globalStiffnessTimeStepper->defaultDt = defaultDt;
	globalStiffnessTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	
	shared_ptr<ElasticContactLaw> elasticContactLaw(new ElasticContactLaw);
	elasticContactLaw->sdecGroupMask = 2;
	
	// capillary
	shared_ptr<CapillaryCohesiveLaw> capillaryCohesiveLaw(new
	CapillaryCohesiveLaw); 
	capillaryCohesiveLaw->sdecGroupMask = 2;	
	capillaryCohesiveLaw->CapillaryPressure = CapillaryPressure;
	
	shared_ptr<GlobalStiffnessCounter> globalStiffnessCounter(new
	GlobalStiffnessCounter);
	globalStiffnessCounter->sdecGroupMask = 2;
	globalStiffnessCounter->interval = timeStepUpdateInterval;
	
	
	// moving walls to regulate the stress applied + compress when the
	//packing is dense an stable
	//cerr << "triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);" << std::endl;
	triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);
	triaxialcompressionEngine-> stiffnessUpdateInterval = wallStiffnessUpdateInterval;// = stiffness update interval
	//triaxialcompressionEngine->sigma_iso = sigma_iso;
	triaxialcompressionEngine-> sigmaIsoCompaction = sigma_iso;
	triaxialcompressionEngine-> sigmaLateralConfinement = sigma_iso;
	triaxialcompressionEngine->max_vel = max_vel;
	triaxialcompressionEngine->thickness = thickness;
	triaxialcompressionEngine->strainRate = strainRate;
	triaxialcompressionEngine->StabilityCriterion = StabilityCriterion;
	triaxialcompressionEngine->autoCompressionActivation = autoCompressionActivation;
	triaxialcompressionEngine->internalCompaction = internalCompaction;
	triaxialcompressionEngine->maxMultiplier = maxMultiplier;
	triaxialcompressionEngine->finalMaxMultiplier = finalMaxMultiplier;
	//cerr <<"fin section triaxialcompressionEngine = shared_ptr"<< std::endl;
	
	
	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new
	PhysicalActionContainerReseter));
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new
	PersistentSAPCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(elasticContactLaw);
	
	// capillary
	if(water)
	{
		rootBody->engines.push_back(capillaryCohesiveLaw);
		rootBody->engines.push_back(capillaryStressRecorder);
	}
	
	rootBody->engines.push_back(globalStiffnessCounter);
	rootBody->engines.push_back(globalStiffnessTimeStepper);
	//rootBody->engines.push_back(triaxialstressController);
	rootBody->engines.push_back(triaxialcompressionEngine);
	//rootBody->engines.push_back(forcerec);	
	rootBody->engines.push_back(triaxialStateRecorder);
	rootBody->engines.push_back(contactStressRecorder);
	rootBody->engines.push_back(actionDampingDispatcher);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);
	//if(!rotationBlocked)
	//	rootBody->engines.push_back(orientationIntegrator);
		
	//rootBody->engines.push_back(triaxialcompressionEngine);
		
	//rootBody->engines.push_back(averagePositionRecorder);
	//rootBody->engines.push_back(velocityRecorder);
	//rootBody->engines.push_back(forcerec);
	
	//rootBody->engines.push_back(triaxialStateRecorder);
	//rootBody->engines.push_back(contactStressRecorder);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(physicalActionInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	
}


void TriaxialTestWater::positionRootBody(shared_ptr<MetaBody>& rootBody)
{
	rootBody->isDynamic		= false;

	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3			= Se3r(Vector3r(0,0,0),q);
	physics->mass			= 0;
	physics->velocity		= Vector3r::ZERO;
	physics->acceleration		= Vector3r::ZERO;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	
	set->diffuseColor		= Vector3r(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);
	
	rootBody->interactingGeometry	= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume	= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->physicalParameters 	= physics;
	
}



string GenerateCloud(vector<BasicSphere>& sphere_list, Vector3r lowerCorner, Vector3r upperCorner, long number, Real rad_std_dev, Real porosity)
{
	typedef boost::minstd_rand StdGenerator;
	static StdGenerator generator;
	static boost::variate_generator<StdGenerator&, boost::uniform_real<> >
			random1(generator, boost::uniform_real<>(0,1));
        //         static boost::variate_generator<StdGenerator&, boost::normal_distribution<> >
        //         randomN(generator, boost::normal_distribution<>(aggregateMeanRadius,aggregateSigmaRadius));

	sphere_list.clear();
	long tries = 1000; //nb of tries for positionning the next sphere
	Vector3r dimensions = upperCorner - lowerCorner;
		
	Real mean_radius = std::pow(dimensions.X()*dimensions.Y()*dimensions.Z()*(1-porosity)/(3.1416*1.3333*number),0.333333);
        //cerr << mean_radius;
        Real Rmin=mean_radius, Rmax=mean_radius;

	std::cerr << "generating aggregates ... ";
	
	long t, i;
	for (i=0; i<number; ++i) {
		BasicSphere s;
		for (t=0; t<tries; ++t) {
			s.second = (random1()-0.5)*rad_std_dev*mean_radius+mean_radius;
			s.first.X() = lowerCorner.X()+s.second+(dimensions.X()-2*s.second)*random1();
			s.first.Y() = lowerCorner.Y()+s.second+(dimensions.Y()-2*s.second)*random1();
			s.first.Z() = lowerCorner.Z()+s.second+(dimensions.Z()-2*s.second)*random1();
			bool overlap=false;
			for (long j=0; (j<i && !overlap); j++)
				if ( pow(sphere_list[j].second+s.second, 2) > (sphere_list[j].first-s.first).SquaredLength()) overlap=true;
			if (!overlap)
			{
				sphere_list.push_back(s);
				break;
			}	
			
			Rmin = std::min(Rmin,s.second);
			Rmax = std::max(Rmax,s.second);
				
		}
		if (t==tries) return "More than " + lexical_cast<string>(tries) +
					" tries while generating sphere number " +
					lexical_cast<string>(i+1) + "/" + lexical_cast<string>(number) + ".";
	}
	return "Generated a sample with " + lexical_cast<string>(number) + "spheres inside box of dimensions: (" 
			+ lexical_cast<string>(dimensions[0]) + "," 
			+ lexical_cast<string>(dimensions[1]) + "," 
			+ lexical_cast<string>(dimensions[2]) + ")."
			+ "  mean radius=" + lexical_cast<string>(mean_radius) +
			+ "  Rmin =" + lexical_cast<string>(Rmin) +
			+ "  Rmax =" + lexical_cast<string>(Rmax) + ".";
}



YADE_PLUGIN();
