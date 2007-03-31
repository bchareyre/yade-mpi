/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "HangingCloth.hpp"

#include "MassSpringLaw.hpp"
#include "ParticleSetParameters.hpp"
#include "SpringGeometry.hpp"
#include "SpringPhysics.hpp"


#include<yade/core/Body.hpp>
#include<yade/pkg-common/Mesh2D.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/core/MetaBody.hpp>
//#include<yade/pkg-common/FrictionLessElasticContactLaw.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>
//#include<yade/pkg-common/MassSpringBody2RigidBodyLaw.hpp>

#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>

#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>

#include<yade/pkg-common/CundallNonViscousForceDamping.hpp>
#include<yade/pkg-common/CundallNonViscousMomentumDamping.hpp>

#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/GeometricalModelMetaEngine.hpp>

#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/MacroMicroElasticRelationships.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
//#include<yade/pkg-common/MassSpringBody2RigidBodyLaw.hpp>

#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/PhysicalActionContainerInitializer.hpp>
#include<yade/pkg-common/GravityEngine.hpp>

#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>

#include<yade/pkg-common/BodyRedirectionVector.hpp>
#include<yade/pkg-common/InteractionVecSet.hpp>
#include<yade/pkg-common/PhysicalActionVectorVector.hpp>


HangingCloth::HangingCloth () : FileGenerator()
{
	width = 20;
	height = 20;
	springStiffness = 400000;//500;
	springDamping   = 1.0;
	particleDamping = 0.997;
	clothMass = 4000;//10;
	cellSize = 20;
	fixPoint1 = true;
	fixPoint2 = true;
	fixPoint3 = true;
	fixPoint4 = true;
	ground = true;
	dampingForce = 0.3;

// spheres	
	nbSpheres = Vector3r(2,3,4);
	minRadius = 6;
	maxRadius = 10;
	disorder = 2.1;
	spacing = 17;
	dampingMomentum = 0.3;
	density = 2;
	linkSpheres = true;

	sphereYoungModulus   = 10000;
	spherePoissonRatio  = 0.2;
	sphereFrictionDeg   = 18.0;
	gravity 	= Vector3r(0,-9.81,0);
}


HangingCloth::~HangingCloth ()
{

}


void HangingCloth::postProcessAttributes(bool)
{
}


void HangingCloth::registerAttributes()
{
	FileGenerator::registerAttributes();
	REGISTER_ATTRIBUTE(width);
	REGISTER_ATTRIBUTE(height);
	REGISTER_ATTRIBUTE(springStiffness);
	REGISTER_ATTRIBUTE(springDamping);
	//REGISTER_ATTRIBUTE(particleDamping); 	// FIXME - ignored - delete it, or start using it
	REGISTER_ATTRIBUTE(clothMass);
	REGISTER_ATTRIBUTE(cellSize);
	REGISTER_ATTRIBUTE(fixPoint1);
	REGISTER_ATTRIBUTE(fixPoint2);
	REGISTER_ATTRIBUTE(fixPoint3);
	REGISTER_ATTRIBUTE(fixPoint4);
	REGISTER_ATTRIBUTE(ground);
	
	// spheres
	//REGISTER_ATTRIBUTE(nbSpheres);
	//REGISTER_ATTRIBUTE(minRadius);
	//REGISTER_ATTRIBUTE(maxRadius);
	//rEGISTER_ATTRIBUTE(density); 		
	//REGISTER_ATTRIBUTE(disorder);
	//REGISTER_ATTRIBUTE(spacing);		
	
	REGISTER_ATTRIBUTE(gravity);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	//REGISTER_ATTRIBUTE(linkSpheres);

//	REGISTER_ATTRIBUTE(sphereYoungModulus);
//	REGISTER_ATTRIBUTE(spherePoissonRatio);
//	REGISTER_ATTRIBUTE(sphereFrictionDeg);
}


