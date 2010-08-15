// 2004 © Janek Kozicki <cosurgi@berlios.de> 
// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 


#include"ForceEngine.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/LinearInterpolate.hpp>
#include<yade/pkg-dem/Shop.hpp>

YADE_PLUGIN((ForceEngine)(InterpolatingDirectedForceEngine));

void ForceEngine::action(){
	FOREACH(Body::id_t id, subscribedBodies){
		assert(scene->bodies->exists(id));
		scene->forces.addForce(id,force);
	}
}

void InterpolatingDirectedForceEngine::action(){
	Real virtTime=wrap ? Shop::periodicWrap(scene->simulationTime,*times.begin(),*times.rbegin()) : scene->simulationTime;
	direction.normalize(); 
	force=linearInterpolate<Real>(virtTime,times,magnitudes,_pos)*direction;
	ForceEngine::action();
}


