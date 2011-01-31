/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include<yade/pkg/common/NormShearPhys.hpp>

class FrictPhys: public NormShearPhys
{
	public :
		virtual ~FrictPhys();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(FrictPhys,NormShearPhys,"The simple linear elastip-plastic interaction with friction angle, like in the traditional [CundallStrack1979]_",
		((Real,tangensOfFrictionAngle,NaN,,"tan of angle of friction")),
		createIndex()
	);
	REGISTER_CLASS_INDEX(FrictPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(FrictPhys);
