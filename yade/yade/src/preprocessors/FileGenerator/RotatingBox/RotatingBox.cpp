#include "RotatingBox.hpp"

#include "Rand.hpp" 
#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "NonConnexBody.hpp"
#include "SimpleSpringDynamicEngine.hpp"
#include "SAPCollider.hpp"
#include "Rotor.hpp"
#include "SimpleNarrowCollider.hpp"
#include "RigidBody.hpp"
#include "BallisticDynamicEngine.hpp"
#include <fstream>
#include "IOManager.hpp"

RotatingBox::RotatingBox () : Serializable()
{
	exec();
}

RotatingBox::~RotatingBox ()
{

}

void RotatingBox::processAttributes()
{
}

void RotatingBox::registerAttributes()
{
}

void RotatingBox::exec()
{
	shared_ptr<NonConnexBody> rootBody(new NonConnexBody);
	int nbSpheres = 1000;
	Quaternion q;
	q.fromAngleAxis(0, Vector3(0,0,1));
	
	rootBody->dynamic	   = shared_ptr<DynamicEngine>(new SimpleSpringDynamicEngine);
	rootBody->kinematic	   = shared_ptr<KinematicEngine>(new Rotor);
	rootBody->broadCollider		= shared_ptr<BroadPhaseCollider>(new SAPCollider);
	rootBody->narrowCollider	= shared_ptr<NarrowPhaseCollider>(new SimpleNarrowCollider);
	rootBody->isDynamic      = false;
	rootBody->velocity       = Vector3(0,0,0);
	rootBody->angularVelocity= Vector3(0,0,0);
	rootBody->se3		 = Se3(Vector3(0,0,0),q);

	for(int i=0;i<7;i++)
		rootBody->kinematic->subscribedBodies.push_back(i);
		
	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;
	
	shared_ptr<RigidBody> box1(new RigidBody);
	aabb=shared_ptr<AABB>(new AABB);
	box=shared_ptr<Box>(new Box);
	box1->isDynamic		= false;
	box1->angularVelocity	= Vector3(0,0,0);
	box1->velocity		= Vector3(0,0,0);
	box1->mass		= 0;
	box1->inertia		= Vector3(0,0,0);
	box1->se3		= Se3(Vector3(0,0,10),q);
	aabb->color		= Vector3(1,0,0);
	aabb->center		= Vector3(0,0,10);
	aabb->halfSize		= Vector3(50,5,40);
	box1->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(50,5,40);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box1->cm		= dynamic_pointer_cast<CollisionModel>(box);
	box1->gm		= dynamic_pointer_cast<CollisionModel>(box);

	shared_ptr<RigidBody> box2(new RigidBody);
	aabb=shared_ptr<AABB>(new AABB);
	box=shared_ptr<Box>(new Box);	
	box2->isDynamic		= false;
	box2->angularVelocity	= Vector3(0,0,0);
	box2->velocity		= Vector3(0,0,0);
	box2->mass		= 0;
	box2->inertia		= Vector3(0,0,0);
	box2->se3		= Se3(Vector3(-55,0,0),q);
	aabb->color		= Vector3(0,0,1);
	aabb->center		= Vector3(-55,0,0);
	aabb->halfSize		= Vector3(5,60,50);
	box2->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(5,60,50);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box2->cm		= dynamic_pointer_cast<CollisionModel>(box);
	box2->gm		= dynamic_pointer_cast<CollisionModel>(box);

	shared_ptr<RigidBody> box3(new RigidBody);
	aabb=shared_ptr<AABB>(new AABB);
	box=shared_ptr<Box>(new Box);
	box3->isDynamic		= false;
	box3->angularVelocity	= Vector3(0,0,0);
	box3->velocity		= Vector3(0,0,0);
	box3->mass		= 0;
	box3->inertia		= Vector3(0,0,0);
	box3->se3		= Se3(Vector3(55,0,0),q);
	aabb->color		= Vector3(0,0,1);
	aabb->center		= Vector3(55,0,0);
	aabb->halfSize		= Vector3(5,60,50);
	box3->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(5,60,50);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box3->cm		= dynamic_pointer_cast<CollisionModel>(box);
	box3->gm		= dynamic_pointer_cast<CollisionModel>(box);

	shared_ptr<RigidBody> box4(new RigidBody);
	aabb=shared_ptr<AABB>(new AABB);
	box=shared_ptr<Box>(new Box);	
	box4->isDynamic		= false;
	box4->angularVelocity	= Vector3(0,0,0);
	box4->velocity		= Vector3(0,0,0);
	box4->mass		= 0;
	box4->inertia		= Vector3(0,0,0);
	box4->se3		= Se3(Vector3(0,-55,0),q);
	aabb->color		= Vector3(0,0,1);
	aabb->center		= Vector3(0,-55,0);
	aabb->halfSize		= Vector3(50,5,50);
	box4->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(50,5,50);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box4->cm		= dynamic_pointer_cast<CollisionModel>(box);
	box4->gm		= dynamic_pointer_cast<CollisionModel>(box);

	shared_ptr<RigidBody> box5(new RigidBody);
	aabb=shared_ptr<AABB>(new AABB);
	box=shared_ptr<Box>(new Box);	
	box5->isDynamic		= false;
	box5->angularVelocity	= Vector3(0,0,0);
	box5->velocity		= Vector3(0,0,0);
	box5->mass		= 0;
	box5->inertia		= Vector3(0,0,0);
	box5->se3		= Se3(Vector3(0,55,0),q);
	aabb->color		= Vector3(0,0,1);
	aabb->center		= Vector3(0,55,0);
	aabb->halfSize		= Vector3(50,5,50);
	box5->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(50,5,50);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box5->cm		= dynamic_pointer_cast<CollisionModel>(box);
	box5->gm		= dynamic_pointer_cast<CollisionModel>(box);

	shared_ptr<RigidBody> box6(new RigidBody);
	aabb=shared_ptr<AABB>(new AABB);
	box=shared_ptr<Box>(new Box);
	box6->isDynamic		= false;
	box6->angularVelocity	= Vector3(0,0,0);
	box6->velocity		= Vector3(0,0,0);
	box6->mass		= 0;
	box6->inertia		= Vector3(0,0,0);
	box6->se3		= Se3(Vector3(0,0,-55),q);
	aabb->color		= Vector3(0,0,1);
	aabb->center		= Vector3(0,0,-55);
	aabb->halfSize		= Vector3(60,60,5);
	box6->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(60,60,5);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box6->cm		= dynamic_pointer_cast<CollisionModel>(box);
	box6->gm		= dynamic_pointer_cast<CollisionModel>(box);

	shared_ptr<RigidBody> box7(new RigidBody);
	aabb=shared_ptr<AABB>(new AABB);
	box=shared_ptr<Box>(new Box);	
	box7->isDynamic		= false;
	box7->angularVelocity	= Vector3(0,0,0);
	box7->velocity		= Vector3(0,0,0);
	box7->mass		= 0;
	box7->inertia		= Vector3(0,0,0);
	box7->se3		= Se3(Vector3(0,0,55),q);
	aabb->color		= Vector3(0,0,1);
	aabb->center		= Vector3(0,0,55);
	aabb->halfSize		= Vector3(60,60,5);
	box7->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(60,60,5);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box7->cm		= dynamic_pointer_cast<CollisionModel>(box);
	box7->gm		= dynamic_pointer_cast<CollisionModel>(box);



	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box1));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box2));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box3));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box4));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box5));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box6));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box7));

	for(int i=0;i<10;i++)
		for(int j=0;j<10;j++)
			for(int k=0;k<10;k++)
	{
		shared_ptr<RigidBody> s(new RigidBody);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> sphere(new Sphere);
		
		Vector3 translation;
		translation = Vector3(i,j,k)*10-Vector3(45,45,45)+Vector3(Rand::symmetricRandom(),Rand::symmetricRandom(),Rand::symmetricRandom());
		float radius = (4+Rand::symmetricRandom());
		
		s->dynamic		= shared_ptr<DynamicEngine>(new BallisticDynamicEngine);;
		s->isDynamic		= true;
		s->angularVelocity	= Vector3(0,0,0);
		s->velocity		= Vector3(0,0,0);
		s->mass			= 1;
		s->inertia		= Vector3(1,1,1);
		s->se3			= Se3(translation,q);

		aabb->color		= Vector3(0,1,0);
		aabb->center		= translation;
		aabb->halfSize		= Vector3(radius,radius,radius);
		s->bv			= dynamic_pointer_cast<BoundingVolume>(aabb);

		sphere->radius		= radius;
		sphere->diffuseColor	= Vector3(Rand::unitRandom(),Rand::unitRandom(),Rand::unitRandom());
		sphere->wire		= false;
		sphere->visible		= true;
		s->cm			= dynamic_pointer_cast<CollisionModel>(sphere);
		s->gm			= dynamic_pointer_cast<GeometricalModel>(sphere);

		rootBody->bodies.push_back(dynamic_pointer_cast<Body>(s));
	}

	IOManager::saveToFile("XMLManager", "../data/scene.xml", "rootBody", rootBody);
}
