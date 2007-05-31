/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef CAPILLARY_PARAMETERS_HPP
#define CAPILLARY_PARAMETERS_HPP

//#include <yade/core/InteractionPhysics.hpp>
#include <yade/pkg-dem/ElasticContactInteraction.hpp>

class CapillaryParameters : public ElasticContactInteraction
{
	public :CapillaryParameters();

		Real		 Vmeniscus
				,CapillaryPressure
				,Delta1
				,Delta2;
				
		Vector3r	Fcap;
						
		bool 		meniscus;
		int		currentIndexes [4];// used for faster interpolation (stores previous positions in tables)
				
		virtual ~CapillaryParameters();
	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(CapillaryParameters);
	REGISTER_BASE_CLASS_NAME(ElasticContactInteraction);

};

REGISTER_SERIALIZABLE(CapillaryParameters,false);

#endif // CAPILLARY_PARAMETERS_HPP

