
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
#include "SDECPermanentLink.hpp"
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

// FIXME : remove std::list<shared_ptr<Interaction> >& interactions
void SDECDynamicEngine::filter(Body* body)
{

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	vector<shared_ptr<Body> >& bodies = ncb->bodies;

	if (first)
		interactionsPerBody.resize(bodies.size());

	vector<set<tuple<int,bool,shared_ptr<InteractionGeometry> >,lessThanTuple > >::iterator ipbi    = interactionsPerBody.begin();
	vector<set<tuple<int,bool,shared_ptr<InteractionGeometry> >,lessThanTuple > >::iterator ipbiEnd = interactionsPerBody.end();
	for(;ipbi!=ipbiEnd;++ipbi)
	{
		set<tuple<int,bool,shared_ptr<InteractionGeometry> >,lessThanTuple >::iterator ii = (*ipbi).begin();
		set<tuple<int,bool,shared_ptr<InteractionGeometry> >,lessThanTuple >::iterator iiEnd = (*ipbi).end();
		set<tuple<int,bool,shared_ptr<InteractionGeometry> >,lessThanTuple >::iterator tmpi;
		for(;ii!=iiEnd;)
		{
			tmpi = ii;
			tmpi++;
			bool &access = const_cast<bool&>((*ii).get<1>());
			if (access == false)
				(*ipbi).erase(ii);
			else
				access = false;
			ii = tmpi;
		}
	}
	
	list<shared_ptr<Interaction> >::const_iterator cti = ncb->interactions.begin();
	list<shared_ptr<Interaction> >::const_iterator ctiEnd = ncb->interactions.end();
	for( ; cti!=ctiEnd ; ++cti)
	{
		shared_ptr<Contact> contact = static_pointer_cast<Contact>(*cti);

		int id1 = contact->id1;
		int id2 = contact->id2;
		
		if (id1>=id2)
			swap(id1,id2);

		bool accessed = false;
		tuple<int,bool,shared_ptr<InteractionGeometry> > t(id2,accessed,(*cti)->interactionGeometry);
		
		pair<set<tuple<int,bool,shared_ptr<InteractionGeometry> >,lessThanTuple >::iterator,bool> intertionResult;

		intertionResult	= interactionsPerBody[id1].insert(t);
		
		bool& wasAccessed = const_cast<bool&>(intertionResult.first->get<1>());
		wasAccessed	= true;


		(*cti)->isNew = intertionResult.second;
		
		shared_ptr<SDECContactModel> scm = dynamic_pointer_cast<SDECContactModel>((*cti)->interactionGeometry);

		// here we want to get new geometrical info about contact but we want to remember physical infos from previous time step about it
		Vector3 cp = scm->contactPoint;
		Vector3 n = scm->normal;
		float pd = scm->penetrationDepth;
		float r1 = scm->radius1;
		float r2 = scm->radius2;
		
		(*cti)->interactionGeometry  = intertionResult.first->get<2>();

		scm = dynamic_pointer_cast<SDECContactModel>((*cti)->interactionGeometry);
		scm->contactPoint= cp;
		scm->normal=n;
		scm->penetrationDepth=pd;
		scm->radius1=r1;
		scm->radius2=r2;
	}
		
}

//FIXME : add reset function so it will remove bool first
void SDECDynamicEngine::respondToCollisions(Body* body)
{
	filter(body);

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	vector<shared_ptr<Body> >& bodies = ncb->bodies;
	
	Vector3 gravity = Omega::instance().getGravity();
	float dt = Omega::instance().dt;

	if (first)
	{
		forces.resize(bodies.size());
		moments.resize(bodies.size());
		first = false;
	}

	fill(forces.begin(),forces.end(),Vector3(0,0,0));
	fill(moments.begin(),moments.end(),Vector3(0,0,0));



	
	std::vector<shared_ptr<Interaction> >::const_iterator pii = ncb->permanentInteractions.begin();
	std::vector<shared_ptr<Interaction> >::const_iterator piiEnd = ncb->permanentInteractions.end();
	for( ; pii!=piiEnd ; ++pii)
	{

		shared_ptr<Contact> contact = static_pointer_cast<Contact>(*pii);

		int id1 = contact->id1;
		int id2 = contact->id2;

		/// FIXME : those lines are too dirty !
		bool accessed = false;
		int tmpId1,tmpId2;
		if (id1>=id2)
		{
			tmpId1 = id2;
			tmpId2 = id1;
		}
		else
		{
			tmpId1 = id1;
			tmpId2 = id2;
		}

		tuple<int,bool,shared_ptr<InteractionGeometry> > t(tmpId2,accessed,(*pii)->interactionGeometry);
		interactionsPerBody[tmpId1].erase(t);

		std::list<shared_ptr<Interaction> >::iterator cti = ncb->interactions.begin();
		std::list<shared_ptr<Interaction> >::iterator ctiEnd = ncb->interactions.end();
		for( ; cti!=ctiEnd ; ++cti)
		{
			shared_ptr<Contact> contact = static_pointer_cast<Contact>(*cti);
			if ((contact->id1==id1 && contact->id2==id2) || (contact->id1==id2 && contact->id2==id1))
			{
				ncb->interactions.erase(cti);
				cti=ctiEnd;
			}
		}
		
		///////////////////
		
		
		shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id1]);
		shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id2]);
		shared_ptr<SDECPermanentLink> currentContact = dynamic_pointer_cast<SDECPermanentLink>(contact->interactionGeometry);
	
		// FIXME : put these lines into another dynlib
		currentContact->kn = currentContact->initialKn;
		currentContact->ks = currentContact->initialKs;
		currentContact->equilibriumDistance = currentContact->initialEquilibriumDistance;		
