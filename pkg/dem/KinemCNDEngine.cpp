/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include<yade/pkg-dem/KinemCNDEngine.hpp>


YADE_PLUGIN((KinemCNDEngine));

void KinemCNDEngine::action()
{
	KinemSimpleShearBox::setBoxes_Dt();

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
			Omega::instance().saveSimulation(Key+"_"+lexical_cast<string> (floor(gamma*1000)) + "mmsheared.xml");
			temoin_save[j]=1;
		}
	}
	
}







