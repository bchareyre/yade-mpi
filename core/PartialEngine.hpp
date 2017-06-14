/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include <vector>
#include<core/Engine.hpp>
#include<core/Body.hpp>

class PartialEngine: public Engine{
	public:
		virtual ~PartialEngine() {};
	YADE_CLASS_BASE_DOC_ATTRS(PartialEngine,Engine,"Engine affecting only particular bodies in the simulation, defined by :yref:`ids attribute<PartialEngine::ids>`.",
		((std::vector<int>,ids,,,":yref:`Ids<Body::id>` list of bodies affected by this PartialEngine."))
	);
};
REGISTER_SERIALIZABLE(PartialEngine);


