/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre, Janek Kozicki                   *
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
	detectBrokenBodies = false;
	always_use_moment_law = false;

//CREEP
	shear_creep=false;
	twist_creep=false;
	creep_viscosity = 1.0;
}


void CohesiveFrictionalContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
	REGISTER_ATTRIBUTE(erosionActivated);
	REGISTER_ATTRIBUTE(detectBrokenBodies);
	REGISTER_ATTRIBUTE(always_use_moment_law);
	
	REGISTER_ATTRIBUTE(shear_creep);
	REGISTER_ATTRIBUTE(twist_creep);
	REGISTER_ATTRIBUTE(creep_viscosity);
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
if(shear_creep){
	shearForce                            -= currentContactPhysics->ks*(shearDisplacement + shearForce*dt/creep_viscosity);
} else {
shearForce 			       -= currentContactPhysics->ks*shearDisplacement;
}
///////////////////////// CREEP END /////////////////////////////

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
		if(momentRotationLaw && (currentContactPhysics->cohesionBroken == false || always_use_moment_law) )
		{
			// Not necessary. OK.
			//{// updates only orientation of contact (local coordinate system)
			//	Vector3r axis = currentContactPhysics->prevNormal.UnitCross(currentContactGeometry->normal);
			//	Real angle =  unitVectorsAngle(currentContactPhysics->prevNormal,currentContactGeometry->normal);
			//	Quaternionr align(axis,angle);
			//	currentContactPhysics->currentContactOrientation =  align * currentContactPhysics->currentContactOrientation;
			//}

			Quaternionr delta( de1->se3.orientation * currentContactPhysics->initialOrientation1.Conjugate() *
		                           currentContactPhysics->initialOrientation2 * de2->se3.orientation.Conjugate());
			if(twist_creep){
				delta = delta * currentContactPhysics->twistCreep;
			}

			Vector3r axis;	// axis of rotation - this is the Moment direction UNIT vector.
			Real angle;	// angle represents the power of resistant ELASTIC moment
			delta.ToAxisAngle(axis,angle);
			if(angle > Mathr::PI) angle -= Mathr::TWO_PI; // angle is between 0 and 2*pi, but should be between -pi and pi 

	//This indentation is a rewrite of original equations (the two commented lines), should work exactly the same.
//Real elasticMoment = currentContactPhysics->kr * std::abs(angle); // positive value (*)

	Real angle_twist(angle * axis.Dot(currentContactGeometry->normal) );
	Vector3r axis_twist(angle_twist * currentContactGeometry->normal);

			if(twist_creep){
				Real viscosity_twist = creep_viscosity * std::pow((2 * std::min(currentContactGeometry->radius1,currentContactGeometry->radius2)),2) / 16.0;
				Real angle_twist_creeped = angle_twist * (1 - dt/viscosity_twist);
				Quaternionr q_twist(currentContactGeometry->normal , angle_twist);
				//Quaternionr q_twist_creeped(currentContactGeometry->normal , angle_twist*0.996);
				Quaternionr q_twist_creeped(currentContactGeometry->normal , angle_twist_creeped);
				Quaternionr q_twist_delta(q_twist_creeped * q_twist.Conjugate() );
		currentContactPhysics->twistCreep = currentContactPhysics->twistCreep * q_twist_delta;
				// modify the initialRelativeOrientation to substract some twisting
			//	currentContactPhysics->initialRelativeOrientation = currentContactPhysics->initialRelativeOrientation * q_twist_delta;
			//currentContactPhysics->initialOrientation1 = currentContactPhysics->initialOrientation1 * q_twist_delta;
			//currentContactPhysics->initialOrientation2 = currentContactPhysics->initialOrientation2 * q_twist_delta.Conjugate();
			}


	Vector3r moment_twist(axis_twist * currentContactPhysics->kr);

	Vector3r axis_bending(angle*axis - axis_twist);
	Vector3r moment_bending(axis_bending * currentContactPhysics->kr);

//Vector3r moment = axis * elasticMoment * (angle<0.0?-1.0:1.0); // restore sign. (*)

	Vector3r moment = moment_twist + moment_bending;

			static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= moment;
			static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += moment;
		}
	/// Moment law	END				 	 ///

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
