/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"TranslationEngine.hpp"
#include<yade/core/Scene.hpp>

void TranslationEngine::action(){
	const Real& dt=scene->dt;
	FOREACH(Body::id_t id,ids){
		assert(id<(Body::id_t)scene->bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(!b) continue;
		b->state->pos+=dt*velocity*translationAxis;
		b->state->vel=velocity*translationAxis;
	}
}

YADE_PLUGIN((TranslationEngine));
