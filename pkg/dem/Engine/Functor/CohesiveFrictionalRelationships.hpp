/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@imag.fr>         *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>

class CohesiveFrictionalRelationships : public InteractionPhysicsFunctor
{
	public :
		CohesiveFrictionalRelationships();

		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);

		Real 		normalCohesion,
				shearCohesion;

		bool		setCohesionNow,
				setCohesionOnNewContacts;
				
		int cohesionDefinitionIteration;
		long iter;//REMOVE THIS

		REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,(normalCohesion)(shearCohesion)(setCohesionNow)(setCohesionOnNewContacts));
	FUNCTOR2D(CohesiveFrictionalBodyParameters,CohesiveFrictionalBodyParameters);
	REGISTER_CLASS_NAME(CohesiveFrictionalRelationships);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsFunctor);
};

REGISTER_SERIALIZABLE(CohesiveFrictionalRelationships);


