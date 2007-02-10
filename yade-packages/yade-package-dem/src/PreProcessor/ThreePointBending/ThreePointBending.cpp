/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

///
// FIXME : add CohesiveContactLaw engine to make it work
///


#include "ThreePointBending.hpp"

#include "ElasticContactLaw.hpp"
#include "ElasticCohesiveLaw.hpp"
#include "MacroMicroElasticRelationships.hpp"
#include "BodyMacroParameters.hpp"
#include "SDECLinkGeometry.hpp"
#include "SDECLinkPhysics.hpp"
#include "ElasticCriterionTimeStepper.hpp"


#include <yade/yade-core/yadeExceptions.hpp>
#include <yade/yade-package-common/Box.hpp>
#include <yade/yade-package-common/AABB.hpp>
#include <yade/yade-package-common/Sphere.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/SAPCollider.hpp>
#include <yade/yade-package-common/PersistentSAPCollider.hpp>
#include <yade/yade-lib-serialization/IOFormatManager.hpp>
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-package-common/BoundingVolumeMetaEngine.hpp>
#include <yade/yade-package-common/MetaInteractingGeometry2AABB.hpp>
#include <yade/yade-package-common/MetaInteractingGeometry.hpp>

#include <yade/yade-package-common/PhysicalActionDamper.hpp>
#include <yade/yade-package-common/PhysicalActionApplier.hpp>
#include <yade/yade-package-common/CundallNonViscousForceDamping.hpp>
#include <yade/yade-package-common/CundallNonViscousMomentumDamping.hpp>
#include <yade/yade-package-common/GravityEngine.hpp>

#include <yade/yade-package-common/InteractionGeometryMetaEngine.hpp>
#include <yade/yade-package-common/InteractionPhysicsMetaEngine.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-package-common/InteractingBox.hpp>
#include <yade/yade-package-common/InteractingSphere.hpp>

#include <yade/yade-package-common/PhysicalActionContainerReseter.hpp>
#include <yade/yade-package-common/PhysicalActionContainerInitializer.hpp>
#include <yade/yade-package-common/PhysicalParametersMetaEngine.hpp>

#include <yade/yade-package-common/BodyRedirectionVector.hpp>
#include <yade/yade-package-common/InteractionVecSet.hpp>
#include <yade/yade-package-common/PhysicalActionVectorVector.hpp>

#include <yade/yade-package-common/TranslationEngine.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>


ThreePointBending::ThreePointBending () : FileGenerator()
{
	yadeFileWithSpheres	="../data/someFileFromTriaxial.yade";
	gravity			= Vector3r(0,-9.81,0);

	supportDepth		= 10;
	pistonWidth		= 5;
	pistonVelocity		= 0.1;
	dampingForce		= 0.2;
	dampingMomentum		= 0.2;
	
	timeStepUpdateInterval	= 300;
	momentRotationLaw       = true;
	
	//sphereYoungModulus	= 10000000;
	sphereYoungModulus	= 30000000;
	spherePoissonRatio	= 0.2;
	sphereFrictionDeg       = 18.0;
	
	linkKn			= 50000000;
	linkKs			= 5000000;
	linkMaxNormalForce	= 5500000;
	linkMaxShearForce       = 550000;
}


ThreePointBending::~ThreePointBending ()
{

}


void ThreePointBending::postProcessAttributes(bool)
{
}


void ThreePointBending::registerAttributes()
{
	FileGenerator::registerAttributes();
	
	REGISTER_ATTRIBUTE(yadeFileWithSpheres);
	REGISTER_ATTRIBUTE(gravity);

	REGISTER_ATTRIBUTE(pistonWidth);
	REGISTER_ATTRIBUTE(supportDepth);
	REGISTER_ATTRIBUTE(pistonVelocity);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);

	REGISTER_ATTRIBUTE(timeStepUpdateInterval);
	REGISTER_ATTRIBUTE(momentRotationLaw);
	
	REGISTER_ATTRIBUTE(sphereYoungModulus);
	REGISTER_ATTRIBUTE(spherePoissonRatio);
	REGISTER_ATTRIBUTE(sphereFrictionDeg);
	
	REGISTER_ATTRIBUTE(linkKn);
	REGISTER_ATTRIBUTE(linkKs);
	REGISTER_ATTRIBUTE(linkMaxNormalForce);
	REGISTER_ATTRIBUTE(linkMaxShearForce);
}


string ThreePointBending::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

////////////////////////////////////
	
	rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->transientInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->physicalActions		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

