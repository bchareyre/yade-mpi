#include "BoxStack.hpp"

#include "Rand.hpp" 
#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "NonConnexBody.hpp"
#include "SimpleSpringDynamicEngine.hpp"
#include "SAPCollider.hpp"
#include "SimpleNarrowCollider.hpp"
#include "RigidBody.hpp"
#include "BallisticDynamicEngine.hpp"
#include <fstream>
#include "IOManager.hpp"

BoxStack::BoxStack () : Serializable()
{
	exec();
}

BoxStack::~BoxStack ()
{

}

void BoxStack::processAttributes()
{
}

void BoxStack::registerAttributes()
{
}

void BoxStack::exec()
{
	shared_ptr<NonConnexBody> rootBody(new NonConnexBody);
	int nbSpheres = 10;
	int nbBox = 0;
	Quaternion q;
	q.fromAngleAxis(0, Vector3(0,0,1));
	
	rootBody->dynamic	   = shared_ptr<DynamicEngine>(new SimpleSpringDynamicEngine);
	rootBody->broadCollider		= shared_ptr<BroadCollider>(new SAPCollider);
	rootBody->narrowCollider	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	rootBody->isDynamic      = false;
	rootBody->velocity       = Vector3(0,0,0);
	rootBody->angularVelocity= Vector3(0,0,0);
	rootBody->se3		 = Se3(Vector3(0,0,0),q);
		
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
	box1->se3		= Se3(Vector3(0,0,0),q);
	aabb->color		= Vector3(1,0,0);
	aabb->center		= Vector3(0,0,10);
	aabb->halfSize		= Vector3(100,5,100);
	box1->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(100,5,100);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box1->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->gm		= dynamic_pointer_cast<CollisionGeometry>(box);

	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box1));

	
	int baseSize = 5;
	for(int i=0;i<baseSize;i++)
		for(int j=0;j<i;j++)
		{
			shared_ptr<RigidBody> boxi(new RigidBody);
			aabb=shared_ptr<AABB>(new AABB);
			box=shared_ptr<Box>(new Box);
			shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
			Vector3 size = Vector3(4,4,4);
			ballistic->damping 	= 0.95;
			boxi->dynamic		= dynamic_pointer_cast<DynamicEngine>(ballistic);
			boxi->isDynamic		= true;
			boxi->angularVelocity	= Vector3(0,0,0);
			boxi->velocity		= Vector3(0,0,0);
			float mass = 8*size[0]*size[1]*size[2];
			boxi->mass		= mass;
			boxi->inertia		= Vector3(mass*(size[1]*size[1]+size[2]*size[2])/3,mass*(size[0]*size[0]+size[2]*size[2])/3,mass*(size[1]*size[1]+size[0]*size[0])/3);
			//translation = Vector3(i,j,k)*10-Vector3(15,35,25)+Vector3(Rand::symmetricRandom(),Rand::symmetricRandom(),Rand::symmetricRandom())
			Vector3 translation = Vector3(i*10-baseSize*2+(baseSize-1)*j,j*8+9,0);
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
	
	
	IOManager::saveToFile("XMLManager", "../data/BoxStack.xml", "rootBody", rootBody);
}
