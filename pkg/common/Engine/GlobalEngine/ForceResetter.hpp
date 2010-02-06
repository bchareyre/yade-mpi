/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GlobalEngine.hpp>

class Scene;
class ForceResetter: public GlobalEngine{
	public:
		virtual void action(Scene*);
	YADE_CLASS_BASE_DOC_ATTRDECL_CTOR_PY(ForceResetter,GlobalEngine,"Reset all forces stored in Scene::forces (``O.forces`` in python). Typically, this is the first engine to be run at every step.",
	/* attrs */, /*ctor*/, /*py*/);
};
REGISTER_SERIALIZABLE(ForceResetter);


