
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

#include "SDECDynamicEngine.hpp"
#include "SDECDiscreteElement.hpp"
#include "ClosestFeatures.hpp"
#include "Omega.hpp"
#include "Contact.hpp"
#include "NonConnexBody.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDECDynamicEngine::SDECDynamicEngine() : DynamicEngine()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDECDynamicEngine::~SDECDynamicEngine()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECDynamicEngine::processAttributes()
{
	DynamicEngine::processAttributes();
	// PROCESS DESIRED ATTRIBUTES HERE
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECDynamicEngine::registerAttributes()
{
	DynamicEngine::registerAttributes();
	// REGISTER DESIRED ATTRIBUTES HERE
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECDynamicEngine::respondToCollisions(Body* body, const std::list<shared_ptr<Interaction> >& interactions)
{	
	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	vector<shared_ptr<Body> >& bodies = ncb->bodies;
	
	float stiffness = 10000;
	float viscosity = 10;
	
	Vector3 gravity = Omega::instance().gravity;
	if (first)
	{
		forces.resize(bodies.size());
		normalForces.resize(bodies.size());
		shearForces.resize(bodies.size());
		moments.resize(bodies.size());
		prevVelocities.resize(bodies.size());
	}
	
	fill(forces.begin(),forces.end(),Vector3(0,0,0));
	fill(moments.begin(),moments.end(),Vector3(0,0,0));
	
	std::list<shared_ptr<Interaction> >::const_iterator cti = interactions.begin();
	std::list<shared_ptr<Interaction> >::const_iterator ctiEnd = interactions.end();
	for( ; cti!=ctiEnd ; ++cti)
	{
		shared_ptr<Contact> contact = static_pointer_cast<Contact>(*cti);
		shared_ptr<SDECDiscreteElement> de1 = dynamic_pointer_cast<SDECDiscreteElement>(bodies[contact->id1]);
		shared_ptr<SDECDiscreteElement> de2 = dynamic_pointer_cast<SDECDiscreteElement>(bodies[contact->id2]);
				
		std::vector<std::pair<Vector3,Vector3> >::iterator cpi = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionModel))->closestsPoints.begin();
		std::vector<std::pair<Vector3,Vector3> >::iterator cpiEnd = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionModel))->closestsPoints.end();
		float size = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionModel))->closestsPoints.size();
		for( ; cpi!=cpiEnd ; ++cpi)
		{	
		
			float kn = 2*(de1->kn*de2->kn)/(de1->kn+de2->kn); 
			float ks = 2*(de1->ks*de2->ks)/(de1->ks+de2->ks); 

			Vector3 p1 = (*cpi).first;
			Vector3 p2 = (*cpi).second;

			Vector3 p = 0.5*(p1+p2);
			
			Vector3 o1p = (p - de1->se3.translation);
			Vector3 o2p = (p - de2->se3.translation);
			
			Vector3 dir = p2-p1;
			float l  = dir.unitize();
			float elongation  = l*l;
			
			Vector3 v1 = de1->velocity+o1p.cross(de1->angularVelocity);
			Vector3 v2 = de2->velocity+o2p.cross(de2->angularVelocity);
			float relativeVelocity = dir.dot(v2-v1);
			Vector3 f = (elongation*stiffness+relativeVelocity*viscosity)/size*dir;

			forces[contact->id1] += f;
			forces[contact->id2] -= f;
		
			moments[contact->id1] += o1p.cross(f);
			moments[contact->id2] -= o2p.cross(f);
		}
	}
	
	for(unsigned int i=0; i < bodies.size(); i++)
        {
		shared_ptr<SDECDiscreteElement> rb = dynamic_pointer_cast<SDECDiscreteElement>(bodies[i]);

		if (rb)
		{
			rb->acceleration += forces[i]*rb->invMass;
			rb->angularAcceleration += moments[i].multTerm(rb->invInertia);
			//if (i==35)
			//	cerr << rb->acceleration << "||" << rb->angularAcceleration << endl;
		}
        }
	
	first = false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

