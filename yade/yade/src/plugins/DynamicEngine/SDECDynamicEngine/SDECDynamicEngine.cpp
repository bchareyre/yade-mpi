
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
#include "Sphere.hpp" //FIXME : remove this line !

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
		moments.resize(bodies.size());

		//normalForces.resize(bodies.size());
		//shearForces.resize(bodies.size());

		prevVelocities.resize(bodies.size());

		prevInteractions.clear();
	}

	fill(forces.begin(),forces.end(),Vector3(0,0,0));
	fill(moments.begin(),moments.end(),Vector3(0,0,0));

	map<pair<int,int> , interactionInfo, lessThanPair>::iterator pi2 = prevInteractions.begin();
	map<pair<int,int> , interactionInfo, lessThanPair>::iterator tmpi;
	for(;pi2!=prevInteractions.end();)
	{
		tmpi = pi2;
		tmpi++;
		if ((*pi2).second.accessed == false)
			prevInteractions.erase(pi2);
		else
			(*pi2).second.accessed = false;
		pi2 = tmpi;
	}

//	if(interactions.size() == 0)
//		prevInteractions.clear();


	std::list<shared_ptr<Interaction> >::const_iterator cti = interactions.begin();
	std::list<shared_ptr<Interaction> >::const_iterator ctiEnd = interactions.end();
	for( ; cti!=ctiEnd ; ++cti)
	{
		shared_ptr<Contact> contact = static_pointer_cast<Contact>(*cti);

		int id1,id2;
		if (contact->id1<contact->id2)
		{
			id1 = contact->id1;
			id2 = contact->id2;
			//shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id1]);
			//shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id2]);
		}
		else
		{
			id1 = contact->id2;
			id2 = contact->id1;
			//shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id1]);
			//shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id2]);
		}

		shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id1]);
		shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id2]);
		shared_ptr<ClosestFeatures> cf 		= dynamic_pointer_cast<ClosestFeatures>(contact->interactionModel);

		Vector3 p1 = cf->closestsPoints[0].first;
		Vector3 p2 = cf->closestsPoints[0].second;

		map<pair<int,int> , interactionInfo, lessThanPair>::iterator pii = prevInteractions.find(pair<int,int>(id1,id2));

		if (pii==prevInteractions.end())
		{
			// FIXME : put these lines into a dynlib - PhysicalCollider
			interactionInfo ni;
			ni.initialKn = 2*(de1->kn*de2->kn)/(de1->kn+de2->kn);
			ni.initialKs = 2*(de1->ks*de2->ks)/(de1->ks+de2->ks);
			ni.normal = (p2-p1).normalize();
			//mi.initialEquilibriumDistance = (de1->se3.translation-de1->se3.translation).length();
			ni.accessed = true;
			shared_ptr<Sphere> s1 = dynamic_pointer_cast<Sphere>(de1->cm);
			shared_ptr<Sphere> s2 = dynamic_pointer_cast<Sphere>(de2->cm);

			if (s1 && s2)
				ni.initialEquilibriumDistance = s1->radius+s2->radius;
			else if (s1)
				ni.initialEquilibriumDistance = s1->radius+2*s1->radius;
			else if (s2)
				ni.initialEquilibriumDistance = s2->radius+2*s2->radius;
			else
				throw;

			pair<int,int> pr(id1,id2);
			pii = (prevInteractions.insert(map<pair<int,int> , interactionInfo, lessThanPair>::value_type( pr , ni ))).first;
		}
		else
			(*pii).second.accessed = true;

// 		Vector3 normalForce;			// normal force applied on a DE
// 		Vector3 shearForce;			// shear force applied on a DE

		// FIXME : put these lines into another dynlib
		(*pii).second.kn = (*pii).second.initialKn;
		(*pii).second.ks = (*pii).second.initialKs;
		(*pii).second.equilibriumDistance = (*pii).second.initialEquilibriumDistance;

		(*pii).second.prevNormal = (*pii).second.normal;
		(*pii).second.normal = (p2-p1).normalize();

		//float un = (*pii).second.initialEquilibriumDistance-(de1->se3.translation-de1->se3.translation).length();

		float un = (p2-p1).length(); // FIXME : it's now a penetration depth, but with initialized conditions it won't be
		(*pii).second.normalForce = (*pii).second.kn*un*(*pii).second.normal;

		(*pii).second.shearForce -= (*pii).second.shearForce.cross((*pii).second.prevNormal.cross((*pii).second.normal));

		float a = 0.5*(*pii).second.normal.dot(de1->angularVelocity+de2->angularVelocity);
		Vector3 axis = a*(*pii).second.normal;

		(*pii).second.shearForce -= (*pii).second.shearForce.cross(axis);

		Vector3 x = 0.5*(p1+p2); // FIXME : it's now a penetration depth, but with initialized conditions it won't be
		Vector3 c1x = (x - de1->se3.translation);
		Vector3 c2x = (x - de2->se3.translation);

		Vector3 relativeVelocity = (de1->velocity+de1->angularVelocity.cross(c1x)) - (de2->velocity+de2->angularVelocity.cross(c2x));

		Vector3 shearVelocity = relativeVelocity-(*pii).second.normal.dot(relativeVelocity)*(*pii).second.normal;
		Vector3 shearDisplacement = shearVelocity*Omega::instance().dt;
		(*pii).second.shearForce -=  (*pii).second.ks*shearDisplacement;

		Vector3 f =(*pii).second.normalForce + (*pii).second.shearForce;

		forces[id1] += f;
		forces[id2] -= f;

		moments[id1] += c1x.cross(f);
		moments[id2] -= c2x.cross(f);
		//moments[id1] += f.cross(c1x);
		//moments[id2] -= f.cross(c2x);

		//cerr << relativeVelocity.normalize() << " || " << (*pii).second.shearForce.normalize() << endl;
	}


/*		Vector3 dir = p2-p1;
		float l  = dir.unitize();
		float elongation  = l*l;

		Vector3 v1 = de1->velocity+o1p.cross(de1->angularVelocity);
		Vector3 v2 = de2->velocity+o2p.cross(de2->angularVelocity);
		float relativeVelocity = dir.dot(v2-v1);
		Vector3 f = (elongation*stiffness+relativeVelocity*viscosity)/nbContactPoints*dir;

		forces[contact->id1] += f;
		forces[contact->id2] -= f;

		moments[contact->id1] += o1p.cross(f);
		moments[contact->id2] -= o2p.cross(f);
*/

	for(unsigned int i=0; i < bodies.size(); i++)
        {
		shared_ptr<SDECDiscreteElement> de = dynamic_pointer_cast<SDECDiscreteElement>(bodies[i]);

		if (de)
		{
			de->acceleration += forces[i]*de->invMass;
			de->angularAcceleration += moments[i].multTerm(de->invInertia);
		}
        }

	first = false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

