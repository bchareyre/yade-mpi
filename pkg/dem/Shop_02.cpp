// 2007 © Václav Šmilauer <eudoxos@arcig.cz>
#include"Shop.hpp"

#include<core/Scene.hpp>
#include<core/Body.hpp>
#include<core/Interaction.hpp>

#include<pkg/common/Aabb.hpp>
#include<core/Clump.hpp>
#include<pkg/common/InsertionSortCollider.hpp>

#include<pkg/common/Box.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/common/ElastMat.hpp>
#include<pkg/dem/ViscoelasticPM.hpp>
#include<pkg/dem/CapillaryPhys.hpp>

#include<pkg/common/Bo1_Aabb.hpp>
#include<pkg/dem/NewtonIntegrator.hpp>
#include<pkg/dem/Ig2_Sphere_Sphere_ScGeom.hpp>
#include<pkg/dem/Ig2_Box_Sphere_ScGeom.hpp>
#include<pkg/dem/FrictPhys.hpp>

#include<pkg/common/ForceResetter.hpp>

#include<pkg/common/Dispatching.hpp>
#include<pkg/common/InteractionLoop.hpp>
#include<pkg/common/GravityEngines.hpp>

#include<pkg/dem/GlobalStiffnessTimeStepper.hpp>
#include<pkg/dem/ElasticContactLaw.hpp>

#include<pkg/dem/ScGeom.hpp>
#include<pkg/dem/FrictPhys.hpp>
#include<pkg/dem/HertzMindlin.hpp>

#include<pkg/common/Grid.hpp>

#include<pkg/dem/Tetra.hpp>

#ifdef YADE_OPENGL
	#include<pkg/common/Gl1_NormPhys.hpp>
#endif

CREATE_LOGGER(Shop);

/*! Flip periodic cell by given number of cells.

Still broken, some interactions are missed. Should be checked.
*/

/* Detremination of time step as according to Rayleigh wave speed of force propagation (see Thornton 2000, ref. MillerPursey_1955) */
Real Shop::RayleighWaveTimeStep(const shared_ptr<Scene> _rb){
	shared_ptr<Scene> rb=(_rb?_rb:Omega::instance().getScene());
	Real dt=std::numeric_limits<Real>::infinity();
	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
		if(!b || !b->material || !b->shape) continue;
		
		shared_ptr<ElastMat> ebp=YADE_PTR_DYN_CAST<ElastMat>(b->material);
		shared_ptr<Sphere> s=YADE_PTR_DYN_CAST<Sphere>(b->shape);
		if(!ebp || !s) continue;
		
		Real density=b->state->mass/((4/3.)*Mathr::PI*pow(s->radius,3));
		Real ShearModulus=ebp->young/(2.*(1+ebp->poisson));
		Real lambda=0.1631*ebp->poisson+0.876605;
		dt=min(dt,Mathr::PI*s->radius/lambda*sqrt(density/ShearModulus));
	}
	return dt;
}

/* Project 3d point into 2d using spiral projection along given axis;
 * the returned tuple is
 *
 *  (height relative to the spiral, distance from axis, theta )
 *
 * dH_dTheta is the inclination of the spiral (height increase per radian),
 * theta0 is the angle for zero height (by given axis).
 */
boost::tuple<Real,Real,Real> Shop::spiralProject(const Vector3r& pt, Real dH_dTheta, int axis, Real periodStart, Real theta0){
	int ax1=(axis+1)%3,ax2=(axis+2)%3;
	Real r=sqrt(pow(pt[ax1],2)+pow(pt[ax2],2));
	Real theta;
	if(r>Mathr::ZERO_TOLERANCE){
		theta=acos(pt[ax1]/r);
		if(pt[ax2]<0) theta=Mathr::TWO_PI-theta;
	}
	else theta=0;
	Real hRef=dH_dTheta*(theta-theta0);
	long period;
	if(std::isnan(periodStart)){
		Real h=Shop::periodicWrap(pt[axis]-hRef,hRef-Mathr::PI*dH_dTheta,hRef+Mathr::PI*dH_dTheta,&period);
		return boost::make_tuple(r,h,theta);
	}
	else{
		// Real hPeriodStart=(periodStart-theta0)*dH_dTheta;
		//Real h=Shop::periodicWrap(pt[axis]-hRef,hPeriodStart,hPeriodStart+2*Mathr::PI*dH_dTheta,&period);
		theta=Shop::periodicWrap(theta,periodStart,periodStart+2*Mathr::PI,&period);
		Real h=pt[axis]-hRef+period*2*Mathr::PI*dH_dTheta;
		return boost::make_tuple(r,h,theta);
	}
}

shared_ptr<Interaction> Shop::createExplicitInteraction(Body::id_t id1, Body::id_t id2, bool force){
	IGeomDispatcher* geomMeta=NULL;
	IPhysDispatcher* physMeta=NULL;
	shared_ptr<Scene> rb=Omega::instance().getScene();
	if(rb->interactions->find(Body::id_t(id1),Body::id_t(id2))!=0) throw runtime_error(string("Interaction #")+boost::lexical_cast<string>(id1)+"+#"+boost::lexical_cast<string>(id2)+" already exists.");
	FOREACH(const shared_ptr<Engine>& e, rb->engines){
		if(!geomMeta) { geomMeta=dynamic_cast<IGeomDispatcher*>(e.get()); if(geomMeta) continue; }
		if(!physMeta) { physMeta=dynamic_cast<IPhysDispatcher*>(e.get()); if(physMeta) continue; }
		InteractionLoop* id(dynamic_cast<InteractionLoop*>(e.get()));
		if(id){ geomMeta=id->geomDispatcher.get(); physMeta=id->physDispatcher.get(); }
		if(geomMeta&&physMeta){break;}
	}
	if(!geomMeta) throw runtime_error("No IGeomDispatcher in engines or inside InteractionLoop.");
	if(!physMeta) throw runtime_error("No IPhysDispatcher in engines or inside InteractionLoop.");
	shared_ptr<Body> b1=Body::byId(id1,rb), b2=Body::byId(id2,rb);
	if(!b1) throw runtime_error(("No body #"+boost::lexical_cast<string>(id1)).c_str());
	if(!b2) throw runtime_error(("No body #"+boost::lexical_cast<string>(id2)).c_str());
	shared_ptr<Interaction> i=geomMeta->explicitAction(b1,b2,/*force*/force);
	assert(force && i);
	if(!i) return i;
	physMeta->explicitAction(b1->material,b2->material,i);
	i->iterMadeReal=rb->iter;
	rb->interactions->insert(i);
	return i;
}

