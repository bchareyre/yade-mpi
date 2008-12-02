/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ef1_BssSnowGrain_glDraw.hpp"
#include<yade/pkg-snow/BssSnowGrain.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>


void Ef1_BssSnowGrain_glDraw::go(const shared_ptr<InteractingGeometry>& cm, const shared_ptr<PhysicalParameters>& pp,bool wire)
{
	s.go(cm,pp,wire);
}


YADE_PLUGIN();


