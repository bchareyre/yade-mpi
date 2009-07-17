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


#include<yade/pkg-dem/BodyMacroParameters.hpp>


class CohesiveFrictionalBodyParameters : public BodyMacroParameters
{
	public :
		bool		isBroken;
		bool		isCohesive;

		CohesiveFrictionalBodyParameters ();
		virtual ~CohesiveFrictionalBodyParameters ();

/// Serialization
	REGISTER_CLASS_NAME(CohesiveFrictionalBodyParameters);
	REGISTER_BASE_CLASS_NAME(BodyMacroParameters);
	REGISTER_ATTRIBUTES(BodyMacroParameters,(isBroken)(isCohesive));
/// Indexable
	REGISTER_CLASS_INDEX(CohesiveFrictionalBodyParameters,BodyMacroParameters);
};

REGISTER_SERIALIZABLE(CohesiveFrictionalBodyParameters);


