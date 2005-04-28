#include "SDECImpactTest.hpp"

#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "MetaBody.hpp"
#include "SAPCollider.hpp"
#include "PersistentSAPCollider.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "Interaction.hpp"
#include "BoundingVolumeMetaEngine.hpp"
#include "InteractionDescriptionSet2AABB.hpp"
#include "MetaInteractingGeometry.hpp"

#include "ElasticContactLaw.hpp"
#include "MacroMicroElasticRelationships.hpp"
#include "BodyMacroParameters.hpp"
#include "SDECLinkGeometry.hpp"
#include "SDECLinkPhysics.hpp"
#include "SDECTimeStepper.hpp"

#include "GravityEngine.hpp"
#include "ActionParameterDispatcher.hpp"
#include "ActionParameterDispatcher.hpp"
#include "CundallNonViscousForceDamping.hpp"
#include "CundallNonViscousMomentumDamping.hpp"

#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "Body.hpp"
#include "InteractingBox.hpp"
#include "InteractingSphere.hpp"
#include "ActionParameterDispatcher.hpp"

#include "ActionParameterReset.hpp"
#include "ActionParameterInitializer.hpp"

#include "AveragePositionRecorder.hpp"
#include "ForceRecorder.hpp"
#include "VelocityRecorder.hpp"
#include "PhysicalParametersDispatcher.hpp"

#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>

using namespace boost;
using namespace std;

SDECImpactTest::SDECImpactTest () : FileGenerator()
{
	lowerCorner 		= Vector3r(1000,1000,1000);
	upperCorner 		= Vector3r(-1000,-1000,-1000);
	thickness 		= 0.01;
	importFilename 		= "../data/small.sdec.xyz";
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
	spheresColor		= Vector3f(0.8,0.3,0.3);
	spheresRandomColor	= false;
	recordBottomForce	= true;
	forceRecordFile		= "../data/force";
	recordAveragePositions	= true;
	positionRecordFile	= "../data/position";
	recordIntervalIter	= 100;
	velocityRecordFile 	= "../data/velocities";
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


void SDECImpactTest::registerAttributes()
{
//	REGISTER_ATTRIBUTE(lowerCorner);
//	REGISTER_ATTRIBUTE(upperCorner);
//	REGISTER_ATTRIBUTE(thickness);
	REGISTER_ATTRIBUTE(importFilename);
	REGISTER_ATTRIBUTE(boxWalls);

	REGISTER_ATTRIBUTE(sphereYoungModulus);
	REGISTER_ATTRIBUTE(spherePoissonRatio);
	REGISTER_ATTRIBUTE(sphereFrictionDeg);

	REGISTER_ATTRIBUTE(boxYoungModulus);
	REGISTER_ATTRIBUTE(boxPoissonRatio);
	REGISTER_ATTRIBUTE(boxFrictionDeg);

	REGISTER_ATTRIBUTE(density);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	REGISTER_ATTRIBUTE(rotationBlocked);
	REGISTER_ATTRIBUTE(timeStepUpdateInterval);
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
//	REGISTER_ATTRIBUTE(recordBottomForce);
	REGISTER_ATTRIBUTE(forceRecordFile);
//	REGISTER_ATTRIBUTE(recordAveragePositions);
	REGISTER_ATTRIBUTE(positionRecordFile);
	REGISTER_ATTRIBUTE(velocityRecordFile)
	REGISTER_ATTRIBUTE(recordIntervalIter);

//	REGISTER_ATTRIBUTE(gravity);
	
	REGISTER_ATTRIBUTE(bigBall);
	REGISTER_ATTRIBUTE(bigBallRadius);
	REGISTER_ATTRIBUTE(bigBallDensity);
	REGISTER_ATTRIBUTE(bigBallDropTimeSeconds);
	REGISTER_ATTRIBUTE(bigBallFrictDeg);
	REGISTER_ATTRIBUTE(bigBallYoungModulus);
	REGISTER_ATTRIBUTE(bigBallPoissonRatio);
	REGISTER_ATTRIBUTE(bigBallDropHeight);

}

string SDECImpactTest::generate()
{
	unsigned int startId=boost::numeric::bounds<unsigned int>::highest(), endId=0; // record forces from group 2
	
	rootBody = shared_ptr<MetaBody>(new MetaBody);
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
				startId = std::min(body->getId() , startId);
				endId   = std::max(body->getId() , endId);
			}
				
		}
	}

