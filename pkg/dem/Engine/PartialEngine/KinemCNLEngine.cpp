/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/pkg-dem/KinemCNLEngine.hpp>


YADE_PLUGIN((KinemCNLEngine));

void KinemCNLEngine::action()
{
	if(LOG)	cout << "debut applyCondi du CNCEngine !!" << endl;
	KinemSimpleShearBox::setBoxes_Dt();
	
	if(LOG)	cout << "gamma = " << lexical_cast<string>(gamma) << "  et gammalim = " << lexical_cast<string>(gammalim) << endl;
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
		it_stop=Omega::instance().getCurrentIteration();
		cout << "Shear stopped : gammaLim reached at it "<< it_stop << endl;
		temoin=2;
	}
	else if (temoin==2 && (Omega::instance().getCurrentIteration()==(it_stop+5000)) )
	{
		Omega::instance().saveSimulation(Key + "endShear" +lexical_cast<string> ( Omega::instance().getCurrentIteration() ) + ".xml");
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



