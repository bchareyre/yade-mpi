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
#include<yade/lib-base/Math.hpp>


class KinemCNDEngine : public PartialEngine
{
	private :
		Real	alpha	// angle from the lower plate to the left box (trigo wise), the Engine finds itself its value
			,dalpha	// the increment over alpha, due to vertical displacement of upper box
			;

		int temoinfin;
		shared_ptr<Body> leftbox;
		shared_ptr<Body> rightbox;
		shared_ptr<Body> topbox;

		void letMove(Scene*);
		void stopMovement();		// to cancel all the velocities when gammalim is reached


	public :
		void action(Scene *),
			computeAlpha();

			;

	protected :

		YADE_CLASS_BASE_DOC_ATTRS_CTOR(KinemCNDEngine,PartialEngine,
			"To apply a constant normal displacement shear for a parallelogram box\n\n \tThis engine, designed for simulations implying a simple shear box (:yref:`SimpleShear` Preprocessor), allows to perform a constant normal displacement shear, by translating horizontally the upper plate, while the lateral ones rotate so that they always keep contact with the lower and upper walls.",
			((Real,shearSpeed,0.0,"the speed at which the shear is performed : speed of the upper plate [m/s]"))
			((Real,gammalim,0.0,"the value of the tangential displacement at wich the displacement is stopped [m]"))
			((Real,gamma,0.0,"the current value of the tangential displacement"))
			((std::vector<Real>,gamma_save,,"vector with the values of gamma at which a save of the simulation is performed [m]"))
			((std::vector<Real>,temoin_save,,"vector (same length as 'gamma_save'), with 0 or 1 depending whether the save for the corresponding value of gamma has been done (1) or not (0)"))
			((body_id_t,id_topbox,3,"the id of the upper wall"))
			((body_id_t,id_boxleft,0,"the id of the left wall"))
			((body_id_t,id_boxright,2,"the id of the right wall"))
			((string,Key,"","string to add at the names of the saved files")),
			temoinfin=0;
						);
};

REGISTER_SERIALIZABLE(KinemCNDEngine);


