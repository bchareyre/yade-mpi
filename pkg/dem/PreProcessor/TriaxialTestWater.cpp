/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr                                            *
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/




#include "TriaxialTestWater.hpp"

#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include <yade/pkg-dem/CapillaryCohesiveLaw.hpp>
// #include<yade/pkg-dem/SimpleElasticRelationships.hpp>
#include<yade/pkg-dem/SimpleElasticRelationshipsWater.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>

#include<yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>

#include<yade/pkg-dem/PositionOrientationRecorder.hpp>
#include<yade/pkg-dem/AveragePositionRecorder.hpp>
#include<yade/pkg-dem/ForceRecorder.hpp>
#include<yade/pkg-dem/VelocityRecorder.hpp>
#include<yade/pkg-dem/TriaxialStressController.hpp>
#include<yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include <yade/pkg-dem/TriaxialStateRecorder.hpp>
#include <yade/pkg-dem/CapillaryStressRecorder.hpp>
#include <yade/pkg-dem/ContactStressRecorder.hpp>

#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/InteractingSphere2AABB.hpp>

#include<yade/pkg-common/GravityEngines.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-dem/NewtonsDampedLaw.hpp>

#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-dem/InteractingSphere2InteractingSphere4SpheresContactGeometry.hpp>

#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>

#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>

#include<yade/extra/Shop.hpp>

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
//! generate a list of non-overlapping spheres
string GenerateCloud(vector<BasicSphere>& sphere_list, Vector3r lowerCorner, Vector3r upperCorner, long number, Real rad_std_dev, Real porosity);


TriaxialTestWater::TriaxialTestWater () : FileGenerator()
{
	lowerCorner 		= Vector3r(0,0,0);
	upperCorner 		= Vector3r(0.001,0.001,0.001);
	thickness 		= 0.00001;
	importFilename 		= ""; // "./small.sdec.xyz";
	Key			="";
	outputFileName 		= "./TriaxialTestWater"+Key+".xml";
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
	recordIntervalIter	= 20;
	saveAnimationSnapshots = false;
	AnimationSnapshotsBaseName = "./snapshots_"+Key+"/snap";
	WallStressRecordFile = "./WallStresses"+Key;
	capillaryStressRecordFile	= "./CapillaryStresses"+Key;
	contactStressRecordFile	= "./ContactStresses"+Key;

	rotationBlocked = false;
	//	boxWalls 		= false;
	boxWalls 		= true;
	internalCompaction	=false;
	
	dampingForce = 0.2;
	dampingMomentum = 0.2;
	defaultDt = 0.0001;
	
	timeStepUpdateInterval = 50;
	timeStepOutputInterval = 50;
	wallStiffnessUpdateInterval = 10;
	radiusControlInterval = 10;
	numberOfGrains = 400;
	Rdispersion = 0.667;
	strainRate = 10;
	maxWallVelocity=0.1;
	StabilityCriterion = 0.01;
	autoCompressionActivation = true;
	maxMultiplier = 1.01;
	finalMaxMultiplier = 1.001;
	
	sphereYoungModulus  = 5000000.0;
	spherePoissonRatio  = 0.5;	
	sphereFrictionDeg = 30.0;
	compactionFrictionDeg   = sphereFrictionDeg;
	density			= 2600;
	
	boxYoungModulus   = 5000000.0;
	boxPoissonRatio  = 0.2;
	boxFrictionDeg   = 0.f;
	gravity 	= Vector3r(0,-9.81,0);
	
	sigmaIsoCompaction = 5000;
	sigmaLateralConfinement=sigmaIsoCompaction;

	water = true;
	CapillaryPressure = 0;
 	fusionDetection = 1;
	binaryFusion = 1;

	wallOversizeFactor=1.3;
	
//	wall_top_id =0;
// 	wall_bottom_id =0;
// 	wall_left_id =0;
// 	all_right_id =0;
// 	wall_front_id =0;
// 	wall_back_id =0;
}


TriaxialTestWater::~TriaxialTestWater ()
{

}


