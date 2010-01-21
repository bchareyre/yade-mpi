/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@imag.fr>         *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CohesiveFrictionalContactLaw.hpp"
#include<yade/pkg-dem/CohesiveFrictionalMat.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-dem/CohesiveFrictionalContactInteraction.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

Vector3r translation_vect_ ( 0.10,0,0 );


CohesiveFrictionalContactLaw::CohesiveFrictionalContactLaw() : InteractionSolver()
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


void out ( Quaternionr q )
{
	Vector3r axis;
	Real angle;
	q.ToAxisAngle ( axis,angle );
	std::cout << " axis: " <<  axis[0] << " " << axis[1] << " " << axis[2] << ", angle: " << angle << " | ";
}

void outv ( Vector3r axis )
{
	std::cout << " axis: " <<  axis[0] << " " << axis[1] << " " << axis[2] << ", length: " << axis.Length() << " | ";
}

void CohesiveFrictionalContactLaw::action ( Scene* ncb )
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();
	InteractionContainer::iterator ii    = ncb->interactions->begin();
	InteractionContainer::iterator iiEnd = ncb->interactions->end();
	for ( ; ii!=iiEnd ; ++ii )
	{
		//if ((*ii)->interactionGeometry && (*ii)->interactionPhysics)
		if ( ( *ii )->isReal() )
		{
			if ( detectBrokenBodies
					  && (*bodies)[(*ii)->getId1()]->shape->getClassName() != "box"
					  && (*bodies)[(*ii)->getId2()]->shape->getClassName() != "box"
			   )
			{
				YADE_CAST<CohesiveFrictionalMat*> ( ( *bodies ) [ ( *ii )->getId1() ]->material.get() )->isBroken = false;
				YADE_CAST<CohesiveFrictionalMat*> ( ( *bodies ) [ ( *ii )->getId2() ]->material.get() )->isBroken = false;
			}

			const shared_ptr<Interaction>& contact = *ii;
			int id1 = contact->getId1();
			int id2 = contact->getId2();

			if (!((*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask))
				continue; // skip other groups,
			Body* b1 = ( *bodies ) [id1].get();
			Body* b2 = ( *bodies ) [id2].get();
			ScGeom* currentContactGeometry  = YADE_CAST<ScGeom*> ( contact->interactionGeometry.get() );
			CohesiveFrictionalContactInteraction* currentContactPhysics = YADE_CAST<CohesiveFrictionalContactInteraction*> ( contact->interactionPhysics.get() );
			Vector3r& shearForce    = currentContactPhysics->shearForce;
			if ( contact->isFresh ( ncb ) ) shearForce   = Vector3r::ZERO;

			Real un     = currentContactGeometry->penetrationDepth;
			Real Fn    = currentContactPhysics->kn*un;
			currentContactPhysics->normalForce = Fn*currentContactGeometry->normal;
			if ( un < 0
					&& ( currentContactPhysics->normalForce.SquaredLength() > pow ( currentContactPhysics->normalAdhesion,2 )
						 || currentContactPhysics->normalAdhesion==0
					   )
			   )
			{
				// BREAK due to tension
				ncb->interactions->requestErase ( contact->getId1(),contact->getId2() );
				// contact->interactionPhysics was reset now; currentContactPhysics still hold the object, but is not associated with the interaction anymore
				currentContactPhysics->cohesionBroken = true;
				currentContactPhysics->normalForce = Vector3r::ZERO;
				currentContactPhysics->shearForce = Vector3r::ZERO;
			}
			else
			{
				Vector3r axis;
				Real angle;
				/// Here is the code with approximated rotations   ///
				axis    = currentContactPhysics->prevNormal.Cross ( currentContactGeometry->normal );
				shearForce         -= shearForce.Cross ( axis );
				angle    = dt*0.5*currentContactGeometry->normal.Dot ( Body::byId ( id1 )->state->angVel+Body::byId ( id2 )->state->angVel );
				axis    = angle*currentContactGeometry->normal;
				shearForce         -= shearForce.Cross ( axis );
				Vector3r x    = currentContactGeometry->contactPoint;
				Vector3r c1x    = ( x - b1->state->pos );
				Vector3r c2x    = ( x - b2->state->pos );
				/// The following definition of c1x and c2x is to avoid "granular ratcheting"
				///  (see F. ALONSO-MARROQUIN, R. GARCIA-ROJO, H.J. HERRMANN,
				///   "Micro-mechanical investigation of granular ratcheting, in Cyclic Behaviour of Soils and Liquefaction Phenomena",
				///   ed. T. Triantafyllidis (Balklema, London, 2004), p. 3-10 - and a lot more papers from the same authors)
				Vector3r _c1x_ = currentContactGeometry->radius1*currentContactGeometry->normal;
				Vector3r _c2x_ = -currentContactGeometry->radius2*currentContactGeometry->normal;
				Vector3r relativeVelocity  = ( b2->state->vel+b2->state->angVel.Cross ( _c2x_ ) ) - ( b1->state->vel+b1->state->angVel.Cross ( _c1x_ ) );
				Vector3r shearVelocity   = relativeVelocity-currentContactGeometry->normal.Dot ( relativeVelocity ) *currentContactGeometry->normal;
				Vector3r shearDisplacement  = shearVelocity*dt;


///////////////////////// CREEP START (commented out) ///////////
				if ( shear_creep )
				{
					shearForce                            -= currentContactPhysics->ks* ( shearDisplacement + shearForce*dt/creep_viscosity );
				}
				else
				{
					shearForce           -= currentContactPhysics->ks*shearDisplacement;
				}
///////////////////////// CREEP END /////////////////////////////

				//  cerr << "shearForce = " << shearForce << endl;
				Real maxFs = 0;
				Real Fs = currentContactPhysics->shearForce.Length();
				maxFs = std::max ( ( Real ) 0, currentContactPhysics->shearAdhesion + Fn*currentContactPhysics->tangensOfFrictionAngle );
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
					maxFs = max ( ( Real ) 0, Fn * currentContactPhysics->tangensOfFrictionAngle );
					maxFs = maxFs / Fs;
					// cerr << "maxFs = " << maxFs << endl;
					if ( maxFs>1 )
						cerr << "maxFs>1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
					shearForce *= maxFs;
					if ( Fn<0 )  currentContactPhysics->normalForce = Vector3r::ZERO;
				}
				////////// PFC3d SlipModel
				Vector3r f    = currentContactPhysics->normalForce + shearForce;
				// cerr << "currentContactPhysics->normalForce= " << currentContactPhysics->normalForce << endl;
				//  cerr << "shearForce " << shearForce << endl;
				// cerr << "f= " << f << endl;
				// it will be some macro( body->physicalActions, ActionType , bodyId )
				ncb->forces.addForce ( id1,-f );
				ncb->forces.addForce ( id2, f );
				ncb->forces.addTorque ( id1,-c1x.Cross ( f ) );
				ncb->forces.addTorque ( id2, c2x.Cross ( f ) );



				/// Moment law        ///
				if ( momentRotationLaw && ( currentContactPhysics->cohesionBroken == false || always_use_moment_law ) )
				{
					// Not necessary. OK.
					//{// updates only orientation of contact (local coordinate system)
					// Vector3r axis = currentContactPhysics->prevNormal.UnitCross(currentContactGeometry->normal);
					// Real angle =  unitVectorsAngle(currentContactPhysics->prevNormal,currentContactGeometry->normal);
					// Quaternionr align(axis,angle);
					// currentContactPhysics->currentContactOrientation =  align * currentContactPhysics->currentContactOrientation;
					//}

					Quaternionr delta ( b1->state->ori * currentContactPhysics->initialOrientation1.Conjugate() *
										currentContactPhysics->initialOrientation2 * b2->state->ori.Conjugate() );
					if ( twist_creep )
					{
						delta = delta * currentContactPhysics->twistCreep;
					}

					Vector3r axis; // axis of rotation - this is the Moment direction UNIT vector.
					Real angle; // angle represents the power of resistant ELASTIC moment
					delta.ToAxisAngle ( axis,angle );
					if ( angle > Mathr::PI ) angle -= Mathr::TWO_PI; // angle is between 0 and 2*pi, but should be between -pi and pi

					//This indentation is a rewrite of original equations (the two commented lines), should work exactly the same.
//Real elasticMoment = currentContactPhysics->kr * std::abs(angle); // positive value (*)

					Real angle_twist ( angle * axis.Dot ( currentContactGeometry->normal ) );
					Vector3r axis_twist ( angle_twist * currentContactGeometry->normal );

					if ( twist_creep )
					{
						Real viscosity_twist = creep_viscosity * std::pow ( ( 2 * std::min ( currentContactGeometry->radius1,currentContactGeometry->radius2 ) ),2 ) / 16.0;
						Real angle_twist_creeped = angle_twist * ( 1 - dt/viscosity_twist );
						Quaternionr q_twist ( currentContactGeometry->normal , angle_twist );
						//Quaternionr q_twist_creeped(currentContactGeometry->normal , angle_twist*0.996);
						Quaternionr q_twist_creeped ( currentContactGeometry->normal , angle_twist_creeped );
						Quaternionr q_twist_delta ( q_twist_creeped * q_twist.Conjugate() );
						currentContactPhysics->twistCreep = currentContactPhysics->twistCreep * q_twist_delta;
						// modify the initialRelativeOrientation to substract some twisting
						// currentContactPhysics->initialRelativeOrientation = currentContactPhysics->initialRelativeOrientation * q_twist_delta;
						//currentContactPhysics->initialOrientation1 = currentContactPhysics->initialOrientation1 * q_twist_delta;
						//currentContactPhysics->initialOrientation2 = currentContactPhysics->initialOrientation2 * q_twist_delta.Conjugate();
					}


					Vector3r moment_twist ( axis_twist * currentContactPhysics->kr );

					Vector3r axis_bending ( angle*axis - axis_twist );
					Vector3r moment_bending ( axis_bending * currentContactPhysics->kr );

//Vector3r moment = axis * elasticMoment * (angle<0.0?-1.0:1.0); // restore sign. (*)

					Vector3r moment = moment_twist + moment_bending;
					currentContactPhysics->moment_twist = moment_twist;
					currentContactPhysics->moment_bending = moment_bending;
					ncb->forces.addTorque ( id1,-moment );
					ncb->forces.addTorque ( id2, moment );
				}
				/// Moment law END       ///

				currentContactPhysics->prevNormal = currentContactGeometry->normal;
			}
		}
	}
}

YADE_PLUGIN ((CohesiveFrictionalContactLaw));



