/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include <pkg/dem/deformablecohesive/CohesiveMat.hpp>
YADE_PLUGIN((CohesiveDeformableElementMaterial)(LinCohesiveElasticMaterial)(LinCohesiveStiffPropDampElastMat));

CohesiveDeformableElementMaterial::~CohesiveDeformableElementMaterial(void){}
LinCohesiveElasticMaterial::~LinCohesiveElasticMaterial(void){}
LinCohesiveStiffPropDampElastMat::~LinCohesiveStiffPropDampElastMat(void){}

