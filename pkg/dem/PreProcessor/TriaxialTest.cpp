/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  Copyright (C) 2007 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

 


#include "TriaxialTest.hpp"

#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/ElasticCohesiveLaw.hpp>
#include<yade/pkg-dem/SimpleElasticRelationships.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/pkg-dem/GlobalStiffnessCounter.hpp>
#include<yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>
#include<yade/pkg-dem/MakeItFlat.hpp>

#include<yade/pkg-dem/AveragePositionRecorder.hpp>
#include<yade/pkg-dem/ForceRecorder.hpp>
#include<yade/pkg-dem/VelocityRecorder.hpp>
#include<yade/pkg-dem/TriaxialStressController.hpp>
#include<yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include <yade/pkg-dem/TriaxialStateRecorder.hpp>

#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/SAPCollider.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>

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

#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/PhysicalActionContainerInitializer.hpp>

#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>

#include<yade/pkg-common/BodyRedirectionVector.hpp>
#include<yade/pkg-common/InteractionVecSet.hpp>
#include<yade/pkg-common/InteractionHashMap.hpp>
#include<yade/pkg-common/PhysicalActionVectorVector.hpp>

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


CREATE_LOGGER(TriaxialTest);

using namespace boost;
using namespace std;



 
TriaxialTest::TriaxialTest () : FileGenerator()
{
	lowerCorner 		= Vector3r(0,0,0);
	upperCorner 		= Vector3r(1,1,1);
	thickness 		= 0.001;
	importFilename 		= ""; // oh, PLEASE, keep this empty -- i.e., by default, generate spheres in the box, not load them.
	Key			="";
	outputFileName 		= "./TriaxialTest"+Key+".xml";
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

	rotationBlocked = false;
	//	boxWalls 		= false;
	boxWalls 		= true;
	internalCompaction	=false;
	
	dampingForce = 0.2;
	dampingMomentum = 0.2;
	defaultDt = 0.001;
	
	timeStepUpdateInterval = 50;
	timeStepOutputInterval = 50;
	wallStiffnessUpdateInterval = 10;
	radiusControlInterval = 10;
	numberOfGrains = 400;
	strainRate = 0.1;
	maxWallVelocity=10;
	StabilityCriterion = 0.01;
	autoCompressionActivation = true;
	autoUnload = true;
	maxMultiplier = 1.01;
	finalMaxMultiplier = 1.001;
	
	sphereYoungModulus  = 15000000.0;
	spherePoissonRatio  = 0.5;	
	sphereFrictionDeg = 18.0;
	compactionFrictionDeg   = sphereFrictionDeg;
	density			= 2600;
	
	boxYoungModulus   = 15000000.0;
	boxPoissonRatio  = 0.2;
	boxFrictionDeg   = 0.f;
	gravity 	= Vector3r(0,-9.81,0);
	
	sigmaIsoCompaction = 50000;
	sigmaLateralConfinement=sigmaIsoCompaction;

	wallOversizeFactor=1.3;

	biaxial2dTest=false;

	radiusStdDev=0.3;
	radiusMean=-1; // no radius specified

	isotropicCompaction=false;
	translationSpeed = 0;
	fixedPorosity = 1;

 

	
//	wall_top_id =0;
// 	wall_bottom_id =0;
// 	wall_left_id =0;
// 	all_right_id =0;
// 	wall_front_id =0;
// 	wall_back_id =0;
}


TriaxialTest::~TriaxialTest ()
{

}


void TriaxialTest::registerAttributes()
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
	REGISTER_ATTRIBUTE(biaxial2dTest);
	REGISTER_ATTRIBUTE(maxMultiplier);
	REGISTER_ATTRIBUTE(finalMaxMultiplier);
	REGISTER_ATTRIBUTE(radiusStdDev);
	REGISTER_ATTRIBUTE(radiusMean);

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
	REGISTER_ATTRIBUTE(strainRate);
	REGISTER_ATTRIBUTE(maxWallVelocity);
	REGISTER_ATTRIBUTE(StabilityCriterion);
	REGISTER_ATTRIBUTE(autoCompressionActivation);
	REGISTER_ATTRIBUTE(autoUnload);
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
	REGISTER_ATTRIBUTE(isotropicCompaction);
	REGISTER_ATTRIBUTE(translationSpeed);
	REGISTER_ATTRIBUTE(fixedPorosity);
	REGISTER_ATTRIBUTE(fixedBoxDims);
 



}


