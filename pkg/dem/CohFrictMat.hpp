/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once


#include<yade/pkg/common/ElastMat.hpp>


class CohFrictMat : public FrictMat
{
	public :
		virtual ~CohFrictMat ();

/// Serialization
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CohFrictMat,FrictMat,"",
		((bool,isCohesive,true,,""))
		((Real,alphaKr,2.0,,"Dimensionless rolling stiffness."))
		((Real,alphaKtw,2.0,,"Dimensionless twist stiffness."))
		((Real,etaRoll,-1.,,"Dimensionless rolling (aka 'bending') strength. If negative, rolling will be elastic."))	
		((Real,normalCohesion,0,,"Tensile strength, homogeneous to a pressure"))
		((Real,shearCohesion,0,,"Shear strength, homogeneous to a pressure"))
		((bool,momentRotationLaw,false,,"Use bending/twisting moment at contact. The contact will have moments only if both bodies have this flag true. See :yref:`CohFrictPhys::cohesionDisablesFriction` for details."))
		,
		createIndex();
		);
/// Indexable
	REGISTER_CLASS_INDEX(CohFrictMat,FrictMat);
};

REGISTER_SERIALIZABLE(CohFrictMat);


