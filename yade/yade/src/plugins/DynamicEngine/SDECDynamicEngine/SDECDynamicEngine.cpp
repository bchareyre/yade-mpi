
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
		prevInteractions.clear();
		first = false;
	}

	fill(forces.begin(),forces.end(),Vector3(0,0,0));
	fill(moments.begin(),moments.end(),Vector3(0,0,0));

	map<pair<int,int> , interactionInfo, lessThanPair>::iterator pii = prevInteractions.begin();
	map<pair<int,int> , interactionInfo, lessThanPair>::iterator tmpi;
	for(;pii!=prevInteractions.end();)
	{
		tmpi = pii;
		tmpi++;
		if ((*pii).second.accessed == false)
			prevInteractions.erase(pii);
		else
			(*pii).second.accessed = false;
		pii = tmpi;
	}

//	if(interactions.size() == 0)
//		prevInteractions.clear();

	std::list<shared_ptr<Interaction> >::const_iterator cti = interactions.begin();
	std::list<shared_ptr<Interaction> >::const_iterator ctiEnd = interactions.end();
	for( ; cti!=ctiEnd ; ++cti)
	{

		shared_ptr<Contact> contact = static_pointer_cast<Contact>(*cti);

		int id1 = contact->id1;
		int id2 = contact->id2;

		shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id1]);
		shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>(bodies[id2]);
		shared_ptr<ClosestFeatures> cf 		= dynamic_pointer_cast<ClosestFeatures>(contact->interactionModel);

		Vector3 p1 = cf->closestsPoints[0].first;
		Vector3 p2 = cf->closestsPoints[0].second;

		pair<int,int> idPair;
		if (id1<id2)
			idPair = pair<int,int>(id1,id2);
		else
			idPair = pair<int,int>(id2,id1);

		/*if(id1==2 && id2==3)
		{
			//cerr << "..\n";
			static int oo=0;
			cerr << ++oo << endl;
			Omega::instance().dt=0.000005;
			dt=Omega::instance().dt;
		}*/

		pii = prevInteractions.find(idPair);

		if (pii==prevInteractions.end())
		{
			// FIXME : put these lines into a dynlib - PhysicalCollider
			interactionInfo ni;
			ni.initialKn = 2*(de1->kn*de2->kn)/(de1->kn+de2->kn);
			ni.initialKs = 2*(de1->ks*de2->ks)/(de1->ks+de2->ks);
			ni.normal = (p1-p2).normalize();
			ni.accessed = true;
			(*pii).second.shearForce = Vector3(0,0,0);
			shared_ptr<Sphere> s1 = dynamic_pointer_cast<Sphere>(de1->cm);
			shared_ptr<Sphere> s2 = dynamic_pointer_cast<Sphere>(de2->cm);

			if (s1 && s2) // 1
				ni.initialEquilibriumDistance = s1->radius+s2->radius;
			else if (s1 && !s2)
				ni.initialEquilibriumDistance = s1->radius+2*s1->radius;
			else if (s2 && !s1)
				ni.initialEquilibriumDistance = s2->radius+2*s2->radius;
			else
				throw;
			pii = (prevInteractions.insert(map<pair<int,int> , interactionInfo, lessThanPair>::value_type( idPair , ni ))).first;
		}
		else
			(*pii).second.accessed = true;

		// FIXME : put these lines into another dynlib
		(*pii).second.kn = (*pii).second.initialKn;
		(*pii).second.ks = (*pii).second.initialKs;
		(*pii).second.equilibriumDistance = (*pii).second.initialEquilibriumDistance;

		(*pii).second.prevNormal = (*pii).second.normal;
		(*pii).second.normal = (p1-p2).normalize();

		//float un = (*pii).second.initialEquilibriumDistance-(de1->se3.translation-de1->se3.translation).length();

		// 2
		float un = (p2-p1).length(); // FIXME : it's now a penetration depth, but with initialized conditions it won't be
		(*pii).second.normalForce = (*pii).second.kn*un*(*pii).second.normal;

//cout << id1 << " " << id2 << " 1 - " << (*pii).second.shearForce << endl;

		Vector3 nn = (*pii).second.prevNormal.cross((*pii).second.normal);
		//if ( nn.length() > 0.0000001 )
		//	nn=nn.normalize();

		(*pii).second.shearForce -= (*pii).second.shearForce.cross(   nn   );
//cout << id1 << " " << id2 << " 2 - " << (*pii).second.shearForce << endl;
		float a = dt*0.5*(*pii).second.normal.dot(de1->angularVelocity+de2->angularVelocity);
		Vector3 axis = a*(*pii).second.normal;

		(*pii).second.shearForce -= (*pii).second.shearForce.cross(axis);
//cout << id1 << " " << id2 << " 3 - " << (*pii).second.shearForce << endl;
		Vector3 x = 0.5*(p1+p2); // FIXME : it's now a penetration depth, but with initialized conditions it won't be
		Vector3 c1x = (x - de1->se3.translation);
		Vector3 c2x = (x - de2->se3.translation);

		Vector3 relativeVelocity = (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));

		//(*pii).second.shearForce = relativeVelocity.normalize()*(*pii).second.shearForce.length();
		//cout << (*pii).second.shearForce.normalize() << "  || " <<relativeVelocity.normalize() << endl;

		Vector3 shearVelocity = relativeVelocity-(*pii).second.normal.dot(relativeVelocity)*(*pii).second.normal;
		Vector3 shearDisplacement = shearVelocity*dt;
		(*pii).second.shearForce -=  (*pii).second.ks*shearDisplacement;
//cout << id1 << " " << id2 << " 4 - " << (*pii).second.shearForce << endl;
		Vector3 f = (*pii).second.normalForce + (*pii).second.shearForce;

		forces[id1] -= f;
		forces[id2] += f;

		moments[id1] -= c1x.cross(f);
		moments[id2] += c2x.cross(f);
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

