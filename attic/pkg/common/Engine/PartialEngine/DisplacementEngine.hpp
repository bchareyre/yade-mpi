/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PartialEngine.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class DisplacementEngine : public PartialEngine
{
	public :
		Real displacement;
		Vector3r translationAxis;
		void applyCondition(Scene *);
		bool active;
		DisplacementEngine():active(true){};
		bool isActivated(Scene*);

	protected :
		virtual void postProcessAttributes(bool);
	REGISTER_ATTRIBUTES(PartialEngine,(displacement)(translationAxis)(active));
	REGISTER_CLASS_NAME(DisplacementEngine);
	REGISTER_BASE_CLASS_NAME(PartialEngine);
};

REGISTER_SERIALIZABLE(DisplacementEngine);


