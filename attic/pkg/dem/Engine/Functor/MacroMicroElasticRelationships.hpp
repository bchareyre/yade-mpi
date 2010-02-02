/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>

class MacroMicroElasticRelationships : public InteractionPhysicsFunctor
{
	public :
		Real		 alpha
				,beta
				,gamma;

		MacroMicroElasticRelationships();

		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);

	FUNCTOR2D(BodyMacroParameters,BodyMacroParameters);
	REGISTER_CLASS_NAME(MacroMicroElasticRelationships);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsFunctor);
	REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,(alpha)(beta)(gamma));
};

REGISTER_SERIALIZABLE(MacroMicroElasticRelationships);


