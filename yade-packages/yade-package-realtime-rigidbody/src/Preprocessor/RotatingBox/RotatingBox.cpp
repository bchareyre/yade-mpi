/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "RotatingBox.hpp"
#include "FrictionLessElasticContactLaw.hpp"


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-package-common/Box.hpp>
#include <yade/yade-package-common/AABB.hpp>
#include <yade/yade-package-common/Sphere.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-package-common/SAPCollider.hpp>
#include <yade/yade-package-common/RigidBodyParameters.hpp>
#include <yade/yade-package-common/RotationEngine.hpp>

#include <yade/yade-lib-serialization/IOFormatManager.hpp>
#include <yade/yade-package-common/InteractingBox.hpp>
#include <yade/yade-package-common/InteractingSphere.hpp>
#include <yade/yade-package-common/InteractionGeometryMetaEngine.hpp>
#include <yade/yade-package-common/CundallNonViscousForceDamping.hpp>
#include <yade/yade-package-common/CundallNonViscousMomentumDamping.hpp>
#include <yade/yade-package-common/PhysicalActionContainerReseter.hpp>
#include <yade/yade-package-common/PhysicalActionContainerInitializer.hpp>

#include <yade/yade-package-common/BoundingVolumeMetaEngine.hpp>
#include <yade/yade-package-common/InteractionDescriptionSet2AABB.hpp>
#include <yade/yade-package-common/MetaInteractingGeometry.hpp>
#include <yade/yade-package-common/GravityEngine.hpp>
#include <yade/yade-package-common/PhysicalParametersMetaEngine.hpp>

#include <yade/yade-package-common/BodyRedirectionVector.hpp>
#include <yade/yade-package-common/InteractionVecSet.hpp>
#include <yade/yade-package-common/PhysicalActionVectorVector.hpp>

#include <yade/yade-package-common/PhysicalActionDamper.hpp>
#include <yade/yade-package-common/PhysicalActionApplier.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

