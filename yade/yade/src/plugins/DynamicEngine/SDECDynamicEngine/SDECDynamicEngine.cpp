
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
#include "SDECContactModel.hpp"
#include "Omega.hpp"
#include "Contact.hpp"
#include "NonConnexBody.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDECDynamicEngine::SDECDynamicEngine() : DynamicEngine()
{
	first=true;
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

//FIXME : add reset function so it will remove bool first
void SDECDynamicEngine::respondToCollisions(Body* body, const std::list<shared_ptr<Interaction> >& interactions)
{

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	vector<shared_ptr<Body> >& bodies = ncb->bodies;

	Vector3 gravity = Omega::instance().gravity;
	float dt = Omega::instance().dt;

	if (first)
	{
		forces.resize(bodies.size());
		moments.resize(bodies.size());
		first = false;
		interactionsPerBody.resize(bodies.size());
	}

	fill(forces.begin(),forces.end(),Vector3(0,0,0));
	fill(moments.begin(),moments.end(),Vector3(0,0,0));

	vector<set<pair<int,interactionInfo>,lessThanPair > >::iterator ipbi    = interactionsPerBody.begin();
	vector<set<pair<int,interactionInfo>,lessThanPair > >::iterator ipbiEnd = interactionsPerBody.end();	
	for(;ipbi!=ipbiEnd;++ipbi)
	{
		set<pair<int,interactionInfo>,lessThanPair >::iterator ii = (*ipbi).begin();
		set<pair<int,interactionInfo>,lessThanPair >::iterator iiEnd = (*ipbi).end();
		set<pair<int,interactionInfo>,lessThanPair >::iterator tmpi;
		for(;ii!=iiEnd;)
		{
			tmpi = ii;
			tmpi++;
			interactionInfo& prevContact = const_cast<interactionInfo&>((*ii).second);
			if (prevContact.accessed == false)
				(*ipbi).erase(ii);
			else
				prevContact.accessed = false;
			ii = tmpi;
		}
	}
	
	std::list<shared_ptr<Interaction> >::const_iterator cti = interactions.begin();
	std::list<shared_ptr<Interaction> >::const_iterator ctiEnd = interactions.end();
	for( ; cti!=ctiEnd ; ++cti)
	{

		shared_ptr<Contact> contact = static_pointer_cast<Contact>(*cti);

		int id1 = contact->id1;
		int id2 = contact->id2;

		shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id1]);
		shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id2]);
		shared_ptr<SDECContactModel> scm 	= dynamic_pointer_cast<SDECContactModel>(contact->interactionModel);
		
		if (id1>=id2)
			swap(id1,id2);

		interactionInfo ni;
		pair<int,interactionInfo> p(id2,ni);
		pair<set<pair<int,interactionInfo>,lessThanPair >::iterator,bool> intertionResult = interactionsPerBody[id1].insert(p);

		interactionInfo& currentContact = const_cast<interactionInfo&>(intertionResult.first->second);
		currentContact.accessed		= true;
		
		if (intertionResult.second)
		{
			cout << "not here" << endl;
			// FIXME : put these lines into a dynlib - PhysicalCollider
			currentContact.initialKn			= 2*(de1->kn*de2->kn)/(de1->kn+de2->kn);
			currentContact.initialKs			= 2*(de1->ks*de2->ks)/(de1->ks+de2->ks);
			currentContact.normal 				= scm->normal;
			currentContact.shearForce			= Vector3(0,0,0);
			currentContact.initialEquilibriumDistance	= scm->radius1+scm->radius2;
		}		
		
		// FIXME : put these lines into another dynlib
		currentContact.kn = currentContact.initialKn;
		currentContact.ks = currentContact.initialKs;
		currentContact.equilibriumDistance = currentContact.initialEquilibriumDistance;

		currentContact.prevNormal = currentContact.normal;
		currentContact.normal = scm->normal;

		//float un = currentContact.initialEquilibriumDistance-(de1->se3.translation-de1->se3.translation).length();

		float un = scm->penetrationDepth; // FIXME : it's now a penetration depth, but with initialized conditions it won't be
		currentContact.normalForce = currentContact.kn*un*currentContact.normal;
		Vector3 nn = currentContact.prevNormal.cross(currentContact.normal);

		
		currentContact.shearForce      -= currentContact.shearForce.cross(nn);
		float a 			= dt*0.5*currentContact.normal.dot(de1->angularVelocity+de2->angularVelocity);
		Vector3 axis 			= a*currentContact.normal;
		currentContact.shearForce      -= currentContact.shearForce.cross(axis);
		
		Vector3 x	= scm->contactPoint; // FIXME : it's now a penetration depth, but with initialized conditions it won't be
		Vector3 c1x	= (x - de1->se3.translation);
		Vector3 c2x	= (x - de2->se3.translation);

		Vector3 relativeVelocity 	= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
		Vector3 shearVelocity		= relativeVelocity-currentContact.normal.dot(relativeVelocity)*currentContact.normal;
		Vector3 shearDisplacement	= shearVelocity*dt;
		currentContact.shearForce      -=  currentContact.ks*shearDisplacement;
		
		Vector3 f = currentContact.normalForce + currentContact.shearForce;
		
		forces[id1]	-= f;
		forces[id2]	+= f;
		moments[id1]	-= c1x.cross(f);
		moments[id2]	+= c2x.cross(f);
	}

	for(unsigned int i=0; i < bodies.size(); i++)
        {
		shared_ptr<SDECDiscreteElement> de = dynamic_pointer_cast<SDECDiscreteElement>(bodies[i]);
		if (de)
		{

			forces[i] += gravity*de->mass;
			int sign;
			float f = forces[i].length();

			for(int j=0;j<3;j++)
			{
				if (de->velocity[j]==0)
					sign=0;
				else if (de->velocity[j]>0)
					sign=1;
				else
					sign=-1;
				forces[i][j] -= 0.3*f*sign;
			}


			float m = moments[i].length();

			for(int j=0;j<3;j++)
			{
				if (de->angularVelocity[j]==0)
					sign=0;
				else if (de->angularVelocity[j]>0)
					sign=1;
				else
					sign=-1;
				moments[i][j] -= 0.3*m*sign;
			}

			de->acceleration += forces[i]*de->invMass;
			de->angularAcceleration += moments[i].multTerm(de->invInertia);
		}
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

