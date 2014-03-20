/*************************************************************************
*  Copyright (C) 2009-2012 by Franck Lominé		                         *
*  franck.lomine@insa-rennes.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v3 or later. See file LICENSE for details. *
*                                                                        *
*************************************************************************/
#ifdef LBM_ENGINE

#include "LBMlink.hpp"

YADE_PLUGIN((LBMlink));
LBMlink::~LBMlink(){};
void LBMlink::ReinitDynamicalProperties(){sid=-1;fid=-1;idx_sigma_i=-1;isBd=false;VbMid=Vector3r::Zero();DistMid=Vector3r::Zero();ct=0.;return;}

#endif //LBM_ENGINE
