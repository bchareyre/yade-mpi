/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Omega.hpp>
#include<yade/pkg-common/BoundaryController.hpp>
#include<yade/core/Body.hpp>
#include<yade/lib-base/Math.hpp>
#include<yade/pkg-dem/NormalInelasticityLaw.hpp>



class KinemCNSEngine : public BoundaryController
{
	private :
		shared_ptr<Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity> myLdc;
		Real	alpha	// angle from the lower plate to the left box (trigo wise), the Engine finds itself its value
			,dalpha	// the increment over alpha
			,deltaH	// the vertical increment of displacement to allow on the upper plate on this time step to verify the normal rigidity
			,stiffness	// the normal stifness on the sample acting below the upper plate
			;

		int	temoin,it_stop
			;

		shared_ptr<Body> leftbox;
		shared_ptr<Body> rightbox;
		shared_ptr<Body> frontbox;
		shared_ptr<Body> backbox;
		shared_ptr<Body> topbox;
		shared_ptr<Body> boxbas;
	public :
		void 	action()
			,computeAlpha()
			;


	protected :
		void letMove();
		void computeDu();
		void stopMovement();		// to cancel all the velocities when gammalim is reached
		void computeStiffness();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(KinemCNSEngine,BoundaryController,
		"To apply a constant normal rigidity shear for a parallelogram box (simple shear)\n\nThis engine, useable in simulations implying one deformable parallelepipedic box (e.g. :yref:`SimpleShear` Preprocessor), allows to translate horizontally the upper plate while the lateral ones rotate so that they always keep contact with the lower and upper walls. The upper plate can move not only horizontally but also vertically, so that the normal rigidity defined by DeltaF(upper plate)/DeltaU(upper plate) = constant (= *KnC* defined by the user).\n\nThe movement is moreover controlled by the user via a *shearSpeed* which is the horizontal speed of the upper wall, and by a maximum value of horizontal displacement *gammalim* (of the upper plate), after which the shear stops.\n\n.. note::\n\t not only the positions of walls are updated but also their speeds, which is all but useless considering the fact that in the contact laws these velocities of bodies are used to compute values of tangential relative displacements.\n\n.. warning::\n\tBut, because of this last point, if you want to use later saves of simulations executed with this Engine, but without that stopMovement was executed, your boxes will keep their speeds => you will have to cancel them by hand in the .xml",
		((Real,shearSpeed,0.0,"the speed at wich the shearing is performed : speed of the upper plate [m/s]"))
		((Real,gammalim,0.0,"the value of tangential displacement (of upper plate) at wich the shearing is stopped [m]"))
		((Real,gamma,0.0,"current value of tangential displacement [m]"))
		((bool,firstRun,true,"boolean set to false as soon as the engine has done its job one time : usefull to know if the force acting on the plate is known or not (and if F_0 has to be initialized)"))
		((body_id_t,id_topbox,3,"the id of the upper wall"))
		((body_id_t,id_boxbas,1,"the id of the lower wall"))
		((body_id_t,id_boxleft,0,"the id of the left wall"))
		((body_id_t,id_boxright,2,"the id of the right wall"))
		((body_id_t,id_boxfront,5,"the id of the wall in front of the sample"))
		((body_id_t,id_boxback,4,"the id of the wall at the back of the sample"))
		((Real,Y0,0.0,"the height of the upper plate at the very first time step : the engine finds its value"))
		((Real,F_0,0.0,"the (vertical) force acting on the upper plate on the very first time step (determined by the Engine) [N]"))
		((Real,KnC,10.0e6,"the normal rigidity choosen by the user [MPa/mm]"))
		((Real,max_vel,1.0,"to limit the speed of the vertical displacements applied to control upper plate [m/s]"))
		((string,Key,"","string to add at the names of the saved files"))
		((bool,LOG,false,"boolean controling the output of messages on the screen"))
		((Real,coeff_dech,1.0,"in the case of the use of 'Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity' for ex, where kn(unload)#kn(load). The engine cares to find the value at the first run"))
		((Real,wallDamping,0.2,"the vertical displacements done to maintain F equal to F_0 are in fact damped, through this wallDamping")),
		alpha=Mathr::PI/2.0;
		temoin=0;
	);

};

REGISTER_SERIALIZABLE(KinemCNSEngine);


