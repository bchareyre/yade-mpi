/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

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
	public:
		virtual std::string get1DFunctorType1(void){throw runtime_error("Class "+this->getClassName()+" did not use FUNCTOR1D to declare its argument type?"); }
		virtual vector<string> getFunctorTypes(void){vector<string> ret; ret.push_back(get1DFunctorType1()); return ret;};
	REGISTER_CLASS_AND_BASE(EngineUnit1D,EngineUnit FunctorWrapper);
	/* do not REGISTER_ATTRIBUTES here, since we are template; derived classes should call REGISTER_ATTRIBUTES(EngineUnit,(their)(own)(attributes)), bypassing EngineUnit1D */
};


