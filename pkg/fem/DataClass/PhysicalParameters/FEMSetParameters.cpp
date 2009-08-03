/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FEMSetParameters.hpp"


FEMSetParameters::FEMSetParameters() : PhysicalParameters()
{
	createIndex();
	nodeGroupMask = 1;
	tetrahedronGroupMask = 1;
}


FEMSetParameters::~FEMSetParameters()
{

}



YADE_PLUGIN((FEMSetParameters));