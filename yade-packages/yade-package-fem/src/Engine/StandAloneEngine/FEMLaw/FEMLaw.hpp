/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FEMLAW_HPP
#define FEMLAW_HPP

#include <yade/yade-core/Engine.hpp>

class PhysicalAction;

class FEMLaw : public Engine
{
/// Attributes
	private :
		shared_ptr<PhysicalAction> actionForce;
	public :
		int	 nodeGroupMask
			,tetrahedronGroupMask;

		FEMLaw();
		virtual ~FEMLaw();

/// Methods
		void action(Body* body);

/// Serializtion
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(FEMLaw);
	REGISTER_BASE_CLASS_NAME(Engine);


};

REGISTER_SERIALIZABLE(FEMLaw,false);

#endif // FEMLAW_HPP


