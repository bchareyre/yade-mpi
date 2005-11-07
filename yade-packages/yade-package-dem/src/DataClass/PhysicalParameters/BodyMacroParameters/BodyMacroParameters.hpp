/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#ifndef SDECDISCRETEELEMENT_HPP
#define SDECDISCRETEELEMENT_HPP


#include <yade/yade-package-common/RigidBodyParameters.hpp>


class BodyMacroParameters : public RigidBodyParameters
{
	public :
		Real		 young
				,poisson
				,frictionAngle;

		BodyMacroParameters ();
		virtual ~BodyMacroParameters ();

/// Serialization
	REGISTER_CLASS_NAME(BodyMacroParameters);
	REGISTER_BASE_CLASS_NAME(RigidBodyParameters);

	public : void registerAttributes();

/// Indexable
	REGISTER_CLASS_INDEX(BodyMacroParameters,RigidBodyParameters);
};

REGISTER_SERIALIZABLE(BodyMacroParameters,false);

#endif // __SDECDISCRETEELEMENT_H__

