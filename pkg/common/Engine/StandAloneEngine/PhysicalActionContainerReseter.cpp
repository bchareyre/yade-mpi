/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PhysicalActionContainerReseter.hpp"
#include<yade/core/MetaBody.hpp>


PhysicalActionContainerReseter::PhysicalActionContainerReseter() 
{
}

PhysicalActionContainerReseter::~PhysicalActionContainerReseter() 
{
}

void PhysicalActionContainerReseter::action(MetaBody* ncb)
{
	ncb->bex.reset();
}


YADE_PLUGIN();
