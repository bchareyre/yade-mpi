/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/



#include<yade/pkg/dem/KinemCNSEngine.hpp>


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
		it_stop=scene->iter;
		cout << "Cisaillement arrete : gammaLim atteint a l'iteration "<< it_stop << endl;
		temoin=2;
	}
	else if (temoin==2 && (scene->iter==(it_stop+5000)) )
	{
		Omega::instance().saveSimulation(Key + "finCis" +lexical_cast<string> (scene->iter ) + ".xml");
		Omega::instance().pause();
	}

}






