#include "SDECLinkedSpheres.hpp"

#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "ComplexBody.hpp"
#include "SAPCollider.hpp"
#include "PersistentSAPCollider.hpp"
#include "SDECDiscreteElement.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "SDECDynamicEngine.hpp"
#include "SDECDiscreteElement.hpp"
#include "SDECPermanentLink.hpp"
#include "Interaction.hpp"
#include "BoundingVolumeUpdator.hpp"
#include "CollisionGeometrySet2AABBFactory.hpp"
#include "CollisionGeometrySet.hpp"
#include "SDECLinearContactModel.hpp"
#include "ActionDispatcher.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "SimpleBody.hpp"
#include "InteractionBox.hpp"
#include "InteractionSphere.hpp"
#include "TimeIntegratorDispatcher.hpp"
#include "SDECPermanentLinkPhysics.hpp"

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
	REGISTER_ATTRIBUTE(disorder);
	REGISTER_ATTRIBUTE(spacing);
	REGISTER_ATTRIBUTE(supportSize);
	REGISTER_ATTRIBUTE(support1);
	REGISTER_ATTRIBUTE(support2);
}

string SDECLinkedSpheres::generate()
{
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);

	shared_ptr<InteractionGeometryDispatcher> igd(new InteractionGeometryDispatcher);
	igd->addInteractionGeometryFunctor("InteractionSphere","InteractionSphere","Sphere2Sphere4SDECContactModel");
	igd->addInteractionGeometryFunctor("InteractionSphere","InteractionBox","Box2Sphere4SDECContactModel");

	shared_ptr<InteractionPhysicsDispatcher> ipd(new InteractionPhysicsDispatcher);
	ipd->addInteractionPhysicsFunctor("SDECDiscreteElement","SDECDiscreteElement","SDECLinearContactModel");
		
	shared_ptr<BoundingVolumeUpdator> bvu	= shared_ptr<BoundingVolumeUpdator>(new BoundingVolumeUpdator);
	bvu->addBVFactories("InteractionSphere","AABB","Sphere2AABBFactory");
	bvu->addBVFactories("InteractionBox","AABB","Box2AABBFactory");
	bvu->addBVFactories("CollisionGeometrySet","AABB","CollisionGeometrySet2AABBFactory");
	
	shared_ptr<ActionDispatcher> ad(new ActionDispatcher);
	ad->addActionFunctor("ActionForce","RigidBody","ActionForce2RigidBody");
	ad->addActionFunctor("ActionMomentum","RigidBody","ActionMomentum2RigidBody");
	
	shared_ptr<TimeIntegratorDispatcher> ti(new TimeIntegratorDispatcher);
	ti->addTimeIntegratorFunctor("SDECDiscreteElement","LeapFrogIntegrator");
	
	rootBody->actors.resize(7);
	rootBody->actors[0] 		= bvu;	
	rootBody->actors[1] 		= shared_ptr<Actor>(new PersistentSAPCollider);
	rootBody->actors[2] 		= igd;
	rootBody->actors[3] 		= ipd;
	rootBody->actors[4] 		= shared_ptr<Actor>(new SDECDynamicEngine);
	rootBody->actors[5] 		= ad;
	rootBody->actors[6] 		= ti;
	
	rootBody->isDynamic		= false;
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<Particle> physics(new Particle); // FIXME : fix indexable class BodyPhysicalParameters
	physics->se3		= Se3r(Vector3r(0,0,0),q);
	physics->mass		= 0;
	physics->velocity	= Vector3r::ZERO;
	physics->acceleration	= Vector3r::ZERO;
	
	shared_ptr<CollisionGeometrySet> set(new CollisionGeometrySet());
	set->diffuseColor	= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor	= Vector3r(0,0,1);
	
	rootBody->interactionGeometry		= dynamic_pointer_cast<CollisionGeometry>(set);	
	rootBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->physicalParameters = physics;
	
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

	rootBody->permanentInteractions->clear();
	
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

			shared_ptr<SDECDiscreteElement> a = dynamic_pointer_cast<SDECDiscreteElement>(bodyA->physicalParameters);
			shared_ptr<SDECDiscreteElement> b = dynamic_pointer_cast<SDECDiscreteElement>(bodyB->physicalParameters);
			shared_ptr<InteractionSphere>	as = dynamic_pointer_cast<InteractionSphere>(bodyA->interactionGeometry);
			shared_ptr<InteractionSphere>	bs = dynamic_pointer_cast<InteractionSphere>(bodyB->interactionGeometry);

			if ((a->se3.translation - b->se3.translation).length() < (as->radius + bs->radius))  
			{
				shared_ptr<Interaction> 		link(new Interaction( bodyA->getId() , bodyB->getId() ));
				shared_ptr<SDECPermanentLink>		geometry(new SDECPermanentLink);
				shared_ptr<SDECPermanentLinkPhysics>	physics(new SDECPermanentLinkPhysics);
				
				geometry->radius1			= as->radius - fabs(as->radius - bs->radius)*0.5;
				geometry->radius2			= bs->radius - fabs(as->radius - bs->radius)*0.5;

				physics->initialKn			= 500000;
				physics->initialKs			= 50000;
				physics->heta				= 1;
				physics->initialEquilibriumDistance	= (a->se3.translation - b->se3.translation).length();
				physics->knMax				= 75000;
				physics->ksMax				= 7500;

				link->interactionGeometry 		= geometry;
				link->interactionPhysics 		= physics;
				link->isReal 				= true;
				link->isNew 				= false;
				
				rootBody->permanentInteractions->insert(link);
			}
		}

		rootBody->bodies->popIterator();
	}
	
	return "total number of permament links created: " + lexical_cast<string>(rootBody->permanentInteractions->size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECLinkedSpheres::createSphere(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new SimpleBody);
	shared_ptr<SDECDiscreteElement> physics(new SDECDiscreteElement);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractionSphere> iSphere(new InteractionSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	
		
	Vector3r translation 	= Vector3r(i,j,k)*spacing-Vector3r(nbSpheres[0]/2*spacing,nbSpheres[1]/2*spacing-90,nbSpheres[2]/2*spacing) +
				  Vector3r(Mathr::symmetricRandom()*disorder,Mathr::symmetricRandom()*disorder,Mathr::symmetricRandom()*disorder);

	Real radius 		= (Mathr::intervalRandom(minRadius,maxRadius));
	
	body->isDynamic		= true;
	
	physics->angularVelocity= Vector3r(0,0,0);
	physics->velocity	= Vector3r(0,0,0);
	physics->mass		= 4.0/3.0*Mathr::PI*radius*radius; //*density
	physics->inertia	= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3		= Se3r(translation,q);
	physics->kn		= 100000;
	physics->ks		= 10000;

	aabb->diffuseColor	= Vector3r(0,1,0);


	gSphere->radius		= radius;
	gSphere->diffuseColor	= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gSphere->wire		= false;
	gSphere->visible	= true;
	gSphere->shadowCaster	= true;
	
	iSphere->radius		= radius;
	iSphere->diffuseColor	= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());

	body->interactionGeometry		= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECLinkedSpheres::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new SimpleBody);
	shared_ptr<SDECDiscreteElement> physics(new SDECDiscreteElement);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractionBox> iBox(new InteractionBox);
	
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 0;
	physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);
	physics->kn			= 100000;
	physics->ks			= 10000;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3f(1,1,1);

	body->boundingVolume			= aabb;
	body->interactionGeometry			= iBox;
	body->geometricalModel			= gBox;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
