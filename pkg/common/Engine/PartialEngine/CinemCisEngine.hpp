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

/*! \brief To apply a zero normal displacement shear for a parallelogram box

This engine, used in simulations issued from "DirectShearCis" Preprocessor, allows to translate horizontally the upper plate while the lateral ones rotate so that they always keep contact with the lower and upper walls
*/


class CinemCisEngine : public PartialEngine
{
	private :
		Real	Yplaqsup// height of the upper wall, the engine cares to find its value
			,theta;	// the angle between a lateral plate and its original orienation : will increase from 0 to thetalim

	public :
		CinemCisEngine();
		void applyCondition(Scene * body);

		Real	 shearSpeed	// to be defined in the PreProcessor
			,thetalim 	// the maximum value of theta, at wich the displacement is stopped
			;
		body_id_t id_boxhaut;	// the id of the upper wall : defined in the constructor
		Vector3r rotationAxis;	// defined in the constructor
		void applyRotTranslation(Scene *);	// to let move (rotation combined with translation) the lateral walls
		void applyTranslation(Scene *);	// to let move (translation) the upper wall
	REGISTER_ATTRIBUTES(PartialEngine,(shearSpeed)(rotationAxis)(theta)(thetalim)(id_boxhaut));
	REGISTER_CLASS_NAME(CinemCisEngine);
	REGISTER_BASE_CLASS_NAME(PartialEngine);
};

REGISTER_SERIALIZABLE(CinemCisEngine);


