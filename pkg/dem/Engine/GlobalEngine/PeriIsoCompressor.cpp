
// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#include<yade/pkg-dem/PeriIsoCompressor.hpp>
#include<yade/pkg-dem/Shop.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/lib-pyutil/gil.hpp>

using namespace std;

YADE_PLUGIN((PeriIsoCompressor)(PeriTriaxController))


CREATE_LOGGER(PeriIsoCompressor);
void PeriIsoCompressor::action(){
	if(!scene->isPeriodic){ LOG_FATAL("Being used on non-periodic simulation!"); throw; }
	if(state>=stresses.size()) return;
	// initialize values
	if(charLen<=0){
		Bound* bv=Body::byId(0,scene)->bound.get();
		if(!bv){ LOG_FATAL("No charLen defined and body #0 has no bound"); throw; }
		const Vector3r sz=bv->max-bv->min;
		charLen=(sz[0]+sz[1]+sz[2])/3.;
		LOG_INFO("No charLen defined, taking avg bbox size of body #0 = "<<charLen);
	}
	if(maxSpan<=0){
		FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			if(!b || !b->bound) continue;
			for(int i=0; i<3; i++) maxSpan=max(maxSpan,b->bound->max[i]-b->bound->min[i]);
		}
		
	}
	if(maxDisplPerStep<0) maxDisplPerStep=1e-2*charLen; // this should be tuned somehow…
	const long& step=scene->currentIteration;
	Vector3r cellSize=scene->cell->getSize(); //unused: Real cellVolume=cellSize[0]*cellSize[1]*cellSize[2];
	Vector3r cellArea=Vector3r(cellSize[1]*cellSize[2],cellSize[0]*cellSize[2],cellSize[0]*cellSize[1]);
	Real minSize=min(cellSize[0],min(cellSize[1],cellSize[2])), maxSize=max(cellSize[0],max(cellSize[1],cellSize[2]));
	if(minSize<2.1*maxSpan){ throw runtime_error("Minimum cell size is smaller than 2.1*span_of_the_biggest_body! (periodic collider requirement)"); }
	if(((step%globalUpdateInt)==0) || avgStiffness<0 || sigma[0]<0 || sigma[1]<0 || sigma[2]<0){
		Vector3r sumForces=Shop::totalForceInVolume(avgStiffness,scene);
		sigma=-Vector3r(sumForces[0]/cellArea[0],sumForces[1]/cellArea[1],sumForces[2]/cellArea[2]);
		LOG_TRACE("Updated sigma="<<sigma<<", avgStiffness="<<avgStiffness);
	}
	Real sigmaGoal=stresses[state]; assert(sigmaGoal<0);
	// expansion of cell in this step (absolute length)
	Vector3r cellGrow(Vector3r::Zero());
	// is the stress condition satisfied in all directions?
	bool allStressesOK=true;
	if(keepProportions){ // the same algo as below, but operating on quantitites averaged over all dimensions
		Real sigAvg=(sigma[0]+sigma[1]+sigma[2])/3., avgArea=(cellArea[0]+cellArea[1]+cellArea[2])/3., avgSize=(cellSize[0]+cellSize[1]+cellSize[2])/3.;
		Real avgGrow=1e-4*(sigmaGoal-sigAvg)*avgArea/(avgStiffness>0?avgStiffness:1);
		Real maxToAvg=maxSize/avgSize;
		if(abs(maxToAvg*avgGrow)>maxDisplPerStep) avgGrow=Mathr::Sign(avgGrow)*maxDisplPerStep/maxToAvg;
		Real okGrow=-(minSize-2.1*maxSpan)/maxToAvg;
		if(avgGrow<okGrow) throw runtime_error("Unable to shring cell due to maximum body size (although required by stress condition). Increase particle rigidity, increase total sample dimensions, or decrease goal stress.");
		// avgGrow=max(avgGrow,-(minSize-2.1*maxSpan)/maxToAvg);
		if(avgStiffness>0) { sigma+=(avgGrow*avgStiffness)*Vector3r::Ones(); sigAvg+=avgGrow*avgStiffness; }
		if(abs((sigAvg-sigmaGoal)/sigmaGoal)>5e-3) allStressesOK=false;
		cellGrow=(avgGrow/avgSize)*cellSize;
	}
	else{ // handle each dimension separately
		for(int axis=0; axis<3; axis++){
			// Δσ=ΔεE=(Δl/l)×(l×K/A) ↔ Δl=Δσ×A/K
			// FIXME: either NormShearPhys::{kn,ks} is computed wrong or we have dimensionality problem here
			// FIXME: that is why the fixup 1e-4 is needed here
			// FIXME: or perhaps maxDisplaPerStep=1e-2*charLen is too big??
			cellGrow[axis]=1e-4*(sigmaGoal-sigma[axis])*cellArea[axis]/(avgStiffness>0?avgStiffness:1);  // FIXME: wrong dimensions? See PeriTriaxController
			if(abs(cellGrow[axis])>maxDisplPerStep) cellGrow[axis]=Mathr::Sign(cellGrow[axis])*maxDisplPerStep;
			cellGrow[axis]=max(cellGrow[axis],-(cellSize[axis]-2.1*maxSpan));
			// crude way of predicting sigma, for steps when it is not computed from intrs
			if(avgStiffness>0) sigma[axis]+=cellGrow[axis]*avgStiffness; // FIXME: dimensions
			if(abs((sigma[axis]-sigmaGoal)/sigmaGoal)>5e-3) allStressesOK=false;
		}
	}
	TRVAR4(cellGrow,sigma,sigmaGoal,avgStiffness);
	assert(scene->dt>0);
	for(int axis=0; axis<3; axis++){ scene->cell->velGrad(axis,axis)=cellGrow[axis]/(scene->dt*scene->cell->refSize[axis]); }
	// scene->cell->refSize+=cellGrow;

	// handle state transitions
	if(allStressesOK){
		if((step%globalUpdateInt)==0) currUnbalanced=Shop::unbalancedForce(/*useMaxForce=*/false,scene);
		if(currUnbalanced<maxUnbalanced){
			state+=1;
			// sigmaGoal reached and packing stable
			if(state==stresses.size()){ // no next stress to go for
				LOG_INFO("Finished");
				if(!doneHook.empty()){ LOG_DEBUG("Running doneHook: "<<doneHook); pyRunString(doneHook); }
			} else { LOG_INFO("Loaded to "<<sigmaGoal<<" done, going to "<<stresses[state]<<" now"); }
		} else {
			if((step%globalUpdateInt)==0) LOG_DEBUG("Stress="<<sigma<<", goal="<<sigmaGoal<<", unbalanced="<<currUnbalanced);
		}
	}
}

