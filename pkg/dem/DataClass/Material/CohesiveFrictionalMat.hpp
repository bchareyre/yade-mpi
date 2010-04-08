/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@imag.fr>         *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once


#include<yade/pkg-common/ElastMat.hpp>


class CohesiveFrictionalMat : public FrictMat
{
	public :
		virtual ~CohesiveFrictionalMat ();

/// Serialization
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CohesiveFrictionalMat,FrictMat,"",
		((bool,isBroken,true,""))
		((bool,isCohesive,true,"")),
		createIndex();
					);
/// Indexable
	REGISTER_CLASS_INDEX(CohesiveFrictionalMat,FrictMat);
};

REGISTER_SERIALIZABLE(CohesiveFrictionalMat);


