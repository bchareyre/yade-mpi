/*************************************************************************
*  Copyright (C) 2006 by Luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CapillaryPressureEngine.hpp"
#include <yade/pkg-dem/Law2_ScGeom_CapillaryPhys_Capillarity.hpp>
#include <yade/core/Scene.hpp>

YADE_REQUIRE_FEATURE(!! This engine is not functional; remove this line to enable it again !!);

YADE_PLUGIN((CapillaryPressureEngine));
CREATE_LOGGER(CapillaryPressureEngine);

CapillaryPressureEngine::~CapillaryPressureEngine()
{
}

void CapillaryPressureEngine::action()
{
	capillaryCohesiveLaw->CapillaryPressure = Pressure;
	if (Pressure<100000) Pressure += PressureVariation;
	//capillaryCohesiveLaw->CapillaryPressure += PressureVariation;
	
	capillaryCohesiveLaw->scene=scene;
	capillaryCohesiveLaw->action();
	
}

//YADE_REQUIRE_FEATURE(PHYSPAR);
