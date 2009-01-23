/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko				 				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<boost/random.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingFacet.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include"SpheresFactory.hpp"

CREATE_LOGGER(SpheresFactory);

SpheresFactory::SpheresFactory() : first_run(true) 
{

}

SpheresFactory::~SpheresFactory()
{
	
}



void SpheresFactory::action(MetaBody* ncb)
{
	if (first_run)
	{
		//FIXME: Why dynamic_cast failed here???
		Engine* eng = ncb->engineByLabel(labelBroadInteractor).get();
		bI=dynamic_cast<BroadInteractor*>(eng);
		if (!bI) 
		{
			LOG_FATAL("For engine with label '" << labelBroadInteractor << "' dynamic_cast from class '" << eng->getClassName() << "' to class 'BroadInteractor' failed!" );
			return;
		}
		first_run=false;
	}

	static boost::variate_generator<boost::minstd_rand,boost::uniform_int<> > randomFacet(boost::minstd_rand(),boost::uniform_int<>(0,factoryFacets.size()-1));

	static boost::variate_generator<boost::mt19937,boost::uniform_real<> > random(boost::mt19937(),boost::uniform_real<>(0,1));

	body_id_t facetId = factoryFacets[randomFacet()];
	Real t1 = random();
	Real t2 = random()*(1-t1);

	shared_ptr<Body> facet = Body::byId(factoryFacets[facetId]);
	InteractingFacet* ifacet = static_cast<InteractingFacet*>(facet->interactingGeometry.get());

	Vector3r position = t1*(ifacet->vertices[1]-ifacet->vertices[0])+t2*(ifacet->vertices[2]-ifacet->vertices[0])+ifacet->vertices[0]+facet->physicalParameters->se3.position;

	Real radius=0.1;

	shared_ptr<Body> sphere;
	createSphere(sphere,position,radius);
	ncb->bodies->insert(sphere);

	bI->action(ncb);
}

void SpheresFactory::createSphere(shared_ptr<Body>& body, const Vector3r& position, Real radius)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),1));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*2400;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);
	physics->young			= 0.15e9;
	physics->poisson		= 0.3;
	//physics->frictionAngle	= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(0,1,0);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(0.8,0.3,0.3);

	body->interactingGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}

