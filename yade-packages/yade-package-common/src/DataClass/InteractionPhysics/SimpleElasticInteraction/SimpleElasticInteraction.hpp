/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMPLEELASTICINTERACTION_HPP
#define SIMPLEELASTICINTERACTION_HPP

#include <yade/yade-core/InteractionPhysics.hpp>

//class ip_simple_elastic : public interaction_physics
class SimpleElasticInteraction : public InteractionPhysics
{
	public :
		Real		 kn;				// normal elastic constant.
	
		virtual ~SimpleElasticInteraction();
	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(SimpleElasticInteraction);
	REGISTER_BASE_CLASS_NAME(InteractionPhysics);

};

REGISTER_SERIALIZABLE(SimpleElasticInteraction,false);

#endif // ELASTIC_CONTACT_PARAMETERS_HPP

