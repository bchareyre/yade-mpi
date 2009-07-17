/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractionSolver.hpp>

class FEMLaw : public InteractionSolver
{
/// Attributes
	public :
		int	 nodeGroupMask
			,tetrahedronGroupMask;

		FEMLaw();
		virtual ~FEMLaw();

/// Methods
		void action(MetaBody*);

/// Serializtion
	REGISTER_ATTRIBUTES(InteractionSolver,(nodeGroupMask)(tetrahedronGroupMask));
	REGISTER_CLASS_NAME(FEMLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);


};

REGISTER_SERIALIZABLE(FEMLaw);