void TriaxialTestWater::registerAttributes()
{
	FileGenerator::registerAttributes();
	REGISTER_ATTRIBUTE(lowerCorner);
	REGISTER_ATTRIBUTE(upperCorner);
	REGISTER_ATTRIBUTE(thickness);
	REGISTER_ATTRIBUTE(importFilename);
	REGISTER_ATTRIBUTE(outputFileName);
	//REGISTER_ATTRIBUTE(nlayers);
	//REGISTER_ATTRIBUTE(boxWalls);
	REGISTER_ATTRIBUTE(internalCompaction);
	REGISTER_ATTRIBUTE(maxMultiplier);
	REGISTER_ATTRIBUTE(finalMaxMultiplier);

	REGISTER_ATTRIBUTE(sphereYoungModulus);
	REGISTER_ATTRIBUTE(spherePoissonRatio);
	REGISTER_ATTRIBUTE(sphereFrictionDeg);
	REGISTER_ATTRIBUTE(compactionFrictionDeg);
	REGISTER_ATTRIBUTE(boxYoungModulus);
	REGISTER_ATTRIBUTE(boxPoissonRatio);
	REGISTER_ATTRIBUTE(boxFrictionDeg);

	REGISTER_ATTRIBUTE(density);
	REGISTER_ATTRIBUTE(defaultDt);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	REGISTER_ATTRIBUTE(rotationBlocked);
	REGISTER_ATTRIBUTE(timeStepUpdateInterval);
	REGISTER_ATTRIBUTE(timeStepOutputInterval);
	REGISTER_ATTRIBUTE(wallStiffnessUpdateInterval);
	REGISTER_ATTRIBUTE(radiusControlInterval);
	REGISTER_ATTRIBUTE(numberOfGrains);
	REGISTER_ATTRIBUTE(Rdispersion);
	REGISTER_ATTRIBUTE(strainRate);
	REGISTER_ATTRIBUTE(maxWallVelocity);
	REGISTER_ATTRIBUTE(StabilityCriterion);
	REGISTER_ATTRIBUTE(autoCompressionActivation);
//	REGISTER_ATTRIBUTE(wall_top);
//	REGISTER_ATTRIBUTE(wall_bottom);
//	REGISTER_ATTRIBUTE(wall_1);
//	REGISTER_ATTRIBUTE(wall_2);
//	REGISTER_ATTRIBUTE(wall_3);
//	REGISTER_ATTRIBUTE(wall_4);
//	REGISTER_ATTRIBUTE(wall_top_wire);
//	REGISTER_ATTRIBUTE(wall_bottom_wire);
//	REGISTER_ATTRIBUTE(wall_1_wire);
//	REGISTER_ATTRIBUTE(wall_2_wire);
//	REGISTER_ATTRIBUTE(wall_3_wire);
//	REGISTER_ATTRIBUTE(wall_4_wire);
//	REGISTER_ATTRIBUTE(spheresColor);
//	REGISTER_ATTRIBUTE(spheresRandomColor);
	REGISTER_ATTRIBUTE(recordIntervalIter);
	REGISTER_ATTRIBUTE(saveAnimationSnapshots);
	REGISTER_ATTRIBUTE(AnimationSnapshotsBaseName);
	REGISTER_ATTRIBUTE(WallStressRecordFile);
// 	REGISTER_ATTRIBUTE(capillaryStressRecordFile);
	REGISTER_ATTRIBUTE(contactStressRecordFile);

	REGISTER_ATTRIBUTE(wallOversizeFactor);

//	REGISTER_ATTRIBUTE(gravity);
	
	//REGISTER_ATTRIBUTE(bigBall);
	//REGISTER_ATTRIBUTE(bigBallRadius);
	//REGISTER_ATTRIBUTE(bigBallDensity);
	//REGISTER_ATTRIBUTE(bigBallDropTimeSeconds);
	//REGISTER_ATTRIBUTE(bigBallFrictDeg);
	//REGISTER_ATTRIBUTE(bigBallYoungModulus);
	//REGISTER_ATTRIBUTE(bigBallPoissonRatio);
	//REGISTER_ATTRIBUTE(bigBallDropHeight);
	//REGISTER_ATTRIBUTE(sigma_iso);
	REGISTER_ATTRIBUTE(sigmaIsoCompaction);
	REGISTER_ATTRIBUTE(sigmaLateralConfinement);
	REGISTER_ATTRIBUTE(Key);
	
	REGISTER_ATTRIBUTE(water);
	REGISTER_ATTRIBUTE(CapillaryPressure);
	REGISTER_ATTRIBUTE(fusionDetection);
	REGISTER_ATTRIBUTE(binaryFusion);

}


