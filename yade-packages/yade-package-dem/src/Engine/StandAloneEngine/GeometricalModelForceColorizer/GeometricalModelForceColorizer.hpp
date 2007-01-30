/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GEOMETRICAL_MODEL_FORCE_COLORIZER_HPP
#define GEOMETRICAL_MODEL_FORCE_COLORIZER_HPP

#include <yade/yade-core/StandAloneEngine.hpp>

class PhysicalAction;

class GeometricalModelForceColorizer : public StandAloneEngine
{
	private :
		shared_ptr<PhysicalAction> actionForce;
	
	public :
		GeometricalModelForceColorizer ();

		virtual void action(Body* b);
		virtual bool isActivated();

	REGISTER_CLASS_NAME(GeometricalModelForceColorizer);
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};

REGISTER_SERIALIZABLE(GeometricalModelForceColorizer,false);

#endif // FORCE_RECORDER_HPP

