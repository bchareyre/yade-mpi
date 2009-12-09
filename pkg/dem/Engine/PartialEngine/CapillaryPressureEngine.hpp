/*************************************************************************
*  Copyright (C) 2006 by Luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/core/PartialEngine.hpp>
#include <Wm3Vector3.h>
#include <yade/lib-base/yadeWm3.hpp>

class CapillaryCohesiveLaw;

class CapillaryPressureEngine : public PartialEngine
{
	public :
		
		
		CapillaryPressureEngine();
		virtual ~CapillaryPressureEngine();
		
		Real PressureVariation;
		Real Pressure;
		//shared_ptr<CapillaryCohesiveLaw>  capillaryCohesiveLaw;
		CapillaryCohesiveLaw* capillaryCohesiveLaw;
				
		void applyCondition(Scene * body);
	REGISTER_ATTRIBUTES(PartialEngine,(PressureVariation)(Pressure));
	REGISTER_CLASS_NAME(CapillaryPressureEngine);
	REGISTER_BASE_CLASS_NAME(PartialEngine);
};

REGISTER_SERIALIZABLE(CapillaryPressureEngine);