Vector3r Shop::inscribedCircleCenter(const Vector3r& v0, const Vector3r& v1, const Vector3r& v2)
{
	return v0+((v2-v0)*(v1-v0).norm()+(v1-v0)*(v2-v0).norm())/((v1-v0).norm()+(v2-v1).norm()+(v0-v2).norm());
}

void Shop::getViscoelasticFromSpheresInteraction( Real tc, Real en, Real es, shared_ptr<ViscElMat> b)
{
    throw runtime_error("Setting parameters in ViscoElastic model is changed. You do not need to use getViscoelasticFromSpheresInteraction function any more, because this functino is deprecated. You need to set the parameters tc, en and es directly in material properties. Please, update your scripts. How to do it you can see in the following commit https://github.com/yade/trunk/commit/1987c2febdb8a6ce2d27f2dc1bb29df0dc5f686e");
}

/* This function is copied almost verbatim from scientific python, module Visualization, class ColorScale
 *
 */
Vector3r Shop::scalarOnColorScale(Real x, Real xmin, Real xmax){
	Real xnorm=min((Real)1.,max((x-xmin)/(xmax-xmin),(Real)0.));
	if(xnorm<.25) return Vector3r(0,4.*xnorm,1);
	if(xnorm<.5)  return Vector3r(0,1,1.-4.*(xnorm-.25));
	if(xnorm<.75) return Vector3r(4*(xnorm-.5),1.,0);
	return Vector3r(1,1-4*(xnorm-.75),0);
}

/* Wrap floating point number into interval (x0,x1〉such that it is shifted
 * by integral number of the interval range. If given, *period will hold
 * this number. The wrapped value is returned.
 */
Real Shop::periodicWrap(Real x, Real x0, Real x1, long* period){
	Real xNorm=(x-x0)/(x1-x0);
	Real xxNorm=xNorm-floor(xNorm);
	if(period) *period=(long)floor(xNorm);
	return x0+xxNorm*(x1-x0);
}

void Shop::getStressForEachBody(vector<Shop::bodyState>& bodyStates){
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	bodyStates.resize(scene->bodies->size());
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		Vector3r normalStress,shearStress;
		if(!I->isReal()) continue;
		
		const FrictPhys* physFP = YADE_CAST<FrictPhys*>(I->phys.get());
		ScGeom* geomScG=YADE_CAST<ScGeom*>(I->geom.get());
		
		const Body::id_t id1=I->getId1(), id2=I->getId2();
		
		if((physFP) and (geomScG)){
			Real minRad=(geomScG->radius1<=0?geomScG->radius2:(geomScG->radius2<=0?geomScG->radius1:min(geomScG->radius1,geomScG->radius2)));
			Real crossSection=Mathr::PI*pow(minRad,2);
				
			normalStress=((1./crossSection)*geomScG->normal.dot(physFP->normalForce))*geomScG->normal;
			for(int i=0; i<3; i++){
				int ix1=(i+1)%3,ix2=(i+2)%3;
				shearStress[i]=geomScG->normal[ix1]*physFP->shearForce[ix1]+geomScG->normal[ix2]*physFP->shearForce[ix2];
				shearStress[i]/=crossSection;
			}
			bodyStates[id1].normStress+=normalStress;
			bodyStates[id2].normStress+=normalStress;
			bodyStates[id1].shearStress+=shearStress;
			bodyStates[id2].shearStress+=shearStress;
		}
	}
}

/* Return the stress tensor decomposed in 2 contributions, from normal and shear forces.
The formulation follows the [Thornton2000]_ article
"Numerical simulations of deviatoric shear deformation of granular media", eq (3) and (4)
 */
py::tuple Shop::normalShearStressTensors(bool compressionPositive, bool splitNormalTensor, Real thresholdForce){
  
	//*** Stress tensor split into shear and normal contribution ***/
	Scene* scene=Omega::instance().getScene().get();
	if (!scene->isPeriodic){ throw runtime_error("Can't compute stress of periodic cell in aperiodic simulation."); }
	Matrix3r sigN(Matrix3r::Zero()),sigT(Matrix3r::Zero());
	//const Matrix3r& cellHsize(scene->cell->Hsize);   //Disabled because of warning.
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		const Vector3r& n=geom->normal;
		// if compression has positive sign, we need to change sign for both normal and shear force
		// make copy to Fs since shearForce is used at multiple places (less efficient, but avoids confusion)
		Vector3r Fs=(compressionPositive?-1:1)*phys->shearForce;
		Real N=(compressionPositive?-1:1)*phys->normalForce.dot(n), T=Fs.norm();
		bool hasShear=(T>0);
		Vector3r t; if(hasShear) t=Fs/T;
		// Real R=(Body::byId(I->getId2(),scene)->state->pos+cellHsize*I->cellDist.cast<Real>()-Body::byId(I->getId1(),scene)->state->pos).norm();
		Real R=.5*(geom->refR1+geom->refR2);
		for(int i=0; i<3; i++) for(int j=i; j<3; j++){
			sigN(i,j)+=R*N*n[i]*n[j];
			if(hasShear) sigT(i,j)+=R*T*n[i]*t[j];
		}
	}
	Real vol=scene->cell->getVolume();
	sigN*=2/vol; sigT*=2/vol;
	// fill terms under the diagonal
	sigN(1,0)=sigN(0,1); sigN(2,0)=sigN(0,2); sigN(2,1)=sigN(1,2);
	sigT(1,0)=sigT(0,1); sigT(2,0)=sigT(0,2); sigT(2,1)=sigT(1,2);
	
	// *** Normal stress tensor split into two parts according to subnetworks of strong and weak forces (or other distinction if a threshold value for the force is assigned) ***/
	Real Fmean(0); Matrix3r f, fs, fw;
	fabricTensor(Fmean,f,fs,fw); // 0,false,NaN for cutoff, split and thresholdForce as default arguments
	Matrix3r sigNStrong(Matrix3r::Zero()), sigNWeak(Matrix3r::Zero());
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		const Vector3r& n=geom->normal;
		Real N=(compressionPositive?-1:1)*phys->normalForce.dot(n);
		// Real R=(Body::byId(I->getId2(),scene)->state->pos+cellHsize*I->cellDist.cast<Real>()-Body::byId(I->getId1(),scene)->state->pos).norm();
		Real R=.5*(geom->refR1+geom->refR2);
		Real Fsplit=(!std::isnan(thresholdForce))?thresholdForce:Fmean;
		if (compressionPositive?(N<Fsplit):(N>Fsplit)){
			for(int i=0; i<3; i++) for(int j=i; j<3; j++){
				sigNStrong(i,j)+=R*N*n[i]*n[j];}
		}
		else{
			for(int i=0; i<3; i++) for(int j=i; j<3; j++){
				sigNWeak(i,j)+=R*N*n[i]*n[j];}
		}
	}
	sigNStrong*=2/vol; sigNWeak*=2/vol;
	// fill terms under the diagonal
	sigNStrong(1,0)=sigNStrong(0,1); sigNStrong(2,0)=sigNStrong(0,2); sigNStrong(2,1)=sigNStrong(1,2);
	sigNWeak(1,0)=sigNWeak(0,1); sigNWeak(2,0)=sigNWeak(0,2); sigNWeak(2,1)=sigNWeak(1,2);
	
	/// tensile forces are taken as positive!
	if (splitNormalTensor){return py::make_tuple(sigNStrong,sigNWeak);} // return strong-weak or tensile-compressive parts of the stress tensor (only normal part)
	return py::make_tuple(sigN,sigT); // return normal and shear components
}

