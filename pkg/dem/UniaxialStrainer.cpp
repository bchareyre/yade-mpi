// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"UniaxialStrainer.hpp"

#include<core/Scene.hpp>
#include<core/InteractionContainer.hpp>
#include<pkg/common/Aabb.hpp>

YADE_PLUGIN((UniaxialStrainer));
/************************ UniaxialStrainer **********************/
CREATE_LOGGER(UniaxialStrainer);

void UniaxialStrainer::init(){
	needsInit=false;

	assert(posIds.size()>0);
	assert(negIds.size()>0);
	posCoords.clear(); negCoords.clear();
	FOREACH(Body::id_t id,posIds){ const shared_ptr<Body>& b=Body::byId(id,scene); posCoords.push_back(b->state->pos[axis]);
		if(blockDisplacements && blockRotations) b->state->blockedDOFs=State::DOF_ALL;
		else{
			if(!blockDisplacements) b->state->blockedDOFs=State::axisDOF(axis); else b->state->blockedDOFs=State::DOF_XYZ;
			if(blockRotations) b->state->blockedDOFs|=State::DOF_RXRYRZ;
		}
	}
	FOREACH(Body::id_t id,negIds){ const shared_ptr<Body>& b=Body::byId(id,scene); negCoords.push_back(b->state->pos[axis]);
		if(blockDisplacements && blockRotations) b->state->blockedDOFs=State::DOF_ALL;
		else{
			if(!blockDisplacements) b->state->blockedDOFs=State::axisDOF(axis); else b->state->blockedDOFs=State::DOF_XYZ;
			if(blockRotations) b->state->blockedDOFs|=State::DOF_RXRYRZ;
		}
	}

	assert(posIds.size()==posCoords.size() && negIds.size()==negCoords.size());

	originalLength=axisCoord(posIds[0])-axisCoord(negIds[0]);
	LOG_DEBUG("Reference particles: positive #"<<posIds[0]<<" at "<<axisCoord(posIds[0])<<"; negative #"<<negIds[0]<<" at "<<axisCoord(negIds[0]));
	LOG_INFO("Setting initial length to "<<originalLength<<" (between #"<<negIds[0]<<" and #"<<posIds[0]<<")");
	if(originalLength<=0) throw runtime_error(("UniaxialStrainer: Initial length is negative or zero (swapped reference particles?)! "+boost::lexical_cast<string>(originalLength)).c_str());
	/* this happens is nan propagates from e.g. brefcom consitutive law in case 2 bodies have _exactly_ the same position
	 * (the the normal strain is 0./0.=nan). That is an user's error, however and should not happen. */
	if(std::isnan(originalLength)) throw logic_error("UniaxialStrainer: Initial length is NaN!");
	assert(originalLength>0 && !std::isnan(originalLength));

	assert(!std::isnan(strainRate) || !std::isnan(absSpeed));
	if(!std::isnan(std::numeric_limits<Real>::quiet_NaN())){ throw runtime_error("UniaxialStrainer: NaN's are not properly supported (compiled with -ffast-math?), which is required."); }

	if(std::isnan(strainRate)){ strainRate=absSpeed/originalLength; LOG_INFO("Computed new strainRate "<<strainRate); }
	else {absSpeed=strainRate*originalLength;}

	if(!setSpeeds){
		initAccelTime_s=initAccelTime>=0 ? initAccelTime : scene->dt*(-initAccelTime);
		LOG_INFO("Strain speed will be "<<absSpeed<<", strain rate "<<strainRate<<", will be reached after "<<initAccelTime_s<<"s ("<<initAccelTime_s/scene->dt<<" steps).");
	} else {
		/* set speed such that it is linear on the strained axis; transversal speed is not set, which can perhaps create some problems.
			Note: all bodies in the simulation will have their speed set, since there is no way to tell which ones are part of the specimen
			and which are not.

			Speeds will be linearly interpolated beween axis positions p0,p1 and velocities v0,v1.
		*/
		initAccelTime_s=0;
		LOG_INFO("Strain speed will be "<<absSpeed<<", strain rate "<<strainRate<<"; velocities will be set directly at the beginning.");
		Real p0=axisCoord(negIds[0]), p1=axisCoord(posIds[0]); // limit positions
		Real v0,v1; // speeds at p0, p1
		switch(asymmetry){
			case -1: v0=-absSpeed; v1=0; break;
			case  0: v0=-absSpeed/2; v1=absSpeed/2; break;
			case  1: v0=0; v1=absSpeed; break;
			default: throw std::invalid_argument(("UniaxialStrainer: unknown asymmetry value "+boost::lexical_cast<string>(asymmetry)+" (should be -1,0,1)").c_str());
		}
		assert(p1>p0);
		// set speeds for particles on the boundary
		FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			// skip bodies on the boundary, since those will have their positions updated directly
			if(std::find(posIds.begin(),posIds.end(),b->id)!=posIds.end() || std::find(negIds.begin(),negIds.end(),b->id)!=negIds.end()) { continue; }
			Real p=axisCoord(b->id);
			Real pNormalized=(p-p0)/(p1-p0);
			b->state->vel[axis]=pNormalized*(v1-v0)+v0;
		}
	}
	if(std::isnan(crossSectionArea)){ throw std::invalid_argument("UniaxialStrain.crossSectionArea must be specified."); }
}

