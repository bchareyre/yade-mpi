#include "SDECSpheresPlane.hpp"


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
#include "SDECPermanentLink.hpp"
#include "Interaction.hpp"


SDECSpheresPlane::SDECSpheresPlane () : FileGenerator()
{
	nbSpheres = 2;
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
}

void SDECSpheresPlane::generate()
{
	shared_ptr<NonConnexBody> rootBody(new NonConnexBody);

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<NarrowCollider> nc	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	nc->addCollisionFunctor("Sphere","Sphere","Sphere2Sphere4SDECContactModel");
	nc->addCollisionFunctor("Sphere","Box","Box2Sphere4SDECContactModel");

	rootBody->actors.resize(3);
	rootBody->actors[0] 		= shared_ptr<Actor>(new SAPCollider);
	rootBody->actors[1] 		= nc;
	rootBody->actors[2] 		= shared_ptr<Actor>(new SDECDynamicEngine);

	rootBody->permanentInteractions->clear();
//	rootBody->permanentInteractions[0] = shared_ptr<Interaction>(new Interaction);
//	rootBody->permanentInteractions[0]->interactionGeometry = shared_ptr<SDECPermanentLink>(new SDECPermanentLink);

	rootBody->isDynamic		= false;
	rootBody->velocity		= Vector3r(0,0,0);
	rootBody->angularVelocity	= Vector3r(0,0,0);
	rootBody->se3			= Se3r(Vector3r(0,0,0),q);

	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;

	shared_ptr<SDECDiscreteElement> box1(new SDECDiscreteElement);

	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	box1->isDynamic		= false;
	box1->angularVelocity	= Vector3r(0,0,0);
	box1->velocity		= Vector3r(0,0,0);
	box1->mass		= 0;
	box1->inertia		= Vector3r(0,0,0);
	box1->se3		= Se3r(Vector3r(0,0,0),q);
	aabb->color		= Vector3r(1,0,0);
	aabb->center		= Vector3r(0,0,10);
	aabb->halfSize		= Vector3r(200,5,200);
	box1->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(200,5,200);
	box->diffuseColor	= Vector3r(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box1->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->kn		= 100000;
	box1->ks		= 10000;


	shared_ptr<Body> b;
	b = dynamic_pointer_cast<Body>(box1);
	rootBody->bodies->insert(b);

	Vector3r translation;

	for(int i=0;i<nbSpheres;i++)
		for(int j=0;j<nbSpheres;j++)
			for(int k=0;k<nbSpheres;k++)
	{
		shared_ptr<SDECDiscreteElement> s(new SDECDiscreteElement);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> sphere(new Sphere);

		translation 		= Vector3r(i,j,k)*10-Vector3r(nbSpheres/2*10,nbSpheres/2*10-90,nbSpheres/2*10)+Vector3r(Mathr::symmetricRandom()*1.3,Mathr::symmetricRandom(),Mathr::symmetricRandom()*1.3);
		float radius 		= (Mathr::intervalRandom(1,5));

		shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
		ballistic->damping 	= 1.0;//0.95;
		s->actors.push_back(ballistic);

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
		sphere->diffuseColor	= Vector3r(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom());
		sphere->wire		= false;
		sphere->visible		= true;
		s->cm			= dynamic_pointer_cast<CollisionGeometry>(sphere);
		s->gm			= dynamic_pointer_cast<GeometricalModel>(sphere);
		s->kn			= 100000;
		s->ks			= 10000;

		b = dynamic_pointer_cast<Body>(s);
		rootBody->bodies->insert(b);
	}


	IOManager::saveToFile("XMLManager", "../data/SDECSpheresPlane.xml", "rootBody", rootBody);
}
