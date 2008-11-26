/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef STANDALONEENGINE_HPP
#define STANDALONEENGINE_HPP

#include "Engine.hpp"

class StandAloneEngine : public Engine
{
	public :
		StandAloneEngine() {};
		virtual ~StandAloneEngine() {};

	REGISTER_CLASS_NAME(StandAloneEngine);	
	REGISTER_BASE_CLASS_NAME(Engine);
};

REGISTER_SERIALIZABLE(StandAloneEngine);

#endif // STANDALONEENGINE_HPP