/////////////////////////////////////
/////////////////////////////////////
	// load file
	
	shared_ptr<MetaBody> metaBodyWithSpheres;

	if ( 	   yadeFileWithSpheres.size()!=0 
		&& filesystem::exists(yadeFileWithSpheres) 
		&& (filesystem::extension(yadeFileWithSpheres)==".xml" || filesystem::extension(yadeFileWithSpheres)==".yade"))
	{
		try
		{
			if(filesystem::extension(yadeFileWithSpheres)==".xml")
				IOFormatManager::loadFromFile("XMLFormatManager",yadeFileWithSpheres,"rootBody",metaBodyWithSpheres);

			else if(filesystem::extension(yadeFileWithSpheres)==".yade" )
				IOFormatManager::loadFromFile("BINFormatManager",yadeFileWithSpheres,"rootBody",metaBodyWithSpheres);

			if( metaBodyWithSpheres->getClassName() != "MetaBody")
				return "Error: cannot load the file that should contain spheres";
		} 
		catch(SerializableError& e)
		{
			return "Error: cannot load the file that should contain spheres";
		}
		catch(yadeError& e)
		{
			return "Error: cannot load the file that should contain spheres";
		}
	}
	else
		return "Error: cannot load the file that should contain spheres";
/////////////////////////////////////
	Vector3r min(10000,10000,10000),max(-10000,-10000,-10000);
	{// calc min/max
		BodyContainer::iterator bi    = metaBodyWithSpheres->bodies->begin();
		BodyContainer::iterator biEnd = metaBodyWithSpheres->bodies->end();
		for( ; bi!=biEnd ; ++bi )
		{
			if((*bi)->geometricalModel->getClassName() == "Sphere" )
			{
				shared_ptr<Body> b = *bi;
				min = componentMinVector(min,b->physicalParameters->se3.position - static_cast<Sphere*>(b->geometricalModel.get())->radius * Vector3r(1,1,1));
				max = componentMaxVector(max,b->physicalParameters->se3.position + static_cast<Sphere*>(b->geometricalModel.get())->radius * Vector3r(1,1,1));

				BodyMacroParameters* bm = dynamic_cast<BodyMacroParameters*>(b->physicalParameters.get());
				if(!bm)
					return "Error: spheres don't use BodyMacroParameters for physical parameters";

				bm->young		= sphereYoungModulus;
				bm->poisson		= spherePoissonRatio;
				bm->frictionAngle	= sphereFrictionDeg * Mathr::PI/180.0;
				bm->mass		/= 1000.0; // FIXME !!!
			}
		}
	}
////////////////////////////////////

	shared_ptr<Body> piston;
	shared_ptr<Body> supportBox1;
	shared_ptr<Body> supportBox2;
	
	createBox(piston,      Vector3r( (max[0]+min[0])*0.5 , max[1] + (max[0]-min[0])*0.5 ,          (max[2]+min[2])*0.5                ), Vector3r( (max[0]-min[0])*0.5, (max[0]-min[0])*0.5 , pistonWidth          ));
	createBox(supportBox1, Vector3r( (max[0]+min[0])*0.5 , min[1] - (max[0]-min[0])*0.5 , min[2] - (max[0]-min[0])*0.5 + supportDepth ), Vector3r( (max[0]-min[0])*0.5, (max[0]-min[0])*0.5 , (max[0]-min[0])*0.5  ));
	createBox(supportBox2, Vector3r( (max[0]+min[0])*0.5 , min[1] - (max[0]-min[0])*0.5 , max[2] + (max[0]-min[0])*0.5 - supportDepth ), Vector3r( (max[0]-min[0])*0.5, (max[0]-min[0])*0.5 , (max[0]-min[0])*0.5  ));
			
	rootBody->bodies->insert(piston);
	rootBody->bodies->insert(supportBox1);
	rootBody->bodies->insert(supportBox2);

/////////////////////////////////////
        
	{// insert Spheres
		BodyContainer::iterator bi    = metaBodyWithSpheres->bodies->begin();
		BodyContainer::iterator biEnd = metaBodyWithSpheres->bodies->end();
		for( ; bi!=biEnd ; ++bi )
		{
			if((*bi)->geometricalModel->getClassName() == "Sphere" )
			{
				shared_ptr<Body> b = *bi;
			        rootBody->bodies->insert(b);
			}
		}
	}
	