/* Return the fabric tensor as according to [Satake1982]. */
/* as side-effect, set Gl1_NormShear::strongWeakThresholdForce */
void Shop::fabricTensor(Real& Fmean, Matrix3r& fabric, Matrix3r& fabricStrong, Matrix3r& fabricWeak, Real cutoff, bool splitTensor, Real thresholdForce){
	Scene* scene=Omega::instance().getScene().get();

	// *** Fabric tensor ***/
	fabric=Matrix3r::Zero(); 
	int count=0; // number of interactions
	py::tuple aabb = Shop::aabbExtrema(cutoff);
	Vector3r bbMin = py::extract<Vector3r>(aabb[0]), bbMax = py::extract<Vector3r>(aabb[1]);
	Vector3r cp;
	
	Fmean=0; // initialize average contact force for split = 1 fabric measurements
	// interactions loop to compute the fabric tensor returned when split = 0, and also measures average force for subsequent computations for split = 1:
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		cp = geom->contactPoint;
		if( !(cp[0]>=bbMin[0] && cp[0]<=bbMax[0] && cp[1]>=bbMin[1] && cp[1]<=bbMax[1] && cp[2]>=bbMin[2] && cp[2]<=bbMax[2]) ) continue; // possible to use isInBB() from _utils.cpp ? (NB: would exclude the contact points exactly along the BB)
		const Vector3r& n=geom->normal;
		for(int i=0; i<3; i++) for(int j=i; j<3; j++){
			fabric(i,j)+=n[i]*n[j];
		}
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		Real f=-phys->normalForce.dot(n);  // will be < 0 in compression
		Fmean+=f;
		count++;
	}
	Fmean/=count;
	// fill terms under the diagonal
	fabric(1,0)=fabric(0,1); fabric(2,0)=fabric(0,2); fabric(2,1)=fabric(1,2);
	fabric/=count;
	
	#ifdef YADE_OPENGL
		Gl1_NormPhys::maxWeakFn=Fmean;
	#endif
	
	// *** Weak and strong fabric tensors ***/
	// evaluate two different parts of the fabric tensor 
	// making distinction between strong and weak network of contact forces
	fabricStrong=Matrix3r::Zero(); 
	fabricWeak=Matrix3r::Zero(); 
	int nStrong(0), nWeak(0); // number of strong and weak contacts respectively
	if (!splitTensor & !std::isnan(thresholdForce)) {LOG_WARN("The bool splitTensor should be set to True if you specified a threshold value for the contact force, otherwise the function will return only the fabric tensor and not the two separate contributions.");}
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		cp = geom->contactPoint;
		if( !(cp[0]>=bbMin[0] && cp[0]<=bbMax[0] && cp[1]>=bbMin[1] && cp[1]<=bbMax[1] && cp[2]>=bbMin[2] && cp[2]<=bbMax[2]) ) continue; // see above for idea about isInBB() from _utils.cpp
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		const Vector3r& n=geom->normal;
		Real  f=-phys->normalForce.dot(n);
		// slipt the tensor according to the mean contact force or a threshold value if this is given
		Real Fsplit=(!std::isnan(thresholdForce))?thresholdForce:Fmean;
		if (f<Fsplit){ // strong contact network is defined from contacts with the greatest compressive forces
			for(int i=0; i<3; i++) for(int j=i; j<3; j++){
				fabricStrong(i,j)+=n[i]*n[j];
			}
			nStrong++;
		}
		else{
			for(int i=0; i<3; i++) for(int j=i; j<3; j++){
				fabricWeak(i,j)+=n[i]*n[j];
			}
			nWeak++;
		}
	}
	// fill terms under the diagonal
	fabricStrong(1,0)=fabricStrong(0,1); fabricStrong(2,0)=fabricStrong(0,2); fabricStrong(2,1)=fabricStrong(1,2);
	fabricWeak(1,0)=fabricWeak(0,1); fabricWeak(2,0)=fabricWeak(0,2); fabricWeak(2,1)=fabricWeak(1,2);
	fabricStrong/=nStrong;
	fabricWeak/=nWeak;
	
	// *** Compute total fabric tensor from the two tensors above ***/
	Matrix3r fabricTot(Matrix3r::Zero()); 
	int q(0);
	if(!count){ // compute only if there are some interactions
		q=nStrong*1./count; 
		fabricTot=(1-q)*fabricWeak+q*fabricStrong;
	}
}

py::tuple Shop::fabricTensor(Real cutoff,bool splitTensor, Real thresholdForce){
	Real Fmean; Matrix3r fabric, fabricStrong, fabricWeak;
	fabricTensor(Fmean,fabric,fabricStrong,fabricWeak,cutoff,splitTensor,thresholdForce);
	// returns fabric tensor or alternatively the two distinct contributions according to strong and weak subnetworks (or, if thresholdForce is specified, the distinction is made according to that value and not the mean one)
	if (!splitTensor){return py::make_tuple(fabric);} 
	else{return py::make_tuple(fabricStrong,fabricWeak);}
}

