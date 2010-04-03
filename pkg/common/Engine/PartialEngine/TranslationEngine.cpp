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
	Real dt=Omega::instance().getTimeStep();
	FOREACH(body_id_t id,subscribedBodies){
		assert(id<(body_id_t)scene->bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(!b) continue;
		b->state->pos+=dt*velocity*translationAxis;
		b->state->vel=velocity*translationAxis;
	}
}

YADE_PLUGIN((TranslationEngine));
