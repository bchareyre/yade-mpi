
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

void SDECDynamicEngine::postProcessAttributes(bool deserializing)
{
	DynamicEngine::postProcessAttributes(deserializing);
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


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//FIXME : add reset function so it will remove bool first
void SDECDynamicEngine::respondToCollisions(Body* body)
{
	//filter(body);

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	Vector3r gravity = Omega::instance().getGravity();
	Real dt = Omega::instance().getTimeStep();

	if (first)
	{
		forces.resize(bodies->size());
		moments.resize(bodies->size());
	}

	fill(forces.begin(),forces.end(),Vector3r(0,0,0));
	fill(moments.begin(),moments.end(),Vector3r(0,0,0));



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Permanents Links													///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for( ncb->permanentInteractions->gotoFirst() ; ncb->permanentInteractions->notAtEnd() ; ncb->permanentInteractions->gotoNext() )
	{
		shared_ptr<Interaction> contact2 = ncb->permanentInteractions->getCurrent();

		unsigned int id1 = contact2->getId1();
		unsigned int id2 = contact2->getId2();

////////////////////////////////////////////////////////////
/// FIXME : those lines are dirty !			 ///
////////////////////////////////////////////////////////////
	
	// FIXME - this is much shorter but still dirty (but now in different aspect - the way we store interactions)
		shared_ptr<Interaction> interaction = ncb->interactions->find(id1,id2);
		if (interaction)
			interaction->isReal = false;

////////////////////////////////////////////////////////////
/// 							 ///
////////////////////////////////////////////////////////////

		shared_ptr<SDECDiscreteElement> de1		= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id1]);
		shared_ptr<SDECDiscreteElement> de2 		= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id2]);
		shared_ptr<SDECPermanentLink> currentContact	= dynamic_pointer_cast<SDECPermanentLink>(contact2->interactionGeometry);

		/// FIXME : put these lines into another dynlib
		currentContact->kn 			= currentContact->initialKn;
		currentContact->ks 			= currentContact->initialKs;
		currentContact->equilibriumDistance 	= currentContact->initialEquilibriumDistance;
		currentContact->normal 			= (de2->se3.translation-de1->se3.translation);
		currentContact->normal.normalize();
		Real un 				= currentContact->equilibriumDistance-(de2->se3.translation-de1->se3.translation).length();
		currentContact->normalForce		= currentContact->kn*un*currentContact->normal;

		if (first)
			currentContact->prevNormal = currentContact->normal;

		Vector3r axis;
		Real angle;

////////////////////////////////////////////////////////////
/// Here is the code with approximated rotations 	 ///
////////////////////////////////////////////////////////////

		axis = currentContact->prevNormal.cross(currentContact->normal);
		currentContact->shearForce     -= currentContact->shearForce.cross(axis);
		angle	 			= dt*0.5*currentContact->normal.dot(de1->angularVelocity+de2->angularVelocity);
		axis 				= angle*currentContact->normal;
		currentContact->shearForce     -= currentContact->shearForce.cross(axis);


////////////////////////////////////////////////////////////
/// Here is the code without approximated rotations 	 ///
////////////////////////////////////////////////////////////

// 		Quaternionr q;
//
// 		axis				= currentContact->prevNormal.cross(currentContact->normal);
// 		angle				= acos(currentContact->normal.dot(currentContact->prevNormal));
// 		q.fromAngleAxis(angle,axis);
//
// 		currentContact->shearForce	= q*currentContact->shearForce;
//
// 		angle				= dt*0.5*currentContact->normal.dot(de1->angularVelocity+de2->angularVelocity);
// 		axis				= currentContact->normal;
// 		q.fromAngleAxis(angle,axis);
// 		currentContact->shearForce	= q*currentContact->shearForce;

