/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONSOLVER_HPP
#define INTERACTIONSOLVER_HPP

#include "StandAloneEngine.hpp"

class InteractionSolver : public StandAloneEngine
{
	public :
		InteractionSolver() {};
		virtual ~InteractionSolver() {};

	REGISTER_CLASS_NAME(InteractionSolver);	
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};

REGISTER_SERIALIZABLE(InteractionSolver,false);

#endif // INTERACTIONSOLVER_HPP
