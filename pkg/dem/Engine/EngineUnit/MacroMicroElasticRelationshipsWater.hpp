/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SDECLINEARCONTACTMODEL_HPP
#define SDECLINEARCONTACTMODEL_HPP

#include <yade/pkg-common/InteractionPhysicsEngineUnit.hpp>

class MacroMicroElasticRelationshipsWater : public InteractionPhysicsEngineUnit
{
	public :
		Real		 alpha
				,beta
				,gamma;

		MacroMicroElasticRelationshipsWater();

		virtual void go(	const shared_ptr<PhysicalParameters>& b1,
					const shared_ptr<PhysicalParameters>& b2,
					const shared_ptr<Interaction>& interaction);

	protected :
		virtual void registerAttributes();

	FUNCTOR2D(BodyMacroParameters,BodyMacroParameters);
	
	REGISTER_CLASS_NAME(MacroMicroElasticRelationshipsWater);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsEngineUnit);

};

REGISTER_SERIALIZABLE(MacroMicroElasticRelationshipsWater);

#endif // __SDECLINEARCONTACTMODEL_HPP__

