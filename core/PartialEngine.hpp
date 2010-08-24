/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include <vector>
#include<yade/core/Engine.hpp>
#include<yade/core/Body.hpp>

class PartialEngine: public Engine{
	public:
		virtual ~PartialEngine() {};
	YADE_CLASS_BASE_DOC_ATTRS(PartialEngine,Engine,"Engine affecting only particular bodies in the simulation, defined by *subscribedBodies*.",
		((std::vector<int>,subscribedBodies,,,":yref:`Ids<Body::id>` of bodies affected by this PartialEngine."))
	);
};
REGISTER_SERIALIZABLE(PartialEngine);


