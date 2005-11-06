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
#include <yade/yade-core/Engine.hpp>

#include <yade/yade-package-common/Sphere.hpp>
#include <yade/yade-package-common/GravityEngine.hpp>
#include <yade/yade-package-common/InteractionPhysicsMetaEngine.hpp>
#include <yade/yade-package-common/PhysicalActionDamper.hpp>
#include <yade/yade-package-common/CundallNonViscousForceDamping.hpp>
#include <yade/yade-package-common/CundallNonViscousMomentumDamping.hpp>

#include <yade/yade-package-dem/BodyMacroParameters.hpp>
#include <yade/yade-package-dem/MacroMicroElasticRelationships.hpp>

#include <yade/yade-lib-serialization/IOFormatManager.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SphericalDEMSimulator.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SphericalDEMSimulator::SphericalDEMSimulator() : StandAloneSimulator()
{
	alpha	= 2.5;
	beta	= 2.0;
	gamma	= 2.65;
	dt	= 0.01;
	gravity = Vector3r(0,-9.8,0);
	forceDamping = 0.3;
	momentumDamping = 0.3;
	useTimeStepper = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SphericalDEMSimulator::~SphericalDEMSimulator()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::setTimeStep(Real dt)
{ 
	if (dt<=0)
		useTimeStepper = true;
	else
	{
		useTimeStepper = false;
		this->dt = dt;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::doOneIteration()
{
	// compute dt
	//if (useTimeStepper)
	//	dt = computeDt(spheres,contacts);
	dt=0.01;
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::run(int nbIterations)
{
	// do the simulation loop
	int recordedIter = 0;
	for(int i=0;i<nbIterations;i++)
	{
		doOneIteration();
		if (record && (i%interval)==0)
		{
			string fileName = outputDirectory+"/"+outputBaseName;
		
			string num = lexical_cast<string>(recordedIter);
			while (num.size()<paddle)
				num.insert(num.begin(),'0');
		
			fileName += num;
			ofstream ofile(fileName.c_str());
		
			ofile << spheres.size() << endl;

			vector<SphericalDEM>::iterator si    = spheres.begin();
			vector<SphericalDEM>::iterator siEnd = spheres.end();
			for( ; si!=siEnd ; ++si)
			{
				Real tx=0, ty=0, tz=0, rw=0, rx=0, ry=0, rz=0;
		
				tx = si->position[0];
				ty = si->position[1];
				tz = si->position[2];
			
				rw = si->orientation[0];
				rx = si->orientation[1];
				ry = si->orientation[2];
				rz = si->orientation[3];
				
				ofile <<	lexical_cast<string>(tx) << " " 
						<< lexical_cast<string>(ty) << " " 
						<< lexical_cast<string>(tz) << " "
						<< lexical_cast<string>(rw) << " "
						<< lexical_cast<string>(rx) << " "
						<< lexical_cast<string>(ry) << " "
						<< lexical_cast<string>(rz) << endl;
			}
			ofile.close();
			recordedIter++;
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::loadConfigurationFile(const string& fileName)
{
	IOFormatManager::loadFromFile("XMLFormatManager",fileName,"rootBody",rootBody);
	
	spheres.clear();

	//copy rootBody to more simple data structure
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for( ; bi!=biEnd ; ++bi)
	{
		Sphere * s = static_cast<Sphere*>((*bi)->geometricalModel.get());
		BodyMacroParameters * bmp = static_cast<BodyMacroParameters*>((*bi)->physicalParameters.get());
		SphericalDEM sd;
		
		sd.isDynamic		= (*bi)->isDynamic;
		sd.radius		= s->radius;
		sd.position		= bmp->se3.position;
		sd.orientation		= bmp->se3.orientation;
		sd.velocity		= bmp->velocity;
		sd.angularVelocity	= bmp->angularVelocity;

		sd.mass			= bmp->mass;
		sd.invMass		= bmp->invMass;
		sd.inertia		= bmp->inertia;
		sd.invInertia		= bmp->invInertia;

		sd.young		= bmp->young;
		sd.poisson		= bmp->poisson;
		sd.frictionAngle	= bmp->frictionAngle;

		//sd.acceleration;
		//sd.angularAcceleration;	
		//sd.prevVelocity;
		//sd.prevAngularVelocity;
		//sd.force;
		//sd.momentum;

		spheres.push_back(sd);
	}

	vector<shared_ptr<Engine> >::iterator ei    = rootBody->actors.begin();
	vector<shared_ptr<Engine> >::iterator eiEnd = rootBody->actors.end();
	for( ; ei!=eiEnd ; ++ei)
	{
		shared_ptr<Engine> e = *ei;

		shared_ptr<GravityEngine> ge = dynamic_pointer_cast<GravityEngine>(e);
		if (ge)
			gravity = ge->gravity;

		shared_ptr<InteractionPhysicsMetaEngine> ipme = dynamic_pointer_cast<InteractionPhysicsMetaEngine>(e);
		if (ipme)
		{
			shared_ptr<MacroMicroElasticRelationships> mmer = dynamic_pointer_cast<MacroMicroElasticRelationships>(ipme->getExecutor("BodyMacroParameters","BodyMacroParameters"));
			assert(e);
			alpha	= mmer->alpha;
			beta	= mmer->beta;
			gamma	= mmer->gamma;
		}

		shared_ptr<PhysicalActionDamper> pade = dynamic_pointer_cast<PhysicalActionDamper>(e);
		if (pade)
		{	
			shared_ptr<CundallNonViscousForceDamping> cnvfd = dynamic_pointer_cast<CundallNonViscousForceDamping>(pade->getExecutor("Force","RigidBodyParameters"));
			shared_ptr<CundallNonViscousMomentumDamping> cnvmd = dynamic_pointer_cast<CundallNonViscousMomentumDamping>(pade->getExecutor("Momentum","RigidBodyParameters"));
			assert(cnvfd);
			assert(cnvmd);
			forceDamping 	= cnvfd->damping;
			momentumDamping = cnvmd->damping;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::findRealCollision(const vector<SphericalDEM>& spheres, ContactVecSet& contacts)
{

	ContactVecSet::iterator cvsi    = contacts.begin();
	ContactVecSet::iterator cvsiEnd = contacts.end();
	for(int id1=0 ; cvsi!=cvsiEnd ; ++cvsi, ++id1)
	{
		ContactSet::iterator csi    = cvsi->begin();
		ContactSet::iterator csiEnd = cvsi->end();
		for( ; csi!=csiEnd ; ++csi)
		{
			Contact * c = const_cast<Contact*>(&(*csi));
			
			int id2 = c->id;
			
			/// Sphere2Sphere4MacroMicroContactGeometry
			Vector3r normal = spheres[id2].position-spheres[id1].position;
			Real penetrationDepth = spheres[id1].radius+spheres[id2].radius-normal.normalize();
	
			if (penetrationDepth>0)
			{
				c->isReal = true;
				c->contactPoint = spheres[id1].position+(spheres[id1].radius-0.5*penetrationDepth)*normal;//0.5*(pt1+pt2);
				c->normal = normal;
				c->penetrationDepth = penetrationDepth;
				c->radius1 = spheres[id1].radius;
				c->radius2 = spheres[id2].radius;
			}
			//else	
			//	return;

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


void SphericalDEMSimulator::computeResponse(vector<SphericalDEM>& spheres, ContactVecSet& contacts)
{

	/// PhysicalActionContainerReseter+GravityEngine
	vector<SphericalDEM>::iterator si    = spheres.begin();
	vector<SphericalDEM>::iterator siEnd = spheres.end();
	for( ; si!=siEnd ; ++si)
	{
		(*si).force    = gravity * (*si).mass;
		(*si).momentum = Vector3r(0,0,0);
	}
int nbCollision=0;
	///ElasticContactLaw
	ContactVecSet::iterator cvsi    = contacts.begin();
	ContactVecSet::iterator cvsiEnd = contacts.end();
	for(int id1=0 ; cvsi!=cvsiEnd ; ++cvsi, ++id1)
	{
		ContactSet::iterator csi    = cvsi->begin();
		ContactSet::iterator csiEnd = cvsi->end();
		for( ; csi!=csiEnd ; ++csi)
		{
			Contact * c = const_cast<Contact*>(&(*csi));

			if (c->isReal)
			{
nbCollision++;
				int id2 = c->id;

				Vector3r& shearForce = c->shearForce;
		
				if ( c->isNew)
					shearForce = Vector3r(0,0,0);
						
				Real un		= c->penetrationDepth;
				c->normalForce	= c->kn*un*c->normal;
		
				Vector3r axis;
				Real angle;
		
				axis	 	= c->prevNormal.cross(c->normal);
				shearForce	-= shearForce.cross(axis);
				angle		= dt*0.5*c->normal.dot(spheres[id1].angularVelocity+spheres[id2].angularVelocity);
				axis		= angle*c->normal;
				shearForce	-= shearForce.cross(axis);
			
				Vector3r x			= c->contactPoint;
				Vector3r c1x			= (x - spheres[id1].position);
				Vector3r c2x			= (x - spheres[id2].position);
				Vector3r relativeVelocity	= (spheres[id2].velocity+spheres[id2].angularVelocity.cross(c2x)) - (spheres[id1].velocity+spheres[id1].angularVelocity.cross(c1x));
				Vector3r shearVelocity		= relativeVelocity-c->normal.dot(relativeVelocity)*c->normal;
				Vector3r shearDisplacement	= shearVelocity*dt;
				shearForce 			-= c->ks*shearDisplacement;
		
				/// PFC3d SlipModel, is using friction angle and CoulombCriterion
				Real maxFs = c->normalForce.squaredLength() * pow(c->tangensOfFrictionAngle,2);
				if( shearForce.squaredLength() > maxFs )
				{
					maxFs = Mathr::sqRoot(maxFs) / shearForce.length();
					shearForce *= maxFs;
				}
				/// PFC3d SlipModel
		
				Vector3r f	= c->normalForce + shearForce;
				
				spheres[id1].force -= f;
				spheres[id2].force += f;

				spheres[id1].momentum -= c1x.cross(f);
				spheres[id2].momentum += c2x.cross(f);
				
				c->prevNormal = c->normal;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::addDamping(vector<SphericalDEM>& spheres)
{
	vector<SphericalDEM>::iterator si    = spheres.begin();
	vector<SphericalDEM>::iterator siEnd = spheres.end();
	for( ; si!=siEnd ; ++si)
	{
		///CundallNonViscousForceDamping
		Vector3r& f  = si->force;
		register int sign;
		for(int j=0;j<3;j++)
		{
			if (si->velocity[j] == 0)
				sign = 0;
			else if (si->velocity[j] > 0)
				sign = 1;
			else
				sign = -1;
				
			f[j] -= forceDamping * abs(f[j]) * sign;
		}

		///CundallNonViscousMomentumDamping
		Vector3r& m  = si->momentum;
		for(int j=0;j<3;j++)
		{
			if (si->angularVelocity[j]==0)
				sign = 0;
			else if (si->angularVelocity[j]>0)
				sign = 1;
			else
				sign = -1;
				
			m[j] -= momentumDamping * abs(m[j]) * sign;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::applyResponse(vector<SphericalDEM>& spheres)
{
	vector<SphericalDEM>::iterator si    = spheres.begin();
	vector<SphericalDEM>::iterator siEnd = spheres.end();
	for( ; si!=siEnd ; ++si)
	{
		///NewtonsForceLaw 
		(*si).acceleration = (*si).invMass*(*si).force;
		
		///NewtonsMomentumLaw 
		(*si).angularAcceleration = (*si).momentum.multDiag((*si).invInertia);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SphericalDEMSimulator::timeIntegration(vector<SphericalDEM>& spheres)
{
	static bool first = true;

	vector<SphericalDEM>::iterator si    = spheres.begin();
	vector<SphericalDEM>::iterator siEnd = spheres.end();
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

Real SphericalDEMSimulator::computeDt(const vector<SphericalDEM>& spheres, const ContactVecSet& contacts)
{
	///ElasticCriterionTimeStepper

	newDt = Mathr::MAX_REAL;
	computedSomething = false; // this flag is to avoid setting timestep to MAX_REAL :)

	ContactVecSet::const_iterator cvsi    = contacts.begin();
	ContactVecSet::const_iterator cvsiEnd = contacts.end();
	for(int id1=0 ; cvsi!=cvsiEnd ; ++cvsi, ++id1)
	{
		ContactSet::const_iterator csi    = cvsi->begin();
		ContactSet::const_iterator csiEnd = cvsi->end();
		for( ; csi!=csiEnd ; ++csi)
			findTimeStepFromInteraction(id1,*csi , spheres);
	}

	if(! computedSomething)
	{
		vector<SphericalDEM>::const_iterator si    = spheres.begin();
		vector<SphericalDEM>::const_iterator siEnd = spheres.end();
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

void SphericalDEMSimulator::findTimeStepFromBody(const SphericalDEM& sphere)
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

void SphericalDEMSimulator::findTimeStepFromInteraction(unsigned int id1, const Contact& contact, const vector<SphericalDEM>& spheres)
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

