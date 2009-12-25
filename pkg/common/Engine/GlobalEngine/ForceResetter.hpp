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
	REGISTER_CLASS_AND_BASE(ForceResetter,GlobalEngine);
};
REGISTER_SERIALIZABLE(ForceResetter);


