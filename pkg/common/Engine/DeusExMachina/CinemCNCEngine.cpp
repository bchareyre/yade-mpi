/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include "CinemCNCEngine.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include <yade/lib-miniWm3/Wm3Math.h>


CinemCNCEngine::CinemCNCEngine() : leftbox(new Body), rightbox(new Body), frontbox(new Body), backbox(new Body), topbox(new Body), boxbas(new Body)
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
	Key="";
	it_depart=0;
	k=1;
	LOG=0;
	wallDamping = 0.2;
	coeff_dech=1.0;
}


void CinemCNCEngine::applyCondition(Body * body)
{
	if(LOG)	cout << "debut applyCondi !!" << endl;
	leftbox = Body::byId(id_boxleft);
	rightbox = Body::byId(id_boxright);
	frontbox = Body::byId(id_boxfront);
	backbox = Body::byId(id_boxback);
	topbox = Body::byId(id_boxhaut);
	boxbas = Body::byId(id_boxbas);
	
	
	if(gamma<=gammalim)
	{
		if(temoin==0 || ( (Omega::instance().getCurrentIteration() - it_depart) % k ==0) )

		{
			if(LOG) cout << "Je veux maintenir la Force a F_0 = : " << F_0 << endl; 
			letMove(body);
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

void CinemCNCEngine::letMove(Body * body)
{
	MetaBody * ncb = YADE_CAST<MetaBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	if(LOG)	cout << "It : " << Omega::instance().getCurrentIteration() << endl;
	computeDu(ncb);

	Real dt = Omega::instance().getTimeStep();
	Real dx = shearSpeed * dt;


	Real Ysup = (topbox->physicalParameters.get())->se3.position.Y();
	Real Ylat = (leftbox->physicalParameters.get())->se3.position.Y();

// 	Changes in vertical and horizontal position :

	
	(topbox->physicalParameters.get())->se3.position += Vector3r(dx,deltaU,0);

	(leftbox->physicalParameters.get())->se3.position += Vector3r(dx/2.0,deltaU/2.0,0);
	(rightbox->physicalParameters.get())->se3.position += Vector3r(dx/2.0,deltaU/2.0,0);
	if(LOG)	cout << "deltaU reellemt applique :" << deltaU << endl;
	if(LOG)	cout << "qui nous a emmene en : y = " << ((topbox->physicalParameters.get())->se3.position).Y() << endl;
	if(LOG)	cout << "soit un decalage par rapport a position intiale : " << ((topbox->physicalParameters.get())->se3.position.Y()) - Y0 << endl;
	
	Real Ysup_mod = (topbox->physicalParameters.get())->se3.position.Y();
	Real Ylat_mod = (leftbox->physicalParameters.get())->se3.position.Y();

//	with the corresponding velocities :
	RigidBodyParameters * rb = dynamic_cast<RigidBodyParameters*>(topbox->physicalParameters.get());
	rb->velocity = Vector3r(shearSpeed,deltaU/dt,0);

	rb = dynamic_cast<RigidBodyParameters*>(leftbox->physicalParameters.get());
	rb->velocity = Vector3r(shearSpeed/2.0,deltaU/(2.0*dt),0);

	rb = dynamic_cast<RigidBodyParameters*>(rightbox->physicalParameters.get());
	rb->velocity = Vector3r(shearSpeed/2.0,deltaU/(2.0*dt),0);

//	Then computation of the angle of the rotation to be done :
	computeAlpha();
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

// On applique la rotation en changeant l'orientation des plaques, leurs vang et en affectant donc alpha
	rb = dynamic_cast<RigidBodyParameters*>(leftbox->physicalParameters.get());
	rb->se3.orientation	= qcorr*rb->se3.orientation;
	rb->angularVelocity	= Vector3r(0,0,1)*dalpha/dt;

	rb = dynamic_cast<RigidBodyParameters*>(rightbox->physicalParameters.get());
	rb->se3.orientation	= qcorr*rb->se3.orientation;
	rb->angularVelocity	= Vector3r(0,0,1)*dalpha/dt;

	gamma+=dx;
}
	

void CinemCNCEngine::computeAlpha()
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


void CinemCNCEngine::computeDu(MetaBody* ncb)
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
					if(LOG) cout << "ContactLaw1 engine found" << endl;
					myLdc =  YADE_PTR_CAST<ContactLaw1> ( *itFirst );
					coeff_dech = myLdc ->coeff_dech;
				}
			}
		}
		
		it_depart = Omega::instance().getCurrentIteration();
		alpha=Mathr::PI/2.0;;
		Y0 = (topbox->physicalParameters.get())->se3.position.Y();
		cout << "Y0 initialise à : " << Y0 << endl;
		F_0 = F_sup.Y();
		cout << "F_0 initialise à : " << F_0 << endl;
		prevF_sup=F_sup;
		previousdeltaU=0.0;
		firstRun=false;
	}
	
	computeStiffness(ncb);
	if( (stiffness==0) )
	{
		deltaU=0;
		cerr << "stiffness(sample) = 0 => DNC en fait <=> à CNC..." << endl;
	}
	else
	{
		Real Ycourant = (topbox->physicalParameters.get())->se3.position.Y();
		deltaU = ( F_sup.Y() - F_0 )/(stiffness);
		if(LOG) cout << "Lors du calcul de DU (utile pour deltaU) : F_0 = " << F_0 << "; Y0 = " << Y0 << "; Ycourant = " << Ycourant << endl;
	}


	if(LOG)	cout << "PrevF_sup : " << prevF_sup << "	F sup : " << F_sup.Y() << endl;
	if(LOG)	cout << "deltaU a permettre normalemt :" << deltaU << endl;

// 	Il va falloir prendre en compte la loi de contact qui induit une rigidite plus grande en decharge qu'en charge
	if(deltaU>0)
	{
		deltaU/=coeff_dech;
		if(LOG) cout << "Comme deltaU>0 => decharge, il est divise par " << coeff_dech << endl;
	}

	deltaU = (1-wallDamping)*deltaU;
	if(LOG)	cout << "deltaU apres amortissement :" << deltaU << endl;
// 	deltaU += 0.7*previousdeltaU;
	if(LOG)	cout << "deltaU apres correction avec previousdeltaU :" << deltaU << endl;
	if(abs(deltaU) > max_vel*Omega::instance().getTimeStep())
	{
		if(LOG)	cout << "v induite pour cet it n° " <<Omega::instance().getCurrentIteration()<<" : " << deltaU/Omega::instance().getTimeStep() << endl;
		deltaU=deltaU/abs(deltaU)*max_vel*Omega::instance().getTimeStep();
		
		if(LOG)	cout << "Correction appliquee pour ne pas depasser vmax(comp) = " << max_vel << endl;
	}

	previousdeltaU=deltaU;
	prevF_sup=F_sup;	// Now the value of prevF_sup is used for computing deltaU, it is actualized
}

void CinemCNCEngine::stopMovement()
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

void CinemCNCEngine::computeStiffness(MetaBody* ncb)
{
	int nbre_contacts = 0;
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
						nbre_contacts += 1;
					}
			}
		}
	}
	if(LOG)	cout << "nbre billes en contacts : " << nbre_contacts << endl;
	if(LOG)	cout << "rigidite echantillon calculee : " << stiffness << endl;

}



YADE_REQUIRE_FEATURE(PHYSPAR);

