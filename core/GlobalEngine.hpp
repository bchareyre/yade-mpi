/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include "Engine.hpp"

class GlobalEngine : public Engine
{
	public :
		GlobalEngine() {};
		virtual ~GlobalEngine() {};
	REGISTER_ATTRIBUTES(Engine,/* no own attributes*/);
	REGISTER_CLASS_AND_BASE(GlobalEngine,Engine);
};
REGISTER_SERIALIZABLE(GlobalEngine);


