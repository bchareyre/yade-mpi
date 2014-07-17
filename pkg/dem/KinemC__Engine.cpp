/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/pkg/dem/KinemC__Engine.hpp>

YADE_PLUGIN((KinemCTDEngine)(KinemCNDEngine)(KinemCNLEngine)(KinemCNSEngine));

void KinemCTDEngine::action()
{
	KinemSimpleShearBox::getBoxes_Dt();


	scene->forces.sync();
	Real current_NormalForce=(scene->forces.getForce(id_topbox)).y();
	KinemSimpleShearBox::computeScontact();
	current_sigma=current_NormalForce/(1000.0*Scontact);	// so we have the current value of sigma, in kPa

	if( ((compSpeed > 0) && (current_sigma < targetSigma)) || ((compSpeed < 0) && (current_sigma > targetSigma)) )
	{
		if(temoin!=0)
		{
// 			cout << "j'ai ici un temoin #0 visiblement. En effet temoin =" <<boost::lexical_cast<string>(temoin) << endl;
			temoin=0;
// 			cout << "Maintenant (toujours dans le if temoin!=0), temoin =" <<boost::lexical_cast<string>(temoin) << endl;
		}
		
		letMove(0.0,-compSpeed*dt);
	}
	else if (temoin==0)
	{
		stopMovement();
// 		cout << "Mouvement stoppe, temoin = " << boost::lexical_cast<string>(temoin) << endl;
// 		cout << " Dans le if, temoin =" << boost::lexical_cast<string>(temoin) << endl;
		string f;
		if (compSpeed > 0)
			f="Sigmax_";
		else
			f="Sigmin_";

		Omega::instance().saveSimulation(Key + f +boost::lexical_cast<string> (floor(targetSigma)) + "kPaReached.xml");
		temoin=1;
// 		cout << " Fin du if, temoin =" << boost::lexical_cast<string>(temoin) << endl << endl;
	}

	
	for(unsigned int j=0;j<sigma_save.size();j++)
	{
		if( (  ( (compSpeed>0)&&(current_sigma > sigma_save[j]) ) || ((compSpeed<0)&&(current_sigma < sigma_save[j])) ) && (temoin_save[j]==0))
		{
			stopMovement();
			Omega::instance().saveSimulation(Key + "SigInt_" +boost::lexical_cast<string> (floor(current_sigma)) + "kPareached.xml");
			temoin_save[j]=1;
		}
	}
// 	cout << "Fin de ApplyCondi, temoin = " << boost::lexical_cast<string>(temoin) << endl;

}

void KinemCNDEngine::action()
{
	KinemSimpleShearBox::getBoxes_Dt();

	if( ((shearSpeed > 0) && (gamma<=gammalim)) || ((shearSpeed < 0) /*&& (gamma>=gammalim)*/ ) )
	{
		if(temoinfin!=0)
			temoinfin=0;
		letMove(shearSpeed * dt,0);
		gamma+=shearSpeed * dt;
	}
	else
	{
		stopMovement();
		if(temoinfin==0)
		{
			Omega::instance().saveSimulation(Key + "endShear.xml");
			temoinfin=1;
		}
	}

	for(unsigned int j=0;j<gamma_save.size();j++)
	{
		if ( ( ( (shearSpeed>0)&&(gamma > gamma_save[j]) ) || ((shearSpeed<0)&&(gamma < gamma_save[j])) ) && (temoin_save[j]==0) )
		{
			stopMovement();		// reset of all the speeds before the save
			Omega::instance().saveSimulation(Key+"_"+boost::lexical_cast<string> (floor(gamma*1000)) + "mmsheared.xml");
			temoin_save[j]=1;
		}
	}
	
}

void KinemCNLEngine::action()
{
	if(LOG)	cout << "debut applyCondi du CNCEngine !!" << endl;
	KinemSimpleShearBox::getBoxes_Dt();
	
	if(LOG)	cout << "gamma = " << boost::lexical_cast<string>(gamma) << "  et gammalim = " << boost::lexical_cast<string>(gammalim) << endl;
	if(gamma<=gammalim)
	{
		if(LOG)	cout << "Je suis bien dans la partie gamma < gammalim" << endl;
		if(temoin==0)

		{
			if(LOG) cout << "Je veux maintenir la Force a f0 = : " << f0 << endl; 
			temoin=1;
		}
		computeDY(0.0);
		
		letMove(shearSpeed*dt,deltaH);
		gamma+=shearSpeed * dt;

	}
	else if (temoin<2)
	{
		stopMovement();		// INDISPENSABLE !
		it_stop=scene->iter;
		cout << "Shear stopped : gammaLim reached at it "<< it_stop << endl;
		temoin=2;
	}
	else if (temoin==2 && (scene->iter==(it_stop+5000)) )
	{
		Omega::instance().saveSimulation(Key + "endShear" +boost::lexical_cast<string> ( scene->iter ) + ".xml");
		Omega::instance().pause();
	}

	for(unsigned int j=0;j<gamma_save.size();j++)
	{
		if ((gamma > gamma_save[j]) && (temoin_save[j]==0))
		{
			stopMovement();		// reset of all the speeds before the save
			Omega::instance().saveSimulation(Key+"_"+boost::lexical_cast<string> (floor(gamma*1000)) +"_" +boost::lexical_cast<string> (floor(gamma*10000)-10*floor(gamma*1000))+ "mmsheared.xml");
			temoin_save[j]=1;
		}
	}
}

void KinemCNSEngine::action()
{
	if(LOG) cerr << "debut applyCondi !!" << endl;
	KinemSimpleShearBox::getBoxes_Dt();
	
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
		it_stop=scene->iter;
		cout << "Cisaillement arrete : gammaLim atteint a l'iteration "<< it_stop << endl;
		temoin=2;
	}
	else if (temoin==2 && (scene->iter==(it_stop+5000)) )
	{
		Omega::instance().saveSimulation(Key + "finCis" +boost::lexical_cast<string> (scene->iter ) + ".xml");
		Omega::instance().pause();
	}
}
