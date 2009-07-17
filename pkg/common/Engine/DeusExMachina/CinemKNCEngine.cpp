/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include "CinemKNCEngine.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include <yade/lib-miniWm3/Wm3Math.h>


CinemKNCEngine::CinemKNCEngine() : leftbox(new Body), rightbox(new Body), frontbox(new Body), backbox(new Body), topbox(new Body), boxbas(new Body)
{
	prevF_sup=Vector3r(0,0,0);
	firstRun=true;
	shearSpeed=0;
	alpha=Mathr::PI/2.0;;
	gamma=0;
	gammalim=0;
	id_boxhaut=3;
	id_boxbas=1;
	id_boxleft=0;
	id_boxright=2;
	id_boxfront=5;
	id_boxback=4;
	Y0=0;
	F_0=0;
	KnC=10.0e6;
	Key="";
}


void CinemKNCEngine::applyCondition(Body * body)
{
	if(LOG) cerr << "debut applyCondi !!" << endl;
	leftbox = Body::byId(id_boxleft);
	rightbox = Body::byId(id_boxright);
	frontbox = Body::byId(id_boxfront);
	backbox = Body::byId(id_boxback);
	topbox = Body::byId(id_boxhaut);
	boxbas = Body::byId(id_boxbas);
	
// 	shared_ptr<BodyContainer> bodies = ncb->bodies;

	if(gamma<=gammalim)
	{
		letMove(body);
		if(temoin==0)
		{
			temoin=1;
		}
	}
	else if (temoin<2)
	{
		stopMovement();		// INDISPENSABLE !
		it_stop=Omega::instance().getCurrentIteration();
		cout << "Cisaillement arrete : gammaLim atteint a l'iteration "<< it_stop << endl;
		temoin=2;
	}
	else if (temoin==2 && (Omega::instance().getCurrentIteration()==(it_stop+5000)) )
	{
		Omega::instance().saveSimulation(Key + "finCis" +lexical_cast<string> ( Omega::instance().getCurrentIteration() ) + ".xml");
		Omega::instance().stopSimulationLoop();
	}

}

void CinemKNCEngine::letMove(Body * body)
{
	MetaBody * ncb = YADE_CAST<MetaBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	if(LOG) cout << "It : " << Omega::instance().getCurrentIteration() << endl;
	computeDu(ncb);

	Real dt = Omega::instance().getTimeStep();
	Real dx = shearSpeed * dt;


	Real Ysup = (topbox->physicalParameters.get())->se3.position.Y();
	Real Ylat = (leftbox->physicalParameters.get())->se3.position.Y();

// 	Changes in vertical and horizontal position :

	(topbox->physicalParameters.get())->se3.position += Vector3r(dx,deltaH,0);

	(leftbox->physicalParameters.get())->se3.position += Vector3r(dx/2.0,deltaH/2.0,0);
	(rightbox->physicalParameters.get())->se3.position += Vector3r(dx/2.0,deltaH/2.0,0);
	
	Real Ysup_mod = (topbox->physicalParameters.get())->se3.position.Y();
	Real Ylat_mod = (leftbox->physicalParameters.get())->se3.position.Y();

//	with the corresponding velocities :
	RigidBodyParameters * rb = dynamic_cast<RigidBodyParameters*>(topbox->physicalParameters.get());
	rb->velocity = Vector3r(shearSpeed,deltaH/dt,0);

	rb = dynamic_cast<RigidBodyParameters*>(leftbox->physicalParameters.get());
	rb->velocity = Vector3r(shearSpeed/2.0,deltaH/(2.0*dt),0);

	rb = dynamic_cast<RigidBodyParameters*>(rightbox->physicalParameters.get());
	rb->velocity = Vector3r(shearSpeed/2.0,deltaH/(2.0*dt),0);

//	Then computation of the angle of the rotation to be done :
	if (alpha == Mathr::PI/2.0)	// Case of the very beginning
	{
		dalpha = - Mathr::ATan( dx / (Ysup_mod -Ylat_mod) );
	}
	else
	{
		Real A = (Ysup_mod - Ylat_mod) * 2.0*Mathr::Tan(alpha) / (2.0*(Ysup - Ylat) + dx*Mathr::Tan(alpha) );
		dalpha = Mathr::ATan( (A - Mathr::Tan(alpha))/(1.0 + A * Mathr::Tan(alpha)));
	}

	Quaternionr qcorr;
	qcorr.FromAxisAngle(Vector3r(0,0,1),dalpha);
	if(LOG)
		cout << "Quaternion associe a la rotation incrementale : " << qcorr.W() << " " << qcorr.X() << " " << qcorr.Y() << " " << qcorr.Z() << endl;
// On applique la rotation en changeant l'orientation des plaques, leurs vang et en affectant donc alpha
	rb = dynamic_cast<RigidBodyParameters*>(leftbox->physicalParameters.get());
	rb->se3.orientation	= qcorr*rb->se3.orientation;
	if(LOG)
		cout << "Quaternion d'orientation plaq gauche apres correc : "<<rb->se3.orientation.W() << " " << rb->se3.orientation.X() << " " << rb->se3.orientation.Y() << " " << rb->se3.orientation.Z() << endl;
	rb->angularVelocity	= Vector3r(0,0,1)*dalpha/dt;

	rb = dynamic_cast<RigidBodyParameters*>(rightbox->physicalParameters.get());
	rb->se3.orientation	= qcorr*rb->se3.orientation;
	rb->angularVelocity	= Vector3r(0,0,1)*dalpha/dt;

	gamma+=dx;
}
	