bool TriaxialTest::generate()
{
//	unsigned int startId=boost::numeric::bounds<unsigned int>::highest(), endId=0; // record forces from group 2
	message="";
	
	if(biaxial2dTest && (8.0*(upperCorner[2]-lowerCorner[2]))>(upperCorner[0]-lowerCorner[0]))
	{
		message="Biaxial test can be generated only if Z size is more than 8 times smaller than X size";
		return false;
	}
	
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

	//rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionHashMap);
	//rootBody->transientInteractions		= shared_ptr<InteractionContainer>(new InteractionHashMap);

	rootBody->physicalActions		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

	shared_ptr<Body> body;



	/* if _mean_radius is not given (i.e. <=0), then calculate it from box size;
	 * OTOH, if it is specified, scale the box preserving its ratio and lowerCorner so that the radius can be as requested
	 */
	Vector3r dimensions=upperCorner-lowerCorner; Real volume=dimensions.X()*dimensions.Y()*dimensions.Z();
	Real porosity=.75;
	Real really_radiusMean;

	if(radiusMean<=0) really_radiusMean=pow(volume*(1-porosity)/(Mathr::PI*(4/3.)*numberOfGrains),1/3.);
	else {
		bool fixedDims[3];
		fixedDims[0]=fixedBoxDims.find('x')!=string::npos; fixedDims[1]=fixedBoxDims.find('y')!=string::npos; fixedDims[2]=fixedBoxDims.find('z')!=string::npos;
		int nScaled=(3-(int)fixedDims[0]+(int)fixedDims[1]+(int)fixedDims[2]);
		if(nScaled==3) throw std::invalid_argument("At most 2 (not 3) axes can have fixed dimensions in fixedBoxDims if scaling for given radiusMean.");
		Real boxScaleFactor=radiusMean*pow((4/3.)*Mathr::PI*numberOfGrains/(volume*(1-porosity)),1./nScaled);
		LOG_INFO("Mean radius value of "<<radiusMean<<" requested, scaling "<<nScaled<<" dimensions by "<<boxScaleFactor);
		dimensions[0]*=fixedDims[0]?1.:boxScaleFactor; dimensions[1]*=fixedDims[1]?1.:boxScaleFactor; dimensions[2]*=fixedDims[2]?1.:boxScaleFactor;
		upperCorner=lowerCorner+dimensions;
		really_radiusMean=radiusMean;
	}

	
	
	
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
			//(resultantforceEngine->subscribedBodies).push_back(body->getId());
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
	else message+=GenerateCloud(sphere_list, lowerCorner, upperCorner, numberOfGrains, radiusStdDev, really_radiusMean, porosity);
	vector<BasicSphere>::iterator it = sphere_list.begin();
	vector<BasicSphere>::iterator it_end = sphere_list.end();
	FOREACH(const BasicSphere& it, sphere_list){
		LOG_DEBUG("sphere (" << it.first << " " << it.second << ")");
		createSphere(body,it.first,it.second,false,true);
		rootBody->bodies->insert(body);
	}	

	
	
	return true;

}


void TriaxialTest::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius, bool big, bool dynamic )
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
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());

	body->interactingGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}


void TriaxialTest::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents, bool wire)
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
	gBox->visible			= true;
	gBox->shadowCaster		= false;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3r(1,1,1);

	body->boundingVolume		= aabb;
	body->interactingGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}


