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

#include "LatticeBox.hpp"
#include "LatticeBeamParameters.hpp"
#include "LatticeNodeParameters.hpp"
#include "LineSegment.hpp"

#include "SimpleBody.hpp"
#include "InteractionDescriptionSet.hpp"
#include "BoundingVolumeDispatcher.hpp"

#include "AABB.hpp"

using namespace boost;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LatticeBox::LatticeBox() : FileGenerator()
{
	nbNodes 		= Vector3r(5,5,5);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LatticeBox::~LatticeBox()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeBox::registerAttributes()
{
	REGISTER_ATTRIBUTE(nbNodes); 
}

string LatticeBox::generate()
{
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);
	createActors(rootBody);
	positionRootBody(rootBody);

	shared_ptr<Body> body;
	
	for( int i=0 ; i<nbNodes[0] ; i++ )
		for( int j=0 ; j<nbNodes[1] ; j++ )
			for( int k=0 ; k<nbNodes[2] ; k++)
			{
				shared_ptr<Body> beam;
				createBeam(beam,i,j,k);
				rootBody->bodies->insert(beam);
			}
 	return ""; 

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeBox::createBeam(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new SimpleBody(0,0));
	shared_ptr<LatticeBeamParameters> physics(new LatticeBeamParameters);
	shared_ptr<LineSegment> gBeam(new LineSegment);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom()) , Mathr::unitRandom()*Mathr::PI );
	
	Vector3r translation		= Vector3r(i,j,k);
	
	Real length 			= 2;
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 1;
	physics->inertia		= Vector3r(1,1,1);
	physics->se3			= Se3r(translation,q);

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

void LatticeBox::createActors(shared_ptr<ComplexBody>& )
{
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");

	rootBody->actors.clear();
	rootBody->actors.push_back(boundingVolumeDispatcher);
}
 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeBox::positionRootBody(shared_ptr<ComplexBody>& rootBody)
{
	rootBody->isDynamic		= false;

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class BodyPhysicalParameters
	physics->se3			= Se3r(Vector3r(0,0,0),q);
	physics->mass			= 0;
	physics->velocity		= Vector3r::ZERO;
	physics->acceleration		= Vector3r::ZERO;
	
	shared_ptr<InteractionDescriptionSet> set(new InteractionDescriptionSet());
	
	set->diffuseColor		= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);
	
	rootBody->interactionGeometry	= dynamic_pointer_cast<InteractionDescription>(set);	
	rootBody->boundingVolume	= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->physicalParameters 	= physics;
}
	