void PeriTriaxController::strainStressStiffUpdate(){
	// update strain first
	//"Natural" strain, correct for large deformations, only used for comparison with goals
	for (int i=0;i<3;i++) strain[i]=Mathr::Log(scene->cell->trsf(i,i));
	//stress tensor and stiffness
	
	//Compute volume of the deformed cell
	// NOTE : needs refSize, could be generalized to arbitrary initial shapes using trsf*refHsize 
	// → initial cell size is always box, and will be. The cell repeats  periodically, initial shape doesn't (shouldn't, at least) dinfluence interactions at all/
	// → this is one more place where Hsize would make things shorter : volume=Hsize.Determinant; The full code relies on the fact that initial Hsize is a box. You didn't modify the equation btw, result is the same as in r1936, which was (with spaces...)
	//trsf*Matrix3r ( scene->cell->refSize[0],0,0, 0,scene->cell->refSize[1],0,0,0,scene->cell->refSize[2] ) ).Determinant()
	//remark : the volume of a parallelepiped u1,u2,u3 is always det(u1,u2,u3)
	Real volume=scene->cell->trsf.determinant()*scene->cell->refSize[0]*scene->cell->refSize[1]*scene->cell->refSize[2];

	//Compute sum(fi*lj) and stiffness
	stressTensor = Matrix3r::Zero();
	Vector3r sumStiff(Vector3r::Zero());
	int n=0;
	// NOTE : This sort of loops on interactions could be removed if we had callbacks in e.g. constitutive laws
	// → very likely performance hit; do you have some concrete design in mind?
	// → a vector with functors so we can law->functs->pushback(myThing), and access to the fundamental members (forces, stiffness, normal, etc.). Implementing the second part is less clear in my mind. Inheriting from law::funct(force&, stiffness&, ...)?
	FOREACH(const shared_ptr<Interaction>&I, *scene->interactions){
		if ( !I->isReal() ) continue;
		NormShearPhys* nsi=YADE_CAST<NormShearPhys*> ( I->interactionPhysics.get() );
		GenericSpheresContact* gsc=YADE_CAST<GenericSpheresContact*> ( I->interactionGeometry.get() );
		//Contact force
		Vector3r f= ( reversedForces?-1.:1. ) * ( nsi->normalForce+nsi->shearForce );
		//branch vector, FIXME : the first definition generalizes to non-spherical bodies but needs wrapped coords.
		//    Vector3r branch=
		//   (reversedForces?-1.:1.)*(Body::byId(I->getId1())->state->pos-Body::byId(I->getId2())->state->pos);
		Vector3r branch= ( reversedForces?-1.:1. ) *gsc->normal* ( gsc->refR1+gsc->refR2 );
		// tensorial product f*branch
		stressTensor+=makeTensorProduct( f, branch );
		if( !dynCell )
		{
			for ( int i=0; i<3; i++ ) sumStiff[i]+=abs ( gsc->normal[i] ) *nsi->kn+ ( 1-abs ( gsc->normal[i] ) ) *nsi->ks;
			n++;
		}
	}
	// Compute stressTensor=sum(fi*lj)/Volume (Love equation)
	stressTensor /= volume;
	for(int axis=0; axis<3; axis++) stress[axis]=stressTensor(axis,axis);
	LOG_DEBUG ( "stressTensor : "<<endl
				<<stressTensor(0,0)<<" "<<stressTensor(0,1)<<" "<<stressTensor(0,2)<<endl
				<<stressTensor(1,0)<<" "<<stressTensor(1,1)<<" "<<stressTensor(1,2)<<endl
				<<stressTensor(2,0)<<" "<<stressTensor(2,1)<<" "<<stressTensor(2,2)<<endl
				<<"unbalanced = "<<Shop::unbalancedForce ( /*useMaxForce=*/false,scene ) );

	if (n>0) stiff=(1./n)*sumStiff;
	else stiff=Vector3r::Zero();
}


