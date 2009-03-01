/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@imag.fr>         *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>

class CohesiveFrictionalRelationships : public InteractionPhysicsEngineUnit
{
	public :
		CohesiveFrictionalRelationships();

		virtual void go(	const shared_ptr<PhysicalParameters>& b1,
					const shared_ptr<PhysicalParameters>& b2,
					const shared_ptr<Interaction>& interaction);

		Real 		normalCohesion,
				shearCohesion;

		bool		setCohesionNow,
				setCohesionOnNewContacts;
				
		int cohesionDefinitionIteration;
		long iter;//REMOVE THIS

	protected :
		virtual void registerAttributes();

	FUNCTOR2D(CohesiveFrictionalBodyParameters,CohesiveFrictionalBodyParameters);
	REGISTER_CLASS_NAME(CohesiveFrictionalRelationships);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsEngineUnit);

};

REGISTER_SERIALIZABLE(CohesiveFrictionalRelationships);