RotatingBox::RotatingBox () : FileGenerator()
{
	nbSpheres	= Vector3r(3,3,5);
	nbBoxes		= Vector3r(3,3,4);
	minSize		= 3;
	maxSize		= 5;
	disorder	= 1.1;
	densityBox	= 1;
	densitySphere	= 1;
	dampingForce	= 0.2;
	dampingMomentum = 0.6;
	isRotating	= true;
	rotationSpeed	= 0.05;
	rotationAxis	= Vector3r(1,1,1);
	middleWireFrame = true;
	gravity 	= Vector3r(0,-9.81,0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

RotatingBox::~RotatingBox ()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::registerAttributes()
{
	FileGenerator::registerAttributes();
	REGISTER_ATTRIBUTE(nbSpheres);
	REGISTER_ATTRIBUTE(nbBoxes);
	REGISTER_ATTRIBUTE(minSize);
	REGISTER_ATTRIBUTE(maxSize);
	REGISTER_ATTRIBUTE(densityBox);
	REGISTER_ATTRIBUTE(densitySphere);
	REGISTER_ATTRIBUTE(gravity);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	REGISTER_ATTRIBUTE(isRotating);
	REGISTER_ATTRIBUTE(rotationSpeed);
	REGISTER_ATTRIBUTE(rotationAxis);
	REGISTER_ATTRIBUTE(middleWireFrame);
//	REGISTER_ATTRIBUTE(disorder);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

string RotatingBox::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);

	createActors(rootBody);
	positionRootBody(rootBody);
	
	rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->volatileInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->actionParameters		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

	
	shared_ptr<Body> body;
	
	createKinematicBox(body, Vector3r(  0,  0, 10), Vector3r( 50,  5, 40),middleWireFrame);	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(-55,  0,  0), Vector3r(  5, 60, 50),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r( 55,  0,  0), Vector3r(  5, 60, 50),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(  0,-55,  0), Vector3r( 50,  5, 50),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(  0, 55,  0), Vector3r( 50,  5, 50),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(  0,  0,-55), Vector3r( 60, 60,  5),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(  0,  0, 55), Vector3r( 60, 60,  5),true );	rootBody->bodies->insert(body);

	for(int i=0;i<nbSpheres[0];i++)
		for(int j=0;j<nbSpheres[1];j++)
			for(int k=0;k<nbSpheres[2];k++)
			{
				shared_ptr<Body> sphere;
				createSphere(sphere,i,j,k);
				rootBody->bodies->insert(sphere);
			}

	for(int i=0;i<nbBoxes[0];i++)
		for(int j=0;j<nbBoxes[1];j++)
			for(int k=0;k<nbBoxes[2];k++)
 			{
				shared_ptr<Body> box;
				createBox(box,i,j,k);
				rootBody->bodies->insert(box);
 			}

	return "";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::createBox(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(0,0));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
		
	Vector3r position		= Vector3r(i,j,k)*10
					  - Vector3r(15,35,25)
					  + Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom());
				  
	Vector3r size 			= Vector3r(     (Mathr::intervalRandom(minSize,maxSize))
							,(Mathr::intervalRandom(minSize,maxSize))
							,(Mathr::intervalRandom(minSize,maxSize))
						);
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= size[0]*size[1]*size[2]*densityBox;
	physics->inertia		= Vector3r(
							  physics->mass*(size[1]*size[1]+size[2]*size[2])/3
							, physics->mass*(size[0]*size[0]+size[2]*size[2])/3
							, physics->mass*(size[1]*size[1]+size[0]*size[0])/3
						);
	physics->se3			= Se3r(position,q);

	aabb->diffuseColor		= Vector3r(0,1,0);
	
	gBox->extents			= size;
	gBox->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= size;
	iBox->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());

	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::createSphere(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(0,0));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
		
	Vector3r position 		= Vector3r(i,j,k)*10
					  - Vector3r(45,45,45)
					  + Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom());
				  
	Real radius 			= (Mathr::intervalRandom(minSize,maxSize));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*densitySphere;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);

	aabb->diffuseColor		= Vector3r(0,1,0);
	
	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());

	body->interactionGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::createKinematicBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire)
{
	body = shared_ptr<Body>(new Body(0,0));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 0;
	physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= wire;
	gBox->visible			= true;
	gBox->shadowCaster		= false;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3f(1,1,1);

	body->boundingVolume		= aabb;
	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<PhysicalActionContainerInitializer> actionParameterInitializer(new PhysicalActionContainerInitializer);
	actionParameterInitializer->actionParameterNames.push_back("Force");
	actionParameterInitializer->actionParameterNames.push_back("Momentum");
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere","InteractingSphere","Sphere2Sphere4ClosestFeatures");
	interactionGeometryDispatcher->add("InteractingSphere","InteractingBox","Box2Sphere4ClosestFeatures");
	interactionGeometryDispatcher->add("InteractingBox","InteractingBox","Box2Box4ClosestFeatures");

	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere","AABB","Sphere2AABB");
	boundingVolumeDispatcher->add("InteractingBox","AABB","Box2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","InteractionDescriptionSet2AABB");
		
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
	actionDampingDispatcher->add("Force","ParticleParameters","CundallNonViscousForceDamping",actionForceDamping);
	actionDampingDispatcher->add("Momentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("Force","ParticleParameters","NewtonsForceLaw");
	applyActionDispatcher->add("Momentum","RigidBodyParameters","NewtonsMomentumLaw");
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegrator");
	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegrator");
 	
	shared_ptr<RotationEngine> kinematic = shared_ptr<RotationEngine>(new RotationEngine);
 	kinematic->angularVelocity  = rotationSpeed;
	rotationAxis.normalize();
 	kinematic->rotationAxis  = rotationAxis;
 	kinematic->rotateAroundZero = true;
	
 	for(int i=0;i<7;i++)
 		kinematic->subscribedBodies.push_back(i);
	
	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(shared_ptr<Engine>(new SAPCollider));
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(shared_ptr<Engine>(new FrictionLessElasticContactLaw));
	rootBody->actors.push_back(gravityCondition);
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(positionIntegrator);
	rootBody->actors.push_back(orientationIntegrator);
	if(isRotating)
		rootBody->actors.push_back(kinematic);
		
	rootBody->initializers.clear();
	rootBody->initializers.push_back(actionParameterInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::positionRootBody(shared_ptr<MetaBody>& rootBody)
{
	rootBody->isDynamic			= false;
	Quaternionr q;	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->mass				= 0;
	physics->velocity			= Vector3r::ZERO;
	physics->acceleration			= Vector3r::ZERO;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	set->diffuseColor			= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);
	
	rootBody->interactionGeometry		= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->physicalParameters 		= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