CREATE_LOGGER(PeriTriaxController);



void PeriTriaxController::action()
{
	if (!scene->isPeriodic){ throw runtime_error("PeriTriaxController run on aperiodic simulation."); }
	const Vector3r& cellSize=scene->cell->getSize();
	//FIXME : this is wrong I think (almost sure, B.)
	Vector3r cellArea=Vector3r(cellSize[1]*cellSize[2],cellSize[0]*cellSize[2],cellSize[0]*cellSize[1]);
	// initial updates
	const Vector3r& refSize=scene->cell->refSize;
	if (maxBodySpan[0]<=0){
		FOREACH(const shared_ptr<Body>& b,*scene->bodies){
			if(!b || !b->bound) continue;
			for(int i=0; i<3; i++) maxBodySpan[i]=max(maxBodySpan[i],b->bound->max[i]-b->bound->min[i]);
		}
	}
	// check current size
	if(2.1*maxBodySpan[0]>cellSize[0] || 2.1*maxBodySpan[1]>cellSize[1] || 2.1*maxBodySpan[2]>cellSize[2]){
		LOG_DEBUG("cellSize="<<cellSize<<", maxBodySpan="<<maxBodySpan);
		throw runtime_error("Minimum cell size is smaller than 2.1*maxBodySpan (periodic collider requirement)");
	}
	bool doUpdate((scene->currentIteration%globUpdate)==0);
	if(doUpdate || min(stiff[0],min(stiff[1],stiff[2])) <=0 || dynCell){ strainStressStiffUpdate(); }

	bool allOk=true;
	// apply condition along each axis separately (stress or strain)
	assert(scene->dt>0.);
	for(int axis=0; axis<3; axis++){
 		Real& strain_rate = scene->cell->velGrad(axis,axis);//strain rate on axis
		if(stressMask & (1<<axis)){   // control stress
			if(!dynCell){
				// stiffness K=EA; σ₁=goal stress; Δσ wanted stress difference to apply
				// ΔεE=(Δl/l)(K/A) - Grow is Δε, obtained by imposing the strain rate Δε/dt
				strain_rate=1/scene->dt*(goal[axis]-stress[axis])*cellArea[axis]/(stiff[axis]>0?stiff[axis]:1.);
				LOG_TRACE(axis<<": stress="<<stress[axis]<<", goal="<<goal[axis]<<", cellGrow="<<strain_rate*scene->dt);
			} else {  //accelerate the deformation using the density of the period, includes Cundall's damping
				assert( mass>0 );//user set
				Real dampFactor = 1 - growDamping*Mathr::Sign ( strain_rate * ( goal[axis]-stress[axis] ) );
				strain_rate+=dampFactor*scene->dt* ( goal[axis]-stress[axis] ) /mass;
				LOG_TRACE ( axis<<": stress="<<stress[axis]<<", goal="<<goal[axis]<<", velGrad="<<strain_rate );}
		} else {    // control strain, see "true strain" definition here http://en.wikipedia.org/wiki/Finite_strain_theory
			///NOTE : everything could be generalized to 9 independant components by comparing F[i,i] vs. Matrix3r goal[i,i], but it would be simpler in that case to let the user set the prescribed loading rates velGrad[i,i] when [i,i] is not stress-controlled. This "else" would disappear.
			strain_rate = (Mathr::Exp ( goal[axis]-strain[axis] ) -1)/scene->dt;
			LOG_TRACE ( axis<<": strain="<<strain[axis]<<", goal="<<goal[axis]<<", cellGrow="<<strain_rate*scene->dt);
		}		
		// steady evolution with fluctuations; see TriaxialStressController
		if (!dynCell) strain_rate=(1-growDamping)*strain_rate+.8*prevGrow[axis];
		// limit maximum strain rate
		if (abs(strain_rate)>maxStrainRate[axis]) strain_rate = Mathr::Sign(strain_rate)*maxStrainRate[axis];
		// do not shrink below minimum cell size (periodic collider condition), although it is suboptimal WRT resulting stress
		strain_rate=max(strain_rate,-(cellSize[axis]-2.1*maxBodySpan[axis])/scene->dt);

		// crude way of predicting stress, for steps when it is not computed from intrs
		if(doUpdate) LOG_DEBUG(axis<<": cellGrow="<<strain_rate*scene->dt<<", new stress="<<stress[axis]<<", new strain="<<strain[axis]);
		// used only for temporary goal comparisons. The exact value is assigned in strainStressStiffUpdate
		strain[axis]+=strain_rate*scene->dt;
		// signal if condition not satisfied
		if(stressMask&(1<<axis)){
			Real curr=stress[axis];
			if((goal[axis]!=0 && abs((curr-goal[axis])/goal[axis])>relStressTol) || abs(curr-goal[axis])>absStressTol) allOk=false;
		}else{
			Real curr=strain[axis];
			// since strain is prescribed exactly, tolerances need just to accomodate rounding issues
			if((goal[axis]!=0 && abs((curr-goal[axis])/goal[axis])>1e-6) || abs(curr-goal[axis])>1e-6){
				allOk=false;
				if(doUpdate) LOG_DEBUG("Strain not OK; "<<abs(curr-goal[axis])<<">1e-6");
			}
		}
	}
	// update stress and strain
	if (!dynCell) for ( int axis=0; axis<3; axis++ ){		
		// take in account something like poisson's effect here…
		//Real bogusPoisson=0.25; int ax1=(axis+1)%3,ax2=(axis+2)%3;
		//don't modify stress if dynCell, testing only stiff[axis]>0 would not allow switching the control mode in simulations,
		if (stiff[axis]>0) stress[axis]+=(scene->cell->velGrad[axis][axis]*scene->dt/refSize[axis])*(stiff[axis]/cellArea[axis]);
		//-bogusPoisson*(cellGrow[ax1]/refSize[ax1])*(stiff[ax1]/cellArea[ax1])-bogusPoisson*(cellGrow[ax2]/refSize[ax2])*(stiff[ax2]/cellArea[ax2]);
	}
 	for (int k=0;k<3;k++) strainRate[k]=scene->cell->velGrad(k,k);
	prevGrow = strainRate;
	
	if(allOk){
		if(doUpdate || currUnbalanced<0){
			currUnbalanced=Shop::unbalancedForce(/*useMaxForce=*/false,scene);
			LOG_DEBUG("Stress/strain="<<(stressMask&1?stress[0]:strain[0])<<","<<(stressMask&2?stress[1]:strain[1])<<","<<(stressMask&4?stress[2]:strain[2])<<", goal="<<goal<<", unbalanced="<<currUnbalanced );}
		if(currUnbalanced<maxUnbalanced){
			// LOG_INFO("Goal reached, packing stable.");
			if (!doneHook.empty()){
				LOG_DEBUG ( "Running doneHook: "<<doneHook );
				pyRunString(doneHook);}
			else { Omega::instance().stopSimulationLoop(); }
		}
	}
}
