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
#include "SDECContactGeometry.hpp"
#include "SDECPermanentLink.hpp"
#include "SDECContactPhysics.hpp"
#include "SDECPermanentLinkPhysics.hpp"
#include "Omega.hpp"
#include "ComplexBody.hpp"
#include "ActionForce.hpp"
#include "ActionMomentum.hpp"
#include "Action.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDECDynamicEngine::SDECDynamicEngine() : DynamicEngine() , actionForce(new ActionForce) , actionMomentum(new ActionMomentum)
{
	first=true;
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
void SDECDynamicEngine::respondToInteractions(Body* body)
{

	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	Vector3r gravity = Omega::instance().getGravity();
	Real dt = Omega::instance().getTimeStep();

	
	// FIXME - that should be in another dynlib, I CHANGED CONTAINER so IT IS FASTER
	// speed improvement is: 156 -> 142 = 14 sec 9% (rev.339 -> rev.340)
	if(first)
	{
		vector<shared_ptr<Action> > vvv; 
		vvv.clear();
//		cout << "ActionForce id: " << actionForce->getClassIndex() << endl;
//		cout << "ActionMomentum id: " << actionMomentum->getClassIndex() << endl;
		vvv.push_back(actionForce);
		vvv.push_back(actionMomentum);
		ncb->actions->prepare(vvv);
	}

	// FIXME : clearing actions should be called from another actor
	ncb->actions->reset(); // speed improvement to using clear() is: 169 -> 156 seconds = 13 sec, 8 % (rev.338 -> rev.339)

	
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

		shared_ptr<SDECDiscreteElement> de1		= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id1]->physicalParameters);
		shared_ptr<SDECDiscreteElement> de2 		= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id2]->physicalParameters);
		shared_ptr<SDECPermanentLinkPhysics> currentContactPhysics	= dynamic_pointer_cast<SDECPermanentLinkPhysics>(contact2->interactionPhysics);
		shared_ptr<SDECPermanentLink> currentContactGeometry	= dynamic_pointer_cast<SDECPermanentLink>(contact2->interactionGeometry);

		/// FIXME : put these lines into another dynlib
		currentContactPhysics->kn 			= currentContactPhysics->initialKn;
		currentContactPhysics->ks 			= currentContactPhysics->initialKs;
		currentContactPhysics->equilibriumDistance 	= currentContactPhysics->initialEquilibriumDistance;
		currentContactGeometry->normal 			= (de2->se3.translation-de1->se3.translation);
		currentContactGeometry->normal.normalize();
		Real un 				= currentContactPhysics->equilibriumDistance-(de2->se3.translation-de1->se3.translation).length();
		currentContactPhysics->normalForce		= currentContactPhysics->kn*un*currentContactGeometry->normal;

		if (first)
			currentContactPhysics->prevNormal = currentContactGeometry->normal;
		
		Vector3r axis;
		Real angle;

////////////////////////////////////////////////////////////
/// Here is the code with approximated rotations 	 ///
////////////////////////////////////////////////////////////

		axis = currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
		currentContactPhysics->shearForce     -= currentContactPhysics->shearForce.cross(axis);
		angle	 			= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
		axis 				= angle*currentContactGeometry->normal;
		currentContactPhysics->shearForce     -= currentContactPhysics->shearForce.cross(axis);


////////////////////////////////////////////////////////////
/// Here is the code without approximated rotations 	 ///
////////////////////////////////////////////////////////////

// 		Quaternionr q;
//
// 		axis				= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
// 		angle				= acos(currentContactGeometry->normal.dot(currentContactPhysics->prevNormal));
// 		q.fromAngleAxis(angle,axis);
//
// 		currentContactPhysics->shearForce	= q*currentContactPhysics->shearForce;
//
// 		angle				= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
// 		axis				= currentContactGeometry->normal;
// 		q.fromAngleAxis(angle,axis);
// 		currentContactPhysics->shearForce	= q*currentContactPhysics->shearForce;