string HangingCloth::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);

	rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->transientInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->physicalActions		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

	shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
	physicalActionInitializer->physicalActionNames.push_back("Force");
	physicalActionInitializer->physicalActionNames.push_back("Momentum");
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere","InteractingSphere","InteractingSphere2InteractingSphere4SpheresContactGeometry");
	interactionGeometryDispatcher->add("InteractingSphere","InteractingBox","InteractingBox2InteractingSphere4SpheresContactGeometry");

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");

	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere","AABB","InteractingSphere2AABB");
	boundingVolumeDispatcher->add("InteractingBox","AABB","InteractingBox2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","MetaInteractingGeometry2AABB");

	shared_ptr<GeometricalModelMetaEngine> geometricalModelDispatcher	= shared_ptr<GeometricalModelMetaEngine>(new GeometricalModelMetaEngine);
	geometricalModelDispatcher->add("ParticleSetParameters","Mesh2D","ParticleSet2Mesh2D");

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
 	
	shared_ptr<MassSpringLaw> explicitMassSpringConstitutiveLaw(new MassSpringLaw);
	explicitMassSpringConstitutiveLaw->springGroupMask = 1;

	shared_ptr<ElasticContactLaw> elasticContactLaw(new ElasticContactLaw);
	elasticContactLaw->sdecGroupMask = 2;

	//shared_ptr<MassSpringBody2RigidBodyLaw> massSpringBody2RigidBodyConstitutiveLaw(new MassSpringBody2RigidBodyLaw);
	//massSpringBody2RigidBodyConstitutiveLaw->sdecGroupMask = 2;
	//massSpringBody2RigidBodyConstitutiveLaw->springGroupMask = 1;

	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(geometricalModelDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(explicitMassSpringConstitutiveLaw);
	rootBody->engines.push_back(elasticContactLaw);
//	rootBody->engines.push_back(massSpringBody2RigidBodyConstitutiveLaw); // FIXME - is not working...
	rootBody->engines.push_back(gravityCondition);
	rootBody->engines.push_back(actionDampingDispatcher);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);
	rootBody->engines.push_back(orientationIntegrator);

	rootBody->initializers.clear();
	rootBody->initializers.push_back(physicalActionInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	rootBody->initializers.push_back(geometricalModelDispatcher);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	// use ParticleSet so that we have automatic updating for free
	shared_ptr<ParticleSetParameters> physics2(new ParticleSetParameters);
	physics2->se3		= Se3r(Vector3r(0,0,0),q);

	rootBody->isDynamic	= false;

	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	set->diffuseColor	= Vector3r(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor	= Vector3r(0,0,1);

	shared_ptr<Mesh2D> mesh2d(new Mesh2D);
	mesh2d->diffuseColor	= Vector3r(0,0,1);
	mesh2d->wire		= false;
	mesh2d->visible		= true;
	mesh2d->shadowCaster	= false;

	rootBody->geometricalModel			= mesh2d;
	rootBody->interactingGeometry			= set;
	rootBody->boundingVolume			= aabb;
	rootBody->physicalParameters	= physics2;

	rootBody->persistentInteractions->clear();

// make mesh
	
	for(int i=0;i<width;i++)
		for(int j=0;j<height;j++)
		{
			shared_ptr<Body> node(new Body(0,1|2));

			node->isDynamic		= true;
			
			shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
			iSphere->diffuseColor	= Vector3r(0,0,1);
			iSphere->radius		= cellSize/2.2;

// BEGIN ORIGINAL							
//			shared_ptr<ParticleParameters> particle(new ParticleParameters);
//			particle->velocity		= Vector3r(0,0,0);
//			particle->mass			= clothMass/(Real)(width*height);
//			particle->se3			= Se3r(Vector3r(i*cellSize-(cellSize*(width-1))*0.5,0,j*cellSize-(cellSize*(height-1))*0.5),q);
// END									

// BEGIN HACK - to have collision between SDEC spheres and cloth	
			shared_ptr<BodyMacroParameters> particle(new BodyMacroParameters);
			particle->angularVelocity	= Vector3r(0,0,0);
			particle->velocity		= Vector3r(0,0,0);
			particle->mass			= clothMass/(Real)(width*height);
			particle->inertia		= Vector3r(2.0/5.0*particle->mass*iSphere->radius*iSphere->radius,2.0/5.0*particle->mass*iSphere->radius*iSphere->radius,2.0/5.0*particle->mass*iSphere->radius*iSphere->radius);
			particle->se3			= Se3r(Vector3r(i*cellSize-(cellSize*(width-1))*0.5,0,j*cellSize-(cellSize*(height-1))*0.5),q);
			particle->young			= sphereYoungModulus;
			particle->poisson		= spherePoissonRatio;
			particle->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;
// END HACK								

			shared_ptr<AABB> aabb(new AABB);
			aabb->diffuseColor	= Vector3r(0,1,0);

			node->boundingVolume		= aabb;
			//node->geometricalModel		= ??;
			node->interactingGeometry		= iSphere;
			node->physicalParameters= particle;

			rootBody->bodies->insert(node);
			mesh2d->vertices.push_back(particle->se3.position);
		}

	for(int i=0;i<width-1;i++)
		for(int j=0;j<height-1;j++)
		{
			mesh2d->edges.push_back(Edge(offset(i,j),offset(i+1,j)));
			mesh2d->edges.push_back(Edge(offset(i,j),offset(i,j+1)));
			mesh2d->edges.push_back(Edge(offset(i,j+1),offset(i+1,j)));

			rootBody->persistentInteractions->insert(createSpring(rootBody,offset(i,j),offset(i+1,j)));
			rootBody->persistentInteractions->insert(createSpring(rootBody,offset(i,j),offset(i,j+1)));
			rootBody->persistentInteractions->insert(createSpring(rootBody,offset(i,j+1),offset(i+1,j)));

			vector<int> face1,face2;
			face1.push_back(offset(i,j));
			face1.push_back(offset(i+1,j));
			face1.push_back(offset(i,j+1));

			face2.push_back(offset(i+1,j));
			face2.push_back(offset(i+1,j+1));
			face2.push_back(offset(i,j+1));

			mesh2d->faces.push_back(face1);
			mesh2d->faces.push_back(face2);
		}

	for(int i=0;i<width-1;i++)
	{
		mesh2d->edges.push_back(Edge(offset(i,height-1),offset(i+1,height-1)));
		rootBody->persistentInteractions->insert(createSpring(rootBody,offset(i,height-1),offset(i+1,height-1)));

	}

	for(int j=0;j<height-1;j++)
	{
		mesh2d->edges.push_back(Edge(offset(width-1,j),offset(width-1,j+1)));
		rootBody->persistentInteractions->insert(createSpring(rootBody,offset(width-1,j),offset(width-1,j+1)));
	}

	if (fixPoint1)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(0,0)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->mass = 1;
		body->interactingGeometry->diffuseColor = Vector3r(1.0,0.0,0.0);
		body->isDynamic = false;
	}

	if (fixPoint2)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(width-1,0)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->mass = 1;
		body->interactingGeometry->diffuseColor = Vector3r(1.0,0.0,0.0);
		body->isDynamic = false;
	}

	if (fixPoint3)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(0,height-1)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->mass = 1;
		body->interactingGeometry->diffuseColor = Vector3r(1.0,0.0,0.0);
		body->isDynamic = false;
	}

	if (fixPoint4)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(width-1,height-1)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->mass = 1;
		body->interactingGeometry->diffuseColor = Vector3r(1.0,0.0,0.0);
		body->isDynamic = false;
	}

	
