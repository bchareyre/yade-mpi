/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SDECImpactTest.hpp"

#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/MacroMicroElasticRelationships.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/pkg-dem/ElasticCriterionTimeStepper.hpp>

#include<yade/pkg-dem/AveragePositionRecorder.hpp>
#include<yade/pkg-dem/ForceRecorder.hpp>
#include<yade/pkg-dem/VelocityRecorder.hpp>

#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/core/World.hpp>
#include<yade/pkg-common/InsertionSortCollider.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/BoundingVolumeDispatcher.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>

#include<yade/pkg-common/GravityEngines.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>

#include<yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include<yade/pkg-common/InteractionPhysicsDispatcher.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>

#include<yade/pkg-common/StateMetaEngine.hpp>



#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>

YADE_REQUIRE_FEATURE(shape)

using namespace boost;
using namespace std;


SDECImpactTest::SDECImpactTest () : FileGenerator()
{
	lowerCorner 		= Vector3r(1000,1000,1000);
	upperCorner 		= Vector3r(-1000,-1000,-1000);
	thickness 		= 0.01;
	importFilename 		= "./small.sdec.xyz";
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
	recordBottomForce	= true;
	forceRecordFile		= "./force";
	recordAveragePositions	= true;
	positionRecordFile	= "./position";
	recordIntervalIter	= 100;
	velocityRecordFile 	= "./velocities";
	rotationBlocked = true;
	
//	boxWalls 		= false;
	boxWalls 		= true;

//	bigBall 		= true;
	bigBall 		= false;
	bigBallRadius		= 0.075;
	bigBallPoissonRatio 	= 0.3;
	bigBallYoungModulus 	= 40000000000.0;
	bigBallFrictDeg 	= 60;
//	bigBallCohesion 	= 10000000000;
//	bigBallTensionStr 	= 10000000000;
	bigBallDensity		= 7800;
	bigBallDropTimeSeconds	= 30;
	bigBallDropHeight 	= 3.04776;
	
	dampingForce = 0.7;
	dampingMomentum = 0.7;
	timeStepUpdateInterval = 300;
	
	sphereYoungModulus  = 15000000.0;
	spherePoissonRatio  = 0.2;
	sphereFrictionDeg   = 18.0;
	density			= 2600;
	
	boxYoungModulus   = 15000000.0;
	boxPoissonRatio  = 0.2;
	boxFrictionDeg   = -18.0;
	gravity 	= Vector3r(0,-9.81,0);
}


SDECImpactTest::~SDECImpactTest ()
{

}




bool SDECImpactTest::generate()
{
	int startId=boost::numeric::bounds<int>::highest(), endId=0; // record forces from group 2
	
	rootBody = shared_ptr<World>(new World);
	createActors(rootBody);
	positionRootBody(rootBody);


	shared_ptr<Body> body;
	if(importFilename.size() != 0 && filesystem::exists(importFilename) )
	{
		ifstream loadFile(importFilename.c_str());
		long int i=0;
		Real f,g,x,y,z,radius;
		while( ! loadFile.eof() )
		{
			++i;
			loadFile >> x;
			loadFile >> y;
			loadFile >> z;
			Vector3r position = Vector3r(x,z,y);
			loadFile >> radius;
		
			loadFile >> f;
			loadFile >> g;
			if( boxWalls ? f>1 : false ) // skip loading of SDEC walls
				continue;
			if(f==8)
				continue;

	//		if( i % 100 == 0 ) // FIXME - should display a progress BAR !!
	//			cout << "loaded: " << i << endl;
			if(f==1)
			{
				lowerCorner[0] = min(position[0]-radius , lowerCorner[0]);
				lowerCorner[1] = min(position[1]-radius , lowerCorner[1]);
				lowerCorner[2] = min(position[2]-radius , lowerCorner[2]);
				upperCorner[0] = max(position[0]+radius , upperCorner[0]);
				upperCorner[1] = max(position[1]+radius , upperCorner[1]);
				upperCorner[2] = max(position[2]+radius , upperCorner[2]);
			}
			createSphere(body,position,radius,false,f==1);
			rootBody->bodies->insert(body);
			if(f == 2)
			{
				startId = std::min((int)body->getId() , startId);
				endId   = std::max((int)body->getId() , endId);
			}
				
		}
	}
	else
	{
		message="Cannot find input file, you can copy it from examples/ directory"; 
		return false;
	}

// create bigBall
	Vector3r position = (upperCorner+lowerCorner)*0.5 + Vector3r(0,bigBallDropHeight,0);
	createSphere(body,position,bigBallRadius,true,false);	
	int bigId = 0;
	if(bigBall)
		rootBody->bodies->insert(body);
	bigId = body->getId();
	forcerec->startId = startId;
	forcerec->endId   = endId;

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
	 	if(wall_bottom)
			rootBody->bodies->insert(body);
//		forcerec->id = body->getId();
	
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
	 	if(wall_top)
			rootBody->bodies->insert(body);
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
	 	if(wall_1)
			rootBody->bodies->insert(body);
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
	 	if(wall_2)
			rootBody->bodies->insert(body);
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
	 	if(wall_3)
	 		rootBody->bodies->insert(body);
	
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
	 	if(wall_4)
	 		rootBody->bodies->insert(body);
			 
	}

 	message="Generated a sample inside box of dimensions: (" 
 		+ lexical_cast<string>(lowerCorner[0]) + "," 
 		+ lexical_cast<string>(lowerCorner[1]) + "," 
 		+ lexical_cast<string>(lowerCorner[2]) + ") and (" 
 		+ lexical_cast<string>(upperCorner[0]) + "," 
 		+ lexical_cast<string>(upperCorner[1]) + "," 
 		+ lexical_cast<string>(upperCorner[2]) + ").";
	return true;

}