Matrix3r Shop::getStress(Real volume){
	Scene* scene=Omega::instance().getScene().get();
	Real volumeNonPeri = 0;
	if ( volume==0 && !scene->isPeriodic ) {
	  py::tuple extrema = Shop::aabbExtrema();
	  volumeNonPeri = py::extract<Real>( (extrema[1][0] - extrema[0][0])*(extrema[1][1] - extrema[0][1])*(extrema[1][2] - extrema[0][2]) );
	}
	if (volume==0) volume = scene->isPeriodic?scene->cell->hSize.determinant():volumeNonPeri;
	Matrix3r stressTensor = Matrix3r::Zero();
	const bool isPeriodic = scene->isPeriodic;
	FOREACH(const shared_ptr<Interaction>&I, *scene->interactions){
		if (!I->isReal()) continue;
		shared_ptr<Body> b1 = Body::byId(I->getId1(),scene);
		shared_ptr<Body> b2 = Body::byId(I->getId2(),scene);
		if (b1->shape->getClassIndex()==GridNode::getClassIndexStatic()) continue; //no need to check b2 because a GridNode can only be in interaction with an oher GridNode.
		NormShearPhys* nsi=YADE_CAST<NormShearPhys*> ( I->phys.get() );
		Vector3r branch=b1->state->pos -b2->state->pos;
		if (isPeriodic) branch-= scene->cell->hSize*I->cellDist.cast<Real>();
		stressTensor += (nsi->normalForce+nsi->shearForce)*branch.transpose();
	}
	return stressTensor/volume;
}


py::tuple Shop::getStressProfile(Real volume, int nCell, Real dz, Real zRef, vector<Real> vPartAverageX, vector<Real> vPartAverageY, vector<Real> vPartAverageZ){
	int minZ=0;
	int maxZ=0;
	Real minPosZ=0.;
	Real maxPosZ=0.;
	Scene* scene=Omega::instance().getScene().get();
	vector<Matrix3r> stressTensorProfile(nCell,Matrix3r::Zero());
	vector<Matrix3r> kineticStressTensorProfile(nCell,Matrix3r::Zero());
	vector<Real> granularTemperatureProfile(nCell,0.0);
	vector<Real> numPart(nCell,0.0);

	const bool isPeriodic = scene->isPeriodic;

	//
	//Dynamic contribution to the stress tensor
	//
	FOREACH(const shared_ptr<Body>& b,*Omega::instance().getScene()->bodies){
		int Np = floor((b->state->pos[2]-zRef)/dz);	//Define the layer number with 0 corresponding to zRef
		if ((Np>=0)&&(Np<nCell)){	//To avoid non defined vPartAverage
			//Velocity fluctuation wrt the average field
			Vector3r vFluct = b->state->vel - Vector3r(vPartAverageX[Np],vPartAverageY[Np],vPartAverageZ[Np]); 
			//Classical dynamical expression of the stress tensor
			stressTensorProfile[Np]+= -1/volume*b->state->mass*vFluct*vFluct.transpose(); 
			kineticStressTensorProfile[Np]+= -1/volume*b->state->mass*vFluct*vFluct.transpose(); 
			granularTemperatureProfile[Np] += 1/3.*(pow(vFluct[0],2) + pow(vFluct[1],2) + pow(vFluct[2],2));
			numPart[Np]+=1.;
		}
	}
	for(int n=0;n<nCell;n++) {
		if (numPart[n]>0) granularTemperatureProfile[n]/=numPart[n];
	}
	
	//
	//Love Weber contribution (same as getStress(), but with different layers)
	//
	FOREACH(const shared_ptr<Interaction>&I,  *scene->interactions){	// loop over the interactions 
                if (!I->isReal()) continue;
		shared_ptr<Body> b1 = Body::byId(I->getId1(),scene);
		shared_ptr<Body> b2 = Body::byId(I->getId2(),scene);

		if ((b1->state->blockedDOFs!=State::DOF_ALL)||(b2->state->blockedDOFs!=State::DOF_ALL)){// to remove annoying contribution from the fixed particles
			//Layers in which the particle center is contained
			int Np1 = floor((b1->state->pos[2] - zRef)/dz);
			int Np2 = floor((b2->state->pos[2] - zRef)/dz);
			//Vector between the two centers, from 2 to 1
			Vector3r branch = b1->state->pos -b2->state->pos;
			if (isPeriodic) branch -= scene->cell->hSize*I->cellDist.cast<Real>();//to handle periodicity
				
			//Contact vector (from 1 to 2)
			NormShearPhys* nsi=YADE_CAST<NormShearPhys*> ( I->phys.get() );
			Vector3r fContact = nsi->normalForce + nsi->shearForce;

			//The contribution to the stress tensor is taken such that only the part of the branch vector 
			//inside the cell is taken into account
			//If the whole vector is in the cell, add the whole contribution to the cell
			if (Np1==Np2){
				if ((Np1>=0) && (Np1<nCell)){ 
					stressTensorProfile[Np1]+= 1/volume*fContact*branch.transpose();
				}
			}
			//Otherwise, find out the cell crossed by the branch vector and assign it the contribution from this part. 
			else {	
				//Find which one is above the other to prepare the loop
				if (Np1>Np2){
					minZ = Np2;
					minPosZ = b2->state->pos[2]-zRef;
					maxZ = Np1;
					maxPosZ = b1->state->pos[2]-zRef;
				}
				else if (Np2>Np1) {
					minZ = Np1;
					minPosZ = b1->state->pos[2]-zRef;
					maxZ = Np2;
					maxPosZ = b2->state->pos[2]-zRef;
				}
		
				Real branchS_x = pow(branch[0],2.0);
				Real branchS_y = pow(branch[1],2.0);
				Real branchS_z = pow(branch[2],2.0);

				//Normalize the branch vector
				branch/=sqrt(branchS_x + branchS_y+branchS_z);

				//Loop over the cell containing the branch vector
				int numLayer = minZ;
				while (numLayer<=maxZ){
					if ((numLayer>= 0)&&(numLayer<nCell)){
						//Evaluate the branch height inside the cell
						Real deltaZ = dz;
						if (numLayer==minZ) deltaZ = dz - (minPosZ - minZ*dz);
						else if (numLayer==maxZ) deltaZ = maxPosZ - maxZ*dz;
						//From it, trigonometry gives us the vector contained in the cell
						Vector3r branchVectCell = deltaZ*sqrt(1.0 + 1.0/branchS_z*(branchS_x + branchS_y))*branch;
						//Add the contribution to the stress tensor
						stressTensorProfile[numLayer]+= 1.0/volume*fContact*branchVectCell.transpose();
					}
					//Increment the layer/cell number
					numLayer+=1;
						
				}
			}
		}
	}
	return py::make_tuple(stressTensorProfile,kineticStressTensorProfile,granularTemperatureProfile);
}

