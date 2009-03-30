/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Omega.hpp>
#include<yade/core/DeusExMachina.hpp>
#include<yade/core/Body.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/pkg-dem/ContactLaw1.hpp>

/*! \brief To apply a constant normal rigidity shear for a parallelogram box (simple shear)

This engine, used in simulations issued from "DirectShearCis" Preprocessor, allows to translate horizontally the upper plate while the lateral ones rotate so that they always keep contact with the lower and upper walls
In fact the upper plate can move not only horizontally but also vertically, so that the normal rigidity defined by DeltaF(upper plate)/DeltaU(upper plate) = constant (= KnC defined by the user)
The movement is moreover controlled by the user via a "shearSpeed" which will be the speed of the upper wall, and by a maximum value of horizontal displacement "gammalim", after which the shear stops.


Nota : not only the positions of walls are updated but also their speeds, which is all but useless considering the fact that in the contact laws these velocities of bodies are used to compute values of tangential relative displacements

!!WARNING!! : But, because of this last point, if you want to use later saves of simulations executed with this Engine, but without that stopMovement was executed, your boxes will keep their speeds => you will have to cancel them "by hand" in the .xml
*/


class CinemKNCEngine : public DeusExMachina
{
	private :
		shared_ptr<ContactLaw1> myLdc;
		Real	coeff_dech;	// the engine cares about it
		Real	alpha	// angle from the lower plate to the left box (trigo wise), the Engine finds itself its value
			,dalpha	// the increment over alpha
			,gamma	// horizontal displacement done since the launch of the calcul
			,Y0	// the height of the upper plate at the very first time step : the engine finds its value
			,deltaH	// the vertical increment of displacement to allow on the upper plate on this time step to verify the normal rigidity
			,F_0	// the (vertical) force acting on the upper plate on the very first time step : the engine finds its value
			,stiffness	// the normal stifness on the sample acting below the upper plate
			;

		int	temoin,it_stop
// 			,k		// plates are moved only all "k" iterations to let the sample relax
			;
		Vector3r prevF_sup;	// the force acting on the upper plate at the previous time step
					

		bool firstRun;		// set to true as soon as the engine has done its job one time : usefull to know if the force acting on the plate is known or not (and if F_0 and Y0 were initialized)
		shared_ptr<Body> leftbox;
		shared_ptr<Body> rightbox;
		shared_ptr<Body> frontbox;
		shared_ptr<Body> backbox;
		shared_ptr<Body> topbox;
		shared_ptr<Body> boxbas;
	public :
		CinemKNCEngine();
		void 	applyCondition(Body * body)
			,computeAlpha()
			;

		Real	 shearSpeed	// to be defined in the PreProcessor
			,gammalim 	// the maximum value of the horizontal displacemt
			,KnC		// the normal rigidity choosen by the user, in MPa/mm
			,max_vel	// to compute the maximum correction displacement that could be imposed in one time step to the upper plate
			,wallDamping
			;

		bool LOG;		//controls messages output on screen

		body_id_t 	id_boxhaut,	// the id of the upper wall : defined in the constructor
				id_boxbas,	// the id of the lower wall : defined in the constructor
				id_boxleft,
				id_boxright,
				id_boxfront,
				id_boxback;
		string Key;
		


	protected :
		void registerAttributes();
		void letMove(Body* body);
		void computeDu(MetaBody* ncb);
		void stopMovement();		// to cancel all the velocities when gammalim is reached
		void computeStiffness(MetaBody* ncb);
	REGISTER_CLASS_NAME(CinemKNCEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(CinemKNCEngine);