// make ground

	shared_ptr<Body> groundBox;
	createBox(groundBox, Vector3r(0,-200,0), Vector3r(500,10,500));
	if(ground)
		rootBody->bodies->insert(groundBox);

// make spheres
	
// 	for(int i=0;i<nbSpheres[0];i++)
// 		for(int j=0;j<nbSpheres[1];j++)
// 			for(int k=0;k<nbSpheres[2];k++)
// 			{
// 				shared_ptr<Body> sphere;
// 				createSphere(sphere,i,j,k);
// 				rootBody->bodies->insert(sphere);
// 			}

			

// make links

	int linksNum=0;
	if(linkSpheres)
	{
		BodyContainer::iterator bi    = rootBody->bodies->begin();
		BodyContainer::iterator bi2;
		BodyContainer::iterator biEnd = rootBody->bodies->end();
		for(  ; bi!=biEnd ; ++bi )
		{
			shared_ptr<Body> bodyA = *bi;
			bi2=bi;
			++bi2;
			
			for( ; bi2!=biEnd ; ++bi2 )
			{
				shared_ptr<Body> bodyB = *bi2;
	
				shared_ptr<BodyMacroParameters> a = YADE_PTR_CAST<BodyMacroParameters>(bodyA->physicalParameters);
				shared_ptr<BodyMacroParameters> b = YADE_PTR_CAST<BodyMacroParameters>(bodyB->physicalParameters);
				shared_ptr<InteractingSphere>	as = YADE_PTR_CAST<InteractingSphere>(bodyA->interactingGeometry);
				shared_ptr<InteractingSphere>	bs = YADE_PTR_CAST<InteractingSphere>(bodyB->interactingGeometry);
	
				if ( a && b && as && bs && (a->se3.position - b->se3.position).Length() < (as->radius + bs->radius))  
				{
					shared_ptr<Interaction> 		link(new Interaction( bodyA->getId() , bodyB->getId() ));
					shared_ptr<SDECLinkGeometry>		geometry(new SDECLinkGeometry);
					shared_ptr<SDECLinkPhysics>	physics(new SDECLinkPhysics);
					
					geometry->radius1			= as->radius - fabs(as->radius - bs->radius)*0.5;
					geometry->radius2			= bs->radius - fabs(as->radius - bs->radius)*0.5;
	
					physics->initialKn			= 500000;
					physics->initialKs			= 50000;
					physics->heta				= 1;
					physics->initialEquilibriumDistance	= (a->se3.position - b->se3.position).Length();
					physics->knMax				= 75000;
					physics->ksMax				= 7500;
	
					link->interactionGeometry 		= geometry;
					link->interactionPhysics 		= physics;
					link->isReal 				= true;
					link->isNew 				= false;
					
					rootBody->persistentInteractions->insert(link);
					++linksNum;
				}
			}
		}
	}
			
	return "total number of permament links created: " + lexical_cast<string>(linksNum);
}


