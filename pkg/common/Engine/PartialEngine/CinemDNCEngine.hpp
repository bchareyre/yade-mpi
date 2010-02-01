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
#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

/*! \brief To apply a zero normal displacement shear for a parallelogram box

This engine, used in simulations issued from "SimpleShear" Preprocessor, allows to translate horizontally the upper plate while the lateral ones rotate so that they always keep contact with the lower and upper walls
*/


class CinemDNCEngine : public PartialEngine
{
	private :
		Real	alpha	// angle from the lower plate to the left box (trigo wise), the Engine finds itself its value
			,dalpha	// the increment over alpha, due to vertical displacement of upper box
			,gamma	// horizontal displacement done since the launch of the calcul first step
			;

		int temoinfin;
		shared_ptr<Body> leftbox;
		shared_ptr<Body> rightbox;
		shared_ptr<Body> topbox;

		void letMove(Scene* body);
		void stopMovement();		// to cancel all the velocities when gammalim is reached


	public :
		CinemDNCEngine();
		void applyCondition(Scene * body),
			computeAlpha();

		Real	 shearSpeed	// to be defined in the PreProcessor
			,gammalim 	// the maximum value of gamma (tangential displacemt), in meters, at wich the displacement is stopped
			;
		body_id_t 	id_boxhaut,	// the id of the upper wall : defined in the constructor
				id_boxleft,
				id_boxright;
		std::vector<Real>	gamma_save	// the values of gamma, in meters, at which a save is performed
					,temoin_save
					;
		string Key;

	protected :
	REGISTER_ATTRIBUTES(PartialEngine,(shearSpeed)(gammalim)(gamma)(gamma_save)(temoin_save)(id_boxhaut)(id_boxleft)(id_boxright)(Key));
	REGISTER_CLASS_NAME(CinemDNCEngine);
	REGISTER_BASE_CLASS_NAME(PartialEngine);
};

REGISTER_SERIALIZABLE(CinemDNCEngine);