////////////////////////////////////////////////////////////
/// 							 ///
////////////////////////////////////////////////////////////

		Vector3r x	= de1->se3.translation+(currentContactGeometry->radius1-0.5*un)*currentContactGeometry->normal;
		//Vector3r x	= (de1->se3.translation+de2->se3.translation)*0.5;
		//cout << currentContact->contactPoint << " || " << (de1->se3.translation+de2->se3.translation)*0.5 << endl;
		Vector3r c1x	= (x - de1->se3.translation);
		Vector3r c2x	= (x - de2->se3.translation);

		Vector3r relativeVelocity 	= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
		Vector3r shearVelocity		= relativeVelocity-currentContactGeometry->normal.dot(relativeVelocity)*currentContactGeometry->normal;
		Vector3r shearDisplacement	= shearVelocity*dt;
		currentContactPhysics->shearForce      -=  currentContactPhysics->ks*shearDisplacement;

		Vector3r f = currentContactPhysics->normalForce + currentContactPhysics->shearForce;

		static_cast<ActionForce*>   ( ncb->actions->find( id1 , actionForce   ->getClassIndex() ).get() )->force    -= f;
		static_cast<ActionForce*>   ( ncb->actions->find( id2 , actionForce   ->getClassIndex() ).get() )->force    += f;
		
		static_cast<ActionMomentum*>( ncb->actions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= c1x.cross(f);
		static_cast<ActionMomentum*>( ncb->actions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += c2x.cross(f);



////////////////////////////////////////////////////////////
/// Moment law					 	 ///
////////////////////////////////////////////////////////////

		if (first)
		{
			currentContactPhysics->prevRotation1 = de1->se3.rotation;
			currentContactPhysics->prevRotation2 = de2->se3.rotation;
			currentContactPhysics->averageRadius = (currentContactGeometry->radius1+currentContactGeometry->radius2)*0.5;
			currentContactPhysics->kr = currentContactPhysics->ks * currentContactPhysics->averageRadius * currentContactPhysics->averageRadius;
		}

		Vector3r n	= currentContactGeometry->normal;
		Vector3r prevN	= currentContactPhysics->prevNormal;
		Vector3r t1	= currentContactPhysics->shearForce;
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
// 		currentContactPhysics->prevRotation1.toEulerAngles(da);
// 		currentContactPhysics->prevRotation2.toEulerAngles(db);
//
// 		dRotationA -= da;
// 		dRotationB -= db;
//
// 		Vector3r dUr = 	( currentContactGeometry->radius1*(  dRotationA  -  dBeta)
// 				- currentContactGeometry->radius2*(  dRotationB  -  dBeta) ) * 0.5;

////////////////////////////////////////////////////////////
/// Ending of use of eurler angle		 	 ///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Using Quaternionr				 	 ///
////////////////////////////////////////////////////////////

		Quaternionr q,q1,q2;
		Vector3r dRotationAMinusDBeta,dRotationBMinusDBeta;

		q.align(n,prevN);
		q1 = (de1->se3.rotation*currentContactPhysics->prevRotation1.inverse())*q.inverse();
		q2 = (de2->se3.rotation*currentContactPhysics->prevRotation2.inverse())*q.inverse();
		q1.toEulerAngles(dRotationAMinusDBeta);
		q2.toEulerAngles(dRotationBMinusDBeta);
		Vector3r dUr = ( currentContactGeometry->radius1*dRotationAMinusDBeta - currentContactGeometry->radius2*dRotationBMinusDBeta ) * 0.5;

////////////////////////////////////////////////////////////
/// Ending of use of Quaternionr			 	 ///
////////////////////////////////////////////////////////////


		Vector3r dThetar = dUr/currentContactPhysics->averageRadius;

		currentContactPhysics->thetar += dThetar;


		Real fNormal = currentContactPhysics->normalForce.length();

		Real normMPlastic = currentContactPhysics->heta*fNormal;

		Vector3r thetarn = q_i_n*currentContactPhysics->thetar; // rolling angle

		Vector3r mElastic = currentContactPhysics->kr * thetarn;

		//mElastic[0] = 0;  // No moment around normal direction

		Real normElastic = mElastic.length();


		//if (normElastic<=normMPlastic)
		//{
		static_cast<ActionMomentum*>( ncb->actions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= q_n_i*mElastic;
		static_cast<ActionMomentum*>( ncb->actions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += q_n_i*mElastic;

		//}
		//else
		//{
		//	Vector3r mPlastic = mElastic;
		//	mPlastic.normalize();
		//	mPlastic *= normMPlastic;
		//	moments[id1]	-= q_n_i*mPlastic;
		//	moments[id2]	+= q_n_i*mPlastic;
		//	thetarn = mPlastic/currentContactPhysics->kr;
		//	currentContactPhysics->thetar = q_n_i*thetarn;
		//}

		currentContactPhysics->prevRotation1 = de1->se3.rotation;
		currentContactPhysics->prevRotation2 = de2->se3.rotation;

////////////////////////////////////////////////////////////
/// Moment law	END				 	 ///
////////////////////////////////////////////////////////////

		currentContactPhysics->prevNormal = currentContactGeometry->normal;
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

		shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id1]->physicalParameters);
		shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id2]->physicalParameters);
		shared_ptr<SDECContactGeometry> currentContactGeometry = dynamic_pointer_cast<SDECContactGeometry>(contact->interactionGeometry);
		shared_ptr<SDECContactPhysics> currentContactPhysics   = dynamic_pointer_cast<SDECContactPhysics> (contact->interactionPhysics);
		
		if ( contact->isNew)
			currentContactPhysics->shearForce			= Vector3r(0,0,0);
				
		Real un 				= currentContactGeometry->penetrationDepth;
		currentContactPhysics->normalForce	= currentContactPhysics->kn*un*currentContactGeometry->normal;

		Vector3r axis;
		Real angle;

