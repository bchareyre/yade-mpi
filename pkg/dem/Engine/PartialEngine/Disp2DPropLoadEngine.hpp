/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef DIRECRESEARCHENGINE_HPP
#define DIRECRESEARCHENGINE_HPP

#include<yade/core/Omega.hpp>
#include<yade/core/PartialEngine.hpp>
#include<yade/core/Body.hpp>
#include <Wm3Vector3.h>
// #include<yade/lib-base/yadeWm3.hpp>

/*! \brief To disturb a sample in a given direction (initial aim : directionnal research for d²w)

The aim of this engine is to perform what is called a "directionnal research" over a sample. That is to say : considering a sample in a given state, disturbing it in all the directions (directions in the stresses plane for example)

In fact this Engine disturbs in only one direction
The control will be done in displacements in fact since it is much more easier
Nota : not only the positions of walls are updated but also their speeds, which is all but useless considering the fact that in the contact laws these velocities of bodies are used to compute values of tangential relative displacements

!!WARNING!! : But, because of this last point, if you want to use later saves of simulations executed with this Engine, but without that stopMovement was executed, your boxes will keep their speeds => you will have to cancel them "by hand" in the .xml
*/


class Disp2DPropLoadEngine : public DeusExMachina
{
	private :
		shared_ptr<PhysicalAction> actionForce;
		Real	dgamma	// the increment of horizontal displacement in one timestep, part of the disturbation
			,dh	// the increment of vertical displacement in one timestep, part of the disturbation
			,H0	// the height of the top box, at the beginnig of the application of the disturbation
			,X0	// the X-position of the top box, at the beginnig of the application of the disturbation
			,Fn0,Ft0// the normal and tangential force acting on the top box, at...
			,coordSs0,coordTot0
			;
		std::ofstream ofile;		

		Real	alpha	// angle from the lower plate to the left box (trigo wise), as in other shear Engines, but here the Engine is able to find itself its value !
			,dalpha	// the increment over alpha
			;

		bool	firstIt;// true if this is the first iteration, false else.

		int	it_begin// the number of the it at which the computation starts
			;

		shared_ptr<Body> leftbox;
		shared_ptr<Body> rightbox;
		shared_ptr<Body> frontbox;
		shared_ptr<Body> backbox;
		shared_ptr<Body> topbox;
		shared_ptr<Body> boxbas;
		void saveData(Scene* ncb);
		void letDisturb(Scene* ncb);
		void stopMovement();		// to cancel all the velocities


	public :
		Disp2DPropLoadEngine();
		void 	applyCondition(Scene* ncb)
			,computeAlpha()
			;

		Real	v	// the speed at which the perturbation is imposed
			,theta	// the angle, in a (gamma,h) plane from the gamma - axis to the perturbation vector (trigo wise), in degrees For example v = 0.3 - | (0.3-0.04)*sin(theta) | => 0.3 in shear; 0.04 in compression (for 18/11)
			;

		int	nbre_iter	// the number of iterations of disturbation to perform
			;
		bool LOG;		//controls messages output on screen

		body_id_t 	id_topbox,	// the id of the upper wall : defined in the constructor
				id_boxbas,	// the id of the lower wall : defined in the constructor
				id_boxleft,
				id_boxright,
				id_boxfront,
				id_boxback;
		string 	Key
			;
		


	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(Disp2DPropLoadEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(Disp2DPropLoadEngine,false);

#endif // DIRECRESEARCHENGINE_HPP

