#include "RotatingBox.hpp"


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
#include "BoundingVolumeUpdator.hpp"
#include "CollisionGeometrySet2AABBFactory.hpp"
#include "CollisionGeometrySet.hpp"

RotatingBox::RotatingBox () : FileGenerator()
{
	nbSpheres = 0;
	nbBoxes   = 0;
	outputFileName = "../data/RotatingBox.xml";
	serializationDynlib = "XMLManager";

}

RotatingBox::~RotatingBox ()
{

}



void RotatingBox::registerAttributes()
{
	FileGenerator::registerAttributes();
	REGISTER_ATTRIBUTE(nbSpheres);
	REGISTER_ATTRIBUTE(nbBoxes);
}

string RotatingBox::generate()
{
	rootBody = shared_ptr<NonConnexBody>(new NonConnexBody);
	Quaternionr q;
	q.fromAxisAngle(Vector3r(0,0,1),0);

	shared_ptr<NarrowCollider> nc	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	nc->addCollisionFunctor("Sphere","Sphere","Sphere2Sphere4SDECContactModel");
	nc->addCollisionFunctor("Sphere","Box","Box2Sphere4SDECContactModel");
	
	shared_ptr<BoundingVolumeUpdator> bvu	= shared_ptr<BoundingVolumeUpdator>(new BoundingVolumeUpdator);
	bvu->addBVFactories("Sphere","AABB","Sphere2AABBFactory");
	bvu->addBVFactories("Box","AABB","Box2AABBFactory");
	bvu->addBVFactories("CollisionGeometrySet","AABB","CollisionGeometrySet2AABBFactory");


	shared_ptr<Rotor> kinematic = shared_ptr<Rotor>(new Rotor);
	kinematic->angularVelocity  = 0.0785375;
	kinematic->rotationAxis  = Vector3r(1,0,0);

	for(int i=0;i<7;i++)
		kinematic->subscribedBodies.push_back(i);

	rootBody->actors.resize(5);
	rootBody->actors[0] 		= bvu;	
	rootBody->actors[1] 		= shared_ptr<Actor>(new SAPCollider);
	rootBody->actors[2] 		= nc;
	rootBody->actors[3] 		= shared_ptr<Actor>(new SDECDynamicEngine);
	rootBody->actors[4] 		= kinematic;

	rootBody->isDynamic		= false;
	rootBody->velocity		= Vector3r(0,0,0);
	rootBody->angularVelocity	= Vector3r(0,0,0);
	rootBody->se3			= Se3r(Vector3r(0,0,0),q);

	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;

	//shared_ptr<SDECDiscreteElement> box1(new SDECDiscreteElement);
	shared_ptr<SDECDiscreteElement> box1(new SDECDiscreteElement);

	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	box1->isDynamic		= false;
	box1->angularVelocity	= Vector3r(0,0,0);
	box1->velocity		= Vector3r(0,0,0);
	box1->mass		= 0;
	box1->inertia		= Vector3r(0,0,0);
	box1->se3		= Se3r(Vector3r(0,0,10),q);
	aabb->color		= Vector3r(1,0,0);
	aabb->center		= Vector3r(0,0,10);
	aabb->halfSize		= Vector3r(50,5,40);
	box1->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(50,5,40);
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box->shadowCaster	= false;
	box1->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->kn		= 100000;
	box1->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box2(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	box2->isDynamic		= false;
	box2->angularVelocity	= Vector3r(0,0,0);
	box2->velocity		= Vector3r(0,0,0);
	box2->mass		= 0;
	box2->inertia		= Vector3r(0,0,0);
	box2->se3		= Se3r(Vector3r(-55,0,0),q);
	aabb->color		= Vector3r(0,0,1);
	aabb->center		= Vector3r(-55,0,0);
	aabb->halfSize		= Vector3r(5,60,50);
	box2->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(5,60,50);
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box->shadowCaster	= false;
	box2->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box2->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box2->kn		= 100000;
	box2->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box3(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	box3->isDynamic		= false;
	box3->angularVelocity	= Vector3r(0,0,0);
	box3->velocity		= Vector3r(0,0,0);
	box3->mass		= 0;
	box3->inertia		= Vector3r(0,0,0);
	box3->se3		= Se3r(Vector3r(55,0,0),q);
	aabb->color		= Vector3r(0,0,1);
	aabb->center		= Vector3r(55,0,0);
	aabb->halfSize		= Vector3r(5,60,50);
	box3->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(5,60,50);
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box->shadowCaster	= false;	
	box3->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box3->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box3->kn		= 100000;
	box3->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box4(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	box4->isDynamic		= false;
	box4->angularVelocity	= Vector3r(0,0,0);
	box4->velocity		= Vector3r(0,0,0);
	box4->mass		= 0;
	box4->inertia		= Vector3r(0,0,0);
	box4->se3		= Se3r(Vector3r(0,-55,0),q);
	aabb->color		= Vector3r(0,0,1);
	aabb->center		= Vector3r(0,-55,0);
	aabb->halfSize		= Vector3r(50,5,50);
	box4->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(50,5,50);
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box->shadowCaster	= false;		
	box4->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box4->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box4->kn		= 100000;
	box4->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box5(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	box5->isDynamic		= false;
	box5->angularVelocity	= Vector3r(0,0,0);
	box5->velocity		= Vector3r(0,0,0);
	box5->mass		= 0;
	box5->inertia		= Vector3r(0,0,0);
	box5->se3		= Se3r(Vector3r(0,55,0),q);
	aabb->color		= Vector3r(0,0,1);
	aabb->center		= Vector3r(0,55,0);
	aabb->halfSize		= Vector3r(50,5,50);
	box5->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(50,5,50);
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box->shadowCaster	= false;		
	box5->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box5->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box5->kn		= 100000;
	box5->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box6(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	box6->isDynamic		= false;
	box6->angularVelocity	= Vector3r(0,0,0);
	box6->velocity		= Vector3r(0,0,0);
	box6->mass		= 0;
	box6->inertia		= Vector3r(0,0,0);
	box6->se3		= Se3r(Vector3r(0,0,-55),q);
	aabb->color		= Vector3r(0,0,1);
	aabb->center		= Vector3r(0,0,-55);
	aabb->halfSize		= Vector3r(60,60,5);
	box6->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(60,60,5);
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box->shadowCaster	= false;		
	box6->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box6->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box6->kn		= 100000;
	box6->ks		= 10000;

	shared_ptr<SDECDiscreteElement> box7(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	box7->isDynamic		= false;
	box7->angularVelocity	= Vector3r(0,0,0);
	box7->velocity		= Vector3r(0,0,0);
	box7->mass		= 0;
	box7->inertia		= Vector3r(0,0,0);
	box7->se3		= Se3r(Vector3r(0,0,55),q);
	aabb->color		= Vector3r(0,0,1);
	aabb->center		= Vector3r(0,0,55);
	aabb->halfSize		= Vector3r(60,60,5);
	box7->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(60,60,5);
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= true;
	box->visible		= true;
	box->shadowCaster	= false;		
	box7->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box7->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box7->kn		= 100000;
	box7->ks		= 10000;

	shared_ptr<Body> b;
	b = dynamic_pointer_cast<Body>(box1);   rootBody->bodies->insert(b);
	b = dynamic_pointer_cast<Body>(box2);   rootBody->bodies->insert(b);
	b = dynamic_pointer_cast<Body>(box3);   rootBody->bodies->insert(b);
	b = dynamic_pointer_cast<Body>(box4);   rootBody->bodies->insert(b);
	b = dynamic_pointer_cast<Body>(box5);   rootBody->bodies->insert(b);
	b = dynamic_pointer_cast<Body>(box6);   rootBody->bodies->insert(b);
	b = dynamic_pointer_cast<Body>(box7);   rootBody->bodies->insert(b);

	Vector3r translation;

	for(int i=0;i<nbSpheres;i++)
		for(int j=0;j<nbSpheres;j++)
			for(int k=0;k<nbSpheres;k++)
	{
		shared_ptr<SDECDiscreteElement> s(new SDECDiscreteElement);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> sphere(new Sphere);

		translation 		= Vector3r(i,j,k)*10-Vector3r(45,45,45)+Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom());
		float radius 		= (Mathr::intervalRandom(3,4));

		shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
		ballistic->damping 	= 1.0;//0.95;
		s->actors.push_back(ballistic);

		//s->dynamic		= dynamic_pointer_cast<DynamicEngine>(ballistic);
		s->isDynamic		= true;
		s->angularVelocity	= Vector3r(0,0,0);
		s->velocity		= Vector3r(0,0,0);
		s->mass			= 4.0/3.0*Mathr::PI*radius*radius;
		s->inertia		= Vector3r(2.0/5.0*s->mass*radius*radius,2.0/5.0*s->mass*radius*radius,2.0/5.0*s->mass*radius*radius);
		s->se3			= Se3r(translation,q);

		aabb->color		= Vector3r(0,1,0);
		aabb->center		= translation;
		aabb->halfSize		= Vector3r(radius,radius,radius);
		s->bv			= dynamic_pointer_cast<BoundingVolume>(aabb);

		sphere->radius		= radius;
		sphere->diffuseColor	= Vector3f(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom());
		sphere->wire		= false;
		sphere->visible		= true;
		sphere->shadowCaster	= true;	
		s->cm			= dynamic_pointer_cast<CollisionGeometry>(sphere);
		s->gm			= dynamic_pointer_cast<GeometricalModel>(sphere);
		s->kn			= 100000;
		s->ks			= 10000;

		b = dynamic_pointer_cast<Body>(s);
		rootBody->bodies->insert(b);
	}


	for(int i=0;i<nbBoxes;i++)
		for(int j=0;j<nbBoxes;j++)
			for(int k=0;k<nbBoxes;k++)
			{
				shared_ptr<SDECDiscreteElement> boxi(new SDECDiscreteElement);
				aabb=shared_ptr<AABB>(new AABB);
				box=shared_ptr<Box>(new Box);

				shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
				ballistic->damping 	= 1.0;//0.95;
				boxi->actors.push_back(ballistic);

				Vector3r size = Vector3r((4+Mathr::symmetricRandom()),(4+Mathr::symmetricRandom()),(4+Mathr::symmetricRandom()));
				//ballistic->damping 	= 1.0;//0.95;
				//boxi->dynamic		= dynamic_pointer_cast<DynamicEngine>(ballistic);
				boxi->isDynamic		= true;
				boxi->angularVelocity	= Vector3r(0,0,0);
				boxi->velocity		= Vector3r(0,0,0);
				float mass = 8*size[0]*size[1]*size[2];
				boxi->mass		= mass;
				boxi->inertia		= Vector3r(mass*(size[1]*size[1]+size[2]*size[2])/3,mass*(size[0]*size[0]+size[2]*size[2])/3,mass*(size[1]*size[1]+size[0]*size[0])/3);
				translation = Vector3r(i,j,k)*10-Vector3r(15,35,25)+Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom());
				boxi->se3		= Se3r(translation,q);
				aabb->color		= Vector3r(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom());
				aabb->center		= translation;
				aabb->halfSize		= size;
				boxi->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
				box->extents		= size;
				box->diffuseColor	= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
				box->wire		= false;
				box->visible		= true;
				box->shadowCaster	= true;	
				boxi->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
				boxi->gm		= dynamic_pointer_cast<CollisionGeometry>(box);

				b=dynamic_pointer_cast<Body>(boxi);
				rootBody->bodies->insert(b);
			}
	return "";
}
