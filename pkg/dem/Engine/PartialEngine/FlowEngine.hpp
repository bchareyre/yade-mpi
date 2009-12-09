/*************************************************************************
*  Copyright (C) 2009 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PartialEngine.hpp>

class FlowEngine : public PartialEngine 
{
	public :
		Vector3r gravity;
		bool isActivated;
		bool dummyParameter;
		FlowEngine();
		virtual ~FlowEngine();
	
		virtual void applyCondition(Scene*);
	
	protected :
	REGISTER_ATTRIBUTES(PartialEngine,/*(gravity)*/(isActivated)(dummyParameter));
	REGISTER_CLASS_NAME(FlowEngine);
	REGISTER_BASE_CLASS_NAME(PartialEngine);
};

REGISTER_SERIALIZABLE(FlowEngine);