void UniaxialStrainer::action(){
	if(needsInit) init();
	// postconditions for initParams
	assert(posIds.size()==posCoords.size() && negIds.size()==negCoords.size() && originalLength>0 && crossSectionArea>0);
	//nothing to do
	if(posIds.size()==0 || negIds.size()==0) return;
	// linearly increase strain to the desired value
	if(std::abs(currentStrainRate)<std::abs(strainRate)){
		if(initAccelTime_s!=0) currentStrainRate=(scene->time/initAccelTime_s)*strainRate;
		else currentStrainRate=strainRate;
	} else currentStrainRate=strainRate;
	// how much do we move (in total, symmetry handled below)
	Real dAX=currentStrainRate*originalLength*scene->dt;
	if(!std::isnan(stopStrain)){
		Real axialLength=axisCoord(posIds[0])-axisCoord(negIds[0]);
		Real newStrain=(axialLength+dAX)/originalLength-1;
		if((newStrain*stopStrain>0) && std::abs(newStrain)>=stopStrain){ // same sign of newStrain and stopStrain && over the limit from below in abs values
			dAX=originalLength*(stopStrain+1)-axialLength;
			LOG_INFO("Reached stopStrain "<<stopStrain<<", deactivating self and stopping in "<<idleIterations+1<<" iterations.");
			this->active=false;
			scene->stopAtIter=scene->iter+1+idleIterations;
		}
	}
	if(asymmetry==0) dAX*=.5; // apply half on both sides if straining symetrically
	if(asymmetry!=1){
		for(size_t i=0; i<negIds.size(); i++){
			negCoords[i]-=dAX;
			//axisCoord(negIds[i])=negCoords[i]; // this line that modifies state->pos is useless with curent velocity defined below, and use of NewtonIntegrator
			axisVel(negIds[i]) = -dAX/scene->dt; // update current position
		}
	}
	if(asymmetry!=-1){
		for(size_t i=0; i<posIds.size(); i++){
			posCoords[i]+=dAX;
			//axisCoord(posIds[i])=posCoords[i]; // idem
			axisVel(posIds[i]) = dAX/scene->dt;
		}
	}

	Real axialLength=axisCoord(posIds[0])-axisCoord(negIds[0]);
	strain=axialLength/originalLength-1;

	// reverse if we're over the limit strain
	if(notYetReversed && limitStrain!=0 && ((currentStrainRate>0 && strain>limitStrain) || (currentStrainRate<0 && strain<limitStrain))) { currentStrainRate*=-1; notYetReversed=false; LOG_INFO("Reversed strain rate to "<<currentStrainRate); }

	// update forces and stresses
	if(scene->iter%stressUpdateInterval==0) {
		computeAxialForce();
		avgStress=(sumPosForces+sumNegForces)/(2*crossSectionArea); // average nominal stress
	}
}

void UniaxialStrainer::computeAxialForce(){
	sumPosForces=sumNegForces=0;
	scene->forces.sync();
	FOREACH(Body::id_t id, negIds) sumNegForces+=scene->forces.getForce(id)[axis];
	FOREACH(Body::id_t id, posIds) sumPosForces-=scene->forces.getForce(id)[axis];
}
