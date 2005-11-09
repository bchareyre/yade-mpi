/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LATTICELAW_HPP
#define LATTICELAW_HPP


#include <yade/yade-core/InteractionSolver.hpp>


class PhysicalAction;


class LatticeLaw : public InteractionSolver
{

/// Attributes	
	
	private :
		shared_ptr<PhysicalAction> actionForce;

	public :
		int	 nodeGroupMask
			,beamGroupMask;

		Real	 maxDispl;

		LatticeLaw();
		virtual ~LatticeLaw();
		void action(Body* body);

/// Serializtion
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(LatticeLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};


REGISTER_SERIALIZABLE(LatticeLaw,false);


#endif // LATTICELAW_HPP


