/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko				 				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<boost/random.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingFacet.hpp>
#ifdef YADE_SHAPE
	#include<yade/pkg-common/Facet.hpp>
	#include<yade/pkg-common/Sphere.hpp>
#endif
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include"SpheresFactory.hpp"
#include<sstream>

YADE_PLUGIN((SpheresFactory));
CREATE_LOGGER(SpheresFactory);

boost::variate_generator<boost::mt19937,boost::uniform_real<> > 
	SpheresFactory::randomUnit(boost::mt19937(),boost::uniform_real<>(0,1));
boost::variate_generator<boost::mt19937,boost::uniform_real<> >
	SpheresFactory::randomSymmetricUnit(boost::mt19937(),boost::uniform_real<>(-1,1));

SpheresFactory::SpheresFactory() 
{
	factoryFacets.clear();
    volumeSection=false;
	maxAttempts=20;
	radius=0.01;
	radiusRange=0;
	velocity=Vector3r(0,0,0);
	velocityRange=Vector3r(0,0,0);
	angularVelocity=Vector3r(0,0,0);
	angularVelocityRange=Vector3r(0,0,0);
	young		= 0;
	poisson		= 0;
	frictionAngle = 0;
	density 	= 2400;
	first_run = true;
	color=Vector3r(0.8,0.8,0.8);
	pySpheresCreator="";
}

SpheresFactory::~SpheresFactory()
{
	
}

void SpheresFactory::action(MetaBody* ncb)
{
	if (first_run)
	{
		FOREACH(shared_ptr<Engine> eng, ncb->engines)
		{
			bI=dynamic_cast<Collider*>(eng.get());
			if (bI) break;
		}
		if (!bI) 
		{
			LOG_FATAL("Can't find Collider." );
			return;
		}
		iGME=dynamic_cast<InteractionGeometryDispatcher*>(ncb->engineByName("InteractionGeometryDispatcher").get());
		if (!iGME) 
		{
			LOG_FATAL("Can't find InteractionGeometryDispatcher." );
			return;
		}
		first_run=false;
		randomFacet= shared_ptr<RandomInt>(new RandomInt(boost::minstd_rand(),boost::uniform_int<>(0,factoryFacets.size()-1)));
	}



	for (int attempt=0; attempt<maxAttempts; ++attempt)
	{
        Vector3r position = (volumeSection) ? generatePositionInVolume():generatePositionOnSurface();

		Real r=radius+radiusRange*randomSymmetricUnit();

		BoundingVolume bv;
		bv.min = Vector3r(position[0]-r, position[1]-r, position[2]-r);
		bv.max = Vector3r(position[0]+r, position[1]+r, position[2]+r);

		shared_ptr<Body> sphere;
		createSphere(sphere,position,r);

		bool is_overlap=false;
		vector<body_id_t> probedBodies=bI->probeBoundingVolume(bv);
		FOREACH(body_id_t id, probedBodies){
			if (iGME->explicitAction(sphere,Body::byId(bI->probedBodies[i]))->interactionGeometry){
				is_overlap=true;
				break;
			}
		}
		if (is_overlap) continue;
		if (pySpheresCreator!="")
		{
			ostringstream command;
			command << pySpheresCreator << "((" << position[0] << ',' << position[1] << ',' << position[2] << ")," << r << ')';
			PyGILState_STATE gstate;
				gstate = PyGILState_Ensure();
				PyRun_SimpleString(command.str().c_str()); 
			PyGILState_Release(gstate);
		}
		else
		{
			ncb->bodies->insert(sphere);
		}
		//bI->action(ncb);
		return;
	}
	LOG_WARN("Can't placing sphere during " << maxAttempts << " attemps.");
}

Vector3r SpheresFactory::generatePositionOnSurface()
{
    body_id_t facetId = factoryFacets[(*randomFacet)()];
    Real t1 = randomUnit();
    Real t2 = randomUnit()*(1-t1);

    shared_ptr<Body> facet = Body::byId(facetId);
    InteractingFacet* gfacet = static_cast<InteractingFacet*>(facet->interactingGeometry.get());



    return t1*(gfacet->vertices[1]-gfacet->vertices[0])+t2*(gfacet->vertices[2]-gfacet->vertices[0])+gfacet->vertices[0]+facet->physicalParameters->se3.position;
    
}

Vector3r SpheresFactory::generatePositionInVolume()
{
    Vector3r p1 = generatePositionOnSurface();
    Vector3r p2 = generatePositionOnSurface();
    Real t = randomUnit();
    return p1+t*(p2-p1);
    
}
void SpheresFactory::createSphere(shared_ptr<Body>& body, const Vector3r& position, Real r)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),1));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	body->isDynamic			= true;
	
	physics->velocity		= Vector3r(//
			velocity[0]+velocityRange[0]*randomSymmetricUnit(),
			velocity[1]+velocityRange[1]*randomSymmetricUnit(),
			velocity[2]+velocityRange[2]*randomSymmetricUnit());
	physics->angularVelocity= Vector3r(//
			angularVelocity[0]+angularVelocityRange[0]*randomSymmetricUnit(),
			angularVelocity[1]+angularVelocityRange[1]*randomSymmetricUnit(),
			angularVelocity[2]+angularVelocityRange[2]*randomSymmetricUnit());
	physics->mass			= 4.0/3.0*Mathr::PI*r*r*r*density;
	physics->inertia = Vector3r(//
			2.0/5.0*physics->mass*r*r,
			2.0/5.0*physics->mass*r*r,
			2.0/5.0*physics->mass*r*r); 
	physics->se3			= Se3r(position,q);
	if (young) 			physics->young			= young;
	if (poisson) 		physics->poisson		= poisson;
	if (frictionAngle) 	physics->frictionAngle	= frictionAngle;

	aabb->diffuseColor		= Vector3r(0,1,0);

	#ifdef YADE_SHAPE
		shared_ptr<Sphere> gSphere(new Sphere);
		gSphere->radius			= r;
		gSphere->diffuseColor	= color;
		gSphere->wire			= false;
		gSphere->shadowCaster	= true;
		body->geometricalModel		= gSphere;
	#endif
	
	iSphere->radius			= r;
	iSphere->diffuseColor	= Vector3r(0.8,0.3,0.3);

	body->interactingGeometry	= iSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}


YADE_REQUIRE_FEATURE(PHYSPAR);