//cout << currentContact->initialEquilibriumDistance << "  !!  ";
	
		float un 			= currentContact->initialEquilibriumDistance-(de1->se3.translation-de1->se3.translation).length();
//cout << un << "  !!  ";
		currentContact->contactPoint	= de1->se3.translation+(currentContact->radius1-0.5*-un)*currentContact->normalForce;
//cout << currentContact->contactPoint << "  !!  "<<endl;

		currentContact->normalForce	= currentContact->kn*un*currentContact->normal;

		Vector3 nn 			= currentContact->prevNormal.cross(currentContact->normal);		
		currentContact->shearForce     -= currentContact->shearForce.cross(nn);
		float a 			= dt*0.5*currentContact->normal.dot(de1->angularVelocity+de2->angularVelocity);
		Vector3 axis 			= a*currentContact->normal;
		currentContact->shearForce     -= currentContact->shearForce.cross(axis);
		
		Vector3 x	= currentContact->contactPoint; // FIXME : it's now a penetration depth, but with initialized conditions it won't be
		Vector3 c1x	= (x - de1->se3.translation);
		Vector3 c2x	= (x - de2->se3.translation);

		Vector3 relativeVelocity 	= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
		Vector3 shearVelocity		= relativeVelocity-currentContact->normal.dot(relativeVelocity)*currentContact->normal;
		Vector3 shearDisplacement	= shearVelocity*dt;
		currentContact->shearForce      -=  currentContact->ks*shearDisplacement;
		
		Vector3 f = currentContact->normalForce + currentContact->shearForce;
		
		forces[id1]	-= f;
		forces[id2]	+= f;
		moments[id1]	-= c1x.cross(f);
		moments[id2]	+= c2x.cross(f);

		currentContact->prevNormal = currentContact->normal;

	}






	


	
	std::list<shared_ptr<Interaction> >::const_iterator cti = ncb->interactions.begin();
	std::list<shared_ptr<Interaction> >::const_iterator ctiEnd = ncb->interactions.end();
	for( ; cti!=ctiEnd ; ++cti)
	{

		shared_ptr<Contact> contact = static_pointer_cast<Contact>(*cti);

		int id1 = contact->id1;
		int id2 = contact->id2;

		shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id1]);
		shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id2]);
		shared_ptr<SDECContactModel> currentContact = dynamic_pointer_cast<SDECContactModel>(contact->interactionGeometry);
		
		if ((*cti)->isNew)
		{
			// FIXME : put these lines into a dynlib - PhysicalCollider
			currentContact->initialKn			= 2*(de1->kn*de2->kn)/(de1->kn+de2->kn);
			currentContact->initialKs			= 2*(de1->ks*de2->ks)/(de1->ks+de2->ks);
			currentContact->prevNormal 			= currentContact->normal;
			currentContact->shearForce			= Vector3(0,0,0);
			currentContact->initialEquilibriumDistance	= currentContact->radius1+currentContact->radius2;
		}
	
		// FIXME : put these lines into another dynlib
		currentContact->kn = currentContact->initialKn;
		currentContact->ks = currentContact->initialKs;
		currentContact->equilibriumDistance = currentContact->initialEquilibriumDistance;		

		//float un = currentContact->initialEquilibriumDistance-(de1->se3.translation-de1->se3.translation).length();
		float un 			= currentContact->penetrationDepth; // FIXME : it's now a penetration depth, but with initialized conditions it won't be
		currentContact->normalForce	= currentContact->kn*un*currentContact->normal;

		Vector3 nn 			= currentContact->prevNormal.cross(currentContact->normal);		
		currentContact->shearForce     -= currentContact->shearForce.cross(nn);
		float a 			= dt*0.5*currentContact->normal.dot(de1->angularVelocity+de2->angularVelocity);
		Vector3 axis 			= a*currentContact->normal;
		currentContact->shearForce     -= currentContact->shearForce.cross(axis);
		
		Vector3 x	= currentContact->contactPoint; // FIXME : it's now a penetration depth, but with initialized conditions it won't be
		Vector3 c1x	= (x - de1->se3.translation);
		Vector3 c2x	= (x - de2->se3.translation);

		Vector3 relativeVelocity 	= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
		Vector3 shearVelocity		= relativeVelocity-currentContact->normal.dot(relativeVelocity)*currentContact->normal;
		Vector3 shearDisplacement	= shearVelocity*dt;
		currentContact->shearForce      -=  currentContact->ks*shearDisplacement;
		
		Vector3 f = currentContact->normalForce + currentContact->shearForce;
		
		forces[id1]	-= f;
		forces[id2]	+= f;
		moments[id1]	-= c1x.cross(f);
		moments[id2]	+= c2x.cross(f);

		currentContact->prevNormal = currentContact->normal;

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
				forces[i][j] -= 0.1*f*sign;
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
				moments[i][j] -= 0.1*m*sign;
			}

			de->acceleration += forces[i]*de->invMass;
			de->angularAcceleration += moments[i].multTerm(de->invInertia);
		}
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

