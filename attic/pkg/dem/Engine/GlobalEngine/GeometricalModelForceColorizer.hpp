/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GlobalEngine.hpp>

class GeometricalModelForceColorizer : public GlobalEngine
{
	public :
		GeometricalModelForceColorizer ();

		virtual void action();
		virtual bool isActivated(Scene*);
	
	REGISTER_CLASS_NAME(GeometricalModelForceColorizer);
	REGISTER_BASE_CLASS_NAME(GlobalEngine);
};

REGISTER_SERIALIZABLE(GeometricalModelForceColorizer);


