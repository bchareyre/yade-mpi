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
#include "SDECDiscreteElement.hpp"
#include "BallisticDynamicEngine.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "SDECDynamicEngine.hpp"
#include "SDECDiscreteElement.hpp"

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
	int nbSpheres = 10;
	int nbBox = 0;
	Quaternion q;
	q.fromAngleAxis(0, Vector3(0,0,1));


	
	shared_ptr<NarrowCollider> nc	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	nc->addCollisionFunctor("Sphere","Sphere","Sphere2Sphere4SDECContactModel");
	nc->addCollisionFunctor("Sphere","Box","Box2Sphere4SDECContactModel");
	

	shared_ptr<Rotor> kinematic = shared_ptr<Rotor>(new Rotor);
	kinematic->angularVelocity  = 0.0785375;
	
	for(int i=0;i<7;i++)
		kinematic->subscribedBodies.push_back(i);

	rootBody->actors.resize(4);
	rootBody->actors[0] 		= shared_ptr<Actor>(new SAPCollider);
	rootBody->actors[1] 		= nc;
	rootBody->actors[2] 		= shared_ptr<Actor>(new SDECDynamicEngine);
	rootBody->actors[3] 		= kinematic;
	
	rootBody->isDynamic		= false;
	rootBody->velocity		= Vector3(0,0,0);
	rootBody->angularVelocity	= Vector3(0,0,0);
	rootBody->se3			= Se3(Vector3(0,0,0),q);

	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;

	//shared_ptr<SDECDiscreteElement> box1(new SDECDiscreteElement);
	shared_ptr<SDECDiscreteElement> box1(new SDECDiscreteElement);

	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
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
	box1->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->kn		= 100000;
	box1->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box2(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
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
	box2->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box2->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box2->kn		= 100000;
	box2->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box3(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
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
	box3->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box3->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box3->kn		= 100000;
	box3->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box4(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
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
	box4->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box4->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box4->kn		= 100000;
	box4->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box5(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
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
	box5->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box5->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box5->kn		= 100000;
	box5->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box6(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
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
	box6->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box6->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box6->kn		= 100000;
	box6->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box7(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
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
	box7->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box7->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box7->kn		= 100000;
	box7->ks		= 10000;

	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box1));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box2));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box3));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box4));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box5));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box6));
 	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box7));

	Vector3 translation;

	for(int i=0;i<nbSpheres;i++)
		for(int j=0;j<nbSpheres;j++)
			for(int k=0;k<nbSpheres;k++)
	{
		shared_ptr<SDECDiscreteElement> s(new SDECDiscreteElement);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> sphere(new Sphere);

		translation 		= Vector3(i,j,k)*10-Vector3(45,45,45)+Vector3(Rand::symmetricRandom(),Rand::symmetricRandom(),Rand::symmetricRandom());
		float radius 		= (Rand::intervalRandom(3,4));

		shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
		ballistic->damping 	= 1.0;//0.95;		
		s->actors.push_back(ballistic);
		
		//s->dynamic		= dynamic_pointer_cast<DynamicEngine>(ballistic);
		s->isDynamic		= true;
		s->angularVelocity	= Vector3(0,0,0);
		s->velocity		= Vector3(0,0,0);
		s->mass			= 4.0/3.0*Constants::PI*radius*radius;
		s->inertia		= Vector3(2.0/5.0*s->mass*radius*radius,2.0/5.0*s->mass*radius*radius,2.0/5.0*s->mass*radius*radius);
		s->se3			= Se3(translation,q);

		aabb->color		= Vector3(0,1,0);
		aabb->center		= translation;
		aabb->halfSize		= Vector3(radius,radius,radius);
		s->bv			= dynamic_pointer_cast<BoundingVolume>(aabb);

		sphere->radius		= radius;
		sphere->diffuseColor	= Vector3(Rand::unitRandom(),Rand::unitRandom(),Rand::unitRandom());
		sphere->wire		= false;
		sphere->visible		= true;
		s->cm			= dynamic_pointer_cast<CollisionGeometry>(sphere);
		s->gm			= dynamic_pointer_cast<GeometricalModel>(sphere);
		s->kn			= 100000;
		s->ks			= 10000;

		rootBody->bodies.push_back(dynamic_pointer_cast<Body>(s));
	}


	for(int i=0;i<nbBox;i++)
		for(int j=0;j<nbBox;j++)
			for(int k=0;k<nbBox;k++)
			{
				shared_ptr<SDECDiscreteElement> boxi(new SDECDiscreteElement);
				aabb=shared_ptr<AABB>(new AABB);
				box=shared_ptr<Box>(new Box);

				shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
				ballistic->damping 	= 1.0;//0.95;		
				boxi->actors.push_back(ballistic);

				Vector3 size = Vector3((4+Rand::symmetricRandom()),(4+Rand::symmetricRandom()),(4+Rand::symmetricRandom()));
				//ballistic->damping 	= 1.0;//0.95;
				//boxi->dynamic		= dynamic_pointer_cast<DynamicEngine>(ballistic);
				boxi->isDynamic		= true;
				boxi->angularVelocity	= Vector3(0,0,0);
				boxi->velocity		= Vector3(0,0,0);
				float mass = 8*size[0]*size[1]*size[2];
				boxi->mass		= mass;
				boxi->inertia		= Vector3(mass*(size[1]*size[1]+size[2]*size[2])/3,mass*(size[0]*size[0]+size[2]*size[2])/3,mass*(size[1]*size[1]+size[0]*size[0])/3);
				translation = Vector3(i,j,k)*10-Vector3(15,35,25)+Vector3(Rand::symmetricRandom(),Rand::symmetricRandom(),Rand::symmetricRandom());
				boxi->se3		= Se3(translation,q);
				aabb->color		= Vector3(Rand::unitRandom(),Rand::unitRandom(),Rand::unitRandom());
				aabb->center		= translation;
				aabb->halfSize		= size;
				boxi->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
				box->extents		= size;
				box->diffuseColor	= Vector3(Rand::unitRandom(),Rand::unitRandom(),Rand::unitRandom());
				box->wire		= false;
				box->visible		= true;
				boxi->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
				boxi->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
				rootBody->bodies.push_back(dynamic_pointer_cast<Body>(boxi));
			}

	IOManager::saveToFile("XMLManager", "../data/RotatingBox.xml", "rootBody", rootBody);
}
