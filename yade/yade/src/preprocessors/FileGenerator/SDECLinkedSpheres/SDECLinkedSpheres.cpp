#include "SDECLinkedSpheres.hpp"

#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "MetaBody.hpp"
#include "SAPCollider.hpp"
#include "PersistentSAPCollider.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "Interaction.hpp"
#include "BoundingVolumeDispatcher.hpp"
#include "InteractionDescriptionSet2AABBFunctor.hpp"
#include "MetaInteractingGeometry.hpp"

#include "ElasticContactLaw.hpp"
#include "MacroMicroElasticRelationships.hpp"
#include "BodyMacroParameters.hpp"
#include "SDECLinkGeometry.hpp"
#include "SDECLinkPhysics.hpp"
#include "SDECTimeStepper.hpp"

#include "ActionParameterDispatcher.hpp"
#include "ActionParameterDispatcher.hpp"
#include "CundallNonViscousForceDampingFunctor.hpp"
#include "CundallNonViscousMomentumDampingFunctor.hpp"
#include "GravityCondition.hpp"

#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "Body.hpp"
#include "InteractingBox.hpp"
#include "InteractingSphere.hpp"
#include "ActionParameterDispatcher.hpp"

#include "ActionParameterReset.hpp"
#include "ActionParameterInitializer.hpp"
#include "BodyPhysicalParametersDispatcher.hpp"

SDECLinkedSpheres::SDECLinkedSpheres () : FileGenerator()
{
	nbSpheres = Vector3r(2,2,20);
	minRadius = 5.01;
	maxRadius = 5.01;
	disorder = 0;
	spacing = 10;
	supportSize = 0.5;
	support1 = 1;
	support2 = 1;
	dampingForce = 0.2;
	dampingMomentum = 0.2;
	timeStepUpdateInterval = 300;
	//sphereYoungModulus  = 15000000.0;
	sphereYoungModulus  =   10000000;
	spherePoissonRatio  = 0.2;
	sphereFrictionDeg   = 18.0;
	density = 2.6;
	momentRotationLaw = true;
	gravity = Vector3r(0,-9.81,0);
}

SDECLinkedSpheres::~SDECLinkedSpheres ()
{

}

void SDECLinkedSpheres::postProcessAttributes(bool)
{
}

void SDECLinkedSpheres::registerAttributes()
{
	REGISTER_ATTRIBUTE(nbSpheres);
	REGISTER_ATTRIBUTE(minRadius);
	REGISTER_ATTRIBUTE(maxRadius);
	REGISTER_ATTRIBUTE(density);
	REGISTER_ATTRIBUTE(sphereYoungModulus);
	REGISTER_ATTRIBUTE(spherePoissonRatio);
	REGISTER_ATTRIBUTE(sphereFrictionDeg);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	REGISTER_ATTRIBUTE(momentRotationLaw);
	REGISTER_ATTRIBUTE(gravity);
	REGISTER_ATTRIBUTE(disorder);
	REGISTER_ATTRIBUTE(spacing);
	REGISTER_ATTRIBUTE(supportSize);
	REGISTER_ATTRIBUTE(support1);
	REGISTER_ATTRIBUTE(support2);
	REGISTER_ATTRIBUTE(timeStepUpdateInterval);
}

string SDECLinkedSpheres::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

////////////////////////////////////

	shared_ptr<Body> ground;
	shared_ptr<Body> supportBox1;
	shared_ptr<Body> supportBox2;
	
	createBox(ground, Vector3r(0,0,0), Vector3r(200,5,200));
	createBox(supportBox1, Vector3r(0,0,((Real)(nbSpheres[2])/2.0-supportSize+1.5)*spacing), Vector3r(20,50,20));
	createBox(supportBox2, Vector3r(0,0,-((Real)(nbSpheres[2])/2.0-supportSize+2.5)*spacing), Vector3r(20,50,20));
			
	rootBody->bodies->insert(ground);
	if (support1)
		rootBody->bodies->insert(supportBox1);
	if (support2)
		rootBody->bodies->insert(supportBox2);

/////////////////////////////////////

	for(int i=0;i<nbSpheres[0];i++)
		for(int j=0;j<nbSpheres[1];j++)
			for(int k=0;k<nbSpheres[2];k++)
			{
				shared_ptr<Body> sphere;
				createSphere(sphere,i,j,k);
				rootBody->bodies->insert(sphere);
			}
	
