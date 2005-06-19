/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#include "LatticeExample.hpp"

#include <yade-common/LatticeSetParameters.hpp>
#include <yade-common/LatticeBeamParameters.hpp>
#include <yade-common/LatticeNodeParameters.hpp>
#include <yade-common/LineSegment.hpp>
#include <yade-common/Sphere.hpp>

#include <yade/Body.hpp>
#include <yade-common/MetaInteractingGeometry.hpp>
#include <yade-common/BoundingVolumeMetaEngine.hpp>
#include <yade-common/GeometricalModelMetaEngine.hpp>

#include <yade-common/AABB.hpp>

#include <yade-common/BodyRedirectionVector.hpp>
#include <yade-common/InteractionVecSet.hpp>
#include <yade-common/PhysicalActionVectorVector.hpp>

#include <yade-common/TranslationEngine.hpp>
#include <yade-common/PhysicalParametersMetaEngine.hpp>
#include <yade-common/PhysicalActionApplier.hpp>

#include <yade-common/LatticeLaw.hpp>

using namespace boost;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LatticeExample::LatticeExample() : FileGenerator()
{
	nodeGroupMask 		= 1;
	beamGroupMask 		= 2;
	
	nbNodes 		= Vector3r(6,4,4);
	disorder 		= 0.23;
	maxLength 		= 1.4;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LatticeExample::~LatticeExample()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::registerAttributes()
{
	REGISTER_ATTRIBUTE(nbNodes); 
	REGISTER_ATTRIBUTE(disorder);
	REGISTER_ATTRIBUTE(maxLength);
}

string LatticeExample::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

	
	rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->volatileInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->actionParameters		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

	
	shared_ptr<Body> body;
	
	for( int i=0 ; i<nbNodes[0] ; i++ )
		for( int j=0 ; j<nbNodes[1] ; j++ )
			for( int k=0 ; k<nbNodes[2] ; k++)
			{
				shared_ptr<Body> node;
				createNode(node,i,j,k);
				rootBody->bodies->insert(node);
			}
			
	BodyRedirectionVector bc;
	bc.clear();
	
	for( rootBody->bodies->gotoFirst(); rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
	{
		shared_ptr<Body>& bodyA = rootBody->bodies->getCurrent();
		rootBody->bodies->pushIterator();
		rootBody->bodies->gotoNext();
		for( ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
		{
			shared_ptr<Body>& bodyB = rootBody->bodies->getCurrent();
			shared_ptr<LatticeNodeParameters> a = dynamic_pointer_cast<LatticeNodeParameters>(bodyA->physicalParameters);
			shared_ptr<LatticeNodeParameters> b = dynamic_pointer_cast<LatticeNodeParameters>(bodyB->physicalParameters);

			if (a && b && (a->se3.position - b->se3.position).length() < (maxLength))  
			{
				shared_ptr<Body> beam;
				createBeam(beam,bodyA->getId(),bodyB->getId());
				bc.insert(beam);
			}
		}
		rootBody->bodies->popIterator();
	}

	
	for( bc.gotoFirst(); bc.notAtEnd() ; bc.gotoNext() )
		rootBody->bodies->insert(bc.getCurrent());
		
	calcBeamsPositionOrientationLength(rootBody);

	imposeTranslation(rootBody,Vector3r(-1,-1,-1),Vector3r(3,3,3),Vector3r(0,1,0),0.5);

 	return ""; 

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::createNode(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(0,nodeGroupMask));
	shared_ptr<LatticeNodeParameters> physics(new LatticeNodeParameters);
	shared_ptr<Sphere> gSphere(new Sphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom()) , Mathr::unitRandom()*Mathr::PI );
	
	Vector3r position		=   Vector3r(i,j,k)
					  + Vector3r( 	  Mathr::symmetricRandom()*disorder
					  		, Mathr::symmetricRandom()*disorder
							, Mathr::symmetricRandom()*disorder);

	Real radius 			= 0.1;
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 1;
	physics->inertia		= Vector3r(1,1,1);
	physics->se3			= Se3r(position,q);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(0.5,0.5,0.5);
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	body->geometricalModel		= gSphere;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::createBeam(shared_ptr<Body>& body, unsigned int i, unsigned int j)
{
	body = shared_ptr<Body>(new Body(0,beamGroupMask));
	shared_ptr<LatticeBeamParameters> physics(new LatticeBeamParameters);
	shared_ptr<LineSegment> gBeam(new LineSegment);
	
	Real length 			= 0.6;
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 1; // FIXME
	physics->inertia		= Vector3r(1,1,1); // FIXME
	physics->id1 			= i;
	physics->id2 			= j;

	gBeam->length			= length;
//	gBeam->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gBeam->diffuseColor		= Vector3f(0.8,0.8,0.8);
	gBeam->wire			= false;
	gBeam->visible			= true;
	gBeam->shadowCaster		= true;
	
	body->geometricalModel		= gBeam;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::calcBeamsPositionOrientationLength(shared_ptr<MetaBody>& body)
{
	for( rootBody->bodies->gotoFirst(); rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
	{
		shared_ptr<Body>& body = rootBody->bodies->getCurrent();
		if( body->getGroupMask() & beamGroupMask )
		{
			LatticeBeamParameters* beam = static_cast<LatticeBeamParameters*>(body->physicalParameters.get());
			shared_ptr<Body>& bodyA = (*(rootBody->bodies))[beam->id1];
			shared_ptr<Body>& bodyB = (*(rootBody->bodies))[beam->id2];
			Se3r& se3A 		= bodyA->physicalParameters->se3;
			Se3r& se3B 		= bodyB->physicalParameters->se3;
			
			Se3r se3Beam;
			se3Beam.position 	= (se3A.position + se3B.position)*0.5;
			Vector3r dist 		= se3A.position - se3B.position;
			
			Real length 		= dist.normalize();
			beam->length 		= length;
			beam->initialLength 	= length;
			
			se3Beam.orientation.align( Vector3r::UNIT_X , dist );
			beam->se3 		= se3Beam;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::createActors(shared_ptr<MetaBody>& )
{
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","InteractionDescriptionSet2AABB");

	shared_ptr<GeometricalModelMetaEngine> geometricalModelDispatcher	= shared_ptr<GeometricalModelMetaEngine>(new GeometricalModelMetaEngine);
	geometricalModelDispatcher->add("LatticeSetParameters","LatticeSetGeometry","LatticeSet2LatticeBeams");
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegrator");
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("Force","ParticleParameters","NewtonsForceLaw");
	
	shared_ptr<LatticeLaw> latticeLaw(new LatticeLaw);
	latticeLaw->nodeGroupMask = nodeGroupMask;
	latticeLaw->beamGroupMask = beamGroupMask;

	rootBody->actors.clear();
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(geometricalModelDispatcher);
	rootBody->actors.push_back(latticeLaw);
	rootBody->actors.push_back(applyActionDispatcher); // ????
	rootBody->actors.push_back(positionIntegrator);
	
	
	rootBody->initializers.clear();
//	rootBody->initializers.push_back(actionParameterInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	rootBody->initializers.push_back(geometricalModelDispatcher);
}	
 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::positionRootBody(shared_ptr<MetaBody>& rootBody)
{
	rootBody->isDynamic		= false;

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<LatticeSetParameters> physics(new LatticeSetParameters);
	physics->se3			= Se3r(Vector3r(0,0,0),q);
	physics->beamGroupMask 		= beamGroupMask;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	
	set->diffuseColor		= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);

	shared_ptr<GeometricalModel> gm = dynamic_pointer_cast<GeometricalModel>(ClassFactory::instance().createShared("LatticeSetGeometry"));
	gm->diffuseColor 		= Vector3r(1,1,1);
	gm->wire 			= false;
	gm->visible 			= true;
	gm->shadowCaster 		= true;
	
	rootBody->interactionGeometry	= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume	= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->geometricalModel 	= gm;
	rootBody->physicalParameters 	= physics;
}
	
/////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////
 
void LatticeExample::imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real velocity)
{
	shared_ptr<TranslationEngine> translationCondition = shared_ptr<TranslationEngine>(new TranslationEngine);
 	translationCondition->velocity  = velocity;
	direction.normalize();
 	translationCondition->translationAxis = direction;
	
	rootBody->actors.push_back(translationCondition);
	translationCondition->subscribedBodies.clear();
	
	for(rootBody->bodies->gotoFirst() ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
	{
		if( rootBody->bodies->getCurrent()->getGroupMask() & nodeGroupMask )
		{
			Vector3r pos = rootBody->bodies->getCurrent()->physicalParameters->se3.position;
			if(        pos[0] > min[0] 
				&& pos[1] > min[1] 
				&& pos[2] > min[2] 
				&& pos[0] < max[0] 
				&& pos[1] < max[1] 
				&& pos[2] < max[2] 
				&& (rootBody->bodies->getCurrent()->getGroupMask() & nodeGroupMask)
				)
			{
				rootBody->bodies->getCurrent()->isDynamic = false;
				rootBody->bodies->getCurrent()->geometricalModel->diffuseColor = Vector3r(1,0,0);
				translationCondition->subscribedBodies.push_back(rootBody->bodies->getCurrent()->getId());
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////
 
