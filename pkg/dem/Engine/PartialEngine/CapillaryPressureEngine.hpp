/*************************************************************************
*  Copyright (C) 2006 by Luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/core/PartialEngine.hpp>
#include <yade/lib-base/Math.hpp>

class CapillaryCohesiveLaw;

class CapillaryPressureEngine : public PartialEngine
{
	public :
		shared_ptr<CapillaryCohesiveLaw>  capillaryCohesiveLaw;
		//CapillaryCohesiveLaw* capillaryCohesiveLaw;
		
		void action();
		virtual ~CapillaryPressureEngine();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CapillaryPressureEngine,PartialEngine,"Rk: this engine is deprecated and probably not very useful! It was designed to produce a variation of the capillary pressure (see CapillaryCohesiveLaw).",
		((Real,Pressure,0.,"Initial value of the capillary pressure Uc = Ugas-Uliquid. [Pa]."))
		((Real,PressureVariation,0.,"Variation of the capillary pressure (each iteration). [Pa]")),
		;
		);
};
REGISTER_SERIALIZABLE(CapillaryPressureEngine);

//// OLD CODE!!!!
// class CapillaryPressureEngine : public PartialEngine
// {
// 	public :
// 		CapillaryPressureEngine();
// 		virtual ~CapillaryPressureEngine();
// 		
// 		Real PressureVariation;
// 		Real Pressure;
// 		//shared_ptr<CapillaryCohesiveLaw>  capillaryCohesiveLaw;
// 		CapillaryCohesiveLaw* capillaryCohesiveLaw;
// 				
// 		void action();
// 	REGISTER_ATTRIBUTES(PartialEngine,(PressureVariation)(Pressure));
// 	REGISTER_CLASS_NAME(CapillaryPressureEngine);
// 	REGISTER_BASE_CLASS_NAME(PartialEngine);
// };
// REGISTER_SERIALIZABLE(CapillaryPressureEngine);