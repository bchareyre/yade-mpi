#include "SDECSpheresPlane.hpp"


#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "NonConnexBody.hpp"
#include "SAPCollider.hpp"
#include "PersistentSAPCollider.hpp"
#include "SimpleNarrowCollider.hpp"
#include "SDECDiscreteElement.hpp"
#include "BallisticDynamicEngine.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "SDECDynamicEngine.hpp"
#include "SDECDiscreteElement.hpp"
#include "SDECPermanentLink.hpp"
#include "Interaction.hpp"
#include "BoundingVolumeUpdator.hpp"
#include "CollisionGeometrySet2AABBFactory.hpp"
#include "CollisionGeometrySet.hpp"

SDECSpheresPlane::SDECSpheresPlane () : FileGenerator()
{
	nbSpheres = 2;
	minRadius = 5;
	maxRadius = 5;
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
}

string SDECSpheresPlane::generate()
{
	rootBody = shared_ptr<NonConnexBody>(new NonConnexBody);

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<NarrowCollider> nc	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	nc->addCollisionFunctor("Sphere","Sphere","Sphere2Sphere4SDECContactModel");
	nc->addCollisionFunctor("Sphere","Box","Box2Sphere4SDECContactModel");

	shared_ptr<BoundingVolumeUpdator> bvu	= shared_ptr<BoundingVolumeUpdator>(new BoundingVolumeUpdator);
	bvu->addBVFactories("Sphere","AABB","Sphere2AABBFactory");
	bvu->addBVFactories("Box","AABB","Box2AABBFactory");
	bvu->addBVFactories("CollisionGeometrySet","AABB","CollisionGeometrySet2AABBFactory");
	
	rootBody->actors.resize(4);
	rootBody->actors[0] 		= bvu;	
	rootBody->actors[1] 		= shared_ptr<Actor>(new PersistentSAPCollider);
	rootBody->actors[2] 		= nc;
	rootBody->actors[3] 		= shared_ptr<Actor>(new SDECDynamicEngine);

	rootBody->permanentInteractions->clear();
//	rootBody->permanentInteractions[0] = shared_ptr<Interaction>(new Interaction);
//	rootBody->permanentInteractions[0]->interactionGeometry = shared_ptr<SDECPermanentLink>(new SDECPermanentLink);

	rootBody->isDynamic		= false;
	rootBody->velocity		= Vector3r(0,0,0);
	rootBody->angularVelocity	= Vector3r(0,0,0);
	rootBody->se3			= Se3r(Vector3r(0,0,0),q);

	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;
	
	shared_ptr<CollisionGeometrySet> set(new CollisionGeometrySet());
	set->diffuseColor	= Vector3f(0,0,1);
	set->wire		= false;
	set->visible		= true;
	set->shadowCaster	= false;
	rootBody->cm		= dynamic_pointer_cast<CollisionGeometry>(set);	
	
	aabb			= shared_ptr<AABB>(new AABB);
	aabb->color		= Vector3r(0,0,1);
	rootBody->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);

	
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

	Vector3r translation;

	for(int i=0;i<nbSpheres;i++)
		for(int j=0;j<nbSpheres;j++)
			for(int k=0;k<nbSpheres;k++)
	{
		shared_ptr<SDECDiscreteElement> s(new SDECDiscreteElement);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> sphere(new Sphere);

		translation 		= Vector3r(i,j,k)*(2*maxRadius*1.1)-Vector3r(nbSpheres/2*(2*maxRadius*1.1),-7-maxRadius*2,nbSpheres/2*(2*maxRadius*1.1))+Vector3r(Mathr::symmetricRandom()*1.1,Mathr::symmetricRandom()*1.1,Mathr::symmetricRandom()*1.1);
		Real radius 		= (Mathr::intervalRandom(minRadius,maxRadius));

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
	
	return "";
}
