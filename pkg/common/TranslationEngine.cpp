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
	#ifdef YADE_OPENMP
	const long size=ids.size();
	#pragma omp parallel for schedule(static)
	for(long i=0; i<size; i++){
		const Body::id_t& id=ids[i];
	#else
	FOREACH(Body::id_t id,ids){
	#endif
		assert(id<(Body::id_t)scene->bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(!b) continue;
		b->state->vel=velocity*translationAxis;
	}
}

void HarmonicMotionEngine::action(){
	const Real& dt=scene->dt;
	const Real& time=scene->time;
	Vector3r w = f*2.0*Mathr::PI; 																										//Angular frequency
	Vector3r velocity = ((((w*time + fi).cwise().sin())*(-1.0)).cwise()*A).cwise()*w;	//Linear velocity at current time
	
	FOREACH(Body::id_t id,ids){
		assert(id<(Body::id_t)scene->bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(!b) continue;
		b->state->vel=velocity;
	}
}

YADE_PLUGIN((TranslationEngine)(HarmonicMotionEngine));
