#include "SDECLinkedSpheres.hpp"

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

SDECLinkedSpheres::SDECLinkedSpheres () : FileGenerator()
{
	nbSpheresX = 3;
	nbSpheresY = 4;
	nbSpheresZ = 7;
	minRadius = 5.01;
	maxRadius = 5.01;
	disorder = 0;
	spacing = 10;
}

SDECLinkedSpheres::~SDECLinkedSpheres ()
{

}

void SDECLinkedSpheres::postProcessAttributes(bool)
{
}

void SDECLinkedSpheres::registerAttributes()
{
	REGISTER_ATTRIBUTE(nbSpheresX);
	REGISTER_ATTRIBUTE(nbSpheresY);
	REGISTER_ATTRIBUTE(nbSpheresZ);
	REGISTER_ATTRIBUTE(minRadius);
	REGISTER_ATTRIBUTE(maxRadius);
	REGISTER_ATTRIBUTE(disorder);
	REGISTER_ATTRIBUTE(spacing);
}

void SDECLinkedSpheres::generate()
{
	shared_ptr<NonConnexBody> rootBody(new NonConnexBody);
	
	Quaternionr q,qbox;
	q.fromAxisAngle( Vector3r(0,0,1),0);
//	qbox.fromAxisAngle( Vector3r(0,0,1),0.6); 		// FIXME - damping must be 0.01, and timestep must be 0.001

	shared_ptr<NarrowCollider> nc	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	nc->addCollisionFunctor("Sphere","Sphere","Sphere2Sphere4SDECContactModel");
	nc->addCollisionFunctor("Sphere","Box","Box2Sphere4SDECContactModel");

	rootBody->actors.resize(3);
	rootBody->actors[0] 		= shared_ptr<Actor>(new SAPCollider);
	rootBody->actors[1] 		= nc;
	rootBody->actors[2] 		= shared_ptr<Actor>(new SDECDynamicEngine);

	rootBody->permanentInteractions->clear();

	rootBody->isDynamic		= false;
	rootBody->velocity		= Vector3r(0,0,0);
	rootBody->angularVelocity	= Vector3r(0,0,0);
	rootBody->se3			= Se3r(Vector3r(0,0,0),q);

	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;


////////////////////////////////////
	
	shared_ptr<SDECDiscreteElement> box1(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	
	box1->isDynamic		= false;
	box1->angularVelocity	= Vector3r(0,0,0);
	box1->velocity		= Vector3r(0,0,0);
	box1->mass		= 0;
	box1->inertia		= Vector3r(0,0,0);
	box1->se3		= Se3r(Vector3r(0,0,0),q);
	//box1->se3		= Se3r(Vector3r(0,0,0),qbox); 	// FIXME - damping must be 0.01, and timestep must be 0.001
	aabb->color		= Vector3r(1,0,0);
	aabb->center		= Vector3r(0,0,10);
	aabb->halfSize		= Vector3r(200,5,200); 		// FIXME - this must be automatically computed
//	aabb->halfSize		= Vector3r(200,200,200); 	// FIXME - damping must be 0.01, and timestep must be 0.001
	box1->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(200,5,200);
	box->diffuseColor	= Vector3r(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box->shadowCaster	= false;
	box1->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->kn		= 100000;
	box1->ks		= 10000;
	shared_ptr<Body> b;
	b = dynamic_pointer_cast<Body>(box1);
	rootBody->bodies->insert(b);
	
/* FIXME : MOMENT LAW is completely not working!!!!!!!

/////////////////////////////////////
	
	shared_ptr<SDECDiscreteElement> box2(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	
	box2->isDynamic		= false;
	box2->angularVelocity	= Vector3r(0,0,0);
	box2->velocity		= Vector3r(0,0,0);
	box2->mass		= 0;
	box2->inertia		= Vector3r(0,0,0);
	box2->se3		= Se3r(Vector3r(0,0,40),q);
	aabb->color		= Vector3r(1,0,0);
	aabb->center		= Vector3r(0,0,40);
	aabb->halfSize		= Vector3r(20,50,20); 		// FIXME - this must be automatically computed
	box2->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(20,50,20);
	box->diffuseColor	= Vector3r(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box->shadowCaster	= false;
	box2->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box2->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box2->kn		= 100000;
	box2->ks		= 10000;
	shared_ptr<Body> b2;
	b2 = dynamic_pointer_cast<Body>(box2);
	rootBody->bodies->insert(b2);

/////////////////////////////////////

	shared_ptr<SDECDiscreteElement> box3(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);
	
	box3->isDynamic		= false;
	box3->angularVelocity	= Vector3r(0,0,0);
	box3->velocity		= Vector3r(0,0,0);
	box3->mass		= 0;
	box3->inertia		= Vector3r(0,0,0);
	box3->se3		= Se3r(Vector3r(0,0,-40),q);
	aabb->color		= Vector3r(1,0,0);
	aabb->center		= Vector3r(0,0,-40);
	aabb->halfSize		= Vector3r(20,50,20); 		// FIXME - this must be automatically computed
	box3->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(20,50,20);
	box->diffuseColor	= Vector3r(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box->shadowCaster	= false;
	box3->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box3->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box3->kn		= 100000;
	box3->ks		= 10000;
	shared_ptr<Body> b3;
	b3 = dynamic_pointer_cast<Body>(box3);
	rootBody->bodies->insert(b3);

/////////////////////////////////////
*/
	Vector3r translation;

	for(int i=0;i<nbSpheresX;i++)
		for(int j=0;j<nbSpheresY;j++)
			for(int k=0;k<nbSpheresZ;k++)
	{
		shared_ptr<SDECDiscreteElement> s(new SDECDiscreteElement);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> sphere(new Sphere);

		translation 		= Vector3r(i,j,k)*spacing-Vector3r(nbSpheresX/2*spacing,nbSpheresY/2*spacing-90,nbSpheresZ/2*spacing)
		+
		Vector3r(Mathr::symmetricRandom()*disorder,Mathr::symmetricRandom()*disorder,Mathr::symmetricRandom()*disorder);
		
		float radius 		= (Mathr::intervalRandom(minRadius,maxRadius));
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
		sphere->shadowCaster	= true;
		
		s->cm			= dynamic_pointer_cast<CollisionGeometry>(sphere);
		s->gm			= dynamic_pointer_cast<GeometricalModel>(sphere);
		s->kn			= 100000;
		s->ks			= 10000;

		b = dynamic_pointer_cast<Body>(s);
		rootBody->bodies->insert(b);
	}

	shared_ptr<Body> bodyA;
	for( rootBody->bodies->gotoFirst() ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
	{
		bodyA = rootBody->bodies->getCurrent();
		rootBody->bodies->pushIterator();

		rootBody->bodies->gotoNext();
		for( ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
		{
			shared_ptr<Body> bodyB;
			bodyB = rootBody->bodies->getCurrent();

			shared_ptr<SDECDiscreteElement> a = dynamic_pointer_cast<SDECDiscreteElement>(bodyA);
			shared_ptr<SDECDiscreteElement> b = dynamic_pointer_cast<SDECDiscreteElement>(bodyB);
			shared_ptr<Sphere>             as = dynamic_pointer_cast<Sphere>(a->cm);
			shared_ptr<Sphere>             bs = dynamic_pointer_cast<Sphere>(a->cm);

			if(
				a &&
				b &&
				as &&
				bs &&
				( (a->se3.translation - b->se3.translation).length() < (as->radius + bs->radius) )
			)

			{
				shared_ptr<Interaction> 	c(new Interaction( a->getId() , b->getId() ));
				shared_ptr<SDECPermanentLink>	link(new SDECPermanentLink);

				link->initialKn			= 500000;
				link->initialKs			= 50000;
				link->heta			= 1;
				link->initialEquilibriumDistance= (a->se3.translation - b->se3.translation).length();
				link->radius1			= as->radius - fabs(as->radius - bs->radius)*0.5;
				link->radius2			= bs->radius - fabs(as->radius - bs->radius)*0.5;
				link->knMax			= 75000;
				link->ksMax			= 7500;

				c->interactionGeometry = link;

				rootBody->permanentInteractions->insert(c);
			}
		}

		rootBody->bodies->popIterator();
	}
	cout << "total number of permament links created: " << rootBody->permanentInteractions->size() << endl;


	IOManager::saveToFile("XMLManager", "../data/SDECLinkedSpheres.xml", "rootBody", rootBody);
}
