/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/StandAloneEngine.hpp>

class GeometricalModelForceColorizer : public StandAloneEngine
{
	public :
		GeometricalModelForceColorizer ();

		virtual void action(Scene*);
		virtual bool isActivated(Scene*);
	
	REGISTER_CLASS_NAME(GeometricalModelForceColorizer);
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};

REGISTER_SERIALIZABLE(GeometricalModelForceColorizer);


