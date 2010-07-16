/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/pkg-dem/KinemCTDEngine.hpp>
#include<yade/core/State.hpp>
#include<yade/pkg-common/Box.hpp>
#include <yade/core/Scene.hpp>


YADE_PLUGIN((KinemCTDEngine));

KinemCTDEngine::~KinemCTDEngine()
{
}

void KinemCTDEngine::action()
{
	KinemSimpleShearBox::setBoxes_Dt();


	scene->forces.sync();
	Real current_NormalForce=(scene->forces.getForce(id_topbox)).y();
	KinemSimpleShearBox::computeScontact();
	current_sigma=current_NormalForce/(1000.0*Scontact);	// so we have the current value of sigma, in kPa

	if( ((compSpeed > 0) && (current_sigma < targetSigma)) || ((compSpeed < 0) && (current_sigma > targetSigma)) )
	{
		if(temoin!=0)
		{
// 			cout << "j'ai ici un temoin #0 visiblement. En effet temoin =" <<lexical_cast<string>(temoin) << endl;
			temoin=0;
// 			cout << "Maintenant (toujours dans le if temoin!=0), temoin =" <<lexical_cast<string>(temoin) << endl;
		}
		
		letMove(-compSpeed*dt,0.0);
	}
	else if (temoin==0)
	{
		stopMovement();
// 		cout << "Mouvement stoppe, temoin = " << lexical_cast<string>(temoin) << endl;
// 		cout << " Dans le if, temoin =" << lexical_cast<string>(temoin) << endl;
		string f;
		if (compSpeed > 0)
			f="Sigmax_";
		else
			f="Sigmin_";

		Omega::instance().saveSimulation(Key + f +lexical_cast<string> (floor(targetSigma)) + "kPaReached.xml");
		temoin=1;
// 		cout << " Fin du if, temoin =" << lexical_cast<string>(temoin) << endl << endl;
	}

	
	for(unsigned int j=0;j<sigma_save.size();j++)
	{
		if( (  ( (compSpeed>0)&&(current_sigma > sigma_save[j]) ) || ((compSpeed<0)&&(current_sigma < sigma_save[j])) ) && (temoin_save[j]==0))
		{
			stopMovement();
			Omega::instance().saveSimulation(Key + "SigInt_" +lexical_cast<string> (floor(current_sigma)) + "kPareached.xml");
			temoin_save[j]=1;
		}
	}
// 	cout << "Fin de ApplyCondi, temoin = " << lexical_cast<string>(temoin) << endl;

}


// void KinemCTDEngine::computeAlpha()
// {
// 	Quaternionr orientationLeftBox,orientationRightBox;
// 	orientationLeftBox = leftbox->state->ori;
// 	orientationRightBox = rightbox->state->ori;
// 	if(orientationLeftBox!=orientationRightBox)
// 	{
// 		cout << "WARNING !!! your lateral boxes have not the same orientation, you're not in the case of a box imagined for creating these engines" << endl;
// 	}
// 	AngleAxisr aa(orientationLeftBox);
// 	alpha=Mathr::PI/2.0-aa.angle();		// right if the initial orientation of the body (on the beginning of the simulation) is q =(1,0,0,0) = FromAxisAngle((0,0,1),0)
// }