////////////////////////////////////////////////////////////
/// 							 ///
////////////////////////////////////////////////////////////

		Vector3r x	= de1->se3.translation+(currentContact->radius1-0.5*un)*currentContact->normal;
		//Vector3r x	= (de1->se3.translation+de2->se3.translation)*0.5;
		//cout << currentContact->contactPoint << " || " << (de1->se3.translation+de2->se3.translation)*0.5 << endl;
		Vector3r c1x	= (x - de1->se3.translation);
		Vector3r c2x	= (x - de2->se3.translation);

		Vector3r relativeVelocity 	= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
		Vector3r shearVelocity		= relativeVelocity-currentContact->normal.dot(relativeVelocity)*currentContact->normal;
		Vector3r shearDisplacement	= shearVelocity*dt;
		currentContact->shearForce      -=  currentContact->ks*shearDisplacement;

		Vector3r f = currentContact->normalForce + currentContact->shearForce;

		forces[id1]	-= f;
		forces[id2]	+= f;
		moments[id1]	-= c1x.cross(f);
		moments[id2]	+= c2x.cross(f);




////////////////////////////////////////////////////////////
/// Moment law					 	 ///
////////////////////////////////////////////////////////////

		if (first)
		{
			currentContact->prevRotation1 = de1->se3.rotation;
			currentContact->prevRotation2 = de2->se3.rotation;
			currentContact->averageRadius = (currentContact->radius1+currentContact->radius2)*0.5;
			currentContact->kr = currentContact->ks * currentContact->averageRadius * currentContact->averageRadius;
		}

		Vector3r n	= currentContact->normal;
		Vector3r prevN	= currentContact->prevNormal;
		Vector3r t1	= currentContact->shearForce;
		t1.normalize();
		Vector3r t2	= n.unitCross(t1);

// 		if (n[0]!=0 && n[1]!=0 && n[2]!=0)
// 		{
// 			t1 = Vector3r(0,0,sqrt(1.0/(1+(n[2]*n[2]/(n[1]*n[1])))));
// 			t1[1] = -n[2]/n[1]*t1[2];
// 			t1.normalize();
// 			t2 = n.unitCross(t1);
// 		}
// 		else
// 		{
// 			if (n[0]==0 && n[1]!=0 && n[2]!=0)
// 			{
// 				t1 = Vector3r(1,0,0);
// 				t2 = n.unitCross(t1);
// 			}
// 			else if (n[0]!=0 && n[1]==0 && n[2]!=0)
// 			{
// 				t1 = Vector3r(0,1,0);
// 				t2 = n.unitCross(t1);
// 			}
// 			else if (n[0]!=0 && n[1]!=0 && n[2]==0)
// 			{
// 				t1 = Vector3r(0,0,1);
// 				t2 = n.unitCross(t1);
// 			}
// 			else if (n[0]==0 && n[1]==0 && n[2]!=0)
// 			{
// 				t1 = Vector3r(1,0,0);
// 				t2 = Vector3r(0,1,0);
// 			}
// 			else if (n[0]==0 && n[1]!=0 && n[2]==0)
// 			{
// 				t1 = Vector3r(0,0,1);
// 				t2 = Vector3r(1,0,0);
// 			}
// 			else if (n[0]!=0 && n[1]==0 && n[2]==0)
// 			{
// 				t1 = Vector3r(0,1,0);
// 				t2 = Vector3r(0,0,1);
// 			}
// 		}

		Quaternionr q_i_n,q_n_i;

		q_i_n.fromAxes(n,t1,t2);
		q_i_n.fromAxes(Vector3r(1,0,0),Vector3r(0,1,0),Vector3r(0,0,1)); // use identity matrix
		q_n_i = q_i_n.inverse();

////////////////////////////////////////////////////////////
/// Using Euler angle				 	 ///
////////////////////////////////////////////////////////////

// 		Vector3r dBeta;
// 		Vector3r orientation_Nc,orientation_Nc_old;
// 		for(int i=0;i<3;i++)
// 		{
// 			int j = (i+1)%3;
// 			int k = (i+2)%3;
//
// 			if (n[j]>=0)
// 				orientation_Nc[k] = acos(n[i]); // what is Nc_new
// 			else
// 				orientation_Nc[k] = -acos(n[i]);
//
// 			if (prevN[j]>=0)
// 				orientation_Nc_old[k] = acos(prevN[i]);
// 			else
// 				orientation_Nc_old[k] = -acos(prevN[i]);
// 		}
//
// 		dBeta = orientation_Nc - orientation_Nc_old;
//
//
// 		Vector3r dRotationA,dRotationB,da,db;
// 		de1->se3.rotation.toEulerAngles(dRotationA);
// 		de2->se3.rotation.toEulerAngles(dRotationB);
//
// 		currentContact->prevRotation1.toEulerAngles(da);
// 		currentContact->prevRotation2.toEulerAngles(db);
//
// 		dRotationA -= da;
// 		dRotationB -= db;
//
// 		Vector3r dUr = 	( currentContact->radius1*(  dRotationA  -  dBeta)
// 				- currentContact->radius2*(  dRotationB  -  dBeta) ) * 0.5;

