/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
// YADE_REQUIRE_FEATURE(physpar)

#include "CinemCNCEngine.hpp"
// #include<yade/pkg-common/RigidBodyParameters.hpp> , remplace par : 
#include<yade/core/State.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-dem/FrictPhys.hpp>
#include<yade/core/Scene.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include <yade/lib-miniWm3/Wm3Math.h>


CinemCNCEngine::CinemCNCEngine() : leftbox(new Body), rightbox(new Body), frontbox(new Body), backbox(new Body), topbox(new Body), boxbas(new Body)
{
	prevF_sup=Vector3r(0,0,0);
	firstRun=true;
	shearSpeed=0;
	alpha=Mathr::PI/2.0;;
	gamma_save.resize(0);
	temoin_save.resize(0);
	temoin=0;
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
	LOG=0;
	wallDamping = 0.2;
	coeff_dech=1.0;
}


void CinemCNCEngine::applyCondition(Scene * ncb)
{
	if(LOG)	cout << "debut applyCondi du CNCEngine !!" << endl;
	leftbox = Body::byId(id_boxleft);
	rightbox = Body::byId(id_boxright);
	frontbox = Body::byId(id_boxfront);
	backbox = Body::byId(id_boxback);
	topbox = Body::byId(id_boxhaut);
	boxbas = Body::byId(id_boxbas);
	
	if(LOG)	cout << "gamma = " << lexical_cast<string>(gamma) << "  et gammalim = " << lexical_cast<string>(gammalim) << endl;
	if(gamma<=gammalim)
	{
		if(LOG)	cout << "Je suis bien dans la partie gamma < gammalim" << endl;
		if(temoin==0)

		{
			if(LOG) cout << "Je veux maintenir la Force a F_0 = : " << F_0 << endl; 
			temoin=1;
		}
		letMove(ncb);

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

	for(unsigned int j=0;j<gamma_save.size();j++)
	{
		if ((gamma > gamma_save[j]) && (temoin_save[j]==0))
		{
			stopMovement();		// reset of all the speeds before the save
			Omega::instance().saveSimulation(Key+"_"+lexical_cast<string> (floor(gamma*1000)) +"_" +lexical_cast<string> (floor(gamma*10000)-10*floor(gamma*1000))+ "mmsheared.xml");
			temoin_save[j]=1;
		}
	}

}

void CinemCNCEngine::letMove(Scene * ncb)
{
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	if(LOG)	cout << "It : " << Omega::instance().getCurrentIteration() << endl;
	computeDu(ncb);

	Real dt = Omega::instance().getTimeStep();
	Real dx = shearSpeed * dt;


	Real Ysup = topbox->state->pos.Y();
	Real Ylat = leftbox->state->pos.Y();

// 	Changes in vertical and horizontal position :

	
	topbox->state->pos += Vector3r(dx,deltaU,0);

	leftbox->state->pos += Vector3r(dx/2.0,deltaU/2.0,0);
	rightbox->state->pos += Vector3r(dx/2.0,deltaU/2.0,0);
	if(LOG)	cout << "deltaU reellemt applique :" << deltaU << endl;
	if(LOG)	cout << "qui nous a emmene en : y = " <<(topbox->state->pos).Y() << endl;
	if(LOG)	cout << "soit un decalage par rapport a position intiale : " << (topbox->state->pos.Y()) - Y0 << endl;
	
	Real Ysup_mod = topbox->state->pos.Y();
	Real Ylat_mod = leftbox->state->pos.Y();

//	with the corresponding velocities :
	topbox->state->vel = Vector3r(shearSpeed,deltaU/dt,0);
	leftbox->state->vel = Vector3r(shearSpeed/2.0,deltaU/(2.0*dt),0);
	rightbox->state->vel = Vector3r(shearSpeed/2.0,deltaU/(2.0*dt),0);

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
	leftbox->state->ori	= qcorr*leftbox->state->ori;
	leftbox->state->angVel	= Vector3r(0,0,1)*dalpha/dt;

	rightbox->state->ori	= qcorr*rightbox->state->ori;
	rightbox->state->angVel	= Vector3r(0,0,1)*dalpha/dt;

	gamma+=dx;
}
	

void CinemCNCEngine::computeAlpha()
{
	Quaternionr orientationLeftBox,orientationRightBox;
	orientationLeftBox = leftbox->state->ori;
	orientationRightBox = rightbox->state->ori;
	if(orientationLeftBox!=orientationRightBox)
	{
		cout << "WARNING !!! your lateral boxes have not the same orientation, you're not in the case of a box imagined for creating these engines" << endl;
	}
	Vector3r axis;
	Real angle;
	orientationLeftBox.ToAxisAngle(axis,angle);
	alpha=Mathr::PI/2.0-angle;		// right if the initial orientation of the body (on the beginning of the simulation) is q =(1,0,0,0) = FromAxisAngle((0,0,1),0)
}


void CinemCNCEngine::computeDu(Scene* ncb)
{
	ncb->forces.sync(); Vector3r F_sup=ncb->forces.getForce(id_boxhaut);
	
	if(firstRun)
	{
		if ( !myLdc )
		{
			vector<shared_ptr<Engine> >::iterator itFirst = ncb->engines.begin();
			vector<shared_ptr<Engine> >::iterator itLast = ncb->engines.end();
			for ( ;itFirst!=itLast; ++itFirst )
			{
				if ( ( *itFirst )->getClassName() == "RockJointLaw" ) 
				{
					if(LOG) cout << "RockJointLaw engine found" << endl;
					myLdc =  YADE_PTR_CAST<RockJointLaw> ( *itFirst );
					coeff_dech = myLdc ->coeff_dech;
				}
			}
		}
		
		it_depart = Omega::instance().getCurrentIteration();
		alpha=Mathr::PI/2.0;;
		Y0 = topbox->state->pos.Y();
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
		Real Ycourant = topbox->state->pos.Y();
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
	topbox->state->vel	=  Vector3r(0,0,0);

	// de la plaque gauche
	leftbox->state->vel	=  Vector3r(0,0,0);
	leftbox->state->angVel	=  Vector3r(0,0,0);

	// de la plaque droite
	rightbox->state->vel	=  Vector3r(0,0,0);
	rightbox->state->angVel	=  Vector3r(0,0,0);
}

void CinemCNCEngine::computeStiffness(Scene* ncb)
{
	int nbre_contacts = 0;
	stiffness=0.0;
	InteractionContainer::iterator ii    = ncb->interactions->begin();
	InteractionContainer::iterator iiEnd = ncb->interactions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal())
		{
			const shared_ptr<Interaction>& contact = *ii;
			
			Real fn = (static_cast<FrictPhys*>	(contact->interactionPhysics.get()))->normalForce.Length();

			if (fn!=0)
			{
				int id1 = contact->getId1(), id2 = contact->getId2();
				if ( id_boxhaut==id1 || id_boxhaut==id2 )
					{
						FrictPhys* currentContactPhysics =
						static_cast<FrictPhys*> ( contact->interactionPhysics.get() );
						stiffness  += currentContactPhysics->kn;
						nbre_contacts += 1;
					}
			}
		}
	}
	if(LOG)	cout << "nbre billes en contacts : " << nbre_contacts << endl;
	if(LOG)	cout << "rigidite echantillon calculee : " << stiffness << endl;

}




