
// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#include<yade/pkg-dem/PeriIsoCompressor.hpp>
#include<yade/pkg-dem/Shop.hpp>
#include<yade/core/Scene.hpp>

using namespace std;

YADE_PLUGIN((PeriIsoCompressor))


CREATE_LOGGER(PeriIsoCompressor);
void PeriIsoCompressor::action(Scene* rb){
	if(!rb->isPeriodic){ LOG_FATAL("Being used on non-periodic simulation!"); throw; }
	if(state>=stresses.size()) return;
	// initialize values
	if(charLen<=0){
		Bound* bv=Body::byId(0,rb)->bound.get();
		if(!bv){ LOG_FATAL("No charLen defined and body #0 has no bound"); throw; }
		const Vector3r sz=bv->max-bv->min;
		charLen=(sz[0]+sz[1]+sz[2])/3.;
		LOG_INFO("No charLen defined, taking avg bbox size of body #0 = "<<charLen);
	}
	if(maxSpan<=0){
		FOREACH(const shared_ptr<Body>& b, *rb->bodies){
			if(!b->bound) continue;
			for(int i=0; i<3; i++) maxSpan=max(maxSpan,b->bound->max[i]-b->bound->min[i]);
		}
		
	}
	if(maxDisplPerStep<0) maxDisplPerStep=1e-2*charLen; // this should be tuned somehow…
	const long& step=rb->currentIteration;
	Vector3r cellSize=rb->cellMax-rb->cellMin; //unused: Real cellVolume=cellSize[0]*cellSize[1]*cellSize[2];
	Vector3r cellArea=Vector3r(cellSize[1]*cellSize[2],cellSize[0]*cellSize[2],cellSize[0]*cellSize[1]);
	Real minSize=min(cellSize[0],min(cellSize[1],cellSize[2])), maxSize=max(cellSize[0],max(cellSize[1],cellSize[2]));
	if(minSize<2.1*maxSpan){ throw runtime_error("Minimum cell size is smaller than 2.1*span_of_the_biggest_body! (periodic collider requirement)"); }
	if(((step%globalUpdateInt)==0) || avgStiffness<0 || sigma[0]<0 || sigma[1]<0 || sigma[2]<0){
		Vector3r sumForces=Shop::totalForceInVolume(avgStiffness,rb);
		sigma=Vector3r(sumForces[0]/cellArea[0],sumForces[1]/cellArea[1],sumForces[2]/cellArea[2]);
		LOG_TRACE("Updated sigma="<<sigma<<", avgStiffness="<<avgStiffness);
	}
	Real sigmaGoal=stresses[state]; assert(sigmaGoal>0);
	// expansion of cell in this step (absolute length)
	Vector3r cellGrow(Vector3r::ZERO);
	// is the stress condition satisfied in all directions?
	bool allStressesOK=true;
	if(keepProportions){ // the same algo as below, but operating on quantitites averaged over all dimensions
		Real sigAvg=(sigma[0]+sigma[1]+sigma[2])/3., avgArea=(cellArea[0]+cellArea[1]+cellArea[2])/3., avgSize=(cellSize[0]+cellSize[1]+cellSize[2])/3.;
		Real avgGrow=1e-4*(sigAvg-sigmaGoal)*avgArea/(avgStiffness>0?avgStiffness:1);
		Real maxToAvg=maxSize/avgSize;
		if(abs(maxToAvg*avgGrow)>maxDisplPerStep) avgGrow=Mathr::Sign(avgGrow)*maxDisplPerStep/maxToAvg;
		avgGrow=max(avgGrow,-(minSize-2.1*maxSpan)/maxToAvg);
		if(avgStiffness>0) { sigma-=(avgGrow*avgStiffness)*Vector3r::ONE; sigAvg-=avgGrow*avgStiffness; }
		if(abs((sigAvg-sigmaGoal)/sigmaGoal)>5e-3) allStressesOK=false;
		cellGrow=(avgGrow/avgSize)*cellSize;
	}
	else{ // handle each dimension separately
		for(int axis=0; axis<3; axis++){
			// Δσ=ΔεE=(Δl/l)×(l×K/A) ↔ Δl=Δσ×A/K
			// FIXME: either NormalShearInteraction::{kn,ks} is computed wrong or we have dimensionality problem here
			// FIXME: that is why the fixup 1e-4 is needed here
			// FIXME: or perhaps maxDisplaPerStep=1e-2*charLen is too big??
			cellGrow[axis]=1e-4*(sigma[axis]-sigmaGoal)*cellArea[axis]/(avgStiffness>0?avgStiffness:1);
			if(abs(cellGrow[axis])>maxDisplPerStep) cellGrow[axis]=Mathr::Sign(cellGrow[axis])*maxDisplPerStep;
			cellGrow[axis]=max(cellGrow[axis],-(cellSize[axis]-2.1*maxSpan));
			// crude way of predicting sigma, for steps when it is not computed from intrs
			if(avgStiffness>0) sigma[axis]-=cellGrow[axis]*avgStiffness;
			if(abs((sigma[axis]-sigmaGoal)/sigmaGoal)>5e-3) allStressesOK=false;
		}
	}
	TRVAR4(cellGrow,sigma,sigmaGoal,avgStiffness);
	rb->cellMin-=.5*cellGrow; rb->cellMax+=.5*cellGrow;
	// handle state transitions
	if(allStressesOK){
		if((step%globalUpdateInt)==0) currUnbalanced=Shop::unbalancedForce(/*useMaxForce=*/false,rb);
		if(currUnbalanced<maxUnbalanced){
			state+=1;
			// sigmaGoal reached and packing stable
			if(state==stresses.size()){ // no next stress to go for
				LOG_INFO("Finished");
				if(!doneHook.empty()){ LOG_DEBUG("Running doneHook: "<<doneHook); PyGILState_STATE gstate; gstate=PyGILState_Ensure(); PyRun_SimpleString(doneHook.c_str()); PyGILState_Release(gstate); }
			} else { LOG_INFO("Loaded to "<<sigmaGoal<<" done, going to "<<stresses[state]<<" now"); }
		} else {
			if((step%globalUpdateInt)==0) LOG_DEBUG("Stress="<<sigma<<", goal="<<sigmaGoal<<", unbalanced="<<currUnbalanced);
		}
	}
}