bool TriaxialTestWater::generate()
{
//	unsigned int startId=boost::numeric::bounds<unsigned int>::highest(), endId=0; // record forces from group 2
	message="";
	
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

	//rootBody->transientInteractions		= shared_ptr<InteractionContainer>(new InteractionHashMap);

	shared_ptr<Body> body;
	
	
	
	if(boxWalls)
	{
	// bottom box
	 	Vector3r center		= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						lowerCorner[1]-thickness/2.0,
	 						(lowerCorner[2]+upperCorner[2])/2);
	 	Vector3r halfSize	= Vector3r(
	 						wallOversizeFactor*fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
							thickness/2.0,
	 						wallOversizeFactor*fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
		createBox(body,center,halfSize,wall_bottom_wire);
	 	if(wall_bottom) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_bottom_id = body->getId();
			//triaxialStateRecorder->wall_bottom_id = body->getId();
			}
		//forcerec->id = body->getId();
	
	// top box
	 	center			= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						upperCorner[1]+thickness/2.0,
	 						(lowerCorner[2]+upperCorner[2])/2);
	 	halfSize		= Vector3r(
	 						wallOversizeFactor*fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						thickness/2.0,
	 						wallOversizeFactor*fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
		createBox(body,center,halfSize,wall_top_wire);
	 	if(wall_top) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_top_id = body->getId();
			//triaxialStateRecorder->wall_top_id = body->getId();
			}
	// box 1
	
	 	center			= Vector3r(
	 						lowerCorner[0]-thickness/2.0,
	 						(lowerCorner[1]+upperCorner[1])/2,
	 						(lowerCorner[2]+upperCorner[2])/2);
		halfSize		= Vector3r(
							thickness/2.0,
	 						wallOversizeFactor*fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						wallOversizeFactor*fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
		createBox(body,center,halfSize,wall_1_wire);
	 	if(wall_1) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_left_id = body->getId();
			//triaxialStateRecorder->wall_left_id = body->getId();
			}
	// box 2
	 	center			= Vector3r(
	 						upperCorner[0]+thickness/2.0,
	 						(lowerCorner[1]+upperCorner[1])/2,
							(lowerCorner[2]+upperCorner[2])/2);
	 	halfSize		= Vector3r(
	 						thickness/2.0,
	 						wallOversizeFactor*fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						wallOversizeFactor*fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	 	
		createBox(body,center,halfSize,wall_2_wire);
	 	if(wall_2) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_right_id = body->getId();
			//triaxialStateRecorder->wall_right_id = body->getId();
			}
	// box 3
	 	center			= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						(lowerCorner[1]+upperCorner[1])/2,
	 						lowerCorner[2]-thickness/2.0);
	 	halfSize		= Vector3r(
	 						wallOversizeFactor*fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						wallOversizeFactor*fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						thickness/2.0);
		createBox(body,center,halfSize,wall_3_wire);
	 	if(wall_3) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_back_id = body->getId();
			//triaxialStateRecorder->wall_back_id = body->getId();
			}
	
	// box 4
	 	center			= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						(lowerCorner[1]+upperCorner[1])/2,
	 						upperCorner[2]+thickness/2.0);
	 	halfSize		= Vector3r(
	 						wallOversizeFactor*fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						wallOversizeFactor*fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						thickness/2.0);
		createBox(body,center,halfSize,wall_3_wire);
	 	if(wall_4) {
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_front_id = body->getId();
			//triaxialStateRecorder->wall_front_id = body->getId();
			}
			 
	}
	
	
	vector<BasicSphere> sphere_list;
	if(importFilename!="") sphere_list=Shop::loadSpheresFromFile(importFilename,lowerCorner,upperCorner);
	else message+=GenerateCloud(sphere_list, lowerCorner, upperCorner, numberOfGrains, Rdispersion, 0.75);
	
	vector<BasicSphere>::iterator it = sphere_list.begin();
	vector<BasicSphere>::iterator it_end = sphere_list.end();
			
	for (;it!=it_end; ++it)
	{
		cerr << "sphere (" << it->first << " " << it->second << ")"<<endl;
		createSphere(body,it->first,it->second,false,true);
		rootBody->bodies->insert(body);
	}	
	
	
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
	body = shared_ptr<Body>(new Body(body_id_t(0),2));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	body->isDynamic			= dynamic;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	
	physics->inertia		= Vector3r( 	2.0/5.0*physics->mass*radius*radius,
							2.0/5.0*physics->mass*radius*radius,
							2.0/5.0*physics->mass*radius*radius);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= compactionFrictionDeg * Mathr::PI/180.0;

// 	if((!big) && (!dynamic) && (!boxWalls))
// 	{
// 		physics->young			= boxYoungModulus;
// 		physics->poisson		= boxPoissonRatio;
// 		physics->frictionAngle		= boxFrictionDeg * Mathr::PI/180.0;
// 	}
	
	aabb->diffuseColor		= Vector3r(0,1,0);


	gSphere->radius			= radius;
	gSphere->diffuseColor		= spheresColor;
	gSphere->wire			= false;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());

	body->interactingGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}


