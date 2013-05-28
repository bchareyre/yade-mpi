/*************************************************************************
*  Copyright (C) 2010 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/pkg/dem/KinemSimpleShearBox.hpp>

#include<yade/core/State.hpp>
#include<yade/pkg/dem/FrictPhys.hpp>
#include<yade/pkg/common/Box.hpp>

YADE_PLUGIN((KinemSimpleShearBox))


void KinemSimpleShearBox::computeAlpha()
{
	Quaternionr orientationLeftBox,orientationRightBox;
	orientationLeftBox = leftbox->state->ori;
	orientationRightBox = rightbox->state->ori;
	if(orientationLeftBox!=orientationRightBox)
	{
		cout << "WARNING !!! your lateral boxes have not the same orientation, you're not in the case of a box imagined for creating these engines" << endl;
	}
	Real angle;
	AngleAxisr aa(orientationLeftBox);
	angle = aa.angle();
// 	orientationLeftBox.ToAxisAngle(axis,angle);
	alpha=Mathr::PI/2.0-angle;		// right if the initial orientation of the body (on the beginning of the simulation) is q =(1,0,0,0) = FromAxisAngle((0,0,1),0)
}

void KinemSimpleShearBox::computeScontact()
{
	Real Xleft = leftbox->state->pos.x() + (YADE_CAST<Box*>(leftbox->shape.get()))->extents.x();

	Real Xright = rightbox->state->pos.x() - (YADE_CAST<Box*>(rightbox->shape.get()))->extents.x();

	Real Zfront = frontbox->state->pos.z() - YADE_CAST<Box*>(frontbox->shape.get())->extents.z();
	Real Zback = backbox->state->pos.z() + (YADE_CAST<Box*>(backbox->shape.get()))->extents.z();

	Scontact = (Xright-Xleft)*(Zfront-Zback);	// that's so the value of section at the middle of the height of the box

}

void KinemSimpleShearBox::letMove(Real dX, Real dY)
{

	if(LOG)	cout << "It : " << scene->iter << endl;
// 	computeDu();

// 	const Real& dt = scene->dt; // dt value obtained by getBoxes_Dt

	Real Ysup = topbox->state->pos.y();
	Real Ylat = leftbox->state->pos.y();

// 	Changes in vertical and horizontal position :

	
	topbox->state->pos += Vector3r(dX,dY,0);

	leftbox->state->pos += Vector3r(dX/2.0,dY/2.0,0);
	rightbox->state->pos += Vector3r(dX/2.0,dY/2.0,0);
	if(LOG)	cout << "dY reellemt applique :" << dY << endl;
	if(LOG)	cout << "qui nous a emmene en : y = " <<(topbox->state->pos).y() << endl;
	
	Real Ysup_mod = topbox->state->pos.y();
	Real Ylat_mod = leftbox->state->pos.y();

//	with the corresponding velocities :
	topbox->state->vel = Vector3r(dX/dt,dY/dt,0);
	leftbox->state->vel = Vector3r(dX/(2.0 * dt),dY/(2.0 * dt),0);
	rightbox->state->vel = Vector3r(dX/(2.0 * dt),dY/(2.0*dt),0);

	computeAlpha();
//	Then computation of the angle of the rotation,dalpha, to be done :
	if (alpha == Mathr::PI/2.0)	// Case of the very beginning
	{
		dalpha = - atan( dX / (Ysup_mod -Ylat_mod) );
	}
	else
	{
		Real A = (Ysup_mod - Ylat_mod) * 2.0*tan(alpha) / (2.0*(Ysup - Ylat) + dX*tan(alpha) );
		dalpha = atan( (A - tan(alpha))/(1.0 + A * tan(alpha)));
	}

	Quaternionr qcorr(AngleAxisr(dalpha,Vector3r::UnitZ()));

// Rotation is applied : orientation and angular velocity of plates are modified.
	leftbox->state->ori	= qcorr*leftbox->state->ori;
	leftbox->state->angVel	= Vector3r(0,0,1)*dalpha/dt;

	rightbox->state->ori	= qcorr*rightbox->state->ori;
	rightbox->state->angVel	= Vector3r(0,0,1)*dalpha/dt;

}

void KinemSimpleShearBox::stopMovement()
{
	// upper plate's speed is zeroed
	topbox->state->vel		=  Vector3r(0,0,0);

	// same for left box
	leftbox->state->vel		=  Vector3r(0,0,0);
	leftbox->state->angVel		=  Vector3r(0,0,0);

	// and for rightbox
	rightbox->state->vel		=  Vector3r(0,0,0);
	rightbox->state->angVel		=  Vector3r(0,0,0);
}


void KinemSimpleShearBox::computeStiffness()
{
	int nbre_contacts = 0;
	stiffness=0.0;
	InteractionContainer::iterator ii    = scene->interactions->begin();
	InteractionContainer::iterator iiEnd = scene->interactions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal())
		{
			const shared_ptr<Interaction>& contact = *ii;
			
			Real fn = (static_cast<FrictPhys*>(contact->phys.get()))->normalForce.norm();

			if (fn!=0)
			{
				int id1 = contact->getId1(), id2 = contact->getId2();
				if ( id_topbox==id1 || id_topbox==id2 )
					{
						FrictPhys* currentContactPhysics =
						static_cast<FrictPhys*> ( contact->phys.get() );
						stiffness  += currentContactPhysics->kn;
						nbre_contacts += 1;
					}
			}
		}
	}
	if(LOG)	cout << "nbre billes en contacts : " << nbre_contacts << endl;
	if(LOG)	cout << "rigidite echantillon calculee : " << stiffness << endl;

}

void KinemSimpleShearBox::getBoxes_Dt()
{
	leftbox = Body::byId(id_boxleft);
	rightbox = Body::byId(id_boxright);
	frontbox = Body::byId(id_boxfront);
	backbox = Body::byId(id_boxback);
	topbox = Body::byId(id_topbox);
	boxbas = Body::byId(id_boxbas);
	dt = scene->dt;
}



void KinemSimpleShearBox::computeDY(Real KnC)
{

	scene->forces.sync(); Vector3r fSup=scene->forces.getForce(id_topbox);
	
	if(firstRun)
	{
// 		if ( !myLdc )	// FIXME : reenable this feature since Law2 is a functor ?? But difference in stiffnesses normally now taken into account
// 		{
// 			vector<shared_ptr<Engine> >::iterator itFirst = scene->engines.begin();
// 			vector<shared_ptr<Engine> >::iterator itLast = scene->engines.end();
// 			for ( ;itFirst!=itLast; ++itFirst )
// 			{
// 				if ( ( *itFirst )->getClassName() == "Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity" ) 
// 				{
// 					myLdc =  YADE_PTR_CAST<Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity> ( *itFirst );
// 					coeff_dech = myLdc ->coeff_dech;
// 					if(LOG) cout << "Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity engine found, with coeff_dech = " << coeff_dech << endl;
// 				}
// 			}
// 		}

		alpha=Mathr::PI/2.0;;
		y0 = topbox->state->pos.y();
// 		cout << "y0 initialise à : " << y0 << endl;
		f0 = fSup.y();
		firstRun=false;
	}
		

	computeStiffness();
	Real hCurrent = topbox->state->pos.y();
	computeScontact();
	Real fDesired = f0 + KnC * 1.0e9 * Scontact * (hCurrent-y0); // The value of the force desired, with the fact that KnC is in MPa/mm 
// 	cout << "Je veux atteindre a cet it fDesired = "<< fDesired << endl;
// 	cout << "Alors que f0 =  = "<< f0 << endl;
// 	cout << "Car terme correctif = " << KnC * 1.0e9 * Scontact * (hCurrent-y0)<< endl;

	if( stiffness==0 )
	{
		deltaH=0;
		cerr << "Stiffness(sample) = 0 => DNC in fact : not CNL or CNS..." << endl;
	}
	else
	{
		deltaH = ( fSup.y() -  fDesired )/(stiffness);
	}

	if(LOG) cout << "Alors q je veux KnC = " << KnC << " depuis f0 = " << f0 << " et y0 = " << y0 << endl;
	if(LOG) cout << "deltaH a permettre normalement :" << deltaH << endl;

	deltaH = (1-wallDamping)*deltaH;
	if(LOG)	cout << "deltaH apres amortissement :" << deltaH << endl;

	if(abs(deltaH) > max_vel*scene->dt)
	{
		deltaH=deltaH/abs(deltaH)*max_vel*scene->dt;
		if(LOG) cout << "Correction appliquee pour ne pas depasser vmax(comp)" << endl;
	}

}