void SDECImpactTest::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius, bool big, bool dynamic )
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
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());

	body->interactingGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}


void SDECImpactTest::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents, bool wire)
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
	physics->mass			= extents[0]*extents[1]*extents[2]*density*2; 
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


void SDECImpactTest::createActors(shared_ptr<World>& rootBody)
{
// recording average positions
	averagePositionRecorder = shared_ptr<AveragePositionRecorder>(new AveragePositionRecorder);
	averagePositionRecorder -> outputFile 		= positionRecordFile;
	averagePositionRecorder -> interval 		= recordIntervalIter;
// recording forces
	forcerec = shared_ptr<ForceRecorder>(new ForceRecorder);
	forcerec -> outputFile 	= forceRecordFile;
	forcerec -> interval 	= recordIntervalIter;
// recording velocities
	velocityRecorder = shared_ptr<VelocityRecorder>(new VelocityRecorder);
	velocityRecorder-> outputFile 	= velocityRecordFile;
	velocityRecorder-> interval 	= recordIntervalIter;

	
	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	interactionGeometryDispatcher->add("InteractingSphere2InteractingSphere4SpheresContactGeometry");
	interactionGeometryDispatcher->add("InteractingBox2InteractingSphere4SpheresContactGeometry");

	shared_ptr<InteractionPhysicsDispatcher> interactionPhysicsDispatcher(new InteractionPhysicsDispatcher);
	interactionPhysicsDispatcher->add("MacroMicroElasticRelationships");
		
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
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
	
	shared_ptr<StateMetaEngine> positionIntegrator(new StateMetaEngine);
	positionIntegrator->add("LeapFrogPositionIntegrator");
	shared_ptr<StateMetaEngine> orientationIntegrator(new StateMetaEngine);
	orientationIntegrator->add("LeapFrogOrientationIntegrator");

	shared_ptr<ElasticCriterionTimeStepper> sdecTimeStepper(new ElasticCriterionTimeStepper);
	sdecTimeStepper->sdecGroupMask = 2;
	sdecTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	
	
	shared_ptr<ElasticContactLaw> elasticContactLaw(new ElasticContactLaw);
	elasticContactLaw->sdecGroupMask = 2;
	
	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(sdecTimeStepper);
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new InsertionSortCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(elasticContactLaw);
	rootBody->engines.push_back(gravityCondition);
	rootBody->engines.push_back(actionDampingDispatcher);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);
	if(!rotationBlocked)
		rootBody->engines.push_back(orientationIntegrator);
	rootBody->engines.push_back(averagePositionRecorder);
	rootBody->engines.push_back(velocityRecorder);
	rootBody->engines.push_back(forcerec);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	
}


void SDECImpactTest::positionRootBody(shared_ptr<World>& rootBody)
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

YADE_PLUGIN((SDECImpactTest));

YADE_REQUIRE_FEATURE(PHYSPAR);

