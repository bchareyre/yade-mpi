#include "BoxStack.hpp"


#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "ComplexBody.hpp"
#include "SimpleSpringDynamicEngine.hpp"
#include "SAPCollider.hpp"
#include "RigidBody.hpp"
#include <fstream>
#include "IOManager.hpp"

BoxStack::BoxStack () : FileGenerator()
{
}

BoxStack::~BoxStack ()
{

}

void BoxStack::postProcessAttributes(bool)
{
}

void BoxStack::registerAttributes()
{
}

string BoxStack::generate()
{


// 	// FIXME : not working
// 	rootBody = shared_ptr<ComplexBody>(new ComplexBody);
// 	Quaternionr q;
// 	q.fromAxisAngle( Vector3r(0,0,1),0);
// 
// 	shared_ptr<InteractionGeometryDispatcher> nc	= shared_ptr<InteractionGeometryDispatcher>(new SimpleNarrowCollider);
// 	nc->addCollisionFunctor("Box","Box","Box2Box4ClosestFeatures");
// 
// 
// 
// 	rootBody->actors.resize(3);
// 	rootBody->actors[0] 		= shared_ptr<Actor>(new SAPCollider);
// 	rootBody->actors[1] 		= nc;
// 	rootBody->actors[2] 		= shared_ptr<Actor>(new SimpleSpringDynamicEngine);
// 
// 
// 
// 	rootBody->isDynamic      = false;
// 	rootBody->velocity       = Vector3r(0,0,0);
// 	rootBody->angularVelocity= Vector3r(0,0,0);
// 	rootBody->se3		 = Se3r(Vector3r(0,0,0),q);
// 
// 	shared_ptr<AABB> aabb;
// 	shared_ptr<Box> box;
// 
// 	shared_ptr<RigidBody> box1(new RigidBody);
// 	aabb=shared_ptr<AABB>(new AABB);
// 	box=shared_ptr<Box>(new Box);
// 	box1->isDynamic		= false;
// 	box1->angularVelocity	= Vector3r(0,0,0);
// 	box1->velocity		= Vector3r(0,0,0);
// 	box1->mass		= 0;
// 	box1->inertia		= Vector3r(0,0,0);
// 	box1->se3		= Se3r(Vector3r(0,0,0),q);
// 	aabb->color		= Vector3r(1,0,0);
// 	aabb->center		= Vector3r(0,0,10);
// 	aabb->halfSize		= Vector3r(100,5,100);
// 	box1->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
// 	box->extents		= Vector3r(100,5,100);
// 	box->diffuseColor	= Vector3f(1,1,1);
// 	box->wire		= false;
// 	box->visible		= true;
// 	box1->interactionGeometry		= dynamic_pointer_cast<InteractionGeometry>(box);
// 	box1->geometricalModel		= dynamic_pointer_cast<InteractionGeometry>(box);
// 
// 	shared_ptr<Body> b;
// 	b=dynamic_pointer_cast<Body>(box1);
// 	rootBody->bodies->insert(b);
// 
// 
// 	int baseSize = 5;
// 	for(int i=0;i<baseSize;i++)
// 		for(int j=0;j<i;j++)
// 		{
// 			shared_ptr<RigidBody> boxi(new RigidBody);
// 			aabb=shared_ptr<AABB>(new AABB);
// 			box=shared_ptr<Box>(new Box);
// 			Vector3r size = Vector3r(4,4,4);
// 
// 			shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
// 			ballistic->damping 	= 0.95;
// 			boxi->actors.push_back(ballistic);
// 
// 			boxi->isDynamic		= true;
// 			boxi->angularVelocity	= Vector3r(0,0,0);
// 			boxi->velocity		= Vector3r(0,0,0);
// 			Real mass = 8*size[0]*size[1]*size[2];
// 			boxi->mass		= mass;
// 			boxi->inertia		= Vector3r(mass*(size[1]*size[1]+size[2]*size[2])/3,mass*(size[0]*size[0]+size[2]*size[2])/3,mass*(size[1]*size[1]+size[0]*size[0])/3);
// 			//translation = Vector3r(i,j,k)*10-Vector3r(15,35,25)+Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom())
// 			Vector3r translation = Vector3r(i*10-baseSize*2+(baseSize-1)*j,j*8+9,0);
// 			boxi->se3		= Se3r(translation,q);
// 			aabb->color		= Vector3r(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom());
// 			aabb->center		= translation;
// 			aabb->halfSize		= size;
// 			boxi->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
// 			box->extents		= size;
// 			box->diffuseColor	= Vector3f(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom());
// 			box->wire		= false;
// 			box->visible		= true;
// 			boxi->interactionGeometry		= dynamic_pointer_cast<InteractionGeometry>(box);
// 			boxi->geometricalModel		= dynamic_pointer_cast<InteractionGeometry>(box);
// 
// 			b=dynamic_pointer_cast<Body>(boxi);
// 			rootBody->bodies->insert(b);
// 		}

	return "";
}