void TriaxialTest::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
	physicalActionInitializer->physicalActionNames.push_back("Force");
	physicalActionInitializer->physicalActionNames.push_back("Momentum");
	//physicalActionInitializer->physicalActionNames.push_back("StiffnessMatrix");
	physicalActionInitializer->physicalActionNames.push_back("GlobalStiffness");
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere2InteractingSphere4SpheresContactGeometry");
	interactionGeometryDispatcher->add("InteractingBox2InteractingSphere4SpheresContactGeometry");

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
//	interactionPhysicsDispatcher->add("SimpleElasticRelationships");
// Unhandled exception: St13runtime_error : Class `SimpleElasticRelationships' could not be cast to required 2D EngineUnit	
	shared_ptr<InteractionPhysicsEngineUnit> ss(new SimpleElasticRelationships);
	interactionPhysicsDispatcher->add(ss);
	
		
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere2AABB");
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

	shared_ptr<ElasticCohesiveLaw> elasticCohesiveLaw(new ElasticCohesiveLaw);
	elasticCohesiveLaw->sdecGroupMask = 2;
	elasticCohesiveLaw->momentRotationLaw = true;

	
	//shared_ptr<StiffnessCounter> stiffnesscounter(new StiffnessCounter);
	//stiffnesscounter->sdecGroupMask = 2;
	//stiffnesscounter->interval = timeStepUpdateInterval;
	
	shared_ptr<GlobalStiffnessCounter> globalStiffnessCounter(new GlobalStiffnessCounter);
	// globalStiffnessCounter->sdecGroupMask = 2;
	globalStiffnessCounter->interval = timeStepUpdateInterval;
	
	// moving walls to regulate the stress applied + compress when the packing is dense an stable
	//cerr << "triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);" << std::endl;
	triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);
	triaxialcompressionEngine-> stiffnessUpdateInterval = wallStiffnessUpdateInterval;// = stiffness update interval
	triaxialcompressionEngine-> radiusControlInterval = radiusControlInterval;// = stiffness update interval
	//triaxialcompressionEngine-> sigma_iso = sigma_iso;
	triaxialcompressionEngine-> sigmaIsoCompaction = sigmaIsoCompaction;
	triaxialcompressionEngine-> sigmaLateralConfinement = sigmaLateralConfinement;
	triaxialcompressionEngine->max_vel = maxWallVelocity;
	triaxialcompressionEngine-> thickness = thickness;
	triaxialcompressionEngine->strainRate = strainRate;
	triaxialcompressionEngine->StabilityCriterion = StabilityCriterion;
	triaxialcompressionEngine->autoCompressionActivation = autoCompressionActivation;
	triaxialcompressionEngine->autoUnload = autoUnload;
	triaxialcompressionEngine->internalCompaction = internalCompaction;
	triaxialcompressionEngine->maxMultiplier = maxMultiplier;
	triaxialcompressionEngine->finalMaxMultiplier = finalMaxMultiplier;
	triaxialcompressionEngine->Key = Key;
	triaxialcompressionEngine->frictionAngleDegree = sphereFrictionDeg;
	triaxialcompressionEngine->translationSpeed = translationSpeed;
	triaxialcompressionEngine->fixedPorosity = fixedPorosity;
	triaxialcompressionEngine->isotropicCompaction = isotropicCompaction;
		
	//cerr << "fin de section triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);" << std::endl;
	
// recording global stress
	if(recordIntervalIter>0){
		triaxialStateRecorder = shared_ptr<TriaxialStateRecorder>(new TriaxialStateRecorder);
		triaxialStateRecorder-> outputFile 		= WallStressRecordFile + Key;
		triaxialStateRecorder-> interval 		= recordIntervalIter;
		//triaxialStateRecorderer-> thickness 		= thickness;
	}

	shared_ptr<MakeItFlat> makeItFlat(new MakeItFlat);
	makeItFlat->direction=2;
	makeItFlat->plane_position = (lowerCorner[2]+upperCorner[2])*0.5;
	makeItFlat->reset_force = false;	

	
	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
//	rootBody->engines.push_back(sdecTimeStepper);	
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(elasticContactLaw);
	//rootBody->engines.push_back(elasticCohesiveLaw);
	
	//rootBody->engines.push_back(stiffnesscounter);
	//rootBody->engines.push_back(stiffnessMatrixTimeStepper);
	rootBody->engines.push_back(globalStiffnessCounter);
	rootBody->engines.push_back(globalStiffnessTimeStepper);
	rootBody->engines.push_back(triaxialcompressionEngine);
	if(recordIntervalIter>0) rootBody->engines.push_back(triaxialStateRecorder);
	//rootBody->engines.push_back(gravityCondition);
	
	rootBody->engines.push_back(shared_ptr<Engine> (new NewtonsDampedLaw));

	if(biaxial2dTest) rootBody->engines.push_back(makeItFlat);
	
	//if(!rotationBlocked)
	//	rootBody->engines.push_back(orientationIntegrator);
	//rootBody->engines.push_back(resultantforceEngine);
	//rootBody->engines.push_back(triaxialstressController);
	
		
	if (saveAnimationSnapshots) {
		shared_ptr<PositionOrientationRecorder> positionOrientationRecorder(new PositionOrientationRecorder);
		positionOrientationRecorder->outputFile = AnimationSnapshotsBaseName;
		rootBody->engines.push_back(positionOrientationRecorder);
	}
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(physicalActionInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	
}


void TriaxialTest::positionRootBody(shared_ptr<MetaBody>& rootBody)
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


string TriaxialTest::GenerateCloud(vector<BasicSphere>& sphere_list, Vector3r lowerCorner, Vector3r upperCorner, long number, Real rad_std_dev, Real mean_radius, Real porosity)
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

	LOG_INFO("Generating aggregates ...");
	
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
		}
		if (t==tries) return "More than " + lexical_cast<string>(tries) +
					" tries while generating sphere number " +
					lexical_cast<string>(i+1) + "/" + lexical_cast<string>(number) + ".";
	}
	return "Generated a sample with " + lexical_cast<string>(number) + "spheres inside box of dimensions: (" 
			+ lexical_cast<string>(dimensions[0]) + "," 
			+ lexical_cast<string>(dimensions[1]) + "," 
			+ lexical_cast<string>(dimensions[2]) + ").";
}



YADE_PLUGIN();
