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
	YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(PartialEngine,Engine,"Engine affecting only particular bodies in the simulation, defined by *ids*.",
		((std::vector<int>,ids,,,":yref:`Ids<Body::id>` of bodies affected by this PartialEngine.")),
		/*deprec*/ ((subscribedBodies,ids,"The old name was too long")), /*init*/, /* ctor */, /* py */
	);
};
REGISTER_SERIALIZABLE(PartialEngine);