////////////////////////////////////////////////////////////
/// Here is the code with approximated rotations 	 ///
////////////////////////////////////////////////////////////

		axis	 				= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
		currentContactPhysics->shearForce      -= currentContactPhysics->shearForce.cross(axis);
		angle 					= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
		axis 					= angle*currentContactGeometry->normal;
		currentContactPhysics->shearForce      -= currentContactPhysics->shearForce.cross(axis);


	
// it will be some macro(	body->actions,	ActionType , bodyId )
////////////////////////////////////////////////////////////
/// Here is the code without approximated rotations 	 ///
////////////////////////////////////////////////////////////

// 		Quaternionr q;
//
// 		axis				= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
// 		angle				= acos(currentContactGeometry->normal.dot(currentContactPhysics->prevNormal));
// 		q.fromAngleAxis(angle,axis);
//
// 		currentContactPhysics->shearForce	= currentContactPhysics->shearForce*q;
//
// 		angle				= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
// 		axis				= currentContactGeometry->normal;
// 		q.fromAngleAxis(angle,axis);
// 		currentContactPhysics->shearForce	= q*currentContactPhysics->shearForce;

////////////////////////////////////////////////////////////
/// 							 ///
////////////////////////////////////////////////////////////

		Vector3r x				= currentContactGeometry->contactPoint;
		Vector3r c1x				= (x - de1->se3.translation);
		Vector3r c2x				= (x - de2->se3.translation);
		Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
		Vector3r shearVelocity			= relativeVelocity-currentContactGeometry->normal.dot(relativeVelocity)*currentContactGeometry->normal;
		Vector3r shearDisplacement		= shearVelocity*dt;
		currentContactPhysics->shearForce      -= currentContactPhysics->ks*shearDisplacement;

		Vector3r f				= currentContactPhysics->normalForce + currentContactPhysics->shearForce;

// THIS IS for plotting with gnuplot shearForce between sphere 1 and sphere 10
// FIXME - we really need an easy way to obtain results like this one !!!! so don't delete those lines until we FIX that problem!!!
//		if( (id1 == 1 || id2 == 1) && (id1 == 10 || id2 == 10) )
// 		cout	<< Omega::instance().getCurrentIteration() << " "
// 			<< lexical_cast<string>(-currentContactPhysics->shearForce[0]) << " "
// 			<< lexical_cast<string>(currentContactPhysics->shearForce[1]) << " " 
// 			<< lexical_cast<string>(currentContactPhysics->shearForce[2]) << endl;
// 		else 
// 			cout	<< Omega::instance().getCurrentIteration() << endl;
// 		

// 		ActionForce f(currentContactPhysics->normalForce + currentContactPhysics->shearForce);
//  		ActionForce  af(f);
// 		vector[id1].add(af,id1,id2);


//		forces[id1]	-= f;
//		forces[id2]	+= f;
//		moments[id1]	-= c1x.cross(f);
//		moments[id2]	+= c2x.cross(f);

