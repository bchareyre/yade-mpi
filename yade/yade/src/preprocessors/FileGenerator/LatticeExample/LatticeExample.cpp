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
#include "LatticeSetParameters.hpp"
#include "LatticeBeamParameters.hpp"
#include "LatticeNodeParameters.hpp"
#include "LineSegment.hpp"
#include "Sphere.hpp"

#include "SingleBody.hpp"
#include "InteractionDescriptionSet.hpp"
#include "BoundingVolumeDispatcher.hpp"
#include "GeometricalModelDispatcher.hpp"

#include "AABB.hpp"

#include "BodyRedirectionVector.hpp"

using namespace boost;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LatticeExample::LatticeExample() : FileGenerator()
{
	nodeGroupMask 		= 1;
	beamGroupMask 		= 2;
	
	nbNodes 		= Vector3r(5,5,5);
	disorder 		= 0.5;
	maxLength 		= 1.3;
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
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);
	createActors(rootBody);
	positionRootBody(rootBody);

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

			if (a && b && (a->se3.translation - b->se3.translation).length() < (maxLength))  
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

 	return ""; 

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::createNode(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new SingleBody(0,nodeGroupMask));
	shared_ptr<LatticeNodeParameters> physics(new LatticeNodeParameters);
	shared_ptr<Sphere> gSphere(new Sphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom()) , Mathr::unitRandom()*Mathr::PI );
	
	Vector3r translation		=   Vector3r(i,j,k)
					  + Vector3r( 	  Mathr::symmetricRandom()*disorder
					  		, Mathr::symmetricRandom()*disorder
							, Mathr::symmetricRandom()*disorder);

	Real radius 			= 0.1;
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 1;
	physics->inertia		= Vector3r(1,1,1);
	physics->se3			= Se3r(translation,q);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(0.9,0.9,0.3);
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
	body = shared_ptr<Body>(new SingleBody(0,beamGroupMask));
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
	gBeam->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gBeam->wire			= false;
	gBeam->visible			= true;
	gBeam->shadowCaster		= true;
	
	body->geometricalModel		= gBeam;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::calcBeamsPositionOrientationLength(shared_ptr<ComplexBody>& body)
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
			se3Beam.translation 	= (se3A.translation + se3B.translation)*0.5;
			Vector3r dist 		= se3A.translation - se3B.translation;
			
			Real length 		= dist.normalize();
			beam->length 		= length;
			beam->initialLength 	= length;
			
			se3Beam.rotation.align( Vector3r::UNIT_X , dist );
			beam->se3 		= se3Beam;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::createActors(shared_ptr<ComplexBody>& )
{
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");

	shared_ptr<GeometricalModelDispatcher> geometricalModelDispatcher	= shared_ptr<GeometricalModelDispatcher>(new GeometricalModelDispatcher);
	geometricalModelDispatcher->add("LatticeSetParameters","LatticeSetGeometry","LatticeSet2LatticeBeams");
	
	rootBody->actors.clear();
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(geometricalModelDispatcher);
}
 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeExample::positionRootBody(shared_ptr<ComplexBody>& rootBody)
{
	rootBody->isDynamic		= false;

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<LatticeSetParameters> physics(new LatticeSetParameters);
	physics->se3			= Se3r(Vector3r(0,0,0),q);
	physics->beamGroupMask 		= beamGroupMask;
	
	shared_ptr<InteractionDescriptionSet> set(new InteractionDescriptionSet());
	
	set->diffuseColor		= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);

	shared_ptr<GeometricalModel> gm = dynamic_pointer_cast<GeometricalModel>(ClassFactory::instance().createShared("LatticeSetGeometry"));
	gm->diffuseColor 		= Vector3r(1,1,1);
	gm->wire 			= false;
	gm->visible 			= true;
	gm->shadowCaster 		= true;
	
	rootBody->interactionGeometry	= dynamic_pointer_cast<InteractionDescription>(set);	
	rootBody->boundingVolume	= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->geometricalModel 	= gm;
	rootBody->physicalParameters 	= physics;
}
	