py::tuple Shop::getStressProfile_contact(Real volume, int nCell, Real dz, Real zRef){
	int minZ=0;
	int maxZ=0;
	Real minPosZ=0.;
	Real maxPosZ=0.;
	Scene* scene=Omega::instance().getScene().get();
	vector<Matrix3r> stressTensorProfile(nCell,Matrix3r::Zero());
	vector<Real> numPart(nCell,0.0);

	const bool isPeriodic = scene->isPeriodic;
	//
	//Love Weber contribution (same as getStress(), but with different layers)
	//
	FOREACH(const shared_ptr<Interaction>&I,  *scene->interactions){	// loop over the interactions 
                if (!I->isReal()) continue;
		shared_ptr<Body> b1 = Body::byId(I->getId1(),scene);
		shared_ptr<Body> b2 = Body::byId(I->getId2(),scene);

		if ((b1->state->blockedDOFs!=State::DOF_ALL)||(b2->state->blockedDOFs!=State::DOF_ALL)){// to remove annoying contribution from the fixed particles
			//Layers in which the particle center is contained
			int Np1 = floor((b1->state->pos[2] - zRef)/dz);
			int Np2 = floor((b2->state->pos[2] - zRef)/dz);
			//Vector between the two centers, from 2 to 1
			Vector3r branch = b1->state->pos -b2->state->pos;
			if (isPeriodic) branch -= scene->cell->hSize*I->cellDist.cast<Real>();//to handle periodicity
				
			//Contact vector (from 1 to 2)
			NormShearPhys* nsi=YADE_CAST<NormShearPhys*> ( I->phys.get() );
			Vector3r fContact = nsi->normalForce + nsi->shearForce;

			//The contribution to the stress tensor is taken such that only the part of the branch vector 
			//inside the cell is taken into account
			//If the whole vector is in the cell, add the whole contribution to the cell
			if (Np1==Np2){
				if ((Np1>=0) && (Np1<nCell)){ 
					stressTensorProfile[Np1]+= 1/volume*fContact*branch.transpose();
				}
			}
			//Otherwise, find out the cell crossed by the branch vector and assign it the contribution from this part. 
			else {	
				//Find which one is above the other to prepare the loop
				if (Np1>Np2){
					minZ = Np2;
					minPosZ = b2->state->pos[2]-zRef;
					maxZ = Np1;
					maxPosZ = b1->state->pos[2]-zRef;
				}
				else if (Np2>Np1) {
					minZ = Np1;
					minPosZ = b1->state->pos[2]-zRef;
					maxZ = Np2;
					maxPosZ = b2->state->pos[2]-zRef;
				}
		
				Real branchS_x = pow(branch[0],2.0);
				Real branchS_y = pow(branch[1],2.0);
				Real branchS_z = pow(branch[2],2.0);

				//Normalize the branch vector
				branch/=sqrt(branchS_x + branchS_y+branchS_z);

				//Loop over the cell containing the branch vector
				int numLayer = minZ;
				while (numLayer<=maxZ){
					if ((numLayer>= 0)&&(numLayer<nCell)){
						//Evaluate the branch height inside the cell
						Real deltaZ = dz;
						if (numLayer==minZ) deltaZ = dz - (minPosZ - minZ*dz);
						else if (numLayer==maxZ) deltaZ = maxPosZ - maxZ*dz;
						//From it, trigonometry gives us the vector contained in the cell
						Vector3r branchVectCell = deltaZ*sqrt(1.0 + 1.0/branchS_z*(branchS_x + branchS_y))*branch;
						//Add the contribution to the stress tensor
						stressTensorProfile[numLayer]+= 1.0/volume*fContact*branchVectCell.transpose();
					}
					//Increment the layer/cell number
					numLayer+=1;
						
				}
			}
		}
	}
	return py::make_tuple(stressTensorProfile);
}

py::tuple Shop::getDepthProfiles(Real vCell, int nCell, Real dz, Real zRef,bool activateCond, Real radiusPy, int dir){
	//Initialization
	int minZ;
	int maxZ;
	int numLayer;
	Real deltaCenter;
	Real zInf;
	Real zSup;
	Real volPart;

	vector<Real> velAverageX(nCell,0.0);
        vector<Real> velAverageY(nCell,0.0);
        vector<Real> velAverageZ(nCell,0.0);
	vector<Real> phiAverage(nCell,0.0);

	//Loop over the particles
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getScene()->bodies){
		shared_ptr<Sphere> s=YADE_PTR_DYN_CAST<Sphere>(b->shape); if(!s) continue;
		if (activateCond==true){
			const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
			if (sphere->radius!=radiusPy) continue;
		} //select diameters asked
                const Real zPos = b->state->pos[dir]-zRef;
		int Np = floor(zPos/dz);	//Define the layer number with 0 corresponding to zRef. Let the z position wrt to zero, that way all z altitude are positive. (otherwise problem with volPart evaluation)

		minZ= floor((zPos-s->radius)/dz);
		maxZ= floor((zPos+s->radius)/dz);
		deltaCenter = zPos - Np*dz;
	
		// Loop over the cell in which the particle is contained
		numLayer = minZ;
		while (numLayer<=maxZ){
			if ((numLayer>=0)&&(numLayer<nCell)){ //average under zRef does not interest us, avoid also negative values not compatible with the evaluation of volPart
				zInf=(numLayer-Np-1)*dz + deltaCenter;
				zSup=(numLayer-Np)*dz + deltaCenter;
				if (zInf<-s->radius) zInf = -s->radius;
				if (zSup>s->radius) zSup = s->radius;

				//Analytical formulation of the volume of a slice of sphere
				volPart = Mathr::PI*pow(s->radius,2)*(zSup - zInf +(pow(zInf,3)-pow(zSup,3))/(3*pow(s->radius,2)));

				phiAverage[numLayer]+=volPart;
				velAverageX[numLayer]+=volPart*b->state->vel[0];
                                velAverageY[numLayer]+=volPart*b->state->vel[1];
                                velAverageZ[numLayer]+=volPart*b->state->vel[2];
			}
			numLayer+=1;
		}
	}
	//Normalized the weighted velocity by the volume of particles contained inside the cell
	for(int n=0;n<nCell;n++){
		if (phiAverage[n]!=0){
			velAverageX[n]/=phiAverage[n];
                        velAverageY[n]/=phiAverage[n];
                        velAverageZ[n]/=phiAverage[n];
			//Normalize the concentration after
			phiAverage[n]/=vCell;
		}
		else {
			velAverageX[n] = 0.0;
                        velAverageY[n] = 0.0;
                        velAverageZ[n] = 0.0;
		}
	}
	return py::make_tuple(phiAverage,velAverageX,velAverageY,velAverageZ);
}


