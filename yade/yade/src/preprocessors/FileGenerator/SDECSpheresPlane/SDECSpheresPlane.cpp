#include "SDECSpheresPlane.hpp"

#include "Rand.hpp"
#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "NonConnexBody.hpp"
#include "SAPCollider.hpp"
#include "SimpleNarrowCollider.hpp"
#include "SDECDiscreteElement.hpp"
#include "BallisticDynamicEngine.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "SDECDynamicEngine.hpp"
#include "SDECDiscreteElement.hpp"

SDECSpheresPlane::SDECSpheresPlane () : Serializable()
{
	exec();
}

SDECSpheresPlane::~SDECSpheresPlane ()
{

}

void SDECSpheresPlane::processAttributes()
{
}

void SDECSpheresPlane::registerAttributes()
{
}

void SDECSpheresPlane::exec()
{
	shared_ptr<NonConnexBody> rootBody(new NonConnexBody);
	int nbSpheres = 9;
	Quaternion q;
	q.fromAngleAxis(0, Vector3(0,0,1));

	rootBody->dynamic		= shared_ptr<DynamicEngine>(new SDECDynamicEngine);
	//rootBody->kinematic		= shared_ptr<KinematicEngine>(new Rotor);
	rootBody->broadCollider		= shared_ptr<BroadCollider>(new SAPCollider);
	rootBody->narrowCollider	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);

	rootBody->narrowCollider->addCollisionFunctor("Sphere","Sphere","Sphere2Sphere4SDECContactModel");
	rootBody->narrowCollider->addCollisionFunctor("Sphere","Box","Box2Sphere4SDECContactModel");
	rootBody->isDynamic		= false;
	rootBody->velocity		= Vector3(0,0,0);
	rootBody->angularVelocity	= Vector3(0,0,0);
	rootBody->se3			= Se3(Vector3(0,0,0),q);

	//for(int i=0;i<7;i++)
	//	rootBody->kinematic->subscribedBodies.push_back(i);

	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;

	shared_ptr<SDECDiscreteElement> box1(new SDECDiscreteElement);

	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	box1->isDynamic		= false;
	box1->angularVelocity	= Vector3(0,0,0);
	box1->velocity		= Vector3(0,0,0);
	box1->mass		= 0;
	box1->inertia		= Vector3(0,0,0);
	box1->se3		= Se3(Vector3(0,0,0),q);
	aabb->color		= Vector3(1,0,0);
	aabb->center		= Vector3(0,0,10);
	aabb->halfSize		= Vector3(200,5,200);
	box1->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(200,5,200);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box1->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->kn		= 100000;
	box1->ks		= 10000;


	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box1));

	Vector3 translation;

	for(int i=0;i<nbSpheres;i++)
		for(int j=0;j<nbSpheres;j++)
			for(int k=0;k<nbSpheres;k++)
	{
		shared_ptr<SDECDiscreteElement> s(new SDECDiscreteElement);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> sphere(new Sphere);

		translation 		= Vector3(i,j,k)*10-Vector3(nbSpheres/2*10,nbSpheres/2*10-90,nbSpheres/2*10)+Vector3(Rand::symmetricRandom()*1.3,Rand::symmetricRandom(),Rand::symmetricRandom()*1.3);
		float radius 		= (Rand::intervalRandom(1,5));

		shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
		ballistic->damping 	= 1.0;//0.95;
		s->dynamic		= dynamic_pointer_cast<DynamicEngine>(ballistic);
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

	IOManager::saveToFile("XMLManager", "../data/SDECSpheresPlane.xml", "rootBody", rootBody);
}
