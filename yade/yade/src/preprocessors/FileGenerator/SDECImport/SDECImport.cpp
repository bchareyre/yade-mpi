#include "SDECImport.hpp"

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

#include "BoundingVolumeUpdator.hpp"

#include <boost/filesystem/convenience.hpp>

SDECImport::SDECImport () : FileGenerator()
{
	lowerCorner 	= Vector3r(1000,1000,1000);
	upperCorner 	= Vector3r(-1000,-1000,-1000);
	thickness 	= 0.01;
	importFilename 	= "";
	scale 		= 50;
	kn_Spheres 	= 100000;
	ks_Spheres 	= 10000;
	kn_Box		= 100000;
	ks_Box		= 10000;
	wall_top 	= false;
	wall_bottom 	= true;
	wall_1		= true;
	wall_2		= true;
	wall_3		= true;
	wall_4		= true;
	wall_top_wire 	= true;
	wall_bottom_wire= true;
	wall_1_wire	= true;
	wall_2_wire	= true;
	wall_3_wire	= true;
	wall_4_wire	= true;
	spheresColor	= Vector3r(0.8,0.3,0.3);
	spheresRandomColor = false;
	
	outputFileName = "../data/SDECImport.xml";
}

SDECImport::~SDECImport ()
{

}

void SDECImport::postProcessAttributes(bool)
{
}

void SDECImport::registerAttributes()
{
//	REGISTER_ATTRIBUTE(lowerCorner);
//	REGISTER_ATTRIBUTE(upperCorner);
	REGISTER_ATTRIBUTE(thickness);
	REGISTER_ATTRIBUTE(importFilename);
	REGISTER_ATTRIBUTE(scale);
	REGISTER_ATTRIBUTE(kn_Spheres);
	REGISTER_ATTRIBUTE(ks_Spheres);
	REGISTER_ATTRIBUTE(kn_Box);
	REGISTER_ATTRIBUTE(ks_Box);
//	REGISTER_ATTRIBUTE(wall_top);
//	REGISTER_ATTRIBUTE(wall_bottom);
//	REGISTER_ATTRIBUTE(wall_1);
//	REGISTER_ATTRIBUTE(wall_2);
//	REGISTER_ATTRIBUTE(wall_3);
//	REGISTER_ATTRIBUTE(wall_4);
//	REGISTER_ATTRIBUTE(wall_top_wire);
//	REGISTER_ATTRIBUTE(wall_bottom_wire);
//	REGISTER_ATTRIBUTE(wall_1_wire);
//	REGISTER_ATTRIBUTE(wall_2_wire);
//	REGISTER_ATTRIBUTE(wall_3_wire);
//	REGISTER_ATTRIBUTE(wall_4_wire);
	REGISTER_ATTRIBUTE(spheresColor);
	REGISTER_ATTRIBUTE(spheresRandomColor);
}

