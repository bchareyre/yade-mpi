/*************************************************************************
*  Copyright (C) 2010 by Jerome Duriez <jerome.duriez@hmg.inpg.fr>       *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once


#include<yade/pkg-common/ElastMat.hpp>


class NormalInelasticMat : public FrictMat
{
	public :
		virtual ~NormalInelasticMat ();

/// Serialization
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(NormalInelasticMat,FrictMat,"Material class for particles whose contact obey to a normal inelasticity (governed by this *coeff_dech*).",
		((Real,coeff_dech,1.0,"=kn(unload) / kn(load)"))
		,
		createIndex();
					);
/// Indexable
	REGISTER_CLASS_INDEX(NormalInelasticMat,FrictMat);
};

REGISTER_SERIALIZABLE(NormalInelasticMat);


