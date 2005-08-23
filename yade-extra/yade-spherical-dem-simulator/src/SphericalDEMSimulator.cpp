/***************************************************************************
 *   Copyright (C) 2005 by Olivier Galizzi   *
 *   olivier.galizzi@imag.fr   *
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

#include <iostream>
#include <vector>
#include <set>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-lib-serialization/IOFormatManager.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SphericalDEMSimulator.hpp"
#include "PersistentSAPCollider.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SphericalDEMSimulator::SphericalDEMSimulator() : Factorable()
{
	alpha	= 2.5;
	beta	= 2.0;
	gamma	= 2.65;
	dt	= 0.001;
	gravity = Vector3r(0,-9.8,0);
	forceDamping = 0.3;
	momentumDamping = 0.3;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SphericalDEMSimulator::~SphericalDEMSimulator()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::run(int nbIterations)
{
	PersistentSAPCollider sap;

	// do the simulation loop
	for(int i=0;i<nbIterations;i++)
	{
		// compute dt
		computeDt(spheres,contacts);

		// detect potential collision
		sap.action(spheres,contacts);
		
		// detect real collision
		findRealCollision(spheres,contacts);
		
		// compute response
		computeResponse(spheres,contacts);

		// add damping
		addDamping(spheres);

		// apply response
		applyResponse(spheres);

		// time integration
		timeIntegration(spheres);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::loadXMLFile(const string& fileName)
{
	IOFormatManager::loadFromFile("XMLFormatManager",fileName,"rootBody",rootBody);

	//copy rootBody to more simple data structure
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for( ; bi!=biEnd ; ++bi)
	{
		

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::findRealCollision(const vector<Sphere>& spheres, ContactVecSet& contacts)
{
	ContactVecSet::iterator cvsi    = contacts.begin();
	ContactVecSet::iterator cvsiEnd = contacts.end();
	for(int id1=0 ; cvsi!=cvsiEnd ; ++cvsi, ++id1)
	{
		ContactSet::iterator csi    = cvsi->begin();
		ContactSet::iterator csiEnd = cvsiEnd->begin();
		for( ; csi!=csiEnd ; ++csi)
		{
			Contact * c = const_cast<Contact*>(&(*csi));
			
			int id2 = c->id;
			
			/// Sphere2Sphere4MacroMicroContactGeometry
			Real r = spheres[id1].radius+spheres[id2].radius;
			Vector3r n = spheres[id2].position-spheres[id1].position;
			Real pd = r-n.normalize();
			if (pd>0)
			{
				c->isReal = true;
				c->contactPoint = spheres[id1].position+n*(spheres[id1].radius-pd*0.5);

				c->radius1 = spheres[id1].radius;
				c->radius2 = spheres[id2].radius;
				c->normal  = n;
				c->penetrationDepth = pd;
			}

			///MacroMicroElasticRelationships
			if( c->isNew)
			{	
				Real Ea 	= spheres[id1].young;
				Real Eb 	= spheres[id2].young;
				Real Va 	= spheres[id1].poisson;
				Real Vb 	= spheres[id2].poisson;
				Real Da 	= c->radius1;
				Real Db 	= c->radius2;
				Real fa 	= spheres[id1].frictionAngle;
				Real fb 	= spheres[id2].frictionAngle;
	
				Real Eab	= 2*Ea*Eb/(Ea+Eb);
				Real Vab	= 2*Va*Vb/(Va+Vb);
	
				Real Dinit 	= Da+Db;
				Real Sinit 	= Mathr::PI * pow( min(Da,Db) , 2);
	
				Real Kn		= (Eab*Sinit/Dinit)*( (1+alpha)/(beta*(1+Vab) + gamma*(1-alpha*Vab) ) );
				c->initialKn	= Kn;
				c->initialKs	= Kn*(1-alpha*Vab)/(1+Vab);

				c->frictionAngle		= (fa+fb)*0.5;
				c->tangensOfFrictionAngle	= tan(c->frictionAngle); 
	
				c->prevNormal 			= c->normal;
				c->initialEquilibriumDistance	= Dinit;
	
				c->kn			= c->initialKn;
				c->ks			= c->initialKs;
				c->equilibriumDistance	= c->initialEquilibriumDistance;
			}
			else
			{
				c->kn = c->initialKn;
				c->ks = c->initialKs;
				c->equilibriumDistance = c->initialEquilibriumDistance;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void SphericalDEMSimulator::computeResponse(vector<Sphere>& spheres, ContactVecSet& contacts)
{

	/// PhysicalActionContainerReseter+GravityEngine
	vector<Sphere>::iterator si    = spheres.begin();
	vector<Sphere>::iterator siEnd = spheres.end();
	for( ; si!=siEnd ; ++si)
	{
		(*si).force    = gravity * (*si).mass;
		(*si).momentum = Vector3r(0,0,0);
	}

	///ElasticContactLaw
	ContactVecSet::iterator cvsi    = contacts.begin();
	ContactVecSet::iterator cvsiEnd = contacts.end();
	for(int id1=0 ; cvsi!=cvsiEnd ; ++cvsi, ++id1)
	{
		ContactSet::iterator csi    = cvsi->begin();
		ContactSet::iterator csiEnd = cvsiEnd->begin();
		for( ; csi!=csiEnd ; ++csi)
		{
			Contact c = *csi;

			if (c.isReal)
			{
				int id2 = c.id;
				Sphere s1 = spheres[id1];
				Sphere s2 = spheres[id2];

				Vector3r& shearForce = c.shearForce;
		
				if ( c.isNew)
					shearForce = Vector3r(0,0,0);
						
				Real un		= c.penetrationDepth;
				c.normalForce	= c.kn*un*c.normal;
		
				Vector3r axis;
				Real angle;
		
				axis	 	= c.prevNormal.cross(c.normal);
				shearForce	-= shearForce.cross(axis);
				angle		= dt*0.5*c.normal.dot(s1.angularVelocity+s2.angularVelocity);
				axis		= angle*c.normal;
				shearForce	-= shearForce.cross(axis);
			
				Vector3r x			= c.contactPoint;
				Vector3r c1x			= (x - s1.position);
				Vector3r c2x			= (x - s2.position);
				Vector3r relativeVelocity	= (s2.velocity+s2.angularVelocity.cross(c2x)) - (s1.velocity+s1.angularVelocity.cross(c1x));
				Vector3r shearVelocity		= relativeVelocity-c.normal.dot(relativeVelocity)*c.normal;
				Vector3r shearDisplacement	= shearVelocity*dt;
				shearForce 			-= c.ks*shearDisplacement;
		
				/// PFC3d SlipModel, is using friction angle and CoulombCriterion
				Real maxFs = c.normalForce.squaredLength() * pow(c.tangensOfFrictionAngle,2);
				if( shearForce.squaredLength() > maxFs )
				{
					maxFs = Mathr::sqRoot(maxFs) / shearForce.length();
					shearForce *= maxFs;
				}
				/// PFC3d SlipModel
		
				Vector3r f	= c.normalForce + shearForce;
				
				s1.force -= f;
				s2.force += f;

				s1.momentum -= c1x.cross(f);
				s2.momentum += c2x.cross(f);
				
				c.prevNormal = c.normal;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::addDamping(vector<Sphere>& spheres)
{
	vector<Sphere>::iterator si    = spheres.begin();
	vector<Sphere>::iterator siEnd = spheres.end();
	for( ; si!=siEnd ; ++si)
	{
		Sphere s = *si;
		///CundallNonViscousForceDamping
		Vector3r& f  = s.force;
		register int sign;
		for(int j=0;j<3;j++)
		{
			if (s.velocity[j] == 0)
				sign = 0;
			else if (s.velocity[j] > 0)
				sign = 1;
			else
				sign = -1;
				
			f[j] -= forceDamping * abs(f[j]) * sign;
		}

		///CundallNonViscousMomentumDamping
		Vector3r& m  = s.momentum;
		for(int j=0;j<3;j++)
		{
			if (s.angularVelocity[j]==0)
				sign = 0;
			else if (s.angularVelocity[j]>0)
				sign = 1;
			else
				sign = -1;
				
			m[j] -= momentumDamping * abs(m[j]) * sign;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::applyResponse(vector<Sphere>& spheres)
{
	vector<Sphere>::iterator si    = spheres.begin();
	vector<Sphere>::iterator siEnd = spheres.end();
	for( ; si!=siEnd ; ++si)
	{
		///NewtonsForceLaw 
		(*si).acceleration =(*si).invMass*(*si).force;
		
		///NewtonsMomentumLaw 
		(*si).angularAcceleration = (*si).momentum.multDiag((*si).invInertia);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::timeIntegration(vector<Sphere>& spheres)
{
	static bool first = true;

	vector<Sphere>::iterator si    = spheres.begin();
	vector<Sphere>::iterator siEnd = spheres.end();
	for( ; si!=siEnd ; ++si)
	{
		if ((*si).isDynamic)
		{
			if (!first)
			{
				(*si).velocity = (*si).prevVelocity+0.5*dt*(*si).acceleration;
				(*si).angularVelocity = (*si).prevAngularVelocity+0.5*dt*(*si).angularAcceleration;
			}

			///LeapFrogPositionIntegrator
			(*si).prevVelocity = (*si).velocity+0.5*dt*(*si).acceleration;
			(*si).position += (*si).prevVelocity*dt;

			///LeapFrogOrientationIntegrator
			(*si).prevAngularVelocity = (*si).angularVelocity+0.5*dt*(*si).angularAcceleration;
			Vector3r axis = (*si).angularVelocity;
			Real angle = axis.normalize();
			Quaternionr q;
			q.fromAxisAngle(axis,angle*dt);
			(*si).orientation = q*(*si).orientation;
			(*si).orientation.normalize();
		}
	}

	first = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Real SphericalDEMSimulator::computeDt(const vector<Sphere>& spheres, const ContactVecSet& contacts)
{
	///ElasticCriterionTimeStepper

	newDt = Mathr::MAX_REAL;
	computedSomething = false; // this flag is to avoid setting timestep to MAX_REAL :)

	ContactVecSet::const_iterator cvsi    = contacts.begin();
	ContactVecSet::const_iterator cvsiEnd = contacts.end();
	for(int id1=0 ; cvsi!=cvsiEnd ; ++cvsi, ++id1)
	{
		ContactSet::const_iterator csi    = cvsi->begin();
		ContactSet::const_iterator csiEnd = cvsiEnd->begin();
		for( ; csi!=csiEnd ; ++csi)
			findTimeStepFromInteraction(id1,*csi , spheres);
	}

	if(! computedSomething)
	{
		vector<Sphere>::const_iterator si    = spheres.begin();
		vector<Sphere>::const_iterator siEnd = spheres.end();
		for( ; si!=siEnd ; ++si)
			findTimeStepFromBody(*si);
	}	

	if(computedSomething)
		return newDt;
	else
		return dt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::findTimeStepFromBody(const Sphere& sphere)
{
	Real Dab  	= sphere.radius;
	Real rad3 	= pow(Dab,2); // radius to the power of 2, from sphere

	Real Eab 	= sphere.young;
	Real Vab 	= sphere.poisson;
	Real Dinit 	= 2*Dab; // assuming thet sphere is in contact with itself
	Real Sinit 	= Mathr::PI * pow( Dab , 2);

	Real Kn		= abs((Eab*Sinit/Dinit)*( (1+alpha)/(beta*(1+Vab) + gamma*(1-alpha*Dab) ) ));
	Real Ks		= abs(Kn*(1-alpha*Vab)/(1+Vab));

	Real dt = 0.1*min(
			  sqrt( sphere.mass       /  Kn       )
			, sqrt( sphere.inertia[0] / (Ks*rad3) )
		  );

	newDt = min(dt,newDt);
	computedSomething = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::findTimeStepFromInteraction(unsigned int id1, const Contact& contact, const vector<Sphere>& spheres)
{
	unsigned int id2 = contact.id;
			
	Real mass 	= min( spheres[id1].mass, spheres[id2].mass);
	Real inertia 	= min( spheres[id1].inertia[0], spheres[id2].inertia[0]);
	Real rad3 	= pow( std::max(contact.radius1 , contact.radius2 ) , 2); // radius to the power of 2, from sphere

	Real dt = 0.1*min(
			  sqrt( mass     / abs(contact.kn)      )
			, sqrt( inertia  / abs(contact.ks*rad3) )
		  );

	newDt = min(dt,newDt);
	computedSomething = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

