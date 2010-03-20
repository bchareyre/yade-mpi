/*************************************************************************
*  Copyright (C) 2006 by Luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CapillaryPressureEngine.hpp"
#include <yade/pkg-dem/CapillaryCohesiveLaw.hpp>
#include <yade/core/Scene.hpp>

using namespace boost;
using namespace std;

CapillaryPressureEngine::CapillaryPressureEngine()
{	
	//cerr << "constructeur PressureEngine" << endl;
	
	capillaryCohesiveLaw = new CapillaryCohesiveLaw;
	capillaryCohesiveLaw->sdecGroupMask = 2; // absolument n�cessaire !!!
	
	
}

CapillaryPressureEngine::~CapillaryPressureEngine()
{
}



void CapillaryPressureEngine::action()
{		
	//cerr << " CapillaryPressure = " << capillaryCohesiveLaw->CapillaryPressure << endl;
	
	
	capillaryCohesiveLaw->CapillaryPressure = Pressure;
	if (Pressure<100000) Pressure += PressureVariation;
	//capillaryCohesiveLaw->CapillaryPressure += PressureVariation;
	
// 	cerr << " CapillaryPressure = " << capillaryCohesiveLaw->CapillaryPressure << endl;
// 	cerr << " PressureVariation = " << PressureVariation << endl;
	
	capillaryCohesiveLaw->scene=scene;
	capillaryCohesiveLaw->action();
		
	
	
	
}

YADE_PLUGIN((CapillaryPressureEngine));

//YADE_REQUIRE_FEATURE(PHYSPAR);