// Same as getDepthProfiles but taking into account the particles as a point
py::tuple Shop::getDepthProfiles_center(Real vCell, int nCell, Real dz, Real zRef,bool activateCond, Real radiusPy){
	//Initialization
	Real volPart;

	vector<Real> velAverageX(nCell,0.0);
        vector<Real> velAverageY(nCell,0.0);
        vector<Real> velAverageZ(nCell,0.0);
	vector<Real> phiAverage(nCell,0.0);
	vector<Real> Npart(nCell,0.0);

	//Loop over the particles
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getScene()->bodies){
		shared_ptr<Sphere> s=YADE_PTR_DYN_CAST<Sphere>(b->shape); if(!s) continue;
		if (activateCond==true){
			const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
			if (sphere->radius!=radiusPy) continue;
		} //select diameters asked
		const Real zPos = b->state->pos[2]-zRef;
		int Np = floor(zPos/dz);	//Define the layer number with 0 corresponding to zRef. Let the z position wrt to zero, that way all z altitude are positive. (otherwise problem with volPart evaluation)

		if ((Np>=0)&&(Np<nCell)){
			volPart =4./3.* Mathr::PI*pow(s->radius,3);
			phiAverage[Np]+=volPart/vCell;
			Npart[Np]+=1.;
			velAverageX[Np]+=b->state->vel[0];
		        velAverageY[Np]+=b->state->vel[1];
		        velAverageZ[Np]+=b->state->vel[2];
		}
	}
	//Normalized the weighted velocity by the volume of particles contained inside the cell
	for(int n=0;n<nCell;n++){
		if (Npart[n]!=0){
			velAverageX[n]/=Npart[n];
                        velAverageY[n]/=Npart[n];
                        velAverageZ[n]/=Npart[n];
		}
		else {
			velAverageX[n] = 0.0;
                        velAverageY[n] = 0.0;
                        velAverageZ[n] = 0.0;
		}
	}
	return py::make_tuple(phiAverage,velAverageX,velAverageY,velAverageZ);
}


Matrix3r Shop::getCapillaryStress(Real volume, bool mindlin){
	Scene* scene=Omega::instance().getScene().get();
	if (volume==0) volume = scene->isPeriodic?scene->cell->hSize.determinant():1;
	Matrix3r stressTensor = Matrix3r::Zero();
	const bool isPeriodic = scene->isPeriodic;
	FOREACH(const shared_ptr<Interaction>&I, *scene->interactions){
		if (!I->isReal()) continue;
		shared_ptr<Body> b1 = Body::byId(I->getId1(),scene);
		shared_ptr<Body> b2 = Body::byId(I->getId2(),scene);
		Vector3r fCap = mindlin? YADE_CAST<MindlinCapillaryPhys*> (I->phys.get())->fCap : YADE_CAST<CapillaryPhys*> (I->phys.get())->fCap;
		Vector3r branch=b1->state->pos -b2->state->pos;
		if (isPeriodic) branch-= scene->cell->hSize*I->cellDist.cast<Real>();
		stressTensor += fCap*branch.transpose();
	}
	return stressTensor/volume;
}

void Shop::getStressLWForEachBody(vector<Matrix3r>& bStresses){
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	bStresses.resize(scene->bodies->size());
	for (size_t k=0;k<scene->bodies->size();k++) bStresses[k]=Matrix3r::Zero();
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		Vector3r f=phys->normalForce+phys->shearForce;
		//Sum f_i*l_j/V for each contact of each particle
		bStresses[I->getId1()]-=(3.0/(4.0*Mathr::PI*pow(geom->refR1,3)))*f*((geom->contactPoint-Body::byId(I->getId1(),scene)->state->pos).transpose());
		if (!scene->isPeriodic)
			bStresses[I->getId2()]+=(3.0/(4.0*Mathr::PI*pow(geom->refR2,3)))*f*((geom->contactPoint- (Body::byId(I->getId2(),scene)->state->pos)).transpose());
		else
			bStresses[I->getId2()]+=(3.0/(4.0*Mathr::PI*pow(geom->refR2,3)))*f* ((geom->contactPoint- (Body::byId(I->getId2(),scene)->state->pos + (scene->cell->hSize*I->cellDist.cast<Real>()))).transpose());
	}
}

py::list Shop::getStressLWForEachBody(){
	py::list ret;
	vector<Matrix3r> bStresses;
	getStressLWForEachBody(bStresses);
	FOREACH(const Matrix3r& m, bStresses) ret.append(m);
	return ret;
// 	return py::make_tuple(bStresses);
}

void Shop::calm(const shared_ptr<Scene>& _scene, int mask){
	const shared_ptr<Scene> scene=(_scene?_scene:Omega::instance().getScene());
	FOREACH(shared_ptr<Body> b, *scene->bodies){
		if (!b || !b->isDynamic()) continue;
		if(((mask>0) and ((b->groupMask & mask)==0))) continue;
		b->state->vel=Vector3r::Zero();
		b->state->angVel=Vector3r::Zero();
		b->state->angMom=Vector3r::Zero();
	}
}

py::list Shop::getBodyIdsContacts(Body::id_t bodyID) {
	py::list ret;
	if (bodyID < 0) {
		throw std::logic_error("BodyID should be a positive value!");
	}
	
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	const shared_ptr<Body>& b = Body::byId(bodyID,scene);
	
	for(Body::MapId2IntrT::iterator it=b->intrs.begin(),end=b->intrs.end(); it!=end; ++it) {
		ret.append((*it).first);
	}
	return ret;
}

void Shop::setContactFriction(Real angleRad){
	Scene* scene = Omega::instance().getScene().get();
	shared_ptr<BodyContainer>& bodies = scene->bodies;
	FOREACH(const shared_ptr<Body>& b,*scene->bodies){
		if(b->isClump()) continue;
		if (b->isDynamic())
		YADE_PTR_CAST<FrictMat> (b->material)->frictionAngle = angleRad;
	}
	FOREACH(const shared_ptr<Interaction>& ii, *scene->interactions){
		if (!ii->isReal()) continue;
		const shared_ptr<FrictMat>& sdec1 = YADE_PTR_CAST<FrictMat>((*bodies)[(Body::id_t) ((ii)->getId1())]->material);
		const shared_ptr<FrictMat>& sdec2 = YADE_PTR_CAST<FrictMat>((*bodies)[(Body::id_t) ((ii)->getId2())]->material);
		//FIXME - why dynamic_cast fails here?
		FrictPhys* contactPhysics = YADE_CAST<FrictPhys*>((ii)->phys.get());
		const Real& fa = sdec1->frictionAngle;
		const Real& fb = sdec2->frictionAngle;
		contactPhysics->tangensOfFrictionAngle = std::tan(std::min(fa,fb));
	}
}