void CinemKNCEngine::computeAlpha()
{
	Quaternionr orientationLeftBox,orientationRightBox;
	orientationLeftBox = (dynamic_cast<RigidBodyParameters*>(leftbox->physicalParameters.get()) )->se3.orientation;
	orientationRightBox = (dynamic_cast<RigidBodyParameters*>(rightbox->physicalParameters.get()) )->se3.orientation;
	if(orientationLeftBox!=orientationRightBox)
	{
		cout << "WARNING !!! your lateral boxes have not the same orientation, you're not in the case of a box imagined for creating these engines" << endl;
	}
	Vector3r axis;
	Real angle;
	orientationLeftBox.ToAxisAngle(axis,angle);
	alpha=Mathr::PI/2.0-angle;		// right if the initial orientation of the body (on the beginning of the simulation) is q =(1,0,0,0) = FromAxisAngle((0,0,1),0)
}


void CinemKNCEngine::computeDu(MetaBody* ncb)
{

	ncb->bex.sync(); Vector3r F_sup=ncb->bex.getForce(id_boxhaut);
	
	if(firstRun)
	{
		if ( !myLdc )
		{
			vector<shared_ptr<Engine> >::iterator itFirst = ncb->engines.begin();
			vector<shared_ptr<Engine> >::iterator itLast = ncb->engines.end();
			for ( ;itFirst!=itLast; ++itFirst )
			{
				if ( ( *itFirst )->getClassName() == "ContactLaw1" ) 
				{
					myLdc =  YADE_PTR_CAST<ContactLaw1> ( *itFirst );
					coeff_dech = myLdc ->coeff_dech;
					if(LOG) cout << "My ContactLaw engine found, de coeff_dech = " << coeff_dech << endl;
				}
			}
		}

		alpha=Mathr::PI/2.0;;
		Y0 = (topbox->physicalParameters.get())->se3.position.Y();
		cout << "Y0 initialise à : " << Y0 << endl;
		F_0 = F_sup.Y();
		prevF_sup=F_sup;
		firstRun=false;
	}
		
// Computation of the current dimensions of the box : //
	Real Xleft = (leftbox->physicalParameters.get())->se3.position.X() + (YADE_CAST<InteractingBox*>(leftbox->interactingGeometry.get()))->extents.X();
	Real Xright = (rightbox->physicalParameters.get())->se3.position.X() - (YADE_CAST<InteractingBox*>(rightbox->interactingGeometry.get()))->extents.X();

	Real Zfront = (frontbox->physicalParameters.get())->se3.position.Z() - YADE_CAST<InteractingBox*>(frontbox->interactingGeometry.get())->extents.Z();
	Real Zback = (backbox->physicalParameters.get())->se3.position.Z() + (YADE_CAST<InteractingBox*>(backbox->interactingGeometry.get()))->extents.Z();

	Real Scontact = (Xright-Xleft)*(Zfront-Zback);	// that's so the value of section at the middle of the height of the box
// End of computation of the current dimensions of the box //

	computeStiffness(ncb);
	Real Hcurrent = (topbox->physicalParameters.get())->se3.position.Y();
	Real Fdesired = F_0 + KnC * 1.0e9 * Scontact * (Hcurrent-Y0); // The value of the force desired

// Prise en compte de la difference de rigidite entre charge et decharge dans le cadre de ContactLaw1 :
	if( F_sup.Y() > Fdesired )	// cas ou l'on va monter la plaq <=> (normalemt) a une decharge
		stiffness *= coeff_dech;

	if( (KnC==0) && (stiffness==0) )
	{
		deltaH=0;
		cerr << "KnC et stiffness(sample) = 0 => DNC en fait et non CNC..." << endl;
	}
	else
	{
		deltaH = ( F_sup.Y() - ( Fdesired ))/(stiffness+KnC* 1.0e9 * Scontact);
	}

	if(LOG) cout << "PrevF_sup : " << prevF_sup << "	F sup : " << F_sup.Y() << endl;
	if(LOG) cout << "Alors q je veux KnC = " << KnC << " depuis F_0 = " << F_0 << " et Y0 = " << Y0 << endl;
	if(LOG) cout << "deltaH a permettre normalement :" << deltaH << endl;

	deltaH = (1-wallDamping)*deltaH;
	if(LOG)	cout << "deltaH apres amortissement :" << deltaH << endl;

	if(abs(deltaH) > max_vel*Omega::instance().getTimeStep())
	{
		deltaH=deltaH/abs(deltaH)*max_vel*Omega::instance().getTimeStep();
		if(LOG) cout << "Correction appliquee pour ne pas depasser vmax(comp)" << endl;
	}

	prevF_sup=F_sup;	// Now the value of prevF_sup is used for computing deltaH, it is actualized
}

