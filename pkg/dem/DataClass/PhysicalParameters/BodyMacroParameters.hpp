/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#ifndef BODYMACROPARAMETERS_HPP
#define BODYMACROPARAMETERS_HPP


#include<yade/pkg-common/ElasticBodyParameters.hpp>


class BodyMacroParameters : public ElasticBodyParameters
{
	public :
		Real		 poisson
				,frictionAngle;

		BodyMacroParameters ();
		virtual ~BodyMacroParameters ();

/// Serialization
	REGISTER_CLASS_NAME(BodyMacroParameters);
	REGISTER_BASE_CLASS_NAME(ElasticBodyParameters);

	public : void registerAttributes();

/// Indexable
	REGISTER_CLASS_INDEX(BodyMacroParameters,ElasticBodyParameters);
};

REGISTER_SERIALIZABLE(BodyMacroParameters,false);

#endif // __SDECDISCRETEELEMENT_H__