void Shop::growParticles(Real multiplier, bool updateMass, bool dynamicOnly)
{
	Scene* scene = Omega::instance().getScene().get();
	const int sphereIdx = Sphere::getClassIndexStatic();
	FOREACH(const shared_ptr<Body>& b,*scene->bodies){
		if (dynamicOnly && !b->isDynamic()) continue;
		//We grow only spheres and clumps 
		if(b->isClump() or sphereIdx == b->shape->getClassIndex()){			
			if (updateMass) {b->state->mass*=pow(multiplier,3); b->state->inertia*=pow(multiplier,5);}
			if (b->isClump())  {
				//update local Se3 of each clump member
				//FIXME: the velocity of clump members is invalid after a growth since the growth is not reflected (see Clump::moveMembers)
				// for clumps we updated inertia, nothing else to do
				FOREACH(Clump::MemberMap::value_type& B, YADE_PTR_CAST<Clump>(b->shape)->members){
					// B.first is Body::id_t, B.second is local Se3r of that body in the clump
					B.second.position *= multiplier;}
				// for clumps we are done
				continue;
			}
			// for spheres, we update radius
			(YADE_CAST<Sphere*> (b->shape.get()))->radius *= multiplier;
			// and if they are clump members,clump volume variation with homothetic displacement of all members
			if (b->isClumpMember()) b->state->pos += (multiplier-1) * (b->state->pos - Body::byId(b->clumpId, scene)->state->pos);
		}
	}
	FOREACH(const shared_ptr<Interaction>& ii, *scene->interactions){
		int ci1=(*(scene->bodies))[ii->getId1()]->shape->getClassIndex();
		int ci2=(*(scene->bodies))[ii->getId2()]->shape->getClassIndex();
		if (ii->isReal()) {
			GenericSpheresContact* contact = YADE_CAST<GenericSpheresContact*>(ii->geom.get());
			if ((!dynamicOnly || (*(scene->bodies))[ii->getId1()]->isDynamic()) && ci1==sphereIdx)
				contact->refR1 = YADE_CAST<Sphere*>((* (scene->bodies))[ii->getId1()]->shape.get())->radius;
			if ((!dynamicOnly || (*(scene->bodies))[ii->getId2()]->isDynamic()) && ci2==sphereIdx)
				contact->refR2 = YADE_CAST<Sphere*>((* (scene->bodies))[ii->getId2()]->shape.get())->radius;
			const shared_ptr<FrictPhys>& contactPhysics = YADE_PTR_CAST<FrictPhys>(ii->phys);
			contactPhysics->kn*=multiplier; contactPhysics->ks*=multiplier;
		}
		
	}
}

void Shop::growParticle(Body::id_t bodyID, Real multiplier, bool updateMass)
{
	const shared_ptr<Body>& b = Body::byId(bodyID);
	Real& rad = YADE_CAST<Sphere*>(b->shape.get())->radius;	
	rad *= multiplier;
	if (updateMass) {b->state->mass*=pow(multiplier,3); b->state->inertia*=pow(multiplier,5);}
	for(Body::MapId2IntrT::iterator it=b->intrs.begin(),end=b->intrs.end(); it!=end; ++it) {  //Iterate over all bodie's interactions
		if(!(*it).second->isReal()) continue;
		GenericSpheresContact* contact = YADE_CAST<GenericSpheresContact*>((*it).second->geom.get());
		if (bodyID==it->second->getId1()) contact->refR1 = rad;
		else contact->refR2 = rad;
	}
}

py::tuple Shop::aabbExtrema(Real cutoff, bool centers){
	if(cutoff<0. || cutoff>1.) throw invalid_argument("Cutoff must be >=0 and <=1.");
	Real inf=std::numeric_limits<Real>::infinity();
	Vector3r minimum(inf,inf,inf),maximum(-inf,-inf,-inf);
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getScene()->bodies){
		shared_ptr<Sphere> s=YADE_PTR_DYN_CAST<Sphere>(b->shape); if(!s) continue;
		Vector3r rrr(s->radius,s->radius,s->radius);
		minimum=minimum.cwiseMin(b->state->pos-(centers?Vector3r::Zero():rrr));
		maximum=maximum.cwiseMax(b->state->pos+(centers?Vector3r::Zero():rrr));
	}
	Vector3r dim=maximum-minimum;
	return py::make_tuple(Vector3r(minimum+.5*cutoff*dim),Vector3r(maximum-.5*cutoff*dim));
}

/*! Added function for 2D calculation: sphere volume. Optional. By Ning Guo */
Real Shop::getSpheresVolume2D(const shared_ptr<Scene>& _scene, int mask){
	const shared_ptr<Scene> scene=(_scene?_scene:Omega::instance().getScene());
	Real vol=0;
	FOREACH(shared_ptr<Body> b, *scene->bodies){
		if (!b) continue;
		Sphere* s=dynamic_cast<Sphere*>(b->shape.get());
		if((!s) or ((mask>0) and ((b->groupMask & mask)==0))) continue;
		vol += Mathr::PI*pow(s->radius,2);
	}
	return vol;
}

/*! Added function for 2D calculation: void ratio. Optional. By Ning Guo */
Real Shop::getVoidRatio2D(const shared_ptr<Scene>& _scene, Real _zlen){
	const shared_ptr<Scene> scene=(_scene?_scene:Omega::instance().getScene());
	Real V;
	if(!scene->isPeriodic){
		throw std::invalid_argument("utils.voidratio2D applies only to aperiodic simulations.");
	} else {
		V=scene->cell->getVolume()/_zlen;
	}
	Real Vs=Shop::getSpheresVolume2D();
	return (V-Vs)/Vs;
}

