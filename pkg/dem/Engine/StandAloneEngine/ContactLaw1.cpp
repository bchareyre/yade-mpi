/*************************************************************************
*  Copyright (C) 2008 by Jérôme DURIEZ                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ContactLaw1.hpp"
#include<yade/pkg-dem/CohesiveFrictionalBodyParameters.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ContactLaw1Interaction.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>

#include <yade/lib-miniWm3/Wm3Math.h>

Vector3r translation_vect (0.10,0,0);


ContactLaw1::ContactLaw1() : InteractionSolver()
{
	sdecGroupMask=1;
	momentRotationLaw = true;
	coeff_dech=1.0;
	momentAlwaysElastic=false;
}



void ContactLaw1::action(MetaBody* ncb)
{
    shared_ptr<BodyContainer>& bodies = ncb->bodies;

    Real dt = Omega::instance().getTimeStep();

    InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
    InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	int nbreInteracTot=0;
	int nbreInteracMomPlastif=0;
    for (  ; ii!=iiEnd ; ++ii )
    {
        //if ((*ii)->interactionGeometry && (*ii)->interactionPhysics)
	if ((*ii)->isReal())
		{
		nbreInteracTot++;
		const shared_ptr<Interaction>& contact = *ii;
		int id1 = contact->getId1();
		int id2 = contact->getId2();
// 		cout << "contact entre " << id1 << " et " << id2 << " reel ? " << contact->isReal() << endl;
		if ( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
			continue; // skip other groups,

		CohesiveFrictionalBodyParameters* de1 			= YADE_CAST<CohesiveFrictionalBodyParameters*>((*bodies)[id1]->physicalParameters.get());
		CohesiveFrictionalBodyParameters* de2 			= YADE_CAST<CohesiveFrictionalBodyParameters*>((*bodies)[id2]->physicalParameters.get());
		SpheresContactGeometry* currentContactGeometry		= YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get());
		ContactLaw1Interaction* currentContactPhysics = YADE_CAST<ContactLaw1Interaction*> (contact->interactionPhysics.get());

		Vector3r& shearForce 			= currentContactPhysics->shearForce;

		if (contact->isFresh(ncb))
			{
			shearForce			= Vector3r::ZERO;
			currentContactPhysics->previousun=0.0;
			currentContactPhysics->previousFn=0.0;
			currentContactPhysics->unMax=0.0;
			}


		Real previousun = currentContactPhysics->previousun;
		Real previousFn = currentContactPhysics->previousFn;
		Real kn = currentContactPhysics->kn;
		Real Fn; // la valeur de Fn qui va etre calculee selon différentes manieres puis affectee

		Real un = currentContactGeometry->penetrationDepth; // compte positivement lorsq vraie interpenetration
// 		cout << "un = " << un << "  previousun = " << previousun << "  previousFn =" << previousFn << endl;
		if(un > currentContactPhysics->unMax)	// on est en charge, et au delà et sur la "droite principale"
			{
			Fn = kn*un;
			currentContactPhysics->unMax = std::abs(un);
// 			cout << "je suis dans le calcul normal " << endl;
			}
		else
			{
			Fn = previousFn + coeff_dech * kn * (un-previousun);	// Calcul incrémental de la nvlle force
// 			cout << "je suis dans l'autre calcul" << endl;
			if(std::abs(Fn) > std::abs(kn * un))		// verif qu'on ne depasse la courbe
				Fn = kn*un;
			if(Fn < 0.0 )	// verif qu'on reste positif FIXME ATTENTION ON S'EST FICHU DU NORMAL ADHESION !!!!
				{Fn = 0;
// 				cout << "j'ai corrige pour ne pas etre negatif" << endl;
				}
			}
		
		currentContactPhysics->normalForce	= Fn*currentContactGeometry->normal;
// 		cout << "Fn appliquee " << Fn << endl << endl;
		// actualisation :
		currentContactPhysics->previousFn = Fn;
		currentContactPhysics->previousun = un;

            if (   un < 0      )
            	{ // BREAK due to tension

                //currentContactPhysics->SetBreakingState();


					 ncb->interactions->requestErase(contact->getId1(),contact->getId2());
					 // probably not useful anymore
                currentContactPhysics->normalForce = Vector3r::ZERO;
                currentContactPhysics->shearForce = Vector3r::ZERO;

                //return;
                //    else
                //    currentContactPhysics->normalForce	= -currentContactPhysics->normalAdhesion*currentContactGeometry->normal;
            	}
            else
            	{

                Vector3r axis;
                Real angle;

                /// Here is the code with approximated rotations 	 ///

		axis	 		= currentContactPhysics->prevNormal.Cross(currentContactGeometry->normal);
		shearForce 	       -= shearForce.Cross(axis);
		angle 			= dt*0.5*currentContactGeometry->normal.Dot(de1->angularVelocity+de2->angularVelocity);
		axis 			= angle*currentContactGeometry->normal;
		shearForce 	       -= shearForce.Cross(axis);

                /// Here is the code with exact rotations 		 ///

                // 		Quaternionr q;
                //
                // 		axis					= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
                // 		angle					= acos(currentContactGeometry->normal.dot(currentContactPhysics->prevNormal));
                // 		q.fromAngleAxis(angle,axis);
                //
                // 		currentContactPhysics->shearForce	= currentContactPhysics->shearForce*q;
                //
                // 		angle					= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
                // 		axis					= currentContactGeometry->normal;
                // 		q.fromAngleAxis(angle,axis);
                // 		currentContactPhysics->shearForce	= q*currentContactPhysics->shearForce;

                /// 							 ///

                Vector3r x				= currentContactGeometry->contactPoint;
                Vector3r c1x				= (x - de1->se3.position);
                Vector3r c2x				= (x - de2->se3.position);
                /// The following definition of c1x and c2x is to avoid "granular ratcheting" 
		///  (see F. ALONSO-MARROQUIN, R. GARCIA-ROJO, H.J. HERRMANN, 
		///   "Micro-mechanical investigation of granular ratcheting, in Cyclic Behaviour of Soils and Liquefaction Phenomena",
		///   ed. T. Triantafyllidis (Balklema, London, 2004), p. 3-10 - and a lot more papers from the same authors)
                Vector3r _c1x_	= currentContactGeometry->radius1*currentContactGeometry->normal;
                Vector3r _c2x_	= -currentContactGeometry->radius2*currentContactGeometry->normal;
                Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(_c2x_)) - (de1->velocity+de1->angularVelocity.Cross(_c1x_));
                Vector3r shearVelocity			= relativeVelocity-currentContactGeometry->normal.Dot(relativeVelocity)*currentContactGeometry->normal;
                Vector3r shearDisplacement		= shearVelocity*dt;


///////////////////////// CREEP START (commented out) ///////////
//	Real    viscosity = 300000.0;
//	shearForce                            -= currentContactPhysics->ks*(shearDisplacement + shearForce*dt/viscosity);

		shearForce -= currentContactPhysics->ks*shearDisplacement;
///////////////////////// CREEP END /////////////////////////////

                //  cerr << "shearForce = " << shearForce << endl;
                Real maxFs = 0;
                Real Fs = currentContactPhysics->shearForce.Length();
                maxFs = std::max((Real) 0,Fn*currentContactPhysics->tangensOfFrictionAngle);
                
                if ( Fs  > maxFs )
                {
			
                    
			currentContactPhysics->SetBreakingState();

                    //     maxFs = currentContactPhysics->shearAdhesion;
                    //    if (!currentContactPhysics->cohesionDisablesFriction && un>0)
                    //         maxFs += Fn * currentContactPhysics->tangensOfFrictionAngle;

                    	maxFs = max((Real) 0, Fn * currentContactPhysics->tangensOfFrictionAngle);

                    //cerr << "currentContactPhysics->tangensOfFrictionAngle = " << currentContactPhysics->tangensOfFrictionAngle << endl;
                    // cerr << "maxFs = " << maxFs << endl;

                    	maxFs = maxFs / Fs;
                    // cerr << "maxFs = " << maxFs << endl;
                    	if (maxFs>1)
                        	cerr << "maxFs>1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    	shearForce *= maxFs;
			if (Fn<0)  currentContactPhysics->normalForce = Vector3r::ZERO;
                }

                

                ////////// PFC3d SlipModel

                Vector3r f				= currentContactPhysics->normalForce + shearForce;
                // cerr << "currentContactPhysics->normalForce= " << currentContactPhysics->normalForce << endl;
                //  cerr << "shearForce " << shearForce << endl;
                // cerr << "f= " << f << endl;
                // it will be some macro(	body->physicalActions,	ActionType , bodyId )
					ncb->bex.addForce (id1,-f);
					ncb->bex.addForce (id2,+f);
					ncb->bex.addTorque(id1,-c1x.Cross(f));
					ncb->bex.addTorque(id2, c2x.Cross(f));

/////	/// Moment law					 	 ///
/////		if(momentRotationLaw /*&& currentContactPhysics->cohesionBroken == false*/ )
/////		{	
/////			Quaternionr delta = ...
/////
/////	//This indentation is a rewrite of original equations (the two commented lines), should work exactly the same.
///////Real elasticMoment = currentContactPhysics->kr * std::abs(angle); // positive value (*)
/////
/////	Real angle_twist(angle * axis.Dot(currentContactGeometry->normal) );
/////	Vector3r axis_twist(angle_twist * currentContactGeometry->normal);
//////* no creep	
////////////////////////////// CREEP START ///////////////////////////
/////			Real viscosity_twist = viscosity * std::pow((2 * std::min(currentContactGeometry->radius1,currentContactGeometry->radius2)),2) / 16.0;
/////			Real angle_twist_creeped = angle_twist * (1 - dt/viscosity_twist);
/////			Quaternionr q_twist(currentContactGeometry->normal , angle_twist);
/////			//Quaternionr q_twist_creeped(currentContactGeometry->normal , angle_twist*0.996);
/////			Quaternionr q_twist_creeped(currentContactGeometry->normal , angle_twist_creeped);
/////			Quaternionr q_twist_delta(q_twist_creeped * q_twist.Conjugate() );
/////			// modify the initialRelativeOrientation to substract some twisting
/////			currentContactPhysics->initialRelativeOrientation = currentContactPhysics->initialRelativeOrientation * q_twist_delta;
////////////////////////////// CREEP END /////////////////////////////
/////*/
/////	Vector3r moment_twist(axis_twist * currentContactPhysics->kr);
/////
/////	Vector3r axis_bending(angle*axis - axis_twist);
/////	Vector3r moment_bending(axis_bending * currentContactPhysics->kr);
/////
/////			/*
/////			// We cannot have insanely big moment, so we take a min value of ELASTIC and PLASTIC moment.
/////			Real avgRadius = 0.5 * (currentContactGeometry->radius1 + currentContactGeometry->radius2);
/////			// FIXME - elasticRollingLimit is currently assumed to be 1.0
/////			Real plasticMoment = currentContactPhysics->elasticRollingLimit * avgRadius * std::abs(Fn); // positive value (*)
/////			Real moment(std::min(elasticMoment, plasticMoment)); // RESULT
/////			*/
/////
///////Vector3r moment = axis * elasticMoment * (angle<0.0?-1.0:1.0); // restore sign. (*)
/////
/////	Vector3r moment = moment_twist + moment_bending;
/////
/////			static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum += moment;
/////			static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum -= moment;
/////		}
/////	/// Moment law	END				 	 ///

	/// Moment law					 	 ///
		if(momentRotationLaw)
		{
			{// updates only orientation of contact (local coordinate system)
				Vector3r axis = currentContactPhysics->prevNormal.UnitCross(currentContactGeometry->normal);
				Real angle =  unitVectorsAngle(currentContactPhysics->prevNormal,currentContactGeometry->normal);
				Quaternionr align(axis,angle);
				currentContactPhysics->currentContactOrientation =  align * currentContactPhysics->currentContactOrientation;
			}

			Quaternionr delta( de1->se3.orientation * currentContactPhysics->initialOrientation1.Conjugate() *
		                           currentContactPhysics->initialOrientation2 * de2->se3.orientation.Conjugate());

			Vector3r axis;	// axis of rotation - this is the Moment direction UNIT vector.
			Real angle;	// angle represents the power of resistant ELASTIC moment
			delta.ToAxisAngle(axis,angle);
			if(angle > Mathr::PI) angle -= Mathr::TWO_PI; // angle is between 0 and 2*pi, but should be between -pi and pi 

	//This indentation is a rewrite of original equations (the two commented lines), should work exactly the same.
//Real elasticMoment = currentContactPhysics->kr * std::abs(angle); // positive value (*)

			Real angle_twist(angle * axis.Dot(currentContactGeometry->normal) );
			Vector3r axis_twist(angle_twist * currentContactGeometry->normal);
			Vector3r moment_twist(axis_twist * currentContactPhysics->kr);

			Vector3r axis_bending(angle*axis - axis_twist);
			Vector3r moment_bending(axis_bending * currentContactPhysics->kr);

//Vector3r moment = axis * elasticMoment * (angle<0.0?-1.0:1.0); // restore sign. (*)

			Vector3r moment = moment_twist + moment_bending;

// 	Limitation par seuil plastique
			if (!momentAlwaysElastic)
			{
				Real normeMomentMax = currentContactPhysics->forMaxMoment * std::fabs(Fn);
				Real normeMoment = moment.Length();
				if(normeMoment>normeMomentMax)
					{
					moment *= normeMomentMax/normeMoment;
					nbreInteracMomPlastif++;
					}
			}
			ncb->bex.addTorque(id1,-moment);
			ncb->bex.addTorque(id2,+moment);
		}
	/// Moment law	END				 	 ///

                currentContactPhysics->prevNormal = currentContactGeometry->normal;
            }
        }
    }
    //cerr << "ncount= " << ncount << endl;//REMOVE
// 	cout << momentAlwaysElastic << endl;
// 	cout << "Sur " << nbreInteracTot << " interactions (reelles) " << nbreInteracMomPlastif << " se sont vues corriger leur moment" << endl;

}

YADE_PLUGIN((ContactLaw1));