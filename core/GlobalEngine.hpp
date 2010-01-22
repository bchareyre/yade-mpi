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
	YADE_CLASS_BASE_DOC_ATTRS(GlobalEngine,Engine,"Engine that will generally affect the whole simulation (contrary to PartialEngine).",/* no own attrs */);
};
REGISTER_SERIALIZABLE(GlobalEngine);


