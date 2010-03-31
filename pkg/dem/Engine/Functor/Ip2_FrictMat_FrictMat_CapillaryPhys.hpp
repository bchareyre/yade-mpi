/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>

class Ip2_FrictMat_FrictMat_CapillaryPhys : public InteractionPhysicsFunctor
{
	public :
		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);

	FUNCTOR2D(FrictMat,FrictMat);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Ip2_FrictMat_FrictMat_CapillaryPhys,InteractionPhysicsFunctor, "RelationShips to use with Law2_ScGeom_CapillaryPhys_Capillarity\n\n In these RelationShips all the interaction attributes are computed. \n\n.. warning::\n\tas in the others :yref:`Ip2 functors<InteractionPhysicsFunctor>`, most of the attributes are computed only once, when the interaction is new.",,;);
	
};
REGISTER_SERIALIZABLE(Ip2_FrictMat_FrictMat_CapillaryPhys);



