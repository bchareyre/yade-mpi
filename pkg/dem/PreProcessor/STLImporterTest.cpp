/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"STLImporterTest.hpp"
#include<yade/pkg-common/SpatialQuickSortCollider.hpp>
#include<yade/lib-import/STLImporter.hpp>
#include<yade/pkg-common/InteractingFacet.hpp>
#include<yade/pkg-common/Facet.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/core/BodyRedirectionVector.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/GravityEngines.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/ElasticCriterionTimeStepper.hpp>
#include<yade/pkg-dem/MacroMicroElasticRelationships.hpp>
#include<yade/pkg-common/RotationEngine.hpp>

STLImporterTest::STLImporterTest() : FileGenerator()
{
	nbSpheres = Vector3r(10,10,10);
	minRadius = 0.07;
	maxRadius = 0.07;
	dampingForce = 0.3;
	dampingMomentum = 0.3;
	timeStepUpdateInterval = 300;
	sphereYoungModulus   = 15000000.0;
	spherePoissonRatio  = 0.2;
	sphereFrictionDeg   = 18.0;
	density = 2600;
	gravity = Vector3r(0,-9.81,0);
	disorder = Vector3r(0.002,0.002,0.002);
	wire=true;
	stlFileName = "hourglass.stl";
	angularVelocity = 0.5;
	rotationAxis = Vector3r(0,0,1);
	spheresHeight = 0;
}


STLImporterTest::~STLImporterTest()
{

}


void STLImporterTest::postProcessAttributes(bool)
{

}


bool STLImporterTest::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	positionRootBody(rootBody);

	rootBody->dt = 0.001; //default time step

////////////////////////////////////
///////// Container
	
		
////////////////////////////////////
///////// walls

	cerr << "Import walls geometry from file: " << stlFileName << endl;
	STLImporter imp;
	if (!imp.open(stlFileName.c_str()))
	{
	    cerr << "ERROR: Bad file: " << stlFileName << endl;
	    message="Input file not found, you can copy it from examples/ directory or make one using blender 3D modelling";
	    return false;
	}
	imp.wire=wire;
	// create bodies
	for(int i=0,e=imp.number_of_facets;i<e;++i)
	{
	    shared_ptr<Body> b(new Body(body_id_t(0),1));
    
	    b->isDynamic		= false;
	    
	    // physical parameters
	    shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	    physics->angularVelocity	= Vector3r(0,0,0);
	    physics->velocity		= Vector3r(0,0,0);
	    physics->mass			= 0;
	    physics->inertia		= Vector3r(0,0,0);
	    physics->young			= sphereYoungModulus;
	    physics->poisson		= spherePoissonRatio;
	    physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;
	    b->physicalParameters	= physics;

	    // bounding box 
		shared_ptr<AABB> aabb(new AABB);
		aabb->diffuseColor		= Vector3r(0,1,0);
		b->boundingVolume	= aabb;
	    
	    rootBody->bodies->insert(b);
	}
	// import bodies (create geometry)
	imp.import(rootBody->bodies);

///////// spheres
	float all = nbSpheres[0]*nbSpheres[1]*nbSpheres[2];
	float current = 0.0;

	for(int i=0;i<nbSpheres[0];i++)
	{
		if(shouldTerminate()) return false;

		for(int j=0;j<nbSpheres[1];j++)
			for(int k=0;k<nbSpheres[2];k++)
			{
				shared_ptr<Body> sphere;
				createSphere(sphere,i,j,k);
				rootBody->bodies->insert(sphere);

				setProgress(current++/all);
			}
	}
	

///////// engines
	createActors(rootBody);

	return true;
}

void STLImporterTest::createSphere(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),1));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	Vector3r position		= Vector3r(i,j+spheresHeight,k)*(2*maxRadius*1.1) 
					  - Vector3r( nbSpheres[0]/2*(2*maxRadius*1.1) , 0 , nbSpheres[2]/2*(2*maxRadius*1.1) )
					  + Vector3r( 	 Mathr::SymmetricRandom()*disorder[0]
					  		,Mathr::SymmetricRandom()*disorder[1]
							,Mathr::SymmetricRandom()*disorder[2])*maxRadius;
	
	Real radius 			= (Mathr::IntervalRandom(minRadius,maxRadius));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(0,1,0);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
	gSphere->wire			= false;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(0.8,0.3,0.3);

	body->interactingGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}

void STLImporterTest::createActors(shared_ptr<MetaBody>& rootBody)
{
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere2InteractingSphere4SpheresContactGeometry");
	interactionGeometryDispatcher->add("InteractingFacet2InteractingSphere4SpheresContactGeometry");

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add("MacroMicroElasticRelationships");
		
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere2AABB");
	boundingVolumeDispatcher->add("InteractingFacet2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry2AABB");
	
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
	actionDampingDispatcher->add(actionForceDamping);
	actionDampingDispatcher->add(actionMomentumDamping);
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("NewtonsForceLaw");
	applyActionDispatcher->add("NewtonsMomentumLaw");
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("LeapFrogPositionIntegrator");
	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("LeapFrogOrientationIntegrator");

	shared_ptr<RotationEngine> kinematic = shared_ptr<RotationEngine>(new RotationEngine);
 	kinematic->angularVelocity  = angularVelocity;
	rotationAxis.Normalize();
 	kinematic->rotationAxis  = rotationAxis;
 	kinematic->rotateAroundZero = true;
	
	
	shared_ptr<InteractingGeometry> facet(new InteractingFacet);
	for(BodyContainer::iterator bi = rootBody->bodies->begin(), biEnd=rootBody->bodies->end(); bi!=biEnd; ++bi)
	    if ( (*bi)->interactingGeometry->getClassIndex() == facet->getClassIndex() )
		kinematic->subscribedBodies.push_back((*bi)->getId());

	shared_ptr<ElasticCriterionTimeStepper> sdecTimeStepper(new ElasticCriterionTimeStepper);
	sdecTimeStepper->sdecGroupMask = 1;
	sdecTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;

	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(sdecTimeStepper);
	rootBody->engines.push_back(boundingVolumeDispatcher);	
	rootBody->engines.push_back(shared_ptr<Engine>(new SpatialQuickSortCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new ElasticContactLaw));
	rootBody->engines.push_back(gravityCondition);
	rootBody->engines.push_back(actionDampingDispatcher);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);
	rootBody->engines.push_back(orientationIntegrator);
	rootBody->engines.push_back(kinematic);
 	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(boundingVolumeDispatcher);
}


void STLImporterTest::positionRootBody(shared_ptr<MetaBody>& rootBody) 
{
	rootBody->isDynamic		= false;
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->mass				= 0;
	physics->velocity			= Vector3r(0,0,0);
	physics->acceleration			= Vector3r::ZERO;
		
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	set->diffuseColor			= Vector3r(0,0,1);
	
	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);
	
	rootBody->interactingGeometry		= YADE_PTR_CAST<InteractingGeometry>(set);	
	rootBody->boundingVolume		= YADE_PTR_CAST<BoundingVolume>(aabb);
	rootBody->physicalParameters 		= physics;
}


YADE_PLUGIN("STLImporterTest");
