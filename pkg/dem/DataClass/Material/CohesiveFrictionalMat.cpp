/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@imag.fr>         *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CohesiveFrictionalMat.hpp"

CohesiveFrictionalMat::CohesiveFrictionalMat () : FrictMat()
{
	createIndex();
	isBroken=true;
	isCohesive=true;
}

CohesiveFrictionalMat::~CohesiveFrictionalMat()
{
}

YADE_PLUGIN((CohesiveFrictionalMat));



