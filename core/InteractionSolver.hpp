/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include "GlobalEngine.hpp"

class InteractionSolver : public GlobalEngine
{
	public :
		InteractionSolver() {};
		virtual ~InteractionSolver() {};

	REGISTER_CLASS_NAME(InteractionSolver);	
	REGISTER_BASE_CLASS_NAME(GlobalEngine);
};

REGISTER_SERIALIZABLE(InteractionSolver);