void TriaxialTestWater::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents, bool wire)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),2));
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
	
	Real distanceFactor = 1.3;//Create potential interactions as soon as the distance is less than factor*(rad1+rad2) 
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	
	shared_ptr<InteractingSphere2InteractingSphere4SpheresContactGeometry> iS2IS4SContactGeometry(new InteractingSphere2InteractingSphere4SpheresContactGeometry);
	iS2IS4SContactGeometry->interactionDetectionFactor = distanceFactor;//Detect potential distant interaction (meniscii)
	
	interactionGeometryDispatcher->add(iS2IS4SContactGeometry);
	interactionGeometryDispatcher->add("InteractingBox2InteractingSphere4SpheresContactGeometry");
	
	

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
//	interactionPhysicsDispatcher->add("SimpleElasticRelationships");
// Unhandled exception: St13runtime_error : Class `SimpleElasticRelationships' could not be cast to required 2D EngineUnit	
	/// OLD
	//interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationshipsWater");
	/// NEW
	shared_ptr<InteractionPhysicsEngineUnit> ss(new SimpleElasticRelationshipsWater);
	interactionPhysicsDispatcher->add(ss);
	
		
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	
	shared_ptr<InteractingSphere2AABB> interactingSphere2AABB(new InteractingSphere2AABB);
	interactingSphere2AABB->aabbEnlargeFactor = distanceFactor;//Detect potential distant interaction (meniscii)
	
	boundingVolumeDispatcher->add(interactingSphere2AABB);
	boundingVolumeDispatcher->add("InteractingBox2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry2AABB");

	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
// 	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
// 	actionForceDamping->damping = dampingForce;
// 	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
// 	actionMomentumDamping->damping = dampingMomentum;
// 	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
// 	actionDampingDispatcher->add(actionForceDamping);
// 	actionDampingDispatcher->add(actionMomentumDamping);
// 	
// 	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
// 	applyActionDispatcher->add("NewtonsForceLaw");
// 	applyActionDispatcher->add("NewtonsMomentumLaw");
// 		
// 	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
// 	positionIntegrator->add("LeapFrogPositionIntegrator");
// 	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
// 	orientationIntegrator->add("LeapFrogOrientationIntegrator");

	//shared_ptr<ElasticCriterionTimeStepper> sdecTimeStepper(new ElasticCriterionTimeStepper);
	//sdecTimeStepper->sdecGroupMask = 2;
	//sdecTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	
	//shared_ptr<StiffnessMatrixTimeStepper> stiffnessMatrixTimeStepper(new StiffnessMatrixTimeStepper);
	//stiffnessMatrixTimeStepper->sdecGroupMask = 2;
	//stiffnessMatrixTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	
	shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper(new GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->sdecGroupMask = 2;
	globalStiffnessTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	globalStiffnessTimeStepper->defaultDt = defaultDt;
	
	shared_ptr<ElasticContactLaw> elasticContactLaw(new ElasticContactLaw);
	elasticContactLaw->sdecGroupMask = 2;

	// capillary
	shared_ptr<CapillaryCohesiveLaw> capillaryCohesiveLaw(new CapillaryCohesiveLaw); 
	capillaryCohesiveLaw->sdecGroupMask = 2;	
	capillaryCohesiveLaw->CapillaryPressure = CapillaryPressure;
// 	capillaryCohesiveLaw->fusionDetection = fusionDetection;
// 	capillaryCohesiveLaw->binaryFusion = binaryFusion;

	//shared_ptr<StiffnessCounter> stiffnesscounter(new StiffnessCounter);
	//stiffnesscounter->sdecGroupMask = 2;
	//stiffnesscounter->interval = timeStepUpdateInterval;
	
	
	// moving walls to regulate the stress applied + compress when the packing is dense an stable
	//cerr << "triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);" << std::endl;
	triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);
	triaxialcompressionEngine-> stiffnessUpdateInterval = wallStiffnessUpdateInterval;// = stiffness update interval
	triaxialcompressionEngine-> radiusControlInterval = radiusControlInterval;// = stiffness update interval
	triaxialcompressionEngine-> sigmaIsoCompaction = sigmaIsoCompaction;
	triaxialcompressionEngine-> sigmaLateralConfinement = sigmaLateralConfinement;
	triaxialcompressionEngine->max_vel = maxWallVelocity;
	triaxialcompressionEngine-> thickness = thickness;
	triaxialcompressionEngine->strainRate = strainRate;
	triaxialcompressionEngine->StabilityCriterion = StabilityCriterion;
	triaxialcompressionEngine->autoCompressionActivation = autoCompressionActivation;
	triaxialcompressionEngine->internalCompaction = internalCompaction;
	triaxialcompressionEngine->maxMultiplier = maxMultiplier;
	triaxialcompressionEngine->finalMaxMultiplier = finalMaxMultiplier;
	triaxialcompressionEngine->Key = Key;
	triaxialcompressionEngine->frictionAngleDegree = sphereFrictionDeg;
		
	//cerr << "fin de section triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);" << std::endl;
	
