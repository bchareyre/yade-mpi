/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#ifndef ELASTICBODYPARAMETERS_HPP
#define ELASTICBODYPARAMETERS_HPP


#include "RigidBodyParameters.hpp"


class ElasticBodyParameters : public RigidBodyParameters
{
	public :
		Real		 young;

		ElasticBodyParameters ();
		virtual ~ElasticBodyParameters ();

/// Serialization
	REGISTER_CLASS_NAME(ElasticBodyParameters);
	REGISTER_BASE_CLASS_NAME(RigidBodyParameters);

	public : void registerAttributes();

/// Indexable
	REGISTER_CLASS_INDEX(ElasticBodyParameters,RigidBodyParameters);
};

REGISTER_SERIALIZABLE(ElasticBodyParameters);

#endif 

