/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMPLECONTACTMODEL_HPP
#define SIMPLECONTACTMODEL_HPP

#include <yade/yade-package-common/InteractionPhysicsEngineUnit.hpp>

class SimpleElasticRelationships : public InteractionPhysicsEngineUnit
{
	public :
		SimpleElasticRelationships();

		virtual void go(	const shared_ptr<PhysicalParameters>& b1,
					const shared_ptr<PhysicalParameters>& b2,
					const shared_ptr<Interaction>& interaction);

	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(SimpleElasticRelationships);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsEngineUnit);

};

REGISTER_SERIALIZABLE(SimpleElasticRelationships,false);

#endif // SIMPLECONTACTMODEL_HPP