// create bigBall
	Vector3r position = (upperCorner+lowerCorner)*0.5 + Vector3r(0,bigBallDropHeight,0);
	createSphere(body,position,bigBallRadius,true,false);	
	int bigId = 0;
	if(bigBall)
		rootBody->bodies->insert(body);
	bigId = body->getId();
	forcerec->bigBallId = bigId;
	forcerec->bigBallReleaseTime = bigBallDropTimeSeconds;
	forcerec->startId = startId;
	forcerec->endId   = endId;
	averagePositionRecorder->bigBallId = bigId;
	velocityRecorder->bigBallId = bigId;

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

 	return "Generated a sample inside box of dimensions: (" 
 		+ lexical_cast<string>(lowerCorner[0]) + "," 
 		+ lexical_cast<string>(lowerCorner[1]) + "," 
 		+ lexical_cast<string>(lowerCorner[2]) + ") and (" 
 		+ lexical_cast<string>(upperCorner[0]) + "," 
 		+ lexical_cast<string>(upperCorner[1]) + "," 
 		+ lexical_cast<string>(upperCorner[2]) + ").";

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECImpactTest::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius, bool big, bool dynamic )
{
	body = shared_ptr<Body>(new Body(0,2));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	
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
	iSphere->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());

	body->interactionGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECImpactTest::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents, bool wire)
{
	body = shared_ptr<Body>(new Body(0,2));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

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
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= wire;
	gBox->visible			= true;
	gBox->shadowCaster		= false;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3f(1,1,1);

	body->boundingVolume		= aabb;
	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECImpactTest::createActors(shared_ptr<MetaBody>& rootBody)
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

	shared_ptr<ActionParameterInitializer> actionParameterInitializer(new ActionParameterInitializer);
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterForce");
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterMomentum");
	
	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	interactionGeometryDispatcher->add("InteractingSphere","InteractingSphere","Sphere2Sphere4MacroMicroContactGeometry");
	interactionGeometryDispatcher->add("InteractingSphere","InteractingBox","Box2Sphere4MacroMicroContactGeometry");

	shared_ptr<InteractionPhysicsDispatcher> interactionPhysicsDispatcher(new InteractionPhysicsDispatcher);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");
		
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere","AABB","Sphere2AABB");
	boundingVolumeDispatcher->add("InteractingBox","AABB","Box2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","InteractionDescriptionSet2AABB");

	

		
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<ActionParameterDispatcher> actionDampingDispatcher(new ActionParameterDispatcher);
	actionDampingDispatcher->add("ActionParameterForce","ParticleParameters","CundallNonViscousForceDamping",actionForceDamping);
	actionDampingDispatcher->add("ActionParameterMomentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
	
	shared_ptr<ActionParameterDispatcher> applyActionDispatcher(new ActionParameterDispatcher);
	applyActionDispatcher->add("ActionParameterForce","ParticleParameters","NewtonsForceLaw");
	applyActionDispatcher->add("ActionParameterMomentum","RigidBodyParameters","NewtonsMomemtumForceLaw");
	
	shared_ptr<PhysicalParametersDispatcher> positionIntegrator(new PhysicalParametersDispatcher);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegrator");
	shared_ptr<PhysicalParametersDispatcher> orientationIntegrator(new PhysicalParametersDispatcher);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegrator");

	shared_ptr<SDECTimeStepper> sdecTimeStepper(new SDECTimeStepper);
	sdecTimeStepper->sdecGroupMask = 2;
	sdecTimeStepper->interval = timeStepUpdateInterval;
	
	
	shared_ptr<ElasticContactLaw> elasticContactLaw(new ElasticContactLaw);
	elasticContactLaw->sdecGroupMask = 2;
	
	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Engine>(new ActionParameterReset));
	rootBody->actors.push_back(sdecTimeStepper);
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(interactionPhysicsDispatcher);
	rootBody->actors.push_back(elasticContactLaw);
	rootBody->actors.push_back(gravityCondition);
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(positionIntegrator);
	if(!rotationBlocked)
		rootBody->actors.push_back(orientationIntegrator);
	rootBody->actors.push_back(averagePositionRecorder);
	rootBody->actors.push_back(velocityRecorder);
	rootBody->actors.push_back(forcerec);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(actionParameterInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECImpactTest::positionRootBody(shared_ptr<MetaBody>& rootBody)
{
	rootBody->isDynamic		= false;

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3			= Se3r(Vector3r(0,0,0),q);
	physics->mass			= 0;
	physics->velocity		= Vector3r::ZERO;
	physics->acceleration		= Vector3r::ZERO;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	
	set->diffuseColor		= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);
	
	rootBody->interactionGeometry	= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume	= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->physicalParameters 	= physics;
	
}
