#include "SDECSpheresPlane.hpp"


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
#include "InteractionGeometrySet2AABBFactory.hpp"
#include "InteractionGeometrySet.hpp"
#include "SDECLinearContactModel.hpp"
#include "ActionDispatcher.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "SimpleBody.hpp"
#include "InteractionBox.hpp"
#include "InteractionSphere.hpp"
#include "TimeIntegratorDispatcher.hpp"

SDECSpheresPlane::SDECSpheresPlane () : FileGenerator()
{
	nbSpheres = 2;
	minRadius = 5;
	maxRadius = 5;
	kn = 100000;
	ks = 10000;
	groundSize = Vector3r(200,5,200);

}

SDECSpheresPlane::~SDECSpheresPlane ()
{

}

void SDECSpheresPlane::postProcessAttributes(bool)
{
}

void SDECSpheresPlane::registerAttributes()
{
	REGISTER_ATTRIBUTE(nbSpheres);
	REGISTER_ATTRIBUTE(minRadius);
	REGISTER_ATTRIBUTE(maxRadius);
	REGISTER_ATTRIBUTE(kn);
	REGISTER_ATTRIBUTE(ks);
	REGISTER_ATTRIBUTE(groundSize);
}

string SDECSpheresPlane::generate()
{
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<InteractionGeometryDispatcher> igd(new InteractionGeometryDispatcher);
	igd->addInteractionGeometryFunctor("InteractionSphere","InteractionSphere","Sphere2Sphere4SDECContactModel");
	igd->addInteractionGeometryFunctor("InteractionSphere","InteractionBox","Box2Sphere4SDECContactModel");

	shared_ptr<InteractionPhysicsDispatcher> ipd(new InteractionPhysicsDispatcher);
	ipd->addInteractionPhysicsFunctor("SDECDiscreteElement","SDECDiscreteElement","SDECLinearContactModel");
		
	shared_ptr<BoundingVolumeUpdator> bvu	= shared_ptr<BoundingVolumeUpdator>(new BoundingVolumeUpdator);
	bvu->addBVFactories("InteractionSphere","AABB","Sphere2AABBFactory");
	bvu->addBVFactories("InteractionBox","AABB","Box2AABBFactory");
	bvu->addBVFactories("InteractionGeometrySet","AABB","InteractionGeometrySet2AABBFactory");
	
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
	

	//FIXME : use a default one
	shared_ptr<Particle> physics2(new Particle); // FIXME : fix indexable class BodyPhysicalParameters
	physics2->se3		= Se3r(Vector3r(0,0,0),q);
	physics2->mass		= 0;
	physics2->velocity	= Vector3r(0,0,0);
	rootBody->physicalParameters = physics2;
		
	rootBody->permanentInteractions->clear();
//	rootBody->permanentInteractions[0] = shared_ptr<Interaction>(new Interaction);
//	rootBody->permanentInteractions[0]->interactionGeometry = shared_ptr<SDECPermanentLink>(new SDECPermanentLink);

	rootBody->isDynamic		= false;
	//rootBody->velocity		= Vector3r(0,0,0);
	//rootBody->angularVelocity	= Vector3r(0,0,0);
	//rootBody->se3			= Se3r(Vector3r(0,0,0),q);

	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;
	
	shared_ptr<InteractionGeometrySet> set(new InteractionGeometrySet());
	set->diffuseColor	= Vector3f(0,0,1);
	//set->wire		= false;
	//set->visible		= true;
	//set->shadowCaster	= false;
	rootBody->interactionGeometry		= dynamic_pointer_cast<BodyInteractionGeometry>(set);	
	
	aabb			= shared_ptr<AABB>(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);
	rootBody->boundingVolume		= dynamic_pointer_cast<BodyBoundingVolume>(aabb);

	
	shared_ptr<SimpleBody> box1(new SimpleBody);
	shared_ptr<SDECDiscreteElement> physics(new SDECDiscreteElement);
	
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	shared_ptr<InteractionBox> interactionBox(new InteractionBox);
	box1->isDynamic		= false;
	physics->angularVelocity= Vector3r(0,0,0);
	physics->velocity	= Vector3r(0,0,0);
	physics->mass		= 0;
	physics->inertia	= Vector3r(0,0,0);
	physics->se3		= Se3r(Vector3r(0,0,0),q);
	aabb->diffuseColor	= Vector3r(1,0,0);
	box1->boundingVolume		= dynamic_pointer_cast<BodyBoundingVolume>(aabb);
	
	box->extents		= groundSize;
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box->shadowCaster	= false;
	
	interactionBox->extents		= groundSize;
	interactionBox->diffuseColor	= Vector3f(1,1,1);
		
	box1->interactionGeometry		= dynamic_pointer_cast<BodyInteractionGeometry>(interactionBox);
	box1->geometricalModel		= dynamic_pointer_cast<BodyGeometricalModel>(box);
	physics->kn		= kn;
	physics->ks		= ks;

	box1->physicalParameters = physics;
	
	shared_ptr<Body> b;
	b = dynamic_pointer_cast<Body>(box1);
	rootBody->bodies->insert(b);

	Vector3r translation;

	for(int i=0;i<nbSpheres;i++)
		for(int j=0;j<nbSpheres;j++)
			for(int k=0;k<nbSpheres;k++)
	{
		shared_ptr<SimpleBody> simple(new SimpleBody);
		shared_ptr<SDECDiscreteElement> s(new SDECDiscreteElement);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> sphere(new Sphere);
		shared_ptr<InteractionSphere> interactionSphere(new InteractionSphere);
		
		translation 		= Vector3r(i,j,k)*(2*maxRadius*1.1)-Vector3r(nbSpheres/2*(2*maxRadius*1.1),-7-maxRadius*2,nbSpheres/2*(2*maxRadius*1.1))+Vector3r(Mathr::symmetricRandom()*1.1,Mathr::symmetricRandom()*1.1,Mathr::symmetricRandom()*1.1);
		Real radius 		= (Mathr::intervalRandom(minRadius,maxRadius));

// 		shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
// 		ballistic->damping 	= 1.0;//0.95;
// 		simple->actors.push_back(ballistic);

		simple->isDynamic		= true;
		s->angularVelocity	= Vector3r(0,0,0);
		s->velocity		= Vector3r(0,0,0);
		s->mass			= 4.0/3.0*Mathr::PI*radius*radius;
		s->inertia		= Vector3r(2.0/5.0*s->mass*radius*radius,2.0/5.0*s->mass*radius*radius,2.0/5.0*s->mass*radius*radius);
		s->se3			= Se3r(translation,q);

		aabb->diffuseColor		= Vector3r(0,1,0);
		simple->boundingVolume			= dynamic_pointer_cast<BodyBoundingVolume>(aabb);

		sphere->radius		= radius;
		sphere->diffuseColor	= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
		sphere->wire		= false;
		sphere->visible		= true;
		sphere->shadowCaster	= true;
		
		interactionSphere->radius	= radius;
		interactionSphere->diffuseColor	= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
		
		
		simple->interactionGeometry			= dynamic_pointer_cast<BodyInteractionGeometry>(interactionSphere);
		simple->geometricalModel			= dynamic_pointer_cast<BodyGeometricalModel>(sphere);
		s->kn			= kn;
		s->ks			= ks;
		simple->physicalParameters = s;
		
		b = dynamic_pointer_cast<Body>(simple);
		rootBody->bodies->insert(b);
	}
	
	return "";
}
