#include "BoxStack.hpp"


#include <yade-common/Box.hpp>
#include <yade-common/AABB.hpp>
#include <yade-common/Sphere.hpp>
#include <yade/MetaBody.hpp>
#include <yade/Body.hpp>
#include <yade-common/FrictionLessElasticContactLaw.hpp>
#include <yade-common/SAPCollider.hpp>
#include <yade-common/RigidBodyParameters.hpp>
#include <yade-common/TranslationEngine.hpp>
#include <fstream>
#include <yade-lib-serialization/IOManager.hpp>
#include <yade-common/InteractingBox.hpp>
#include <yade-common/InteractingSphere.hpp>
#include <yade-common/InteractionGeometryMetaEngine.hpp>

#include <yade-common/PhysicalActionDamper.hpp>
#include <yade-common/PhysicalActionApplier.hpp>


#include <yade-common/PhysicalActionContainerReseter.hpp>
#include <yade-common/CundallNonViscousForceDamping.hpp>
#include <yade-common/CundallNonViscousMomentumDamping.hpp>

#include <yade-common/BoundingVolumeMetaEngine.hpp>
#include <yade-common/InteractionDescriptionSet2AABB.hpp>
#include <yade-common/MetaInteractingGeometry.hpp>

#include <yade-common/PhysicalActionContainerInitializer.hpp>
#include <yade-common/GravityEngine.hpp>
#include <yade-common/PhysicalParametersMetaEngine.hpp>

#include <yade-common/BodyRedirectionVector.hpp>
#include <yade-common/InteractionVecSet.hpp>
#include <yade-common/PhysicalActionVectorVector.hpp>


BoxStack::BoxStack () : FileGenerator()
{
	nbBoxes		= Vector3r(1,5,7);
	boxSize		= Vector3r(6,4,8);
	boxDensity	= 1;
	bulletVelocity	= Vector3r(-120,0,0);
	bulletPosition	= Vector3r(80,0,0);
	bulletSize	= 12;
	bulletDensity	= 2;
	dampingForce	= 0.6;
	dampingMomentum = 0.9;
	kinematicBullet	= false;
	gravity 	= Vector3r(0,-9.81,0);
}

BoxStack::~BoxStack ()
{

}

void BoxStack::registerAttributes()
{
	FileGenerator::registerAttributes();
	REGISTER_ATTRIBUTE(nbBoxes);
	REGISTER_ATTRIBUTE(boxSize);
	REGISTER_ATTRIBUTE(boxDensity);
//	REGISTER_ATTRIBUTE(kinematicBullet);
	REGISTER_ATTRIBUTE(bulletSize);
	REGISTER_ATTRIBUTE(bulletDensity);
	REGISTER_ATTRIBUTE(bulletPosition);
	REGISTER_ATTRIBUTE(bulletVelocity);
	REGISTER_ATTRIBUTE(gravity);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
}

string BoxStack::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);

	createActors(rootBody);
	positionRootBody(rootBody);
	
	////////////////////////////////////

	rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->volatileInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->actionParameters		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

	shared_ptr<Body> body;
	
	createKinematicBox(body, Vector3r(0,  -5,  0), Vector3r(  200, 5, 200), false);
	rootBody->bodies->insert(body);
	
	for(int i=0;i<nbBoxes[0];i++)
		for(int j=0;j<nbBoxes[1];j++)
			for(int k=0;k<nbBoxes[2];k++)
 			{
				shared_ptr<Body> box;
				createBox(box,i,j,k);
				rootBody->bodies->insert(box);
 			}

	createSphere(body);
	rootBody->bodies->insert(body);
 			
	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


// FIXME - all the functions below, are a modified copy of functions in RotatingBox.cpp
void BoxStack::createBox(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(0,1));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
		
	Vector3r position		= Vector3r(i*boxSize[0],j*boxSize[1],k*boxSize[2])*2
					  - Vector3r(0,-boxSize[1],nbBoxes[2]*boxSize[2]-boxSize[2])
					  + Vector3r(0,0,(j%2)*boxSize[2]);
				  
	Vector3r size 			= boxSize;
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= size[0]*size[1]*size[2]*boxDensity;
	physics->inertia		= Vector3r(physics->mass*(size[1]*size[1]+size[2]*size[2])/3,physics->mass*(size[0]*size[0]+size[2]*size[2])/3,physics->mass*(size[1]*size[1]+size[0]*size[0])/3);
	physics->se3			= Se3r(position,q);

	aabb->diffuseColor		= Vector3r(0,1,0);
	
	gBox->extents			= size;
	gBox->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= size;
	iBox->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());

	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void BoxStack::createSphere(shared_ptr<Body>& body)
{
	body = shared_ptr<Body>(new Body(0,1));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
		
	Vector3r position 		= Vector3r(0,nbBoxes[1]*boxSize[1],0)
					  + bulletPosition;
				  
	Real radius 			= bulletSize;
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= bulletVelocity;
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*bulletDensity;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);

	aabb->diffuseColor		= Vector3r(0,1,0);
	
	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
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

void BoxStack::createKinematicBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire)
{
	body = shared_ptr<Body>(new Body(0,2));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 0;
	physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);

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

void BoxStack::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<PhysicalActionContainerInitializer> actionParameterInitializer(new PhysicalActionContainerInitializer);
	actionParameterInitializer->actionParameterNames.push_back("Force");
	actionParameterInitializer->actionParameterNames.push_back("Momentum");
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere","InteractingSphere","Sphere2Sphere4ClosestFeatures");
	interactionGeometryDispatcher->add("InteractingSphere","InteractingBox","Box2Sphere4ClosestFeatures");
	interactionGeometryDispatcher->add("InteractingBox","InteractingBox","Box2Box4ClosestFeatures");

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
	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
	actionDampingDispatcher->add("Force","ParticleParameters","CundallNonViscousForceDamping",actionForceDamping);
	actionDampingDispatcher->add("Momentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("Force","ParticleParameters","NewtonsForceLaw");
	applyActionDispatcher->add("Momentum","RigidBodyParameters","NewtonsMomentumLaw");
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegrator");
	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegrator");
 	
// 	shared_ptr<RotationEngine> kinematic = shared_ptr<RotationEngine>(new RotationEngine);
// 	kinematic->angularVelocity  = rotationSpeed;
// 	rotationAxis.normalize();
// 	kinematic->rotationAxis  = rotationAxis;
// 	kinematic->rotateAroundZero = true;
// 	for(int i=0;i<7;i++)
// 		kinematic->subscribedBodies.push_back(i);
	
	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(shared_ptr<Engine>(new SAPCollider));
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(shared_ptr<Engine>(new FrictionLessElasticContactLaw));
	rootBody->actors.push_back(gravityCondition);
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(positionIntegrator);
	rootBody->actors.push_back(orientationIntegrator);
//	if(isRotating)
//		rootBody->actors.push_back(kinematic);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(actionParameterInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void BoxStack::positionRootBody(shared_ptr<MetaBody>& rootBody)
{
	rootBody->isDynamic			= false;
	Quaternionr q;	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->mass				= 0;
	physics->velocity			= Vector3r::ZERO;
	physics->acceleration			= Vector3r::ZERO;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	set->diffuseColor			= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);
	
	rootBody->interactionGeometry		= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->physicalParameters 		= physics;
}
