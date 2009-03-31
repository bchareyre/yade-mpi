// 2004 © Janek Kozicki <cosurgi@berlios.de> 
// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 


#include"ForceEngine.hpp"
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/LinearInterpolate.hpp>
#include<yade/extra/Shop.hpp>

YADE_PLUGIN("ForceEngine","InterpolatingDirectedForceEngine");

void ForceEngine::applyCondition(MetaBody* ncb){
	FOREACH(body_id_t id, subscribedBodies){
		assert(ncb->bodies->exists(id));
		ncb->bex.addForce(id,force);
	}
}

void InterpolatingDirectedForceEngine::applyCondition(MetaBody* rb){
	Real virtTime=wrap ? Shop::periodicWrap(rb->simulationTime,*times.begin(),*times.rbegin()) : rb->simulationTime;
	direction.Normalize(); 
	force=linearInterpolate<Real>(virtTime,times,magnitudes,_pos)*direction;
	ForceEngine::applyCondition(rb);
}

