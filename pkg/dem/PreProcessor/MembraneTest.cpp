/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <boost/lexical_cast.hpp>
#include<yade/pkg-dem/Shop.hpp>

#include "MembraneTest.hpp"

//#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw.hpp>
//#include<yade/pkg-dem/ElasticBodyParameters2BcpConnection4ElasticContactInteraction.hpp>
#include<yade/pkg-common/ConstitutiveLawDispatcher.hpp>
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/ef2_BshTube_BssSweptSphereLineSegment_makeBssSweptSphereLineSegment.hpp>
#include<yade/pkg-common/ef2_BssSweptSphereLineSegment_AABB_makeAABB.hpp>
#include<yade/pkg-common/BcpConnection.hpp>
#include<yade/pkg-common/BshTube.hpp>
#include<yade/pkg-common/BssSweptSphereLineSegment.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>

#include<yade/pkg-common/AABB.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/core/Interaction.hpp>

#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>

#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>

#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>

#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/GravityEngines.hpp>

#include<yade/pkg-common/InteractingGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingNode.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>


#include<yade/pkg-dem/SimpleViscoelasticBodyParameters.hpp>


MembraneTest::MembraneTest () : FileGenerator()
{
	nbX 			= 9;
	nbZ 			= 10;
	XLength 		= 10.0e-2;
	ZLength 		= 10.0e-2;
	bigBallRadius 		= 1.0e-2;
	membraneThickness 	= 1.0e-3;

	tc		        = 0.001;
        en                      = 0.3;
        es                      = 0.3;
        frictionAngle = 0.38; // rad = 21.8 grad = atan(mu=0.4);
	gravity			= Vector3r(0,-9.81,0);
}


MembraneTest::~MembraneTest ()
{
}


void MembraneTest::postProcessAttributes(bool)
{
}


bool MembraneTest::generate()
{
	Omega::instance().setTimeStep(0.04);
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);


// Nodes
	float all = (float) (nbX * nbZ);
	float current = 0.0;

	setStatus("Generating membrane nodes...");
	for (unsigned int i = 0 ; i <= nbX; i++)
	{
		if(shouldTerminate()) return ""; 

		for (unsigned int j = 0 ; j <= nbZ ; j++)
		{
			shared_ptr<Body> node;
			createNode(node,i,j);
			rootBody->bodies->insert(node);
			
			setProgress(current++/all);
		}
	}
	
	message="Nb nodes = " + boost::lexical_cast<std::string>(current);



// Connection of nodes
        current = 0.0;

        setStatus("Generating node connections...");
        for (unsigned int i = 0 ; i < nbX; i++)
        {
          if(shouldTerminate()) return ""; 

          for (unsigned int j = 0 ; j < nbZ ; j++)
          {
            shared_ptr<Body> c1;
            connectNodes(c1,i*(nbZ+1) + j, i*(nbZ+1) + (j+1));
            rootBody->bodies->insert(c1);
            
            shared_ptr<Body> c2;
            connectNodes(c2,i*(nbZ+1) + j,(i+1)*(nbZ+1) + j);
            rootBody->bodies->insert(c2);           
                        
            setProgress(current++/all);
          }
          
          shared_ptr<Body> c;
          connectNodes(c,i*(nbZ+1) + nbZ,(i+1)*(nbZ+1) + nbZ);
          rootBody->bodies->insert(c);
        }
        
        for (unsigned int j = 0 ; j < nbZ ; j++)
        {
          shared_ptr<Body> c1;
          connectNodes(c1,nbX*(nbZ+1) + j, nbX*(nbZ+1) + (j+1));
          rootBody->bodies->insert(c1);
        }        
        
        
// The big ball
        shared_ptr<Body> bb;       
        createSphere(bb, Vector3r(0.5*XLength,1.05*bigBallRadius+0.5*membraneThickness,0.5*ZLength), bigBallRadius, true);
        rootBody->bodies->insert(bb);
        
	return true;
}

void MembraneTest::connectNodes(shared_ptr<Body>& body, unsigned int id1, unsigned int id2)
{
        body = shared_ptr<Body>(new Body(body_id_t(0),1)); 
        shared_ptr<BcpConnection> connection(new BcpConnection);                  // Bcp + Bst
        shared_ptr<AABB> aabb(new AABB);                                          // Bbv
        shared_ptr<BshTube> tube(new BshTube);                                    // Bsh
        shared_ptr<BssSweptSphereLineSegment> bss(new BssSweptSphereLineSegment); // Bss
          
        connection->id1             = id1;
        connection->id2             = id2;
        connection->mass	    = 1.0; // !!!!
        Shop::getViscoelasticFromSpheresInteraction(connection->mass,tc,en,es, static_pointer_cast<SimpleViscoelasticBodyParameters>(connection));
		connection->frictionAngle              = frictionAngle;
        Vector3r position1,
                 position2;
        position1                   = (*(rootBody->bodies))[id1]->physicalParameters->se3.position;
        position2                   = (*(rootBody->bodies))[id2]->physicalParameters->se3.position;
        connection->se3.position    = 0.5 * (position1 + position2);
        Vector3r link               = position2 - position1;

        
        Vector3r newX               = link; newX.Normalize();
        Vector3r newY               = Vector3r(0.0,1.0,0.0); 
        Vector3r newZ               = newX.Cross(newY);
        Quaternionr q               = quaternionFromAxes (newX,newY,newZ);     
        connection->se3.orientation = q;
        
        tube->radius                = 0.5 * membraneThickness;
        tube->half_height           = 0.5 * link.Length();
        tube->diffuseColor          = Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
        
        bss->diffuseColor           = Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
        
        aabb->diffuseColor          = Vector3r(1.0,0.0,0.0);
        
        body->isDynamic             = false;
        body->geometricalModel      = tube;
        body->interactingGeometry   = bss;
        body->boundingVolume        = aabb;
        body->physicalParameters    = connection;
}
    
