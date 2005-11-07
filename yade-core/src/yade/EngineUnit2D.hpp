/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ENGINEUNIT2D_HPP
#define ENGINEUNIT2D_HPP

#include "EngineUnit.hpp"

#include <yade/yade-lib-multimethods/FunctorWrapper.hpp>

template
<
	class ReturnType,
	class AttributesType
>
class EngineUnit2D :	public EngineUnit,
			public FunctorWrapper<ReturnType, AttributesType>
{
	REGISTER_CLASS_NAME(EngineUnit2D);
	REGISTER_BASE_CLASS_NAME(EngineUnit FunctorWrapper);
};

#endif // ENGINEUNIT2D_HPP

