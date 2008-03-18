/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre                                  *
*  bruno.chareyre@imag.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CohesiveFrictionalContactLaw.hpp"
#include<yade/pkg-dem/CohesiveFrictionalBodyParameters.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-dem/CohesiveFrictionalContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/core/PhysicalAction.hpp>


Vector3r translation_vect (0.10,0,0);


CohesiveFrictionalContactLaw::CohesiveFrictionalContactLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
	sdecGroupMask=1;
	momentRotationLaw = true;
	erosionActivated = false;
	detectBrokenBodies = true;
}


void CohesiveFrictionalContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
	REGISTER_ATTRIBUTE(erosionActivated);
	REGISTER_ATTRIBUTE(detectBrokenBodies);
}

void out(Quaternionr q)
{
	Vector3r axis;
	Real angle;
	q.ToAxisAngle(axis,angle);
	std::cout << " axis: " <<  axis[0] << " " << axis[1] << " " << axis[2] << ", angle: " << angle << " | ";
}

void outv(Vector3r axis)
{
	std::cout << " axis: " <<  axis[0] << " " << axis[1] << " " << axis[2] << ", length: " << axis.Length() << " | ";
}

void CohesiveFrictionalContactLaw::action(Body* body)
{
    MetaBody * ncb = YADE_CAST<MetaBody*>(body);
    shared_ptr<BodyContainer>& bodies = ncb->bodies;

    Real dt = Omega::instance().getTimeStep();
//    static long ncount = 0;//REMOVE
//    ncount = 0;

    ///Reset the isBroken flag
    //if (iter != Omega::instance().getCurrentIteration())
    //
    if (detectBrokenBodies)
    {
        BodyContainer::iterator bi    = bodies->begin();
        BodyContainer::iterator biEnd = bodies->end();
        for ( ; bi!=biEnd ; ++bi )
        {
            shared_ptr<Body> b = *bi;
            if (b->geometricalModel && b->interactingGeometry && b->geometricalModel->getClassName()=="Sphere")
                (static_cast<CohesiveFrictionalBodyParameters*> (b->physicalParameters.get()))->isBroken = true;
            // b->geometricalModel->diffuseColor= Vector3r(0.5,0.3,0.9);
        }
    }
    //iter = Omega::instance().getCurrentIteration();

/// Non Permanents Links												///

    InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
    InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
    for (  ; ii!=iiEnd ; ++ii )
    {
        //if ((*ii)->interactionGeometry && (*ii)->interactionPhysics)
        if ((*ii)->isReal)
        {
            if (detectBrokenBodies 
		    /* FIXME - this had no effect. InteractingBox has its isBroken=false too.
		     *
		     * FIXME - got to replace this with a working test.
		     *
		     * && (*bodies)[(*ii)->getId1()]->interactingGeometry->getClassName() != "box"  
		     * && (*bodies)[(*ii)->getId2()]->interactingGeometry->getClassName() != "box" */
		    )
            {
                YADE_CAST<CohesiveFrictionalBodyParameters*>((*bodies)[(*ii)->getId1()]->physicalParameters.get())->isBroken = false;
                YADE_CAST<CohesiveFrictionalBodyParameters*>((*bodies)[(*ii)->getId2()]->physicalParameters.get())->isBroken = false;
            }

            const shared_ptr<Interaction>& contact = *ii;
            int id1 = contact->getId1();
            int id2 = contact->getId2();

            if ( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
                continue; // skip other groups,

            CohesiveFrictionalBodyParameters* de1 			= YADE_CAST<CohesiveFrictionalBodyParameters*>((*bodies)[id1]->physicalParameters.get());
            CohesiveFrictionalBodyParameters* de2 			= YADE_CAST<CohesiveFrictionalBodyParameters*>((*bodies)[id2]->physicalParameters.get());
            SpheresContactGeometry* currentContactGeometry		= YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get());
            CohesiveFrictionalContactInteraction* currentContactPhysics = YADE_CAST<CohesiveFrictionalContactInteraction*> (contact->interactionPhysics.get());

            Vector3r& shearForce 			= currentContactPhysics->shearForce;

            if (contact->isNew)
                shearForce			= Vector3r::ZERO;

            Real un 				= currentContactGeometry->penetrationDepth;
            Real Fn				= currentContactPhysics->kn*un;
            currentContactPhysics->normalForce	= Fn*currentContactGeometry->normal;
            if (   un < 0 
		&& (   currentContactPhysics->normalForce.SquaredLength() > pow(currentContactPhysics->normalAdhesion,2) 
		    || currentContactPhysics->normalAdhesion==0
		   )
	       )
            { // BREAK due to tension

                //currentContactPhysics->SetBreakingState();
                //if (currentContactPhysics->cohesionBroken) {
                //cerr << "broken" << endl;

                contact->isReal= false;
                currentContactPhysics->cohesionBroken = true;
                currentContactPhysics->normalForce = Vector3r::ZERO;
                currentContactPhysics->shearForce = Vector3r::ZERO;

                //return;
                //    } else
                //    currentContactPhysics->normalForce	= -currentContactPhysics->normalAdhesion*currentContactGeometry->normal;
            }
            else
            {

/* ORIG */                Vector3r axis;
/* ORIG */                Real angle;
/* ORIG */
/* ORIG */                /// Here is the code with approximated rotations 	 ///
/* ORIG */
/* ORIG */		axis	 		= currentContactPhysics->prevNormal.Cross(currentContactGeometry->normal);
/* ORIG */		shearForce 	       -= shearForce.Cross(axis);
/* ORIG */		angle 			= dt*0.5*currentContactGeometry->normal.Dot(de1->angularVelocity+de2->angularVelocity);
/* ORIG */		axis 			= angle*currentContactGeometry->normal;
/* ORIG */		shearForce 	       -= shearForce.Cross(axis);
/* ORIG */
/* ORIG */                /// Here is the code with exact rotations 		 ///
/* ORIG */
/* ORIG */                // 		Quaternionr q;
/* ORIG */                //
/* ORIG */                // 		axis					= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
/* ORIG */                // 		angle					= acos(currentContactGeometry->normal.dot(currentContactPhysics->prevNormal));
/* ORIG */                // 		q.fromAngleAxis(angle,axis);
/* ORIG */                //
/* ORIG */                // 		currentContactPhysics->shearForce	= currentContactPhysics->shearForce*q;
/* ORIG */                //
/* ORIG */                // 		angle					= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
/* ORIG */                // 		axis					= currentContactGeometry->normal;
/* ORIG */                // 		q.fromAngleAxis(angle,axis);
/* ORIG */                // 		currentContactPhysics->shearForce	= q*currentContactPhysics->shearForce;
/* ORIG */
/* ORIG */                /// 							 ///
/* ORIG */
/* ORIG */                Vector3r x				= currentContactGeometry->contactPoint;
/* ORIG */                //Vector3r c1x				= (x - de1->se3.position);
/* ORIG */                //Vector3r c2x				= (x - de2->se3.position);
/* ORIG */                /// The following definition of c1x and c2x is to avoid "granular ratcheting" 
/* ORIG */		///  (see F. ALONSO-MARROQUIN, R. GARCIA-ROJO, H.J. HERRMANN, 
/* ORIG */		///   "Micro-mechanical investigation of granular ratcheting, in Cyclic Behaviour of Soils and Liquefaction Phenomena",
/* ORIG */		///   ed. T. Triantafyllidis (Balklema, London, 2004), p. 3-10 - and a lot more papers from the same authors)
/* BRUNO's ERROR was here*/                Vector3r c1x	= currentContactGeometry->radius1*currentContactGeometry->normal;
/* BRUNO's ERROR was here*/                Vector3r c2x	= -currentContactGeometry->radius2*currentContactGeometry->normal;
/* ORIG */                Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(c2x)) - (de1->velocity+de1->angularVelocity.Cross(c1x));
/* ORIG */                Vector3r shearVelocity			= relativeVelocity-currentContactGeometry->normal.Dot(relativeVelocity)*currentContactGeometry->normal;
/* ORIG */                Vector3r shearDisplacement		= shearVelocity*dt;
/* ORIG */
/* ORIG */
/* ORIG *////////////////////////// CREEP START ///////////////////////////
/* ORIG *///	Real    viscosity = 300000.0;
/* ORIG *///	shearForce                            -= currentContactPhysics->ks*(shearDisplacement + shearForce*dt/viscosity);
/* ORIG */
/* ORIG */shearForce 			       -= currentContactPhysics->ks*shearDisplacement;
/* ORIG *////////////////////////// CREEP END /////////////////////////////

//DEBUGGING// if((id1==22 && id2==6)){
//DEBUGGING// std::cout.precision(17);
//DEBUGGING// std::cout << "shearForce: " << shearForce << " ";
//DEBUGGING// std::cout << "shearDisplacement: " << shearDisplacement << " ";
//DEBUGGING// std::cout << "relativeVelocity: " << relativeVelocity << " ";
//DEBUGGING// std::cout << "velocity1: " << de1->velocity << " ";
//DEBUGGING// std::cout << "velocity2: " << de2->velocity << " ";
//DEBUGGING// std::cout << "angularVelocity1: " << de1->angularVelocity << " ";
//DEBUGGING// std::cout << "angularVelocity2: " << de2->angularVelocity << " ";
//DEBUGGING// }

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//{
//		Vector3r axis;//,axis1,axis2;
//		Real angle,shearDisplacement;//,angle1,angle2;
//		Quaternionr shear1(de1->se3.orientation * currentContactPhysics->initialOrientation1.Conjugate());
//		Quaternionr shear2(de2->se3.orientation * currentContactPhysics->initialOrientation2.Conjugate());
//		Quaternionr shearDelta( shear1 * shear2);
//		//shear1.ToAxisAngle(axis1,angle1);
//		//shear1.ToAxisAngle(axis2,angle2);
//		//if(angle1 > Mathr::PI) angle1 -= Mathr::TWO_PI;
//		//if(angle2 > Mathr::PI) angle2 -= Mathr::TWO_PI;
//		shearDelta.ToAxisAngle(axis,angle);
//		if(angle > Mathr::PI) angle -= Mathr::TWO_PI;
//		//angle1 *= currentContactGeometry->radius1;
//		//angle2 *= currentContactGeometry->radius2;
//		shearDisplacement = angle * std::min(currentContactGeometry->radius1,currentContactGeometry->radius2);//angle1+angle2;
//		Vector3r shearForceXX = -currentContactPhysics->ks*axis.Cross(currentContactGeometry->normal)*shearDisplacement;
//
//if((id1==22 && id2==6)){
//std::cerr /*<< "axis1: " << axis1 << " axis2: " << axis2*/ << " axis: " << axis << "\n";
//std::cerr << "shearDisplacement: " << shearDisplacement << " angle: " << angle /*<< " angle2: " << angle2*/ << "\n";
//std::cerr << "shearForce  : " << shearForce <<   " length: " << shearForce.Length() << "\n";
//std::cerr << "shearForceXX: " << shearForceXX << " length: " << shearForceXX.Length() << "\n\n";
//}
//}
//
//Vector3r contactPointsSpringForce;
//{
//	Quaternionr curr1(de1->se3.orientation * currentContactPhysics->orientationToContact1);
//	Quaternionr curr2(de2->se3.orientation * currentContactPhysics->orientationToContact2);
//	Vector3r initialContactPoint1(de1->se3.position + curr1*Vector3r(     currentContactGeometry->radius1,0,0) );
//	Vector3r initialContactPoint2(de2->se3.position + curr2*Vector3r(-1.0*currentContactGeometry->radius2,0,0) );
//	Vector3r contactDist( initialContactPoint2 - initialContactPoint1 );
//
//	Vector3r contactDir(contactDist); contactDir.Normalize();
//	//Vector3r shearDir(shearForce); shearDir.Normalize();
//	//Vector3r contactShearDisplacement( shearDir * shearDir.Dot(contactDir) * contactDist.Length() );
//	//Vector3r contactNormalDisplacement( contactDist - contactShearDisplacement );
//	//contactPointsSpringForce=(contactShearDisplacement+contactNormalDisplacement) * currentContactPhysics->ks;
//	//contactPointsSpringForce= contactDist * currentContactPhysics->ks;
//	
//	Vector3r contactNormalDisplacement(currentContactGeometry->normal * currentContactGeometry->normal.Dot(contactDir) * contactDist.Length());
//	Vector3r contactShearDisplacement(contactDist - contactNormalDisplacement);
//	contactPointsSpringForce=contactShearDisplacement * currentContactPhysics->ks;// - contactNormalDisplacement*currentContactPhysics->kn;
//
////if((id1==22 && id2==6)){
////std::cerr << "contactPointsSpringForce: " << contactPointsSpringForce << " length:" << contactPointsSpringForce.Length() << "\n";
////std::cerr << "shearForce              : " << shearForce       << " length:" << shearForce      .Length() << "\n";
////std::cerr << "shearForce - cpSpringF  : " << Vector3r(shearForce - contactPointsSpringForce) << " length:" << (shearForce - contactPointsSpringForce).Length() << "\n\n";
////std::cerr << contactDist * currentContactPhysics->ks << "\n" << contactPointsSpringForce << "\n\n";
////}
//}

//Vector3r WhatTheFuckForce(   de1->se3.position - currentContactPhysics->initialPosition1 
//                           - de2->se3.position + currentContactPhysics->initialPosition2 );
//WhatTheFuckForce *= currentContactPhysics->ks;
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

                //  cerr << "shearForce = " << shearForce << endl;
                Real maxFs = 0;
                Real Fs = currentContactPhysics->shearForce.Length();
                //if (!currentContactPhysics->cohesionBroken) {
                maxFs = std::max((Real) 0, currentContactPhysics->shearAdhesion + Fn*currentContactPhysics->tangensOfFrictionAngle);
                // if (!currentContactPhysics->cohesionDisablesFriction)
                //         maxFs += Fn * currentContactPhysics->tangensOfFrictionAngle;
                //} else
                // maxFs = Fn * currentContactPhysics->tangensOfFrictionAngle;
                //  cerr << "maxFs = " << maxFs << "     Fs = " << Fs<< endl;
                if ( Fs  > maxFs )
                {
			currentContactPhysics->cohesionBroken = true;

                    // brokenStatus[
                    //if (currentContactPhysics->fragile && !currentContactPhysics->cohesionBroken)
		    
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

                //if (!currentContactPhysics->cohesionBroken)
                //{
                //cerr << "de1->isBroken == false;" << endl;
//                     if ((*bodies)[id1]->interactingGeometry->getClassName() != "box"  && (*bodies)[id2]->interactingGeometry->getClassName() != "box")
//                     de1->isBroken = false;
//                     de2->isBroken = false;
//                ++ncount;//REMOVE
                //(*bodies)[id1]->geometricalModel->diffuseColor= Vector3r(0.8,0.3,0.3);
                //(*bodies)[id2]->geometricalModel->diffuseColor= Vector3r(0.8,0.3,0.3);
                //}

                ////////// PFC3d SlipModel

                Vector3r f				= currentContactPhysics->normalForce + shearForce;
                // cerr << "currentContactPhysics->normalForce= " << currentContactPhysics->normalForce << endl;
                //  cerr << "shearForce " << shearForce << endl;
                // cerr << "f= " << f << endl;
                // it will be some macro(	body->physicalActions,	ActionType , bodyId )
                static_cast<Force*>   ( ncb->physicalActions->find( id1 , actionForce   ->getClassIndex() ).get() )->force    -= f;
                static_cast<Force*>   ( ncb->physicalActions->find( id2 , actionForce   ->getClassIndex() ).get() )->force    += f;

                static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= c1x.Cross(f);
                static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += c2x.Cross(f);

/////	/// Moment law					 	 ///
/////		if(momentRotationLaw /*&& currentContactPhysics->cohesionBroken == false*/ )
/////		{	
/////			Quaternionr initialRelativeOrientation = currentContactPhysics->initialOrientation1 * currentContactPhysics->initialOrientation2.Conjugate();
/////			Quaternionr currentRelativeOrientation = de1->se3.orientation * de2->se3.orientation.Conjugate();
/////			// difference of rotations between two spheres
/////			Quaternionr delta = /*initialRelativeOrientation */ currentRelativeOrientation.Conjugate();
/////			Vector3r axis;	// axis of rotation - this is the Moment direction UNIT vector.
/////			Real angle;	// angle represents the power of resistant ELASTIC moment
/////			delta.ToAxisAngle(axis,angle);
/////			if(angle > Mathr::PI) angle -= Mathr::TWO_PI; // angle is between 0 and 2*pi, but should be between -pi and pi 
/////
///////if((id1==18 && id2==12)||(id1==12 && id2==18)){
///////out(currentContactPhysics->initialOrientation1 * currentContactPhysics->initialOrientation2.Conjugate());std::cout << "\n";
///////out(currentContactPhysics->initialOrientation2);std::cout << "\n";
///////out(delta * de1->se3.orientation * de2->se3.orientation.Conjugate());std::cout << "\n";
///////out(de2->se3.orientation * delta);std::cout << "\n";
/////
///////Quaternionr A(de1->se3.orientation), B(de2->se3.orientation);
///////out( (  A * B.Conjugate() ).Conjugate() );std::cout << "\n";
///////out( (  B * A.Conjugate() ) );std::cout << "\n";
///////out( (  A.Conjugate() * B ).Conjugate() );std::cout << "\n";
///////out( (  B.Conjugate() * A ) );std::cout << "\n";
///////}
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
///////if((id1==18 && id2==12)||(id1==12 && id2==18)){
///////outv(moment);std::cout << "\n";
///////}
/////
/////			static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum += moment;
/////			static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum -= moment;
/////		}
/////	/// Moment law	END				 	 ///

/* RATHER CORRECT */	/// Moment law					 	 ///
/* RATHER CORRECT */		if(momentRotationLaw && currentContactPhysics->cohesionBroken == false )
/* RATHER CORRECT */		{
/* RATHER CORRECT */			{// updates only orientation of contact (local coordinate system)
/* RATHER CORRECT */				Vector3r axis = currentContactPhysics->prevNormal.UnitCross(currentContactGeometry->normal);
/* RATHER CORRECT */				Real angle =  unitVectorsAngle(currentContactPhysics->prevNormal,currentContactGeometry->normal);
/* RATHER CORRECT */				Quaternionr align(axis,angle);
/* RATHER CORRECT */				currentContactPhysics->currentContactOrientation =  align * currentContactPhysics->currentContactOrientation;
/* RATHER CORRECT */			}
/* RATHER CORRECT */
/* RATHER CORRECT *///			Quaternionr curr1(de1->se3.orientation * currentContactPhysics->orientationToContact1);
/* RATHER CORRECT *///			Quaternionr curr2(de2->se3.orientation * currentContactPhysics->orientationToContact2);
/* RATHER CORRECT *///			Quaternionr delta=curr1 * curr2.Conjugate();
/* RATHER CORRECT */
/* RATHER CORRECT */			Quaternionr delta( de1->se3.orientation * currentContactPhysics->initialOrientation1.Conjugate() *
/* RATHER CORRECT */		                           currentContactPhysics->initialOrientation2 * de2->se3.orientation.Conjugate());
/* RATHER CORRECT */
/* RATHER CORRECT */			Vector3r axis;	// axis of rotation - this is the Moment direction UNIT vector.
/* RATHER CORRECT */			Real angle;	// angle represents the power of resistant ELASTIC moment
/* RATHER CORRECT */			delta.ToAxisAngle(axis,angle);
/* RATHER CORRECT */			if(angle > Mathr::PI) angle -= Mathr::TWO_PI; // angle is between 0 and 2*pi, but should be between -pi and pi 
/* RATHER CORRECT */
/* RATHER CORRECT */	//This indentation is a rewrite of original equations (the two commented lines), should work exactly the same.
/* RATHER CORRECT *///Real elasticMoment = currentContactPhysics->kr * std::abs(angle); // positive value (*)
/* RATHER CORRECT */
/* RATHER CORRECT */	Real angle_twist(angle * axis.Dot(currentContactGeometry->normal) );
/* RATHER CORRECT */	Vector3r axis_twist(angle_twist * currentContactGeometry->normal);
/* RATHER CORRECT */	Vector3r moment_twist(axis_twist * currentContactPhysics->kr);
/* RATHER CORRECT */
/* RATHER CORRECT */	Vector3r axis_bending(angle*axis - axis_twist);
/* RATHER CORRECT */	Vector3r moment_bending(axis_bending * currentContactPhysics->kr);
/* RATHER CORRECT */
/* RATHER CORRECT *///Vector3r moment = axis * elasticMoment * (angle<0.0?-1.0:1.0); // restore sign. (*)
/* RATHER CORRECT */
/* RATHER CORRECT */	Vector3r moment = moment_twist + moment_bending;
/* RATHER CORRECT */
/* RATHER CORRECT *///DEBUGGING// if((id1==22 && id2==6)){
/* RATHER CORRECT *///DEBUGGING// std::cout << "moment: " << moment << " ";
/* RATHER CORRECT *///DEBUGGING// std::cout << "normal: " << currentContactGeometry->normal << "\n";
/* RATHER CORRECT *///DEBUGGING// }
/* RATHER CORRECT */
/* RATHER CORRECT */			static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= moment;
/* RATHER CORRECT */			static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += moment;
/* RATHER CORRECT */		}
/* RATHER CORRECT */	/// Moment law	END				 	 ///
///*EXPERIMENT*/	/// Moment law					 	 ///
///*EXPERIMENT*/		if(momentRotationLaw && currentContactPhysics->cohesionBroken == false )
///*EXPERIMENT*/		{
///*EXPERIMENT*/			{// updates only orientation of contact (local coordinate system)
///*EXPERIMENT*/				Vector3r axis = currentContactPhysics->prevNormal.UnitCross(currentContactGeometry->normal);
///*EXPERIMENT*/				Real angle =  unitVectorsAngle(currentContactPhysics->prevNormal,currentContactGeometry->normal);
///*EXPERIMENT*/				Quaternionr align(axis,angle);
///*EXPERIMENT*/				currentContactPhysics->currentContactOrientation =  align * currentContactPhysics->currentContactOrientation;
///*EXPERIMENT*/			}
///*EXPERIMENT*/
///*EXPERIMENT*/			Quaternionr delta1( de1->se3.orientation * currentContactPhysics->initialOrientation1.Conjugate() *
///*EXPERIMENT*/		                           currentContactPhysics->initialOrientation2 * de2->se3.orientation.Conjugate());
///*EXPERIMENT*/		                           //currentContactPhysics->initialContactOrientation * currentContactPhysics->currentContactOrientation.Conjugate());
///*EXPERIMENT*/
///*EXPERIMENT*/			Vector3r axis1;
///*EXPERIMENT*/			Real angle1;
///*EXPERIMENT*/			delta1.ToAxisAngle(axis1,angle1);
///*EXPERIMENT*/			if(angle1 > Mathr::PI) angle1 -= Mathr::TWO_PI;
///*EXPERIMENT*/
///*EXPERIMENT*/			Vector3r moment1 = axis1 * currentContactPhysics->kr * angle1;
///*EXPERIMENT*/
///*EXPERIMENT*/
///*EXPERIMENT*/			Quaternionr delta2( de1->se3.orientation * currentContactPhysics->initialOrientation1.Conjugate() *
///*EXPERIMENT*/			//Quaternionr delta2( currentContactPhysics->currentContactOrientation * currentContactPhysics->initialContactOrientation.Conjugate() *
///*EXPERIMENT*/		                            currentContactPhysics->initialOrientation2 * de2->se3.orientation.Conjugate());
///*EXPERIMENT*/
///*EXPERIMENT*/			Vector3r axis2;
///*EXPERIMENT*/			Real angle2;
///*EXPERIMENT*/			delta2.ToAxisAngle(axis2,angle2);
///*EXPERIMENT*/			if(angle2 > Mathr::PI) angle2 -= Mathr::TWO_PI;
///*EXPERIMENT*/
///*EXPERIMENT*/			Vector3r moment2 = axis2 * currentContactPhysics->kr * angle2;
///*EXPERIMENT*/
///*EXPERIMENT*/			static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= moment1;
///*EXPERIMENT*/			static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += moment2;
///*EXPERIMENT*/		}
///*EXPERIMENT*/	/// Moment law	END				 	 ///


                currentContactPhysics->prevNormal = currentContactGeometry->normal;
            }
        }
    }
    if (detectBrokenBodies)
    {
    	BodyContainer::iterator bi    = bodies->begin();
        BodyContainer::iterator biEnd = bodies->end();
        for ( ; bi!=biEnd ; ++bi )
        {
            shared_ptr<Body> b = *bi;
            if (b->geometricalModel && b->interactingGeometry && b->geometricalModel->getClassName()=="Sphere" && erosionActivated)
            {
                //cerr << "translate it" << endl;
                if ((static_cast<CohesiveFrictionalBodyParameters*> (b->physicalParameters.get()))->isBroken == true)
                {
                    if (b->isDynamic)
                        (static_cast<CohesiveFrictionalBodyParameters*> (b->physicalParameters.get()))->se3.position += translation_vect;
                    b->isDynamic = false;
                    b->geometricalModel->diffuseColor= Vector3r(0.5,0.3,0.9);


                }
                else  b->geometricalModel->diffuseColor= Vector3r(0.5,0.9,0.3);
            }
        }
    }
    //cerr << "ncount= " << ncount << endl;//REMOVE


}

YADE_PLUGIN();