void SDECImport::generate()
{

// rootBody - the whole scene
	shared_ptr<NonConnexBody> rootBody(new NonConnexBody);

// q - a quaternion that says that we ant zero rotation = rotation around axis 0,0,1 by zero radians
	Quaternionr q;		q.fromAxisAngle( Vector3r(0,0,1) , 0);

// specify narrow collider
	shared_ptr<NarrowCollider> nc	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	nc->addCollisionFunctor("Sphere","Sphere","Sphere2Sphere4SDECContactModel");
	nc->addCollisionFunctor("Sphere","Box","Box2Sphere4SDECContactModel");

// specify bounding volume
	shared_ptr<BoundingVolumeUpdator> bvu	= shared_ptr<BoundingVolumeUpdator>(new BoundingVolumeUpdator);
	bvu->addBVFactories("Sphere","AABB","Sphere2AABBFactory");
	bvu->addBVFactories("Box","AABB","Box2AABBFactory");

	rootBody->actors.resize(4);
	rootBody->actors[0] 		= bvu;
	rootBody->actors[1] 		= shared_ptr<Actor>(new SAPCollider);
	rootBody->actors[2] 		= nc;
// use SDEC law for calculations
	rootBody->actors[3] 		= shared_ptr<Actor>(new SDECDynamicEngine);

	rootBody->permanentInteractions->clear();

	rootBody->isDynamic		= false;
	rootBody->velocity		= Vector3r(0,0,0);
	rootBody->angularVelocity	= Vector3r(0,0,0);
	rootBody->se3			= Se3r(Vector3r(0,0,0),q);

// variables used for generating data
	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;
	shared_ptr<SDECDiscreteElement> sdec;
	shared_ptr<Body> body;
	Vector3r		center,halfSize;
	Vector3r translation;

	if(importFilename.size() != 0 && filesystem::exists(importFilename) )
	{
	
		ifstream loadFile(importFilename.c_str());
		long int i=0;
		Real f,g,x,y,z,radius;

		while( ! loadFile.eof() )
		{
			++i;
		
			shared_ptr<SDECDiscreteElement> s(new SDECDiscreteElement);
			shared_ptr<AABB> aabb(new AABB);
			shared_ptr<Sphere> sphere(new Sphere);
	
			loadFile >> x;
			loadFile >> y;
			loadFile >> z;
			translation = Vector3r(x,z,y);
			loadFile >> radius;
			
		
			loadFile >> f;
			loadFile >> g;
			if(f != 1) // skip loading of SDEC walls
				continue;
//			cout << i << " : " << x << " " << z << " " << y << " " << radius <<  endl;
			lowerCorner[0] = min(translation[0]-radius , lowerCorner[0]);
			lowerCorner[1] = min(translation[1]-radius , lowerCorner[1]);
			lowerCorner[2] = min(translation[2]-radius , lowerCorner[2]);
			upperCorner[0] = max(translation[0]+radius , upperCorner[0]);
			upperCorner[1] = max(translation[1]+radius , upperCorner[1]);
			upperCorner[2] = max(translation[2]+radius , upperCorner[2]);
			
			translation *= scale;
			radius *= scale;

			shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
			ballistic->damping 	= 1.0;
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
			if(spheresRandomColor)
				sphere->diffuseColor	= Vector3r(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom());
			else
			sphere->diffuseColor	= spheresColor;
			sphere->wire		= false;
			sphere->visible		= true;
			sphere->shadowCaster	= true;
	
			s->cm			= dynamic_pointer_cast<CollisionGeometry>(sphere);
			s->gm			= dynamic_pointer_cast<GeometricalModel>(sphere);
			s->kn			= kn_Spheres;
			s->ks			= ks_Spheres;
	
			body = dynamic_pointer_cast<Body>(s);
			rootBody->bodies->insert(body);
		}
	}
	
	cout << "lower: " << lowerCorner[0] << " "<< lowerCorner[1] << " "<< lowerCorner[2] << endl;
	cout << "upper: " << upperCorner[0] << " "<< upperCorner[1] << " "<< upperCorner[2] << endl;

	
///////////////////////////////////////////////////////////////////////////////
/// insert bottom box
///////////////////////////////////////////////////////////////////////////////

	sdec			= shared_ptr<SDECDiscreteElement>(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);

	center			= scale*Vector3r(
					(lowerCorner[0]+upperCorner[0])/2,
					lowerCorner[1]-thickness/2.0,
					(lowerCorner[2]+upperCorner[2])/2);
	halfSize		= scale*Vector3r(
					fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
					thickness/2.0,
					fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
	sdec->isDynamic		= false;					// is not moving
	sdec->angularVelocity	= Vector3r(0,0,0);				// has no angular velocity
	sdec->velocity		= Vector3r(0,0,0);				// has no velocity
	sdec->mass		= 0;						// is not moving, so it's mass is zero
	sdec->inertia		= Vector3r(0,0,0);				// is not moving, so it's inertia is zero
	sdec->se3		= Se3r(center,q);				// position, rotation
	aabb->color		= Vector3r(1,0,0);				// AABB is red
	sdec->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box ->extents		= halfSize;					// size
	box ->diffuseColor	= Vector3r(1,1,1);				// color is white
	box ->wire		= wall_bottom_wire;				// draw as wireframe?
	box ->visible		= true;						// draw
	box ->shadowCaster	= false;					// is not casting shadows
	sdec->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->kn		= kn_Box;					// kn
	sdec->ks		= ks_Box;					// ks
	body = dynamic_pointer_cast<Body>(sdec);
	if(wall_bottom)
		rootBody->bodies->insert(body);

///////////////////////////////////////////////////////////////////////////////
/// insert top box
///////////////////////////////////////////////////////////////////////////////

	sdec			= shared_ptr<SDECDiscreteElement>(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);

	center			= scale*Vector3r(
					(lowerCorner[0]+upperCorner[0])/2,
					upperCorner[1]+thickness/2.0,
					(lowerCorner[2]+upperCorner[2])/2);
	halfSize		= scale*Vector3r(
					fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
					thickness/2.0,
					fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
	sdec->isDynamic		= false;					// is not moving
	sdec->angularVelocity	= Vector3r(0,0,0);				// has no angular velocity
	sdec->velocity		= Vector3r(0,0,0);				// has no velocity
	sdec->mass		= 0;						// is not moving, so it's mass is zero
	sdec->inertia		= Vector3r(0,0,0);				// is not moving, so it's inertia is zero
	sdec->se3		= Se3r(center,q);				// position, rotation
	aabb->color		= Vector3r(1,0,0);				// AABB is red
	sdec->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box ->extents		= halfSize;					// size
	box ->diffuseColor	= Vector3r(1,1,1);				// color is white
	box ->wire		= wall_top_wire;				// draw as wireframe?
	box ->visible		= true;						// draw
	box ->shadowCaster	= false;					// is not casting shadows
	sdec->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->kn		= kn_Box;					// kn
	sdec->ks		= ks_Box;					// ks
	body = dynamic_pointer_cast<Body>(sdec);
	if(wall_top)
		rootBody->bodies->insert(body);

///////////////////////////////////////////////////////////////////////////////
/// insert 1 box
///////////////////////////////////////////////////////////////////////////////

	sdec			= shared_ptr<SDECDiscreteElement>(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);

	center			= scale*Vector3r(
					lowerCorner[0]-thickness/2.0,
					(lowerCorner[1]+upperCorner[1])/2,
					(lowerCorner[2]+upperCorner[2])/2);
	halfSize		= scale*Vector3r(
					thickness/2.0,
					fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
					fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
	sdec->isDynamic		= false;					// is not moving
	sdec->angularVelocity	= Vector3r(0,0,0);				// has no angular velocity
	sdec->velocity		= Vector3r(0,0,0);				// has no velocity
	sdec->mass		= 0;						// is not moving, so it's mass is zero
	sdec->inertia		= Vector3r(0,0,0);				// is not moving, so it's inertia is zero
	sdec->se3		= Se3r(center,q);				// position, rotation
	aabb->color		= Vector3r(1,0,0);				// AABB is red
	sdec->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box ->extents		= halfSize;					// size
	box ->diffuseColor	= Vector3r(1,1,1);				// color is white
	box ->wire		= wall_1_wire;					// draw as wireframe?
	box ->visible		= true;						// draw
	box ->shadowCaster	= false;					// is not casting shadows
	sdec->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->kn		= kn_Box;					// kn
	sdec->ks		= ks_Box;					// ks
	body = dynamic_pointer_cast<Body>(sdec);
	if(wall_1)
		rootBody->bodies->insert(body);

///////////////////////////////////////////////////////////////////////////////
/// insert 2 box
///////////////////////////////////////////////////////////////////////////////

	sdec			= shared_ptr<SDECDiscreteElement>(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);

	center			= scale*Vector3r(
					upperCorner[0]+thickness/2.0,
					(lowerCorner[1]+upperCorner[1])/2,
					(lowerCorner[2]+upperCorner[2])/2);
	halfSize		= scale*Vector3r(
					thickness/2.0,
					fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
					fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
	sdec->isDynamic		= false;					// is not moving
	sdec->angularVelocity	= Vector3r(0,0,0);				// has no angular velocity
	sdec->velocity		= Vector3r(0,0,0);				// has no velocity
	sdec->mass		= 0;						// is not moving, so it's mass is zero
	sdec->inertia		= Vector3r(0,0,0);				// is not moving, so it's inertia is zero
	sdec->se3		= Se3r(center,q);				// position, rotation
	aabb->color		= Vector3r(1,0,0);				// AABB is red
	sdec->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box ->extents		= halfSize;					// size
	box ->diffuseColor	= Vector3r(1,1,1);				// color is white
	box ->wire		= wall_2_wire;					// draw as wireframe?
	box ->visible		= true;						// draw
	box ->shadowCaster	= false;					// is not casting shadows
	sdec->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->kn		= kn_Box;					// kn
	sdec->ks		= ks_Box;					// ks
	body = dynamic_pointer_cast<Body>(sdec);
	if(wall_2)
		rootBody->bodies->insert(body);

///////////////////////////////////////////////////////////////////////////////
/// insert 3 box
///////////////////////////////////////////////////////////////////////////////

	sdec			= shared_ptr<SDECDiscreteElement>(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);

	center			= scale*Vector3r(
					(lowerCorner[0]+upperCorner[0])/2,
					(lowerCorner[1]+upperCorner[1])/2,
					lowerCorner[2]-thickness/2.0);
	halfSize		= scale*Vector3r(
					fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
					fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
					thickness/2.0);
	
	sdec->isDynamic		= false;					// is not moving
	sdec->angularVelocity	= Vector3r(0,0,0);				// has no angular velocity
	sdec->velocity		= Vector3r(0,0,0);				// has no velocity
	sdec->mass		= 0;						// is not moving, so it's mass is zero
	sdec->inertia		= Vector3r(0,0,0);				// is not moving, so it's inertia is zero
	sdec->se3		= Se3r(center,q);				// position, rotation
	aabb->color		= Vector3r(1,0,0);				// AABB is red
	sdec->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box ->extents		= halfSize;					// size
	box ->diffuseColor	= Vector3r(1,1,1);				// color is white
	box ->wire		= wall_3_wire;					// draw as wireframe?
	box ->visible		= true;						// draw
	box ->shadowCaster	= false;					// is not casting shadows
	sdec->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->kn		= kn_Box;					// kn
	sdec->ks		= ks_Box;					// ks
	body = dynamic_pointer_cast<Body>(sdec);
	if(wall_3)
		rootBody->bodies->insert(body);

///////////////////////////////////////////////////////////////////////////////
/// insert 4 box
///////////////////////////////////////////////////////////////////////////////

	sdec			= shared_ptr<SDECDiscreteElement>(new SDECDiscreteElement);
	aabb			= shared_ptr<AABB>(new AABB);
	box			= shared_ptr<Box>(new Box);

	center			= scale*Vector3r(
					(lowerCorner[0]+upperCorner[0])/2,
					(lowerCorner[1]+upperCorner[1])/2,
					upperCorner[2]+thickness/2.0);
	halfSize		= scale*Vector3r(
					fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
					fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
					thickness/2.0);
	
	sdec->isDynamic		= false;					// is not moving
	sdec->angularVelocity	= Vector3r(0,0,0);				// has no angular velocity
	sdec->velocity		= Vector3r(0,0,0);				// has no velocity
	sdec->mass		= 0;						// is not moving, so it's mass is zero
	sdec->inertia		= Vector3r(0,0,0);				// is not moving, so it's inertia is zero
	sdec->se3		= Se3r(center,q);				// position, rotation
	aabb->color		= Vector3r(1,0,0);				// AABB is red
	sdec->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box ->extents		= halfSize;					// size
	box ->diffuseColor	= Vector3r(1,1,1);				// color is white
	box ->wire		= wall_4_wire;					// draw as wireframe?
	box ->visible		= true;						// draw
	box ->shadowCaster	= false;					// is not casting shadows
	sdec->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->gm		= dynamic_pointer_cast<CollisionGeometry>(box);
	sdec->kn		= kn_Box;					// kn
	sdec->ks		= ks_Box;					// ks
	body = dynamic_pointer_cast<Body>(sdec);
	if(wall_4)
		rootBody->bodies->insert(body);

///////////////////////////////////////////////////////////////////////////////
/// end of box inserting
///////////////////////////////////////////////////////////////////////////////

	
	IOManager::saveToFile("XMLManager", "../data/SDECImport.xml" , "rootBody", rootBody);
}
