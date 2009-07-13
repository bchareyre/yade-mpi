/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PhysicalParameters.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/EngineUnit2D.hpp>
#include<yade/core/EngineUnit1D.hpp>

class PhysicalActionApplierUnit: public EngineUnit1D<void,TYPELIST_3(const shared_ptr<PhysicalParameters>&,const Body*, MetaBody*)>{
	public: virtual ~PhysicalActionApplierUnit();
	REGISTER_CLASS_AND_BASE(PhysicalActionApplierUnit,EngineUnit1D);
	REGISTER_ATTRIBUTES(EngineUnit, /* nothing here */ );
};
REGISTER_SERIALIZABLE(PhysicalActionApplierUnit);