shared_ptr<Interaction>& HangingCloth::createSpring(const shared_ptr<MetaBody>& rootBody,int i,int j)
{
	Body * b1 = static_cast<Body*>((*(rootBody->bodies))[i].get());
	Body * b2 = static_cast<Body*>((*(rootBody->bodies))[j].get());

	spring = shared_ptr<Interaction>(new Interaction( b1->getId() , b2->getId() ));
	shared_ptr<SpringGeometry>	geometry(new SpringGeometry);
	shared_ptr<SpringPhysics>	physics(new SpringPhysics);

	geometry->p1			= b1->physicalParameters->se3.position;
	geometry->p2			= b2->physicalParameters->se3.position;

	physics->initialLength		= (geometry->p1-geometry->p2).Length();
	physics->stiffness		= springStiffness;
	physics->damping		= springDamping;

	spring->interactionGeometry = geometry;
	spring->interactionPhysics = physics;
	spring->isReal = true;
	spring->isNew = false;

	return spring;
}


void HangingCloth::createSphere(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(0,2));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
		
	Vector3r position 		=   Vector3r(i,j,k) * spacing 
					  - Vector3r( nbSpheres[0]/2*spacing  ,  nbSpheres[1]/2*spacing-90  ,  nbSpheres[2]/2*spacing)
					  + Vector3r( Mathr::SymmetricRandom()*disorder ,  Mathr::SymmetricRandom()*disorder , Mathr::SymmetricRandom()*disorder );

	Real radius 			= (Mathr::IntervalRandom(minRadius,maxRadius));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density; // FIXME!!!!!!!!!!!!!! I had a mistake everywhre - it was 4/3*PI*radius*radius, EVERYWHERE!!!. the volume of a sphere should be calculated in ONLY one place!!!
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(0,1,0);


	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());

	body->interactingGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}


void HangingCloth::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body(0,2));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 0;
	physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3r(1,1,1);
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3r(1,1,1);

	body->boundingVolume		= aabb;
	body->interactingGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

