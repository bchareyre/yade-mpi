// 2004 © Janek Kozicki <cosurgi@berlios.de> 
// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 


#include"ForceEngine.hpp"
#include<yade/core/Scene.hpp>
#include<yade/lib/smoothing/LinearInterpolate.hpp>
#include<yade/pkg/dem/Shop.hpp>

#include<yade/core/IGeom.hpp>
#include<yade/core/IPhys.hpp>

YADE_PLUGIN((ForceEngine)(InterpolatingDirectedForceEngine)(RadialForceEngine));

void ForceEngine::action(){
	FOREACH(Body::id_t id, ids){
		assert(scene->bodies->exists(id));
		scene->forces.addForce(id,force);
	}
}

void InterpolatingDirectedForceEngine::action(){
	Real virtTime=wrap ? Shop::periodicWrap(scene->time,*times.begin(),*times.rbegin()) : scene->time;
	direction.normalize(); 
	force=linearInterpolate<Real,Real>(virtTime,times,magnitudes,_pos)*direction;
	ForceEngine::action();
}

void RadialForceEngine::postLoad(RadialForceEngine&){ axisDir.normalize(); }

void RadialForceEngine::action(){
	FOREACH(Body::id_t id, ids){
		assert(scene->bodies->exists(id));
		const Vector3r& pos=Body::byId(id,scene)->state->pos;
		Vector3r radial=(pos - (axisPt+axisDir * /* t */ ((pos-axisPt).dot(axisDir)))).normalized();
		if(radial.squaredNorm()==0) continue;
		scene->forces.addForce(id,fNorm*radial);
	}
}
