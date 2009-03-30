/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractionSolver.hpp>

class MassSpringLaw : public InteractionSolver
{
	public :
		int springGroupMask;
		MassSpringLaw ();
		void action(MetaBody*);

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(MassSpringLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(MassSpringLaw);