/*! Added function to get stress tensor and tangent operator tensor. By Ning Guo */
py::tuple Shop::getStressAndTangent(Real volume, bool symmetry){
	Scene* scene=Omega::instance().getScene().get();
	if (volume==0) volume = scene->isPeriodic?scene->cell->hSize.determinant():1;
	Matrix3r stress = Matrix3r::Zero();
	Matrix6r tangent = Matrix6r::Zero();
	const bool isPeriodic = scene->isPeriodic;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		shared_ptr<Body> b1 = Body::byId(I->getId1(),scene);
		shared_ptr<Body> b2 = Body::byId(I->getId2(),scene);
		Vector3r pos1 = Vector3r::Zero();
		Vector3r pos2 = Vector3r::Zero();
		if(b1->isClumpMember()) {
			pos1 = Body::byId(b1->clumpId,scene)->state->pos;
		}
		else {
			pos1 = b1->state->pos;
		}
		if(b2->isClumpMember()) {
			pos2 = Body::byId(b2->clumpId,scene)->state->pos;
		}
		else {
			pos2 = b2->state->pos;
		}
		Vector3r branch=pos1 - pos2;
		if (isPeriodic) branch -= scene->cell->hSize*I->cellDist.cast<Real>();
		NormShearPhys* nsi=YADE_CAST<NormShearPhys*>(I->phys.get());
		Real kN=nsi->kn; Real kT=nsi->ks;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		const Vector3r& n=geom->normal;
		const Vector3r& fT=nsi->shearForce;
		const Vector3r& fTotal=nsi->normalForce+nsi->shearForce;
		Real T=fT.norm();
		bool hasShear=(T>0);
		Vector3r t(Vector3r::Zero()); if(hasShear) t=fT/T;
		stress += fTotal*branch.transpose();
		tangent(0,0) += kN*n[0]*branch[0]*n[0]*branch[0]+kT*t[0]*branch[0]*t[0]*branch[0];
		tangent(0,1) += kN*n[0]*branch[0]*n[1]*branch[1]+kT*t[0]*branch[0]*t[1]*branch[1];
		tangent(0,2) += kN*n[0]*branch[0]*n[2]*branch[2]+kT*t[0]*branch[0]*t[2]*branch[2];
                tangent(0,3) += kN*n[0]*branch[0]*(n[1]*branch[2]+n[2]*branch[1])*0.5+kT*t[0]*branch[0]*(t[1]*branch[2]+t[2]*branch[1])*0.5;
                tangent(0,4) += kN*n[0]*branch[0]*(n[0]*branch[2]+n[2]*branch[0])*0.5+kT*t[0]*branch[0]*(t[0]*branch[2]+t[2]*branch[0])*0.5;
                tangent(0,5) += kN*n[0]*branch[0]*(n[0]*branch[1]+n[1]*branch[0])*0.5+kT*t[0]*branch[0]*(t[0]*branch[1]+t[1]*branch[0])*0.5;
		tangent(1,1) += kN*n[1]*branch[1]*n[1]*branch[1]+kT*t[1]*branch[1]*t[1]*branch[1];
		tangent(1,2) += kN*n[1]*branch[1]*n[2]*branch[2]+kT*t[1]*branch[1]*t[2]*branch[2];
                tangent(1,3) += kN*n[1]*branch[1]*(n[1]*branch[2]+n[2]*branch[1])*0.5+kT*t[1]*branch[1]*(t[1]*branch[2]+t[2]*branch[1])*0.5;
                tangent(1,4) += kN*n[1]*branch[1]*(n[0]*branch[2]+n[2]*branch[0])*0.5+kT*t[1]*branch[1]*(t[0]*branch[2]+t[2]*branch[0])*0.5;
                tangent(1,5) += kN*n[1]*branch[1]*(n[0]*branch[1]+n[1]*branch[0])*0.5+kT*t[1]*branch[1]*(t[0]*branch[1]+t[1]*branch[0])*0.5;
		tangent(2,2) += kN*n[2]*branch[2]*n[2]*branch[2]+kT*t[2]*branch[2]*t[2]*branch[2];
                tangent(2,3) += kN*n[2]*branch[2]*(n[1]*branch[2]+n[2]*branch[1])*0.5+kT*t[2]*branch[2]*(t[1]*branch[2]+t[2]*branch[1])*0.5;
                tangent(2,4) += kN*n[2]*branch[2]*(n[0]*branch[2]+n[2]*branch[0])*0.5+kT*t[2]*branch[2]*(t[0]*branch[2]+t[2]*branch[0])*0.5;
                tangent(2,5) += kN*n[2]*branch[2]*(n[0]*branch[1]+n[1]*branch[0])*0.5+kT*t[2]*branch[2]*(t[0]*branch[1]+t[1]*branch[0])*0.5;
                tangent(3,3) += kN*(n[1]*branch[2]*n[1]*branch[2]+n[1]*branch[2]*n[2]*branch[1]*2+n[2]*branch[1]*n[2]*branch[1])*0.25+kT*(t[1]*branch[2]*t[1]*branch[2]+t[1]*branch[2]*t[2]*branch[1]*2+t[2]*branch[1]*t[2]*branch[1])*0.25;
                tangent(3,4) += kN*(n[1]*branch[2]*n[0]*branch[2]+n[1]*branch[2]*n[2]*branch[0]+n[2]*branch[1]*n[0]*branch[2]+n[2]*branch[1]*n[2]*branch[0])*0.25+kT*(t[1]*branch[2]*t[0]*branch[2]+t[1]*branch[2]*t[2]*branch[0]+t[2]*branch[1]*t[0]*branch[2]+t[2]*branch[1]*t[2]*branch[0])*0.25;
                tangent(3,5) += kN*(n[1]*branch[2]*n[0]*branch[1]+n[1]*branch[2]*n[1]*branch[0]+n[2]*branch[1]*n[0]*branch[1]+n[2]*branch[1]*n[1]*branch[0])*0.25+kT*(t[1]*branch[2]*t[0]*branch[1]+t[1]*branch[2]*t[1]*branch[0]+t[2]*branch[1]*t[0]*branch[1]+t[2]*branch[1]*t[1]*branch[0])*0.25;
                tangent(4,4) += kN*(n[0]*branch[2]*n[0]*branch[2]+n[0]*branch[2]*n[2]*branch[0]*2+n[2]*branch[0]*n[2]*branch[0])*0.25+kT*(t[0]*branch[2]*t[0]*branch[2]+t[0]*branch[2]*t[2]*branch[0]*2+t[2]*branch[0]*t[2]*branch[0])*0.25;
                tangent(4,5) += kN*(n[0]*branch[2]*n[0]*branch[1]+n[0]*branch[2]*n[1]*branch[0]+n[2]*branch[0]*n[0]*branch[1]+n[2]*branch[0]*n[1]*branch[0])*0.25+kT*(t[0]*branch[2]*t[0]*branch[1]+t[0]*branch[2]*t[1]*branch[0]+t[2]*branch[0]*t[0]*branch[1]+t[2]*branch[0]*t[1]*branch[0])*0.25;
                tangent(5,5) += kN*(n[0]*branch[1]*n[0]*branch[1]+n[0]*branch[1]*n[1]*branch[0]*2+n[1]*branch[0]*n[1]*branch[0])*0.25+kT*(t[0]*branch[1]*t[0]*branch[1]+t[0]*branch[1]*t[1]*branch[0]*2+t[1]*branch[0]*t[1]*branch[0])*0.25;
	}
	stress/=volume;
	tangent/=volume;
	return py::make_tuple(stress,tangent);
}
