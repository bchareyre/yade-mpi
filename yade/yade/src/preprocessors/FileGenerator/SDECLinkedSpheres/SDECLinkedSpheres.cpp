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
#include "CollisionGeometrySet.hpp"
#include "CollisionGeometrySet2AABBFactory.hpp"
#include "BoundingVolumeUpdator.hpp"

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
	shared_ptr<AABB> aabb;
	
	rootBody = shared_ptr<NonConnexBody>(new NonConnexBody);

	Quaternionr q,qbox;
	q.fromAxisAngle( Vector3r(0,0,1),0);
//	qbox.fromAxisAngle( Vector3r(0,0,1),0.6); 		// FIXME - damping must be 0.01, and timestep must be 0.001

	shared_ptr<NarrowCollider> nc	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	nc->addCollisionFunctor("Sphere","Sphere","Sphere2Sphere4SDECContactModel");
	nc->addCollisionFunctor("Sphere","Box","Box2Sphere4SDECContactModel");

	shared_ptr<BoundingVolumeUpdator> bvu	= shared_ptr<BoundingVolumeUpdator>(new BoundingVolumeUpdator);
	bvu->addBVFactories("Sphere","AABB","Sphere2AABBFactory");
	bvu->addBVFactories("Box","AABB","Box2AABBFactory");
	bvu->addBVFactories("CollisionGeometrySet","AABB","CollisionGeometrySet2AABBFactory");
	
	rootBody->actors.resize(4);
	rootBody->actors[0] 		= bvu;
	rootBody->actors[1] 		= shared_ptr<Actor>(new SAPCollider);
	rootBody->actors[2] 		= nc;
	rootBody->actors[3] 		= shared_ptr<Actor>(new SDECDynamicEngine);

	rootBody->permanentInteractions->clear();

	rootBody->isDynamic		= false;
	rootBody->velocity		= Vector3r(0,0,0);
	rootBody->angularVelocity	= Vector3r(0,0,0);
	rootBody->se3			= Se3r(Vector3r(0,0,0),q);

	shared_ptr<CollisionGeometrySet> set(new CollisionGeometrySet());
	set->diffuseColor	= Vector3f(0,0,1);
	set->wire		= false;
	set->visible		= true;
	set->shadowCaster	= false;
	rootBody->cm		= dynamic_pointer_cast<CollisionGeometry>(set);	
	
	aabb			= shared_ptr<AABB>(new AABB);
	aabb->color		= Vector3r(0,0,1);
	rootBody->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);

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
//	aabb->halfSize		= Vector3r(200,200,200); 	// FIXME - damping must be 0.01, and timestep must be 0.001
	box1->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(200,5,200);
	box->diffuseColor	= Vector3f(1,1,1);
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

// FIXME : MOMENT LAW is completely not working!!!!!!!

/////////////////////////////////////

	shared_ptr<SDECDiscreteElement> box2(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);

	box2->isDynamic		= false;
	box2->angularVelocity	= Vector3r(0,0,0);
	box2->velocity		= Vector3r(0,0,0);
	box2->mass		= 0;
	box2->inertia		= Vector3r(0,0,0);
	box2->se3		= Se3r(Vector3r(0,0,((float)(nbSpheres[2])/2.0-supportSize+1.5)*spacing),q);
	aabb->color		= Vector3r(1,0,0);
	box2->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(20,50,20);
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box->shadowCaster	= true;
	box2->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box2->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box2->kn		= 100000;
	box2->ks		= 10000;
	shared_ptr<Body> b2;
	b2 = dynamic_pointer_cast<Body>(box2);
	if(support1)
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
	box3->se3		= Se3r(Vector3r(0,0,-((float)(nbSpheres[2])/2.0-supportSize+2.5)*spacing),q);
	aabb->color		= Vector3r(1,0,0);
	box3->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(20,50,20);
	box->diffuseColor	= Vector3f(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box->shadowCaster	= true;
	box3->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box3->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box3->kn		= 100000;
	box3->ks		= 10000;
	shared_ptr<Body> b3;
	b3 = dynamic_pointer_cast<Body>(box3);
	if(support2)
		rootBody->bodies->insert(b3);

/////////////////////////////////////

	Vector3r translation;

	for(int i=0;i<nbSpheres[0];i++)
		for(int j=0;j<nbSpheres[1];j++)
			for(int k=0;k<nbSpheres[2];k++)
	{
		shared_ptr<SDECDiscreteElement> s(new SDECDiscreteElement);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> sphere(new Sphere);

		translation 		= Vector3r(i,j,k)*spacing-Vector3r(nbSpheres[0]/2*spacing,nbSpheres[1]/2*spacing-90,nbSpheres[2]/2*spacing)
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
		s->bv			= dynamic_pointer_cast<BoundingVolume>(aabb);

		sphere->radius		= radius;
		sphere->diffuseColor	= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
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
	
	return "total number of permament links created: " + lexical_cast<string>(rootBody->permanentInteractions->size());

}
