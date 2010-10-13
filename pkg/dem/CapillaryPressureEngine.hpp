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

class Law2_ScGeom_CapillaryPhys_Capillarity;

class CapillaryPressureEngine : public PartialEngine
{
	public :
		shared_ptr<Law2_ScGeom_CapillaryPhys_Capillarity>  capillaryCohesiveLaw;
		
		void action();
		virtual ~CapillaryPressureEngine();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CapillaryPressureEngine,PartialEngine,".. note::\n\tthis engine is deprecated and probably not very useful! It was designed to produce a variation of the capillary pressure (see :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`).",
		((Real,Pressure,0.,,"Initial value of the capillary pressure Uc = Ugas-Uliquid. [Pa]."))
		((Real,PressureVariation,0.,,"Variation of the capillary pressure (each iteration). [Pa]")),
		;
		);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(CapillaryPressureEngine);