// recording global stress
	triaxialStateRecorder = shared_ptr<TriaxialStateRecorder>(new
	TriaxialStateRecorder);
	triaxialStateRecorder-> outputFile 		= WallStressRecordFile + Key;
	triaxialStateRecorder-> interval 		= recordIntervalIter;
	//triaxialStateRecorderer-> thickness 		= thickness;

	// recording capillary stress
	capillaryStressRecorder = shared_ptr<CapillaryStressRecorder>(new
	CapillaryStressRecorder);
	capillaryStressRecorder -> outputFile 	= capillaryStressRecordFile + Key;
	capillaryStressRecorder -> interval 	= recordIntervalIter;
	capillaryStressRecorder-> thickness 	= thickness;
// 	capillaryStressRecorder-> upperCorner 	= upperCorner;
// 	capillaryStressRecorder-> lowerCorner 	= lowerCorner;

//recording contact stress
	contactStressRecorder = shared_ptr<ContactStressRecorder>(new
	ContactStressRecorder);
	contactStressRecorder -> outputFile 	= contactStressRecordFile + Key;
 	contactStressRecorder -> interval 	= recordIntervalIter;
 	contactStressRecorder-> thickness 	= thickness;
//  	contactStressRecorder-> upperCorner 	= upperCorner;
//  	contactStressRecorder-> lowerCorner 	= lowerCorner;
	
	#if 0	
	// moving walls to regulate the stress applied
	//cerr << "triaxialstressController = shared_ptr<TriaxialStressController> (new TriaxialStressController);" << std::endl;
	triaxialstressController = shared_ptr<TriaxialStressController> (new TriaxialStressController);
	triaxialstressController-> stiffnessUpdateInterval = 20;// = recordIntervalIter
	triaxialstressController-> sigma_iso = sigma_iso;
	triaxialstressController-> max_vel = 0.0001;
	triaxialstressController-> thickness = thickness;
	triaxialstressController->wall_bottom_activated = false;
	triaxialstressController->wall_top_activated = false;	
		//cerr << "fin de sezction triaxialstressController = shared_ptr<TriaxialStressController> (new TriaxialStressController);" << std::endl;
	#endif
	
	
	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
//	rootBody->engines.push_back(sdecTimeStepper);	
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(elasticContactLaw);

	// capillary
	
	
	//rootBody->engines.push_back(stiffnesscounter);
	//rootBody->engines.push_back(stiffnessMatrixTimeStepper);
	rootBody->engines.push_back(globalStiffnessTimeStepper);
	if(water)
	{
		rootBody->engines.push_back(capillaryCohesiveLaw);
		rootBody->engines.push_back(capillaryStressRecorder);
	}
	rootBody->engines.push_back(triaxialcompressionEngine);
	rootBody->engines.push_back(triaxialStateRecorder);
	rootBody->engines.push_back(contactStressRecorder);
	//rootBody->engines.push_back(gravityCondition);
	
	rootBody->engines.push_back(shared_ptr<Engine> (new NewtonsDampedLaw));
	
	//if(!rotationBlocked)
	//	rootBody->engines.push_back(orientationIntegrator);
	//rootBody->engines.push_back(triaxialstressController);
	
		
	if (saveAnimationSnapshots) {
	shared_ptr<PositionOrientationRecorder> positionOrientationRecorder(new PositionOrientationRecorder);
	positionOrientationRecorder->outputFile = AnimationSnapshotsBaseName;
	rootBody->engines.push_back(positionOrientationRecorder);}
	
	rootBody->initializers.clear();
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
	
	rootBody->interactingGeometry	= YADE_PTR_CAST<InteractingGeometry>(set);	
	rootBody->boundingVolume	= YADE_PTR_CAST<BoundingVolume>(aabb);
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