/////////////////////////////////////

	rootBody->persistentInteractions->clear();
	
	shared_ptr<Body> bodyA;
	rootBody->bodies->gotoFirst();
	rootBody->bodies->gotoNext(); // skips ground
	if (support1)
		rootBody->bodies->gotoNext(); // skips supportBox1
	if (support2)
		rootBody->bodies->gotoNext(); // skips supportBox2
		
		
	for( ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
	{
		bodyA = rootBody->bodies->getCurrent();
		
		rootBody->bodies->pushIterator();

		rootBody->bodies->gotoNext();
		for( ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
		{
			shared_ptr<Body> bodyB;
			bodyB = rootBody->bodies->getCurrent();

			shared_ptr<BodyMacroParameters> a = dynamic_pointer_cast<BodyMacroParameters>(bodyA->physicalParameters);
			shared_ptr<BodyMacroParameters> b = dynamic_pointer_cast<BodyMacroParameters>(bodyB->physicalParameters);
			shared_ptr<InteractingSphere>	as = dynamic_pointer_cast<InteractingSphere>(bodyA->interactionGeometry);
			shared_ptr<InteractingSphere>	bs = dynamic_pointer_cast<InteractingSphere>(bodyB->interactionGeometry);

			if ((a->se3.position - b->se3.position).length() < (as->radius + bs->radius))  
			{
				shared_ptr<Interaction> 		link(new Interaction( bodyA->getId() , bodyB->getId() ));
				shared_ptr<SDECLinkGeometry>		geometry(new SDECLinkGeometry);
				shared_ptr<SDECLinkPhysics>	physics(new SDECLinkPhysics);
				
				geometry->radius1			= as->radius - fabs(as->radius - bs->radius)*0.5;
				geometry->radius2			= bs->radius - fabs(as->radius - bs->radius)*0.5;

				physics->initialKn			= 50000000; // FIXME - BIG problem here.
				physics->initialKs			= 5000000;
				physics->heta				= 1;
				physics->initialEquilibriumDistance	= (a->se3.position - b->se3.position).length();
				physics->knMax				= 5500000;
				physics->ksMax				= 550000;

				link->interactionGeometry 		= geometry;
				link->interactionPhysics 		= physics;
				link->isReal 				= true;
				link->isNew 				= false;
				
				rootBody->persistentInteractions->insert(link);
			}
		}

		rootBody->bodies->popIterator();
	}
	
	return "total number of permament links created: " + lexical_cast<string>(rootBody->persistentInteractions->size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECLinkedSpheres::createSphere(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(0,55));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	
		
	Vector3r position 		= Vector3r(i,j,k)*spacing
					  - Vector3r(nbSpheres[0]/2*spacing,nbSpheres[1]/2*spacing-90,nbSpheres[2]/2*spacing) 
					  + Vector3r(Mathr::symmetricRandom()*disorder,Mathr::symmetricRandom()*disorder,Mathr::symmetricRandom()*disorder);

	Real radius 			= (Mathr::intervalRandom(minRadius,maxRadius));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

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

void SDECLinkedSpheres::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body(0,55));
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
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3f(1,1,1);

	body->boundingVolume		= aabb;
	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECLinkedSpheres::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<ActionParameterInitializer> actionParameterInitializer(new ActionParameterInitializer);
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterForce");
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterMomentum");
	
	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	interactionGeometryDispatcher->add("InteractingSphere","InteractingSphere","Sphere2Sphere4MacroMicroContactGeometry");
	interactionGeometryDispatcher->add("InteractingSphere","InteractingBox","Box2Sphere4MacroMicroContactGeometry");

	shared_ptr<InteractionPhysicsDispatcher> interactionPhysicsDispatcher(new InteractionPhysicsDispatcher);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");
		
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractingSphere","AABB","Sphere2AABBFunctor");
	boundingVolumeDispatcher->add("InteractingBox","AABB","Box2AABBFunctor");
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","InteractionDescriptionSet2AABBFunctor");
		
	shared_ptr<GravityCondition> gravityCondition(new GravityCondition);
	gravityCondition->gravity = gravity;
	
	shared_ptr<CundallNonViscousForceDampingFunctor> actionForceDamping(new CundallNonViscousForceDampingFunctor);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDampingFunctor> actionMomentumDamping(new CundallNonViscousMomentumDampingFunctor);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<ActionParameterDispatcher> actionDampingDispatcher(new ActionParameterDispatcher);
	actionDampingDispatcher->add("ActionParameterForce","ParticleParameters","CundallNonViscousForceDampingFunctor",actionForceDamping);
	actionDampingDispatcher->add("ActionParameterMomentum","RigidBodyParameters","CundallNonViscousMomentumDampingFunctor",actionMomentumDamping);
	
	shared_ptr<ActionParameterDispatcher> applyActionDispatcher(new ActionParameterDispatcher);
	applyActionDispatcher->add("ActionParameterForce","ParticleParameters","NewtonsForceLawFunctor");
	applyActionDispatcher->add("ActionParameterMomentum","RigidBodyParameters","NewtonsMomentumLawFunctor");
	
	shared_ptr<BodyPhysicalParametersDispatcher> positionIntegrator(new BodyPhysicalParametersDispatcher);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegratorFunctor");
	shared_ptr<BodyPhysicalParametersDispatcher> orientationIntegrator(new BodyPhysicalParametersDispatcher);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegratorFunctor");
 	
	shared_ptr<SDECTimeStepper> sdecTimeStepper(new SDECTimeStepper);
	sdecTimeStepper->sdecGroupMask = 55;
	sdecTimeStepper->interval = timeStepUpdateInterval;

	
	shared_ptr<ElasticContactLaw> constitutiveLaw(new ElasticContactLaw);
	constitutiveLaw->sdecGroupMask = 55;
	constitutiveLaw->momentRotationLaw = momentRotationLaw;
	
	rootBody->actors.clear();
	rootBody->actors.push_back(sdecTimeStepper);
	rootBody->actors.push_back(shared_ptr<Engine>(new ActionParameterReset));
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(interactionPhysicsDispatcher);
	rootBody->actors.push_back(constitutiveLaw);
	rootBody->actors.push_back(gravityCondition);
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(positionIntegrator);
	rootBody->actors.push_back(orientationIntegrator);

	rootBody->initializers.clear();
	rootBody->initializers.push_back(actionParameterInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
}
	



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECLinkedSpheres::positionRootBody(shared_ptr<MetaBody>& rootBody)
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
