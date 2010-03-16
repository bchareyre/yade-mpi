/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ForceResetter.hpp"
#include<yade/core/Scene.hpp>

YADE_PLUGIN((ForceResetter));

void ForceResetter::action(Scene*){ scene->forces.reset(); }

