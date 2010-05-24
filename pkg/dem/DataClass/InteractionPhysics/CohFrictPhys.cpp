/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CohFrictPhys.hpp"

void CohFrictPhys::SetBreakingState()
{	
	if (fragile) {
		cohesionBroken = true;
		normalAdhesion = 0;
		shearAdhesion = 0;}	
}

CohFrictPhys::~CohFrictPhys()
{
}
YADE_PLUGIN((CohFrictPhys));


