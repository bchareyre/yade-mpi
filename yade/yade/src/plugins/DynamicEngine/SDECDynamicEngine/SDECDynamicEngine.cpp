
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

void SDECDynamicEngine::afterDeserialization()
{
	DynamicEngine::afterDeserialization();
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
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	if (first)
		interactionsPerBody.resize(bodies->size());

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

	shared_ptr<Interaction> contact;
//	for( ; cti!=ctiEnd ; ++cti)
	for( ncb->interactions->gotoFirst() ; ncb->interactions->notAtEnd() ; ncb->interactions->gotoNext() )
	{
		contact = ncb->interactions->getCurrent();

		int id1 = contact->getId1();
		int id2 = contact->getId2();

		if (id1>=id2)
			swap(id1,id2);

		bool accessed = false;
		tuple<int,bool,shared_ptr<InteractionGeometry> > t(id2 , accessed , contact->interactionGeometry);

		pair<set<tuple<int,bool,shared_ptr<InteractionGeometry> >,lessThanTuple >::iterator,bool> insertionResult;

		insertionResult	= interactionsPerBody[id1].insert(t);

		bool& wasAccessed = const_cast<bool&>(insertionResult.first->get<1>());
		wasAccessed	= true;


		contact->isNew = insertionResult.second;

		shared_ptr<SDECContactModel> scm = dynamic_pointer_cast<SDECContactModel>( contact->interactionGeometry);

		// here we want to get new geometrical info about contact but we want to remember physical infos from previous time step about it
		Vector3r cp = scm->contactPoint;
		Vector3r n = scm->normal;
		float pd = scm->penetrationDepth;
		float r1 = scm->radius1;
		float r2 = scm->radius2;

		contact->interactionGeometry  = insertionResult.first->get<2>();

		scm = dynamic_pointer_cast<SDECContactModel>(contact->interactionGeometry);
		scm->contactPoint= cp;
		scm->normal=n;
		scm->penetrationDepth=pd;
		scm->radius1=r1;
		scm->radius2=r2;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//FIXME : add reset function so it will remove bool first
void SDECDynamicEngine::respondToCollisions(Body* body)
{
	filter(body);

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	Vector3r gravity = Omega::instance().getGravity();
	float dt = Omega::instance().dt;

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

//	std::vector<shared_ptr<Interaction> >::const_iterator pii = ncb->permanentInteractions.begin();
//	std::vector<shared_ptr<Interaction> >::const_iterator piiEnd = ncb->permanentInteractions.end();
	shared_ptr<Interaction> contact2;
//	for( ; pii!=piiEnd ; ++pii)
//
//	{

	for( ncb->permanentInteractions->gotoFirst() ; ncb->permanentInteractions->notAtEnd() ; ncb->permanentInteractions->gotoNext() )
	{
		contact2 = ncb->permanentInteractions->getCurrent();




//		shared_ptr<Interaction> contact = (*pii);

		unsigned int id1 = contact2->getId1();
		unsigned int id2 = contact2->getId2();

////////////////////////////////////////////////////////////
/// FIXME : those lines are too dirty !			 ///
////////////////////////////////////////////////////////////

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

		tuple<int,bool,shared_ptr<InteractionGeometry> > t( tmpId2 , accessed , contact2->interactionGeometry );
		interactionsPerBody[tmpId1].erase(t);

		shared_ptr<Interaction> contact;
//		for( ; cti!=ctiEnd ; ++cti)
		for( ncb->interactions->gotoFirst() ; ncb->interactions->notAtEnd() ; ncb->interactions->gotoNext() )
		{
			contact = ncb->interactions->getCurrent();

			if ((contact->getId1()==id1 && contact->getId2()==id2 ) || (contact->getId1()==id2 && contact->getId2()==id1))
			{
				ncb->interactions->eraseCurrentAndGotoNext();
				break;
			}
		}

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
		float un 				= currentContact->equilibriumDistance-(de2->se3.translation-de1->se3.translation).length();
		currentContact->normalForce		= currentContact->kn*un*currentContact->normal;

		if (first)
			currentContact->prevNormal = currentContact->normal;

		Vector3r axis;
		float angle;

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
		//q_i_n.invert();					      // in case of the order of rotation is wrong
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


		float fNormal = currentContact->normalForce.length();

		float normMPlastic = currentContact->heta*fNormal;

		Vector3r thetarn = q_i_n*currentContact->thetar; // rolling angle

		Vector3r mElastic = currentContact->kr * thetarn;

		//mElastic[0] = 0;  // No moment around normal direction

		float normElastic = mElastic.length();


		//if (normElastic<=normMPlastic)
		//{
			moments[id1]	-= q_n_i*mElastic;
			moments[id2]	+= q_n_i*mElastic;
		//}
		//else
		//{
		//	Vector3r mPlastic = normMPlastic*mElastic.normalized();
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

		if ( contact ->isNew)
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

		float un 			= currentContact->penetrationDepth;
		currentContact->normalForce	= currentContact->kn*un*currentContact->normal;

		Vector3r axis;
		float angle;

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
			float f = forces[i].length();

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
			de->angularAcceleration += moments[i].multDiag(de->invInertia);
		}
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

