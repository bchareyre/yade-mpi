/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include <core/Material.hpp>
#include <limits>
/*! Elastic material */
class CohesiveDeformableElementMaterial: public Material{
	public:
	virtual ~CohesiveDeformableElementMaterial();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CohesiveDeformableElementMaterial,Material,"Deformable Element Material.",
	//	((Real,density,1,,"Density of the material."))
			,
		/*ctor*/ createIndex();
	);
	REGISTER_CLASS_INDEX(CohesiveDeformableElementMaterial,Material);
};
REGISTER_SERIALIZABLE(CohesiveDeformableElementMaterial);

class LinCohesiveElasticMaterial: public CohesiveDeformableElementMaterial{
	public:
	virtual ~LinCohesiveElasticMaterial();
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(LinCohesiveElasticMaterial,CohesiveDeformableElementMaterial,"Linear Isotropic Elastic material",
			((Real,youngmodulus,.78e5,,"Young's modulus. Initially aluminium."))
			((Real,poissonratio,.33,,"Poisson ratio. Initially aluminium.")),
			createIndex();
		);
		REGISTER_CLASS_INDEX(LinCohesiveElasticMaterial,CohesiveDeformableElementMaterial);
};
REGISTER_SERIALIZABLE(LinCohesiveElasticMaterial);


/*Stiffness proportional damping material*/
class LinCohesiveStiffPropDampElastMat: public LinCohesiveElasticMaterial{
	public:
	virtual ~LinCohesiveStiffPropDampElastMat();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(LinCohesiveStiffPropDampElastMat,LinCohesiveElasticMaterial,"Elastic material with Rayleigh Damping.",
		((Real,alpha,0,,"Mass propotional damping constant of Rayleigh Damping."))
		((Real,beta,0,,"Stiffness propotional damping constant of Rayleigh Damping.")),
		createIndex();
	);
	REGISTER_CLASS_INDEX(LinCohesiveStiffPropDampElastMat,LinCohesiveElasticMaterial);
};
REGISTER_SERIALIZABLE(LinCohesiveStiffPropDampElastMat);
