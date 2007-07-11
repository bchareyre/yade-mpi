/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre                                  *
*  bruno.chareyre@imag.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CohesiveFrictionalContactLaw.hpp"
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-dem/CohesiveFrictionalContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/core/PhysicalAction.hpp>


CohesiveFrictionalContactLaw::CohesiveFrictionalContactLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
	sdecGroupMask=1;
	momentRotationLaw = true;
}


void CohesiveFrictionalContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
}


//FIXME : remove bool first !!!!!
void CohesiveFrictionalContactLaw::action(Body* body)
{
        MetaBody * ncb = YADE_CAST<MetaBody*>(body);
        shared_ptr<BodyContainer>& bodies = ncb->bodies;

        Real dt = Omega::instance().getTimeStep();

        /// Non Permanents Links												///

        InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
        InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
        for(  ; ii!=iiEnd ; ++ii ) {
                //if ((*ii)->interactionGeometry && (*ii)->interactionPhysics)
                if ((*ii)->isReal) {
                        const shared_ptr<Interaction>& contact = *ii;
                        int id1 = contact->getId1();
                        int id2 = contact->getId2();

                        if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
                                continue; // skip other groups,

                        BodyMacroParameters* de1 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
                        BodyMacroParameters* de2 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());
                        SpheresContactGeometry* currentContactGeometry		= YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get());
                        CohesiveFrictionalContactInteraction* currentContactPhysics   	= YADE_CAST<CohesiveFrictionalContactInteraction*> (contact->interactionPhysics.get());

                        Vector3r& shearForce 			= currentContactPhysics->shearForce;

                        if (contact->isNew)
                                shearForce			= Vector3r::ZERO;

                        Real un 				= currentContactGeometry->penetrationDepth;
                        currentContactPhysics->normalForce	= currentContactPhysics->kn*un*currentContactGeometry->normal;
                        if (un < 0 && (currentContactPhysics->normalForce.SquaredLength() > pow(currentContactPhysics->normalAdhesion,2) || currentContactPhysics->normalAdhesion==0)) {
                                //currentContactPhysics->SetBreakingState();
                                //if (currentContactPhysics->cohesionBroken) {
                                cerr << "broken" << endl;
                                contact->isReal= false;
                                currentContactPhysics->cohesionBroken = true;
                                currentContactPhysics->normalForce = Vector3r::ZERO;
                                currentContactPhysics->shearForce = Vector3r::ZERO;
                                //return;
                                //                         } else
                                //                                 currentContactPhysics->normalForce	= -currentContactPhysics->normalAdhesion*currentContactGeometry->normal;
                        } else {

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
                                Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(c2x)) - (de1->velocity+de1->angularVelocity.Cross(c1x));
                                Vector3r shearVelocity			= relativeVelocity-currentContactGeometry->normal.Dot(relativeVelocity)*currentContactGeometry->normal;
                                Vector3r shearDisplacement		= shearVelocity*dt;
                                shearForce 			       -= currentContactPhysics->ks*shearDisplacement;
                              //  cerr << "shearForce0 = " << shearForce << endl;
                                Real maxFs = 0;
                                Real Fn = currentContactPhysics->kn*un;
                                Real Fs = currentContactPhysics->shearForce.Length();
                                //if (!currentContactPhysics->cohesionBroken) {
                                maxFs = max((Real) 0, currentContactPhysics->shearAdhesion + Fn*currentContactPhysics->tangensOfFrictionAngle);
                                // if (!currentContactPhysics->cohesionDisablesFriction)
                                //         maxFs += Fn * currentContactPhysics->tangensOfFrictionAngle;
                                //} else
                                // maxFs = Fn * currentContactPhysics->tangensOfFrictionAngle;
                              //  cerr << "maxFs = " << maxFs << "     Fs = " << Fs<< endl;
                                if( Fs  > maxFs ) {
                                        currentContactPhysics->cohesionBroken = true;
                                        //if (currentContactPhysics->fragile && !currentContactPhysics->cohesionBroken) {
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
                                static_cast<Force*>   ( ncb->physicalActions->find( id1 , actionForce   ->getClassIndex() ).get() )->force    -= f;
                                static_cast<Force*>   ( ncb->physicalActions->find( id2 , actionForce   ->getClassIndex() ).get() )->force    += f;

                                static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= c1x.Cross(f);
                                static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += c2x.Cross(f);

                                currentContactPhysics->prevNormal = currentContactGeometry->normal;
                        }
                }
        }
}