//////////////////////////////////////// PREVIOUS CONTAINER, begin
// PREVIOUS CONTAINER, slower	
// 		shared_ptr<ActionForce> af(new ActionForce);
// 		shared_ptr<ActionMomentum> am(new ActionMomentum);
// 		
// 		af->force = -f;
// 		body->actions->add(af,id1);
// 		af->force = f;
// 		body->actions->add(af,id2);
// 		
// 		am->momentum = -c1x.cross(f);
// 		body->actions->add(am,id1);
// 		am->momentum = c2x.cross(f);
// 		body->actions->add(am,id2);
// 		
// 		currentContactPhysics->prevNormal = currentContactGeometry->normal;
///////////////////// PREVIOUS CONTAINER, end

///////////////////// NEW CONTAINER, faster, begin       speed improvement is: 156 -> 142 = 14 sec 9% (rev.339 -> rev.340)
	
// it will be some macro(	body->actions,	ActionType , bodyId )
		static_cast<ActionForce*>   ( ncb->actions->find( id1 , actionForce   ->getClassIndex() ).get() )->force    -= f;
		static_cast<ActionForce*>   ( ncb->actions->find( id2 , actionForce   ->getClassIndex() ).get() )->force    += f;
		
		static_cast<ActionMomentum*>( ncb->actions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= c1x.cross(f);
		static_cast<ActionMomentum*>( ncb->actions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += c2x.cross(f);
		
		currentContactPhysics->prevNormal = currentContactGeometry->normal;
///////////////////// NEW CONTAINER, end
	}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Damping														///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	shared_ptr<Body> b;
	unsigned int i=0;
	
	shared_ptr<ActionForce> af(new ActionForce);
	shared_ptr<ActionMomentum> am(new ActionMomentum);
	
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() , ++i )
	{
		b = bodies->getCurrent();
 
		shared_ptr<SDECDiscreteElement> de = dynamic_pointer_cast<SDECDiscreteElement>(b->physicalParameters);

///////////////////// PREVIOUS CONTAINER, begin
//			af->force = gravity*de->mass;
//			body->actions->add(af,b->getId());
///////////////////// PREVIOUS CONTAINER, end

///////////////////// NEW CONTAINER, faster, begin
			static_cast<ActionForce*>( ncb->actions->find( b->getId() , actionForce->getClassIndex() ).get() )->force += gravity*de->mass;
///////////////////// NEW CONTAINER, end
			
			int sign;
			//Real f = forces[i].length();
			ActionForce * actionForceTmp = static_cast<ActionForce*>( ncb->actions->find(b->getId(), actionForce->getClassIndex() ).get());
			Real f  = actionForceTmp->force.length();
			
			for(int j=0;j<3;j++)
			{
				if (de->velocity[j]==0)
					sign=0;
				else if (de->velocity[j]>0)
					sign=1;
				else
					sign=-1;
				// FIXME - this must be a parameter in .xml !!!
				//forces[    i  ] [       j        ] -= 0.3*f*sign;
				actionForceTmp->force[j] -= 0.3*f*sign;
				//    [ BodyId] [ (x,y,z): index ]
			}

			// all bodies do not have momentum so we have to test that
			// it is different from forces, because we have added gravity to all bodies
///////////////////// PREVIOUS CONTAINER, begin
//			shared_ptr<Action> action = body->actions->find(b->getId(),am->getClassIndex());
//			if (action)
//			{
//				ActionMomentum * actionMomentum = static_cast<ActionMomentum*>(action.get());
///////////////////// PREVIOUS CONTAINER, end

			ActionMomentum * actionMomentumTmp = static_cast<ActionMomentum*>(ncb->actions->find(b->getId(),actionMomentum->getClassIndex() ) .get());
			Real m  = actionMomentumTmp->momentum.length();

			for(int j=0;j<3;j++)
			{
				if (de->angularVelocity[j]==0)
					sign=0;
				else if (de->angularVelocity[j]>0)
					sign=1;
				else
					sign=-1;
				//moments[   i   ] [       j        ] -= 0.3*m*sign;
				actionMomentumTmp->momentum[j] -= 0.3*m*sign;
				//     [ BodyId] [ (x,y,z): index ]
			}
				
///////////////////// PREVIOUS CONTAINER, begin
//			}
///////////////////// PREVIOUS CONTAINER, end
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