void CinemKNCEngine::stopMovement()
{
	// annulation de la vitesse de la plaque du haut
	RigidBodyParameters * rb = YADE_CAST<RigidBodyParameters*>(topbox->physicalParameters.get());
	rb->velocity		=  Vector3r(0,0,0);

	// de la plaque gauche
	rb = YADE_CAST<RigidBodyParameters*>(leftbox->physicalParameters.get());
	rb->velocity		=  Vector3r(0,0,0);
	rb->angularVelocity	=  Vector3r(0,0,0);

	// de la plaque droite
	rb = YADE_CAST<RigidBodyParameters*>(rightbox->physicalParameters.get());
	rb->velocity		=  Vector3r(0,0,0);
	rb->angularVelocity	=  Vector3r(0,0,0);
}

void CinemKNCEngine::computeStiffness(MetaBody* ncb)
{
	stiffness=0.0;
	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal())
		{
			const shared_ptr<Interaction>& contact = *ii;
			
			Real fn = (static_cast<ElasticContactInteraction*>	(contact->interactionPhysics.get()))->normalForce.Length();

			if (fn!=0)
			{
				int id1 = contact->getId1(), id2 = contact->getId2();
				if ( id_boxhaut==id1 || id_boxhaut==id2 )
					{
						ElasticContactInteraction* currentContactPhysics =
						static_cast<ElasticContactInteraction*> ( contact->interactionPhysics.get() );
						stiffness  += currentContactPhysics->kn;
					}
			}
		}
	}
	if(LOG)	cout << "rigidite echantillon calculee : " << stiffness << endl;

}


