/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once


#include<yade/pkg-common/ElasticMat.hpp>


class CohesiveFrictionalBodyParameters : public GranularMat
{
	public :
		bool		isBroken;
		bool		isCohesive;

		CohesiveFrictionalBodyParameters ();
		virtual ~CohesiveFrictionalBodyParameters ();

/// Serialization
	REGISTER_CLASS_NAME(CohesiveFrictionalBodyParameters);
	REGISTER_BASE_CLASS_NAME(GranularMat);
	REGISTER_ATTRIBUTES(GranularMat,(isBroken)(isCohesive));
/// Indexable
	REGISTER_CLASS_INDEX(CohesiveFrictionalBodyParameters,GranularMat);
};

REGISTER_SERIALIZABLE(CohesiveFrictionalBodyParameters);


