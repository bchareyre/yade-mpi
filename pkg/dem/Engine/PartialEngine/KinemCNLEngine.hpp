/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Omega.hpp>
#include<yade/core/PartialEngine.hpp>
#include<yade/core/Body.hpp>
#include<yade/lib-base/Math.hpp>
#include<yade/pkg-dem/NormalInelasticityLaw.hpp>




class KinemCNLEngine : public PartialEngine
{
	private :
		shared_ptr<NormalInelasticityLaw> myLdc;
		Real	alpha	// angle from the lower plate to the left box (trigo wise), the Engine finds itself its value
			,dalpha	// the increment over alpha, due to vertical displacement of upper box
			,deltaU	// the vertical increment of displacement to allow on the upper plate on this time step to verify the constant normal stress
			,stiffness	// the normal stifness on the sample acting below the upper plate
			;

		int	temoin,// utile pour savoir ou on en est
			it_stop
			;

		shared_ptr<Body> leftbox;
		shared_ptr<Body> rightbox;
		shared_ptr<Body> frontbox;
		shared_ptr<Body> backbox;
		shared_ptr<Body> topbox;
		shared_ptr<Body> boxbas;
	public :
		void	action(Scene * body)
			,computeAlpha()
			;

	protected :
		void letMove(Scene* body);
		void computeDu(Scene* ncb);
		void stopMovement();		// to cancel all the velocities when gammalim is reached
		void computeStiffness(Scene* ncb);
	
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(KinemCNLEngine,PartialEngine,
				 "To apply a constant normal stress shear for a parallelogram box (simple shear)\n\nThis engine, used in simulations issued from :yref:`SimpleShear` Preprocessor, allows to translate horizontally the upper plate while the lateral ones rotate so that they always keep contact with the lower and upper walls.\n\nIn fact the upper plate can move not only horizontally but also vertically, so that the normal stress acting on it remains constant (this constant value is not choosen by the user but is the one that exists at the beginning of the simulation)\n\nThe right vertical displacements which will be allowed are computed from the rigidity Kn of the sample over the wall (so to cancel a deltaSigma, a normal dplt deltaSigma*S/(Kn) is set)\n\nThe movement is moreover controlled by the user via a *shearSpeed* which will be the speed of the upper wall, and by a maximum value of horizontal displacement *gammalim*, after which the shear stops.\n\n.. note::\n\tNot only the positions of walls are updated but also their speeds, which is all but useless considering the fact that in the contact laws these velocities of bodies are used to compute values of tangential relative displacements.\n\n.. warning::\n\tBecause of this last point, if you want to use later saves of simulations executed with this Engine, but without that stopMovement was executed, your boxes will keep their speeds => you will have to cancel them 'by hand' in the .xml.\n",
				 ((Real,shearSpeed,0.0,"the speed at wich the shearing is performed : speed of the upper plate [m/s]"))
				 ((Real,gammalim,0.0,"the value of tangential displacement (of upper plate) at wich the shearing is stopped [m]"))
				 ((Real,gamma,0.0,"current value of tangential displacement [m]"))
				 ((std::vector<Real>,gamma_save,,"vector with the values of gamma at which a save of the simulation is performed [m]"))
				 ((std::vector<Real>,temoin_save,,"vector (same length as 'gamma_save'), with 0 or 1 depending whether the save for the corresponding value of gamma has been done (1) or not (0)"))
				 ((bool,firstRun,true,"boolean set to false as soon as the engine has done its job one time : usefull to know if the force acting on the plate is known or not (and if F_0 has to be initialized)"))
				 ((body_id_t,id_topbox,3,"the id of the upper wall"))
				 ((body_id_t,id_boxbas,1,"the id of the lower wall"))
				 ((body_id_t,id_boxleft,0,"the id of the left wall"))
				 ((body_id_t,id_boxright,2,"the id of the right wall"))
				 ((body_id_t,id_boxfront,5,"the id of the wall in front of the sample"))
                                 ((body_id_t,id_boxback,4,"the id of the wall at the back of the sample"))
				 ((Real,F_0,0.0,"the (vertical) force acting on the upper plate on the very first time step (determined by the Engine). All control will be performed in order to keep this value of F_0 [N]"))
				 ((Real,max_vel,1.0,"to limit the speed of the vertical displacements done to maintain F equal to F_0 [m/s]"))
				 ((Real,wallDamping,0.2,"the vertical displacements done to maintain F equal to F_0 are in fact damped, through this wallDamping"))
				 ((string,Key,"","string to add at the names of the saved files"))
				 ((bool,LOG,false,"boolean controling the output of messages on the screen"))
				 ((Real,coeff_dech,1.0,"in the case of the use of 'NormalInelasticityLaw' for ex, where kn(unload)#kn(load). The engine cares to find the value at the first run")),
				alpha=Mathr::PI/2.0;
				temoin=0;
				 );
};

REGISTER_SERIALIZABLE(KinemCNLEngine);