////////////////////////////////////////////////////////////
/// Ending of use of eurler angle		 	 ///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Using Quaternionr				 	 ///
////////////////////////////////////////////////////////////

		Quaternionr q,q1,q2;
		Vector3r dRotationAMinusDBeta,dRotationBMinusDBeta;

		q.align(n,prevN);
		q1 = (de1->se3.rotation*currentContact->prevRotation1.inverse())*q.inverse();
		q2 = (de2->se3.rotation*currentContact->prevRotation2.inverse())*q.inverse();
		q1.toEulerAngles(dRotationAMinusDBeta);
		q2.toEulerAngles(dRotationBMinusDBeta);
		Vector3r dUr = ( currentContact->radius1*dRotationAMinusDBeta - currentContact->radius2*dRotationBMinusDBeta ) * 0.5;

////////////////////////////////////////////////////////////
/// Ending of use of Quaternionr			 	 ///
////////////////////////////////////////////////////////////


		Vector3r dThetar = dUr/currentContact->averageRadius;

		currentContact->thetar += dThetar;


		Real fNormal = currentContact->normalForce.length();

		Real normMPlastic = currentContact->heta*fNormal;

		Vector3r thetarn = q_i_n*currentContact->thetar; // rolling angle

		Vector3r mElastic = currentContact->kr * thetarn;

		//mElastic[0] = 0;  // No moment around normal direction

		Real normElastic = mElastic.length();


		//if (normElastic<=normMPlastic)
		//{
			moments[id1]	-= q_n_i*mElastic;
			moments[id2]	+= q_n_i*mElastic;
		//}
		//else
		//{
		//	Vector3r mPlastic = mElastic;
		//	mPlastic.normalize();
		//	mPlastic *= normMPlastic;
		//	moments[id1]	-= q_n_i*mPlastic;
		//	moments[id2]	+= q_n_i*mPlastic;
		//	thetarn = mPlastic/currentContact->kr;
		//	currentContact->thetar = q_n_i*thetarn;
		//}

		currentContact->prevRotation1 = de1->se3.rotation;
		currentContact->prevRotation2 = de2->se3.rotation;

////////////////////////////////////////////////////////////
/// Moment law	END				 	 ///
////////////////////////////////////////////////////////////

		currentContact->prevNormal = currentContact->normal;
	}

	first = false;




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Non Permanents Links												///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	shared_ptr<Interaction> contact;
//	for( ; cti!=ctiEnd ; ++cti)
	for( ncb->interactions->gotoFirst() ; ncb->interactions->notAtEnd() ; ncb->interactions->gotoNext() )
	{
		contact = ncb->interactions->getCurrent();

		int id1 = contact->getId1();
		int id2 = contact->getId2();

		shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id1]);
		shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id2]);
		shared_ptr<SDECContactModel> currentContact = dynamic_pointer_cast<SDECContactModel>(contact->interactionGeometry);

		if ( contact->isNew)
		{
			// FIXME : put these lines into a dynlib - PhysicalCollider
			currentContact->initialKn			= 2*(de1->kn*de2->kn)/(de1->kn+de2->kn);
			currentContact->initialKs			= 2*(de1->ks*de2->ks)/(de1->ks+de2->ks);
			currentContact->prevNormal 			= currentContact->normal;
			currentContact->shearForce			= Vector3r(0,0,0);
			currentContact->initialEquilibriumDistance	= currentContact->radius1+currentContact->radius2;
		}
		
		// FIXME : put these lines into another dynlib
		currentContact->kn = currentContact->initialKn;
		currentContact->ks = currentContact->initialKs;
		currentContact->equilibriumDistance = currentContact->initialEquilibriumDistance;

		Real un 			= currentContact->penetrationDepth;
		currentContact->normalForce	= currentContact->kn*un*currentContact->normal;

		Vector3r axis;
		Real angle;

