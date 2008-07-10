/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FiltrEngine.hpp"

bool FiltrEngine::isFiltrationActivated = false;

FiltrEngine::FiltrEngine() : DeusExMachina(), isFilterActivated(false)
{
}

bool FiltrEngine::isActivated() 
{
	return FiltrEngine::isFiltrationActivated && isFilterActivated; 
}
