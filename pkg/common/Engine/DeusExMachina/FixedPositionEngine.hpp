/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FIXEDPOSITIONENGINE_HPP
#define FIXEDPOSITIONENGINE_HPP

#include<yade/core/DeusExMachina.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class FixedPositionEngine : public DeusExMachina
{
	public :
		Vector3r fixedPosition,mask;
		void applyCondition(Body * body);

		FixedPositionEngine();

	protected :
		virtual void postProcessAttributes(bool);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(FixedPositionEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(FixedPositionEngine,false);

#endif //  DISPLACEMENTENGINE_HPP