/////////////////////////////////////

	rootBody->persistentInteractions->clear();
	
	shared_ptr<Body> bodyA;

	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	BodyContainer::iterator bi2;

	++bi; // skips piston
	++bi; // skips supportBox1
	++bi; // skips supportBox2
		
		
	for( ; bi!=biEnd ; ++bi )
	{
		bodyA =*bi;
		bi2=bi;
		++bi2;
		for( ; bi2!=biEnd ; ++bi2 )
		{
			shared_ptr<Body> bodyB = *bi2;

			shared_ptr<BodyMacroParameters> a = dynamic_pointer_cast<BodyMacroParameters>(bodyA->physicalParameters);
			shared_ptr<BodyMacroParameters> b = dynamic_pointer_cast<BodyMacroParameters>(bodyB->physicalParameters);
			shared_ptr<InteractingSphere>	as = dynamic_pointer_cast<InteractingSphere>(bodyA->interactingGeometry);
			shared_ptr<InteractingSphere>	bs = dynamic_pointer_cast<InteractingSphere>(bodyB->interactingGeometry);

			if ((a->se3.position - b->se3.position).Length() < (as->radius + bs->radius))  
			{
				shared_ptr<Interaction> 		link(new Interaction( bodyA->getId() , bodyB->getId() ));
				shared_ptr<SDECLinkGeometry>		geometry(new SDECLinkGeometry);
				shared_ptr<SDECLinkPhysics>	physics(new SDECLinkPhysics);
				
				geometry->radius1			= as->radius - fabs(as->radius - bs->radius)*0.5;
				geometry->radius2			= bs->radius - fabs(as->radius - bs->radius)*0.5;

				physics->initialKn			= linkKn; // FIXME - BIG problem here.
				physics->initialKs			= linkKs;
				physics->heta				= 1;
				physics->initialEquilibriumDistance	= (a->se3.position - b->se3.position).Length();
				physics->knMax				= linkMaxNormalForce;
				physics->ksMax				= linkMaxShearForce;

				link->interactionGeometry 		= geometry;
				link->interactionPhysics 		= physics;
				link->isReal 				= true;
				link->isNew 				= false;
				
				rootBody->persistentInteractions->insert(link);
			}
		}
	}
	
	return "total number of permament links created: " 
		+ lexical_cast<string>(rootBody->persistentInteractions->size()) 
		+ "\nWARNING: link bonds are nearly working, but the formulas are waiting for total rewrite!";
}


void ThreePointBending::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body(0,55));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= extents[0]*extents[1]*extents[2]*2; 
	physics->inertia		= Vector3r(
							  physics->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
							, physics->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
							, physics->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3
						);
//	physics->mass			= 0;
//	physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3f(1,1,1);

	body->boundingVolume		= aabb;
	body->interactingGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}


void ThreePointBending::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
	physicalActionInitializer->physicalActionNames.push_back("Force");
	physicalActionInitializer->physicalActionNames.push_back("Momentum");
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere","InteractingSphere","InteractingSphere2InteractingSphere4SpheresContactGeometry");
	interactionGeometryDispatcher->add("InteractingSphere","InteractingBox","InteractingBox2InteractingSphere4SpheresContactGeometry");

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");
		
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere","AABB","InteractingSphere2AABB");
	boundingVolumeDispatcher->add("InteractingBox","AABB","InteractingBox2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","MetaInteractingGeometry2AABB");
		
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
 	
	shared_ptr<ElasticCriterionTimeStepper> sdecTimeStepper(new ElasticCriterionTimeStepper);
	sdecTimeStepper->sdecGroupMask = 55;
	sdecTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;

// moving wall
	shared_ptr<TranslationEngine> kinematic = shared_ptr<TranslationEngine>(new TranslationEngine);
	kinematic->velocity  = pistonVelocity;
	kinematic->translationAxis  = Vector3r(0,-1,0);
	kinematic->subscribedBodies.push_back(0);

	shared_ptr<ElasticContactLaw> constitutiveLaw(new ElasticContactLaw);
	constitutiveLaw->sdecGroupMask = 55;
	constitutiveLaw->momentRotationLaw = momentRotationLaw;
// FIXME FIXME FIXME ....	
	shared_ptr<ElasticCohesiveLaw> constitutiveLaw2(new ElasticCohesiveLaw);
	constitutiveLaw2->sdecGroupMask = 55;
	constitutiveLaw2->momentRotationLaw = momentRotationLaw;
	
	rootBody->engines.clear();
	rootBody->engines.push_back(sdecTimeStepper);
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(constitutiveLaw);
	rootBody->engines.push_back(constitutiveLaw2);
	rootBody->engines.push_back(gravityCondition);
	rootBody->engines.push_back(actionDampingDispatcher);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);
	rootBody->engines.push_back(orientationIntegrator);
	rootBody->engines.push_back(kinematic);

	rootBody->initializers.clear();
	rootBody->initializers.push_back(physicalActionInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
}
	

void ThreePointBending::positionRootBody(shared_ptr<MetaBody>& rootBody)
{
	rootBody->isDynamic		= false;
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3			= Se3r(Vector3r(0,0,0),q);
	physics->mass			= 0;
	physics->velocity		= Vector3r::ZERO;
	physics->acceleration		= Vector3r::ZERO;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	set->diffuseColor		= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);
	
	rootBody->interactingGeometry	= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume	= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->physicalParameters 	= physics;
	
}

