// 2004 © Janek Kozicki <cosurgi@berlios.de> 
// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 


#include"ForceEngine.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/LinearInterpolate.hpp>
#include<yade/pkg-dem/Shop.hpp>

YADE_PLUGIN((ForceEngine)(InterpolatingDirectedForceEngine));
void ForceEngine::applyCondition(Scene*){
	FOREACH(body_id_t id, subscribedBodies){
		assert(scene->bodies->exists(id));
		scene->forces.addForce(id,force);
	}
}

void InterpolatingDirectedForceEngine::applyCondition(Scene* scene){
	Real virtTime=wrap ? Shop::periodicWrap(scene->simulationTime,*times.begin(),*times.rbegin()) : scene->simulationTime;
	direction.Normalize(); 
	force=linearInterpolate<Real>(virtTime,times,magnitudes,_pos)*direction;
	ForceEngine::applyCondition(scene);
}