void MembraneTest::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius, bool dynamic )
{
  body = shared_ptr<Body>(new Body(body_id_t(0),1));
  shared_ptr<SimpleViscoelasticBodyParameters> physics(new SimpleViscoelasticBodyParameters); // Bcp + Bst
  shared_ptr<AABB> aabb(new AABB);                                      // Bbv
  shared_ptr<Sphere> gSphere(new Sphere);                               // Bsh
  shared_ptr<InteractingSphere> iSphere(new InteractingSphere);         // Bss
        
  Quaternionr q(Mathr::SymmetricRandom(),Mathr::SymmetricRandom(),Mathr::SymmetricRandom(),Mathr::SymmetricRandom());
  q.Normalize();
        
  body->isDynamic           = dynamic;
        
  physics->angularVelocity  = Vector3r(0,0,0);
  physics->velocity         = Vector3r(0,0,0);
  physics->mass             = 4.0/3.0 * 3.1415 * radius * radius * radius * 2500.0;
  
  Real I                    = 2.0/5.0 * physics->mass * radius * radius;
  physics->inertia          = Vector3r(I, I, I);
  physics->se3              = Se3r(position,q);
  Shop::getViscoelasticFromSpheresInteraction(physics->mass,tc,en,es,physics);
  physics->frictionAngle               = frictionAngle;
        
  aabb->diffuseColor        = Vector3r(0.0,1.0,0.0);

  gSphere->radius           = radius;
  gSphere->diffuseColor     = Vector3r(0.5,0.5,1.0);
  gSphere->wire             = false;
  gSphere->shadowCaster     = true;
        
  iSphere->radius           = radius;

  iSphere->diffuseColor     = Vector3r(0.0,0.0,1.0);

  body->interactingGeometry = iSphere;
  body->geometricalModel    = gSphere;
  body->boundingVolume      = aabb;
  body->physicalParameters  = physics;
}  
    
void MembraneTest::createNode(shared_ptr<Body>& body, unsigned int i, unsigned int j)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),2));
	shared_ptr<ParticleParameters> physics(new ParticleParameters); // Bcp + Bst
	shared_ptr<AABB> aabb(new AABB);                                // Bbv (not needed?)
	shared_ptr<Sphere> node(new Sphere);                            // Bsh
        shared_ptr<InteractingNode> inode(new InteractingNode);         // Bss
        
// 	if ((i==0 && j==0)||(i==0 && j==nbZ)||(i==nbX && j==nbZ)||(i==nbX && j==0))
// 		body->isDynamic = false;
// 	else
// 		body->isDynamic = true;
        body->isDynamic = false; // During interaction debuging
        
        node->radius                    = 0.5 * membraneThickness;
	node->diffuseColor		= Vector3r(0.7,0.7,0.7);
	node->wire			= false;
	node->shadowCaster		= false;

        Vector3r position               = Vector3r(i*XLength/(double)nbX,0.0,j*ZLength/(double)nbZ);
        physics->velocity               = Vector3r(0,0,0);
        physics->acceleration           = Vector3r(0,0,0);
	physics->mass                   = 1.0; // FIXME - Random value. It depends on the time step (?)
        physics->se3.position = position;

        //inode->diffuseColor              = Vector3r(0.5,0.5,1.0);
        
	body->interactingGeometry	= inode;
	body->geometricalModel		= node;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}




void MembraneTest::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
        interactionGeometryDispatcher->add("InteractingSphere2BssSweptSphereLineSegment4SpheresContactGeometry");
	
        shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
        interactionPhysicsDispatcher->add("SimpleViscoelasticRelationships");

	shared_ptr<InteractingGeometryMetaEngine> interactingGeometryDispatcher	= shared_ptr<InteractingGeometryMetaEngine>(new InteractingGeometryMetaEngine);
	interactingGeometryDispatcher->add("ef2_BshTube_BssSweptSphereLineSegment_makeBssSweptSphereLineSegment");
	
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere2AABB");
        boundingVolumeDispatcher->add("ef2_BssSweptSphereLineSegment_AABB_makeAABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry2AABB"); 
	
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("NewtonsForceLaw");
	applyActionDispatcher->add("NewtonsMomentumLaw");

	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("LeapFrogPositionIntegrator");

	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("LeapFrogOrientationIntegrator");

	shared_ptr<ConstitutiveLawDispatcher> constitutiveLaw(new ConstitutiveLawDispatcher);
	constitutiveLaw->add("ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw");

	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(boundingVolumeDispatcher);	
	rootBody->engines.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
        rootBody->engines.push_back(constitutiveLaw);
	rootBody->engines.push_back(gravityCondition);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);
        rootBody->engines.push_back(orientationIntegrator);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(interactingGeometryDispatcher);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
}



void MembraneTest::positionRootBody(shared_ptr<MetaBody>& rootBody) 
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



YADE_PLUGIN("MembraneTest");
