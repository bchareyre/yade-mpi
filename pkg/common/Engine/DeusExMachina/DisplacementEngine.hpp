/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef DISPLACEMENTENGINE_HPP
#define DISPLACEMENTENGINE_HPP

#include<yade/core/DeusExMachina.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class DisplacementEngine : public DeusExMachina
{
	public :
		Real displacement;
		Vector3r translationAxis;
		void applyCondition(MetaBody *);
		bool active;
		DisplacementEngine():active(true){};
		bool isActivated();

	protected :
		virtual void postProcessAttributes(bool);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(DisplacementEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(DisplacementEngine);

#endif //  DISPLACEMENTENGINE_HPP