////////////////////////////////////////////////////////////
/// Here is the code with approximated rotations 	 ///
////////////////////////////////////////////////////////////

		axis	 			= currentContact->prevNormal.cross(currentContact->normal);
		currentContact->shearForce     -= currentContact->shearForce.cross(axis);
		angle 				= dt*0.5*currentContact->normal.dot(de1->angularVelocity+de2->angularVelocity);
		axis 				= angle*currentContact->normal;
		currentContact->shearForce     -= currentContact->shearForce.cross(axis);


////////////////////////////////////////////////////////////
/// Here is the code without approximated rotations 	 ///
////////////////////////////////////////////////////////////

// 		Quaternionr q;
//
// 		axis				= currentContact->prevNormal.cross(currentContact->normal);
// 		angle				= acos(currentContact->normal.dot(currentContact->prevNormal));
// 		q.fromAngleAxis(angle,axis);
//
// 		currentContact->shearForce	= currentContact->q*shearForce;
//
// 		angle				= dt*0.5*currentContact->normal.dot(de1->angularVelocity+de2->angularVelocity);
// 		axis				= currentContact->normal;
// 		q.fromAngleAxis(angle,axis);
// 		currentContact->shearForce	= q*currentContact->shearForce;

////////////////////////////////////////////////////////////
/// 							 ///
////////////////////////////////////////////////////////////

		Vector3r x			= currentContact->contactPoint;
		Vector3r c1x			= (x - de1->se3.translation);
		Vector3r c2x			= (x - de2->se3.translation);
		Vector3r relativeVelocity 	= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
		Vector3r shearVelocity		= relativeVelocity-currentContact->normal.dot(relativeVelocity)*currentContact->normal;
		Vector3r shearDisplacement	= shearVelocity*dt;
		currentContact->shearForce     -=  currentContact->ks*shearDisplacement;

		Vector3r f = currentContact->normalForce + currentContact->shearForce;

// THIS IS for plotting with gnuplot shearForce between sphere 1 and sphere 10
// FIXME - we really need an easy way to obtain results like this one !!!! so don't delete those lines until we FIX that problem!!!
//		if( (id1 == 1 || id2 == 1) && (id1 == 10 || id2 == 10) )
// 		cout	<< Omega::instance().getCurrentIteration() << " "
// 			<< lexical_cast<string>(-currentContact->shearForce[0]) << " "
// 			<< lexical_cast<string>(currentContact->shearForce[1]) << " " 
// 			<< lexical_cast<string>(currentContact->shearForce[2]) << endl;
// 		else 
// 			cout	<< Omega::instance().getCurrentIteration() << endl;
// 		
		
		forces[id1]	-= f;
		forces[id2]	+= f;
		moments[id1]	-= c1x.cross(f);
		moments[id2]	+= c2x.cross(f);

		currentContact->prevNormal = currentContact->normal;
	}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Damping														///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	shared_ptr<Body> b;
	unsigned int i=0;
// FIXME - this is broken, because bodies have id, while forces and velocities are just a vector of numbers.
//	for(unsigned int i=0; i < bodies->size(); i++)
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() , ++i )
	{
		b = bodies->getCurrent();

		shared_ptr<SDECDiscreteElement> de = dynamic_pointer_cast<SDECDiscreteElement>(b);
		if (de)
		{
			forces[i] += gravity*de->mass;
			int sign;
			Real f = forces[i].length();

			for(int j=0;j<3;j++)
			{
				if (de->velocity[j]==0)
					sign=0;
				else if (de->velocity[j]>0)
					sign=1;
				else
					sign=-1;
				// FIXME - this must be a parameter in .xml !!!
				forces[i][j] -= 0.3*f*sign;
			}

			Real m = moments[i].length();

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
			de->angularAcceleration += moments[i].multDiag(de->invInertia);
		}
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

