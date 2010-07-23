/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/



#include<yade/pkg-dem/KinemCNSEngine.hpp>


YADE_PLUGIN((KinemCNSEngine));


void KinemCNSEngine::action()
{
	if(LOG) cerr << "debut applyCondi !!" << endl;
	KinemSimpleShearBox::setBoxes_Dt();
	
	if(gamma<=gammalim)
	{
		computeDY(KnC);
		letMove(shearSpeed * dt,deltaH);
		gamma+=shearSpeed * dt;
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
		Omega::instance().saveSimulation(Key + "finCis" +lexical_cast<string> (Omega::instance().getCurrentIteration() ) + ".xml");
		Omega::instance().stopSimulationLoop();
	}

}

/*void KinemCNSEngine::letMove()
{
	shared_ptr<BodyContainer> bodies = scene->bodies;

	if(LOG) cout << "It : " << Omega::instance().getCurrentIteration() << endl;
	

	const Real& dt = scene->dt;
	Real dx = shearSpeed * dt;


	Real Ysup = topbox->state->pos.y();
	Real Ylat = leftbox->state->pos.y();

// 	Changes in vertical and horizontal position :

	topbox->state->pos += Vector3r(dx,deltaH,0);

	leftbox->state->pos += Vector3r(dx/2.0,deltaH/2.0,0);
	rightbox->state->pos += Vector3r(dx/2.0,deltaH/2.0,0);
	
	Real Ysup_mod = topbox->state->pos.y();
	Real Ylat_mod = leftbox->state->pos.y();

//	with the corresponding velocities :
	topbox->state->vel = Vector3r(shearSpeed,deltaH/dt,0);
	leftbox->state->vel = Vector3r(shearSpeed/2.0,deltaH/(2.0*dt),0);
	rightbox->state->vel = Vector3r(shearSpeed/2.0,deltaH/(2.0*dt),0);

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

	Quaternionr qcorr(AngleAxisr(dalpha,Vector3r::UnitZ()));
	if(LOG)
		cout << "Quaternion associe a la rotation incrementale : " << qcorr.w() << " " << qcorr.x() << " " << qcorr.y() << " " << qcorr.z() << endl;
// On applique la rotation en changeant l'orientation des plaques, leurs vang et en affectant donc alpha
	leftbox->state->ori	= qcorr*leftbox->state->ori;
	leftbox->state->angVel	= Vector3r(0,0,1)*dalpha/dt;

	rightbox->state->ori	= qcorr*rightbox->state->ori;
	rightbox->state->angVel	= Vector3r(0,0,1)*dalpha/dt;

	gamma+=dx;
}*/
	

// void KinemCNSEngine::computeDu()
// {
// 
// 	scene->forces.sync(); Vector3r F_sup=scene->forces.getForce(id_topbox);
// 	
// 	if(firstRun)
// 	{
// 		if ( !myLdc )	// FIXME : reenable this feature since Law2 is a functor
// 		{
// // 			vector<shared_ptr<Engine> >::iterator itFirst = scene->engines.begin();
// // 			vector<shared_ptr<Engine> >::iterator itLast = scene->engines.end();
// // 			for ( ;itFirst!=itLast; ++itFirst )
// // 			{
// // 				if ( ( *itFirst )->getClassName() == "Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity" ) 
// // 				{
// // 					myLdc =  YADE_PTR_CAST<Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity> ( *itFirst );
// // 					coeff_dech = myLdc ->coeff_dech;
// // 					if(LOG) cout << "Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity engine found, with coeff_dech = " << coeff_dech << endl;
// // 				}
// // 			}
// 		}
// 
// 		alpha=Mathr::PI/2.0;;
// 		Y0 = topbox->state->pos.y();
// 		cout << "Y0 initialise à : " << Y0 << endl;
// 		F_0 = F_sup.y();
// 		firstRun=false;
// 	}
// 		
// 
// 	computeStiffness();
// 	Real Hcurrent = topbox->state->pos.y();
// 	Real Fdesired = F_0 + KnC * 1.0e9 * Scontact * (Hcurrent-Y0); // The value of the force desired
// 
// // Prise en compte de la difference de rigidite entre charge et decharge dans le cadre de Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity : => INUTILE maintenant ?
// // 	if( F_sup.y() > Fdesired )	// cas ou l'on va monter la plaq <=> (normalemt) a une decharge
// // 		stiffness *= coeff_dech;
// 
// 	if( (KnC==0) && (stiffness==0) )
// 	{
// 		deltaH=0;
// 		cerr << "KnC et stiffness(sample) = 0 => DNC en fait et non CNC..." << endl;
// 	}
// 	else
// 	{
// 		deltaH = ( F_sup.y() - ( Fdesired ))/(stiffness+KnC* 1.0e9 * Scontact);
// 	}
// 
// 	if(LOG) cout << "Alors q je veux KnC = " << KnC << " depuis F_0 = " << F_0 << " et Y0 = " << Y0 << endl;
// 	if(LOG) cout << "deltaH a permettre normalement :" << deltaH << endl;
// 
// 	deltaH = (1-wallDamping)*deltaH;
// 	if(LOG)	cout << "deltaH apres amortissement :" << deltaH << endl;
// 
// 	if(abs(deltaH) > max_vel*scene->dt)
// 	{
// 		deltaH=deltaH/abs(deltaH)*max_vel*scene->dt;
// 		if(LOG) cout << "Correction appliquee pour ne pas depasser vmax(comp)" << endl;
// 	}
// 
// }





