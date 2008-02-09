/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ENGINEUNIT1D_HPP
#define ENGINEUNIT1D_HPP

#include "EngineUnit.hpp"

#include<yade/lib-multimethods/FunctorWrapper.hpp>

#define FUNCTOR1D(type1) public: std::string get1DFunctorType1(void){return string(#type1);}

template
<
	class ReturnType,
	class AttributesType
>
class EngineUnit1D : 	public EngineUnit,
			public FunctorWrapper<ReturnType, AttributesType>
{
	public: virtual std::string get1DFunctorType1(void){throw runtime_error("Class "+this->getClassName()+" did not use FUNCTOR1D to declare its argument type?"); }
	REGISTER_CLASS_NAME(EngineUnit1D);
	REGISTER_BASE_CLASS_NAME(EngineUnit FunctorWrapper);
};

#endif // ENGINEUNIT1D_HPP

