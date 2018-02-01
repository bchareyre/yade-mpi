// 2017 © Raphael Maurin <raphael.maurin@imft.fr> 
// 2017 © Julien Chauchat <julien.chauchat@legi.grenoble-inp.fr> 

#include"HydroForceEngine.hpp"
#include<core/Scene.hpp>
#include<pkg/common/Sphere.hpp>
#include<lib/smoothing/LinearInterpolate.hpp>
#include<pkg/dem/Shop.hpp>

#include<core/IGeom.hpp>
#include<core/IPhys.hpp>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

YADE_PLUGIN((HydroForceEngine));

void HydroForceEngine::action(){
	/* Application of hydrodynamical forces */
        Vector3r gravityBuoyancy = gravity;
	if (steadyFlow==true) gravityBuoyancy[0] = 0.;// If the fluid flow is steady, no streamwise buoyancy contribution from gravity
	FOREACH(Body::id_t id, ids){
		Body* b=Body::byId(id,scene).get();
		if (!b) continue;
		if (!(scene->bodies->exists(id))) continue;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
		if (sphere){
			Vector3r posSphere = b->state->pos;//position vector of the sphere
			int p = floor((posSphere[2]-zRef)/deltaZ); //cell number in which the particle is
			if ((p<nCell)&&(p>=0)) {
				Vector3r liftForce = Vector3r::Zero();
				Vector3r dragForce = Vector3r::Zero();
				Vector3r convAccForce = Vector3r::Zero();
				//deterministic version
// 				Vector3r vRel = Vector3r(vxFluid[p],0,0) -  b->state->vel;//fluid-particle relative velocity
				Vector3r vRel = Vector3r(vxFluid[p]+vFluctX[id],vFluctY[id],vFluctZ[id]) -  b->state->vel;//fluid-particle relative velocity
				//Drag force calculation
				if (vRel.norm()!=0.0) {
					dragForce = 0.5*densFluid*Mathr::PI*pow(sphere->radius,2.0)*(0.44*vRel.norm()+24.4*viscoDyn/(densFluid*sphere->radius*2))*pow(1-phiPart[p],-expoRZ)*vRel;
				}
				//lift force calculation due to difference of fluid pressure between top and bottom of the particle
				int intRadius = floor(sphere->radius/deltaZ);
				if ((p+intRadius<nCell)&&(p-intRadius>0)&&(lift==true)) {
					double vRelTop = vxFluid[p+intRadius] -  b->state->vel[0]; // relative velocity of the fluid wrt the particle at the top of the particle
					double vRelBottom = vxFluid[p-intRadius] -  b->state->vel[0]; // same at the bottom
					liftForce[2] = 0.5*densFluid*Mathr::PI*pow(sphere->radius,2.0)*Cl*(vRelTop*vRelTop-vRelBottom*vRelBottom);
				}
				//buoyant weight force calculation
				Vector3r buoyantForce = -4.0/3.0*Mathr::PI*pow(sphere->radius,3.0)*densFluid*gravityBuoyancy;
				if (convAccOption==true){convAccForce[0] = - convAcc[p];}
				//add the hydro forces to the particle
				scene->forces.addForce(id,dragForce+liftForce+buoyantForce+convAccForce);		
			}
		}
	}
}

void HydroForceEngine::averageProfile(){
	//Initialization
	int minZ;
	int maxZ;
	int numLayer;
	double deltaCenter;
	double zInf;
	double zSup;
	double volPart;
	Vector3r uRel = Vector3r::Zero();
	Vector3r fDrag  = Vector3r::Zero();

	int nMax = nCell;
	vector<double> velAverageX(nMax,0.0);
        vector<double> velAverageY(nMax,0.0);
        vector<double> velAverageZ(nMax,0.0);
	vector<double> phiAverage(nMax,0.0);
	vector<double> dragAverage(nMax,0.0);
	vector<double> phiAverage1(nMax,0.0);
	vector<double> dragAverage1(nMax,0.0);
	vector<double> velAverageX1(nMax,0.0);
        vector<double> velAverageY1(nMax,0.0);
        vector<double> velAverageZ1(nMax,0.0);
	vector<double> phiAverage2(nMax,0.0);
	vector<double> dragAverage2(nMax,0.0);
	vector<double> velAverageX2(nMax,0.0);
        vector<double> velAverageY2(nMax,0.0);
        vector<double> velAverageZ2(nMax,0.0);

	//Loop over the particles
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getScene()->bodies){
		shared_ptr<Sphere> s=YADE_PTR_DYN_CAST<Sphere>(b->shape); if(!s) continue;
		const double zPos = b->state->pos[2]-zRef;
		int Np = floor(zPos/deltaZ);	//Define the layer number with 0 corresponding to zRef. Let the z position wrt to zero, that way all z altitude are positive. (otherwise problem with volPart evaluation)
		if ((b->state->blockedDOFs==State::DOF_ALL)&&(zPos > s->radius)) continue;// to remove contribution from the fixed particles on the sidewalls.

		// Relative fluid/particle velocity using also the associated fluid vel. fluct. 
		if ((Np>=0)&&(Np<nCell)){
			uRel = Vector3r(vxFluid[Np]+vFluctX[b->id], vFluctY[b->id],vFluctZ[b->id]) - b->state->vel;
			// Drag force with a Dallavalle formulation (drag coef.) and Richardson-Zaki Correction (hindrance effect)
			fDrag = 0.5*Mathr::PI*pow(s->radius,2.0)*densFluid*(0.44*uRel.norm()+24.4*viscoDyn/(densFluid*2.0*s->radius))*pow((1-phiPart[Np]),-expoRZ)*uRel;
		}
		else fDrag = Vector3r::Zero();

		minZ= floor((zPos-s->radius)/deltaZ);
		maxZ= floor((zPos+s->radius)/deltaZ);
		deltaCenter = zPos - Np*deltaZ;
	
		// Loop over the cell in which the particle is contained
		numLayer = minZ;
		while (numLayer<=maxZ){
			if ((numLayer>=0)&&(numLayer<nMax)){ //average under zRef does not interest us, avoid also negative values not compatible with the evaluation of volPart
				zInf=(numLayer-Np-1)*deltaZ + deltaCenter;
				zSup=(numLayer-Np)*deltaZ + deltaCenter;
				if (zInf<-s->radius) zInf = -s->radius;
				if (zSup>s->radius) zSup = s->radius;

				//Analytical formulation of the volume of a slice of sphere
				volPart = Mathr::PI*pow(s->radius,2)*(zSup - zInf +(pow(zInf,3)-pow(zSup,3))/(3*pow(s->radius,2)));

				phiAverage[numLayer]+=volPart;
				velAverageX[numLayer]+=volPart*b->state->vel[0];
                                velAverageY[numLayer]+=volPart*b->state->vel[1];
                                velAverageZ[numLayer]+=volPart*b->state->vel[2];
				dragAverage[numLayer]+=volPart*fDrag[0];
				if (twoSize==true){
					if (s->radius==radiusPart1){
						phiAverage1[numLayer]+=volPart; 
						dragAverage1[numLayer]+=volPart*fDrag[0];
						velAverageX1[numLayer]+=volPart*b->state->vel[0];
						velAverageY1[numLayer]+=volPart*b->state->vel[1];
						velAverageZ1[numLayer]+=volPart*b->state->vel[2];
					}
					if (s->radius==radiusPart2){
						phiAverage2[numLayer]+=volPart;
						dragAverage2[numLayer]+=volPart*fDrag[0];
						velAverageX2[numLayer]+=volPart*b->state->vel[0];
						velAverageY2[numLayer]+=volPart*b->state->vel[1];
						velAverageZ2[numLayer]+=volPart*b->state->vel[2];
					}
				}
			}
			numLayer+=1;
		}
	}
	//Normalized the weighted velocity by the volume of particles contained inside the cell
	for(int n=0;n<nMax;n++){
		if (phiAverage[n]!=0){
			velAverageX[n]/=phiAverage[n];
                        velAverageY[n]/=phiAverage[n];
                        velAverageZ[n]/=phiAverage[n];
			dragAverage[n]/=phiAverage[n];
			//Normalize the concentration after
			phiAverage[n]/=vCell;
			if (twoSize==true){
				if (phiAverage1[n]!=0){
					dragAverage1[n]/=phiAverage1[n];
					velAverageX1[n]/=phiAverage1[n];
					velAverageY1[n]/=phiAverage1[n];
					velAverageZ1[n]/=phiAverage1[n];
				}
				else {
					dragAverage1[n]=0.0;
					velAverageX1[n]=0.0;
					velAverageY1[n]=0.0;
					velAverageZ1[n]=0.0;
				}
				if (phiAverage2[n]!=0){
					dragAverage2[n]/=phiAverage2[n];
					velAverageX2[n]/=phiAverage2[n];
					velAverageY2[n]/=phiAverage2[n];
					velAverageZ2[n]/=phiAverage2[n];
				}
				else {
					dragAverage2[n]=0.0;
					velAverageX2[n]=0.0;
					velAverageY2[n]=0.0;
					velAverageZ2[n]=0.0;
				}
				phiAverage1[n]/=vCell;
				phiAverage2[n]/=vCell;
			 }
		}
		else {
			velAverageX[n] = 0.0;
                        velAverageY[n] = 0.0;
                        velAverageZ[n] = 0.0;
			dragAverage[n] = 0.0;
			if (twoSize==true){
				dragAverage1[n] = 0.0;
				dragAverage2[n] = 0.0;
				velAverageX1[n]=0.0;
				velAverageY1[n]=0.0;
				velAverageZ1[n]=0.0;
				velAverageX2[n]=0.0;
				velAverageY2[n]=0.0;
				velAverageZ2[n]=0.0;
			}
		}
	}
	//Assign the results to the global/public variables of HydroForceEngine
	phiPart = phiAverage;
	vxPart = velAverageX;
	vyPart = velAverageY;
        vzPart = velAverageZ;
	averageDrag = dragAverage;
	phiPart1 = phiAverage1;	//Initialize everything to zero if the twoSize option is not activated
	phiPart2 = phiAverage2;
	averageDrag1 = dragAverage1;
	averageDrag2 = dragAverage2;
	vxPart1 = velAverageX1;
	vyPart1 = velAverageY1;
	vzPart1 = velAverageZ1;
	vxPart2 = velAverageX2;
	vyPart2 = velAverageY2;
	vzPart2 = velAverageZ2;
}


/* Velocity fluctuation determination.  To execute at a given (changing) period corresponding to the eddy turn over time*/
/* Should be initialized before running HydroForceEngine */
void HydroForceEngine::turbulentFluctuation(){
	/* check size */
	size_t size=vFluctX.size();
	if(size<scene->bodies->size()){
		size=scene->bodies->size();
		vFluctX.resize(size);
		vFluctY.resize(size);
		vFluctZ.resize(size);
	}
	/* reset stored values to zero */
	memset(& vFluctX[0],0,size);
	memset(& vFluctY[0],0,size);
	memset(& vFluctZ[0],0,size);

	/* Create a random number generator rnd() with a gaussian distribution of mean 0 and stdev 1.0 */
	/* see http://www.boost.org/doc/libs/1_55_0/doc/html/boost_random/reference.html and the chapter 7 of Numerical Recipes in C, second edition (1992) for more details */
	static boost::minstd_rand0 randGen((int)TimingInfo::getNow(true));
	static boost::normal_distribution<double> dist(0.0, 1.0);
	static boost::variate_generator<boost::minstd_rand0&,boost::normal_distribution<double> > rnd(randGen,dist);

	double rand1 = 0.0;
	double rand2 = 0.0;
	double rand3 = 0.0;
	/* Attribute a fluid velocity fluctuation to each body above the bed elevation */
	FOREACH(Body::id_t id, ids){
		Body* b=Body::byId(id,scene).get();
		if (!b) continue;
		if (!(scene->bodies->exists(id))) continue;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
		if (sphere){
			Vector3r posSphere = b->state->pos;//position vector of the sphere
			int p = floor((posSphere[2]-zRef)/deltaZ); //cell number in which the particle is
			// If the particle is inside the water and above the bed elevation, so inside the turbulent flow, evaluate a turbulent fluid velocity fluctuation which will be used to apply the drag.
			// The fluctuation magnitude is linked to the value of the Reynolds stress tensor at the given position, a kind of local friction velocity ustar
			// The fluctuations along wall-normal and streamwise directions are correlated in order to be consistent with the formulation of the Reynolds stress tensor and to recover the result
			// that the magnitude of the fluctuation along streamwise = 2*along wall normal
			if ((p<nCell)&&(posSphere[2]-zRef>bedElevation)) {  // Remove the particles outside of the flow and inside the granular bed, they are not submitted to turbulent fluctuations. 
				double uStar2 = ReynoldStresses[p]/densFluid;
				if (uStar2>0.0){
					double uStar = sqrt(uStar2);
					rand1 = rnd();
					rand2 = rnd();
					rand3 = -rand1 + rnd();// x and z fluctuation are correlated as measured by Nezu 1977 and as expected from the formulation of the Reynolds stress tensor. 
					vFluctZ[id] = rand1*uStar;
					vFluctY[id] = rand2*uStar;
					vFluctX[id] = rand3*uStar;
				}
			}
			else{
				vFluctZ[id] = 0.0;
				vFluctY[id] = 0.0;
				vFluctX[id] = 0.0;

			}
		}
	}
}

/* Alternative Velocity fluctuation model, same as turbulentFluctuation model but with a time step associated with the fluctuation generation depending on z */
/* Should be executed in the python script at a period dtFluct corresponding to the smallest value of the fluctTime vector */
/* Should be initialized before running HydroForceEngine */
void HydroForceEngine::turbulentFluctuationBIS(){
        int idPartMax = vFluctX.size();
        double rand1 = 0.0;
        double rand2 = 0.0;
        double rand3 = 0.0;
        /* Create a random number generator rnd() with a gaussian distribution of mean 0 and stdev 1.0 */
        /* see http://www.boost.org/doc/libs/1_55_0/doc/html/boost_random/reference.html and the chapter 7 of Numerical Recipes in C, second edition (1992) for more details */
        static boost::minstd_rand0 randGen((int)TimingInfo::getNow(true));
        static boost::normal_distribution<double> dist(0.0, 1.0);
        static boost::variate_generator<boost::minstd_rand0&,boost::normal_distribution<double> > rnd(randGen,dist);

	//Loop on the particles
        for(int idPart=0;idPart<idPartMax;idPart++){
		//Remove the time ran since last application of the function (dtFluct define in global)
                fluctTime[idPart]-=dtFluct;
		//If negative, means that the time of application of the fluctuation is over, generate a new one with a new associated time
                if (fluctTime[idPart]<=0){ 
                        fluctTime[idPart] = 10*dtFluct; //Initialisation of the application time
                        Body* b=Body::byId(idPart,scene).get();
                        if (!b) continue;
                        if (!(scene->bodies->exists(idPart))) continue;
                        const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
                        double uStar = 0.0;
                        if (sphere){
                                Vector3r posSphere = b->state->pos;//position vector of the sphere
                                int p = floor((posSphere[2]-zRef)/deltaZ); //cell number in which the particle is
                                if (ReynoldStresses[p]>0.0) uStar = sqrt(ReynoldStresses[p]/densFluid);
                                // Remove the particles outside of the flow and inside the granular bed, they are not submitted to turbulent fluctuations. 
                                if ((p<nCell)&&(posSphere[2]-zRef>bedElevation)) {
                                        rand1 = rnd();
                                        rand2 = rnd();
                                        rand3 = -rand1 + rnd(); // x and z fluctuation are correlated as measured by Nezu 1977 and as expected from the formulation of the Reynolds stress tensor. 
                                        vFluctZ[idPart] = rand1*uStar;
                                        vFluctY[idPart] = rand2*uStar;
                                        vFluctX[idPart] = rand3*uStar;
					// Limit the value to avoid the application of fluctuations in the viscous sublayer
                                        const double zPos = max(b->state->pos[2]-zRef-bedElevation,11.6*viscoDyn/densFluid/uStar);
					// Time of application of the fluctuation as a function of depth from kepsilon model
                                        if (uStar>0.0) fluctTime[idPart]=min(0.33*0.41*zPos/uStar,10.);
                                        }
				else{
					vFluctZ[idPart] = 0.0;
					vFluctY[idPart] = 0.0;
					vFluctX[idPart] = 0.0;
					fluctTime[idPart] = 0.0;

				}
                                }
                        }
        }
}

/* Velocity fluctuation determination.  To execute at a given period*/
/* Should be initialized before running HydroForceEngine */
void HydroForceEngine::turbulentFluctuationFluidizedBed(){
	/* check size */
	size_t size=vFluctX.size();
	if(size<scene->bodies->size()){
		size=scene->bodies->size();
		vFluctX.resize(size);
		vFluctY.resize(size);
		vFluctZ.resize(size);
	}
	/* reset stored values to zero */
	memset(& vFluctX[0],0,size);
	memset(& vFluctY[0],0,size);
	memset(& vFluctZ[0],0,size);

	/* Create a random number generator rnd() with a gaussian distribution of mean 0 and stdev 1.0 */
	/* see http://www.boost.org/doc/libs/1_55_0/doc/html/boost_random/reference.html and the chapter 7 of Numerical Recipes in C, second edition (1992) for more details */
	static boost::minstd_rand0 randGen((int)TimingInfo::getNow(true));
	static boost::normal_distribution<double> dist(0.0, 1.0);
	static boost::variate_generator<boost::minstd_rand0&,boost::normal_distribution<double> > rnd(randGen,dist);

	double rand1 = 0.0;
	double rand2 = 0.0;
	double rand3 = 0.0;
	/* Attribute a fluid velocity fluctuation to each body above the bed elevation */
	FOREACH(Body::id_t id, ids){
		Body* b=Body::byId(id,scene).get();
		if (!b) continue;
		if (!(scene->bodies->exists(id))) continue;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
		if (sphere){
			Vector3r posSphere = b->state->pos;//position vector of the sphere
			int p = floor((posSphere[2]-zRef)/deltaZ); //cell number in which the particle is
			// If the particle is inside the water and above the bed elevation, so inside the turbulent flow, evaluate a turbulent fluid velocity fluctuation which will be used to apply the drag.
			// The fluctuation magnitude is linked to the value of the Reynolds stress tensor at the given position, a kind of local friction velocity ustar
			if ((p<nCell)&&(posSphere[2]-zRef>0.)) {  // Remove the particles outside of the flow
				double uStar2 = ReynoldStresses[p]/densFluid;
				if (uStar2>0.0){
					double uStar = sqrt(uStar2);
					rand1 = rnd();
					rand2 = rnd();
					rand3 = rnd();
					vFluctZ[id] = rand1*uStar;
					vFluctY[id] = rand2*uStar;
					vFluctX[id] = rand3*uStar;
				}
			}
			else{
				vFluctZ[id] = 0.0;
				vFluctY[id] = 0.0;
				vFluctX[id] = 0.0;

			}
		}
	}
}



///////////////////////
/* Fluid Resolution */
///////////////////////
// Fluid resolution routine: 1D vertical volume-averaged fluid momentum balance resolution
void HydroForceEngine::fluidResolution(double tfin,double dt)
{
  //Variables declaration
  int j;
  double dummy,dn1,ds1,dn2,ds2,dzn,dzs,dzm,alphafp,alphafwn,alphafws,viscof,dz,as,an,ap1;
  vector<double> sig(nCell,0.0), dsig(nCell-1,0.), beta(nCell,0.), ufn(nCell,0.), wallFriction(nCell,0.),viscoft(nCell,0.),ufnp(nCell,0.),a(nCell,0.),b(nCell,0.),c(nCell,0.),s(nCell,0.),alphaf(nCell,0.),alphas(nCell,0.),us(nCell,0.);

  //Initialisation
  double time=0;
  ufn = vxFluid;	//Assign the global variable vxFluid to ufn, i.e. the last fluid velocity profile evaluated
  us = vxPart;
  alphas = phiPart;
  viscof = viscoDyn/densFluid;  // compute the kinematic viscosity
  dz = deltaZ;
  double imp=0.5;  // Implicitation factor of the lateral sink term due to wall friction
  for (j=0;j<nCell;j++){
	alphaf[j]=1.-alphas[j];  // compute fluid phase volume fraction or porosity
	}

  // Mesh definition: regular with boundary conditions different: 0.5dz at the top, and 1.5dz at the bottom
  sig[0]=0.; sig[1]=1.5*dz;dsig[0]=1.5*dz;dsig[1]=dz;
  for (j=2;j<nCell-1;j++) {sig[j]=sig[j-1]+dz; dsig[j]=dz;}
  sig[nCell-1]= sig[nCell-2]+0.5*dz;dsig[nCell-2]=0.5*dz;

  sig_cpp.resize(nCell);   dsig_cpp.resize(nCell); 
  sig_cpp = sig;  dsig_cpp = dsig;

//  ////////////////////////////////////  //  computeTaufsi(dt);
//  // Compute the fluid-particle momentum transfer associated to drag force, taufsi = phi/Vp*<fd>/rhof/(uf - up), not changing during the fluid resolution

//  //Initialization
//  taufsi.resize(nCell);  memset(& taufsi[0],0,nCell);  //Resize and initialize taufsi
//  double lim = 1e-5, dragTerm=0., partVolume=1., partVolume1=1., partVolume2=1.;
//  // Evaluate particles volume
//  if (twoSize==true){
//	partVolume1 = 4./3.*Mathr::PI*pow(radiusPart1,3);
//	partVolume2 = 4./3.*Mathr::PI*pow(radiusPart2,3);
//  }
//  else 	partVolume = 4./3.*Mathr::PI*pow(radiusPart,3);
//  // Compute taufsi
//  taufsi[0] = 0.;
//  for(int i=1;i<nCell;i++)
//	{
//	if (twoSize==true){
//		dragTerm = phiPart1[i]/partVolume1*averageDrag1[i] + phiPart2[i]/partVolume2*averageDrag2[i];
//	}
//	else {
//		dragTerm = phiPart[i]/partVolume*averageDrag[i];
//	}

//	double urel = std::abs(ufn[i] - us[i]);
//	double urel_bound = std::max(urel,lim); //limit the value to avoid division by 0
//	taufsi[i] = std::max(0.,std::min(dragTerm/urel_bound/densFluid,pow(10*dt,-1.))); //limit the max value of taufsi to the fluid resolution limit, i.e. 1/(10dt) and required positive (charact. time)
//	}
//  //////////////////////////////////// 
      // Compute the viscosity amplification factor profile (due to the presence of particles), beta
  if (irheolf==0){      // 0 : Viscosite du fluide pur
	for(j=0;j<nCell;j++)	beta[j]=1.;
  }
  else if (irheolf==1){   // 1 : Viscosite d'Einstein
	for(j=0;j<nCell;j++)	beta[j]=1.+2.5*alphas[j];
  }
  
  ///////////////////////////////////////////////
  // FLUID VELOCITY PROFILE RESOLUTION: LOOP OVER TIME (main loop)
  while (time <= tfin){
      // Advance time
      time = time + dt;

      ////////////////////////////////////      calviscotlm(ufn,viscof,viscoft,sig,dsig
      // Compute the eddy viscosity depth profile, viscoft

      double sum,alphasmid,dudz,ustar;
      vector<double> lm(nCell,0.);
      // compute the fluid height
      double fluidHeight = sig[nCell-1];
      // Eddy viscosity 
      if (iturbu==0) {      // 0 : No turbulence
	for(j=0;j<nCell;j++) viscoft[j]=0.;
      }
      else if (iturbu==1) {    // iturbu = 1 : Turbulence activated
          if (ilm==0){	// ilm = 0 : Prandtl mixing length
	      lm[0]=0.;
	      for(j=1;j<nCell;j++) lm[j]=kappa*sig[j];
          }
          else if (ilm==1){        // ilm = 1 : Parabolic profile (free surface flows)
	      lm[0]=0.;
              for(j=1;j<nCell;j++)  lm[j]=kappa*sig[j]*sqrt(1.-sig[j]/fluidHeight);
              lm[nCell-1]=0.;
          }
          else if (ilm==2){        // ilm = 2 : Li and Sawamoto (1995) integral of concentration profile
	      sum = 0.;
	      lm[0]=0.;
              for(j=1;j<nCell;j++){
	          alphasmid=std::min(0.5*(alphas[j-1]+alphas[j]),phiMax);
                  sum=sum+kappa*(phiMax-alphasmid)/phiMax*dsig[j-1];
                  lm[j]=sum;
              }
              lm[nCell-1]=lm[nCell-2];
          }	// end if ilm

          // Compute the velocity gradient and the mixing length
          for(j=1;j<nCell;j++){
	      dzm = 0.5*(dsig[j]+dsig[j-1]);
	      dudz=(ufn[j]-ufn[j-1])/dzm;
	      viscoft[j]=(1.-alphas[j])*pow(lm[j],2)*fabs(dudz);
          }
          viscoft[nCell-1]=viscoft[nCell-2];
          // test on y+ for viscous sublayer
          ustar = sqrt(fabs(gravity[0])*sig[nCell-1]);
          if (viscousSubLayer==1){
  	        for(j=1;j<nCell;j++)  if (sig[j]*ustar/viscof<11.3) viscoft[j]=0.;
          }
      }
      //////////////////////////////////


      //////////////////////////////////
      // Compute the lateral wall friction profile, if activated
      if (fluidWallFriction==true){  //calWallFriction(ufn,channelWidth,viscof,wallFriction);
	      int  maxiter,q;
	      double Re, epsilon, ff, ffold,delta;
	      maxiter = 100;	//Maximum number iteration for the resolution
	      epsilon = 1e-3;	//Tolerance for the equation resolution
	      for (j=0;j<nCell;j++){
		   Re = max(1e-10,fabs(ufn[j])*channelWidth/viscof);
	    	   ffold=pow(0.32,-2);	//Initial guess of the friction factor
	     	   delta=1e10;	//Initialize at a random value greater than epsilon
		   q=0;
		   while ((delta>=epsilon)&&(q<maxiter)){ //Loop while the required precision is reached or the  maximum iteration number is overpassed
			   q+=1;
			   //Graf and Altinakar 1993 formulation of the friction factor
			   ff = pow(2.*log(Re*sqrt(ffold))+0.32,-2);
			   delta = fabs(ff-ffold)/ffold;
			   ffold = ff;
			   }
		   if (q==maxiter) ff=0.;
		   wallFriction[j] = ff;
	      }
      }
      //////////////////////////////////


      //////////////////////////////////
      // Compute the system of equation in matricial form (Compute a,b,c,s)

      // Bottom boundary condition: (always no-slip)
      j=0; a[j]=0.; b[j]=1.; c[j]=0.; s[j]=0.;

      // Top boundary condition: (0: no-slip / 1: zero gradient) 
      j=nCell-1;
      if (iusl==0){ a[j]=0.;  b[j]=1.; c[j]=0.; }
      else if (iusl==1){ a[j]=-1.; b[j]=1.; c[j]=0.; }
      s[j]=0.;       

      //Loop over the spatial mesh to determine the matricial coefficient, (a,b,c,s)
      for(j=1;j<nCell-1;j++)
	{
	  // volume fraction interpolation (staggered grid)
	  alphafp = 0.5*(alphaf[j]+alphaf[j+1]);
	  if (j==1)
	    {
	      alphafwn=0.5*(alphaf[j+2]+alphaf[j+1]); 
	      alphafws=alphaf[j-1]; 
	    }
	  else if (j==nCell-2)
	    {
	      alphafwn=alphaf[j+1]; 
	      alphafws=0.5*(alphaf[j  ]+alphaf[j-1]);
	    }
	  else
	    {
	      alphafwn=0.5*(alphaf[j+2]+alphaf[j+1]); 
	      alphafws=0.5*(alphaf[j  ]+alphaf[j-1]);
	    }
	  dzn = dsig[j];
	  dzs = dsig[j-1];
	  dzm = 0.5*(dzn+dzs);

	  // Diffusion coefficients
	  // Eddy viscosity terms
	  dummy=dt/dzm;   
	  ds1=dummy*viscoft[j  ]/dzs;
	  dn1=dummy*viscoft[j+1]/dzn;
	  // Viscous terms
	  ds2=dummy*viscof*beta[j  ]/dzs*alphafp;
	  dn2=dummy*viscof*beta[j+1]/dzn*alphafp;
	  
	  // Numerical scheme coefficient (diffussion only in 1DV)
	  an=dn1+(dn2)*alphafwn;
	  as=ds1+(ds2)*alphafws;
	
	  ap1=dn1+(dn2)*alphafp+ds1+(ds2)*alphafp;
	  // LHS: algebraic system coefficients
	  a[j] = - as;
	  b[j] = alphafp + ap1 + dt*taufsi[j]  + imp*dt*alphafp*2./channelWidth*0.125*wallFriction[j]*ufn[j]; 
	  c[j] = - an;

	  // RHS: unsteady, gravity, drag, pressure gradient, lateral wall friction
	  s[j]= alphafp*ufn[j] + alphafp*std::abs(gravity[0])*dt + dt*taufsi[j]*us[j] - alphafp*dpdx/densFluid*dt - (1.-imp)*dt*alphafp*2./channelWidth*0.125*wallFriction[j]*pow(ufn[j],2);
        }
      //////////////////////////////////



      ////////////////////////////////////      doubleq(a, b , c, s , ufnp);
      // Solve the matricial tridiagonal system using double-sweep algorithm

      int i,ii;
      vector<double> ddem(nCell,0.), ddfm(nCell,0.);
      double dddiv;
      // downward sweep
      dddiv=b[0];
      ddem[0]=-c[0]/dddiv;
      ddfm[0]=s[0]/dddiv;
  
      for (i=1;i<=nCell-2;i++){
	     dddiv=b[i]+a[i]*ddem[i-1];
	     ddem[i]=-c[i]/dddiv;
	     ddfm[i]=(s[i]-a[i]*ddfm[i-1])/dddiv;
      }
      // upward sweep
      dddiv=b[nCell-1]+a[nCell-1]*ddem[nCell-2];
      ddfm[nCell-1]=(s[nCell-1]-a[nCell-1]*ddfm[nCell-2])/dddiv;
      ufnp[nCell-1]=ddfm[nCell-1];
      for (ii=2;ii<nCell+1;ii++){
	     i=nCell-ii;
	     ufnp[i]=ddem[i]*ufnp[i+1]+ddfm[i];
      }  
      //////////////////////////////////

      // Update solution for next time step
      for(j=0;j<nCell;j++) ufn[j]=ufnp[j];
  }
  ///////////////////////////// END OF THE LOOP ON THE TIME


  //Update Fluid velocity, turbulent viscosity and Reynolds stresses
  ReynoldStresses.resize(nCell);  turbulentViscosity.resize(nCell);  vxFluid.resize(nCell);
  memset(& ReynoldStresses[0],0,nCell);  memset(& turbulentViscosity[0],0,nCell);  memset(& vxFluid[0],0,nCell);
  for (j=0;j<nCell;j++){
  	vxFluid[j] = ufn[j];  // Update fluid velocity
	turbulentViscosity[j] = viscoft[j];
	if (j>0) ReynoldStresses[j] = densFluid*viscoft[j]*(ufn[j+1]-ufn[j])/dsig[j];
  }
  ReynoldStresses[nCell-1] = ReynoldStresses[nCell-2];turbulentViscosity[nCell-1]=viscoft[nCell-1];
}
///////////////////////////////////////////////////////////////////////// END OF HydroForceEngine::fluidResolution






// Compute the effective viscosity accounting for the presence of particles
void HydroForceEngine::calbeta(vector<double> beta_in)
{
  int j;
  // viscosity amplification factor
  if (irheolf==0)
    // 0 : Viscosite du fluide pur
    {
      for(j=0;j<=nCell-1;j++)
	beta_in[j]=1.;
    }
  else if (irheolf==1)
    // 1 : Viscosite d'Einstein
    {
      for(j=0;j<=nCell-1;j++)
	beta_in[j]=1.+2.5*phiPart[j];
    }
}

// Compute the eddy viscosity, turbulentViscosity
void HydroForceEngine::calviscotlm(vector<double> ufn_in,double viscof_in,vector<double> viscoft_in,vector<double> sig_in,vector<double> dsig_in)
{
  int j;
  double lm[nCell],sum,alphasmid,dudz,ustar,dzm;
  // compute the fluid height
  double fluidHeight = sig_in[nCell-1];

  // Eddy viscosity 
  if (iturbu==0)
    // 0 : No turbulence
    {
      for(j=0;j<=nCell-1;j++)
        viscoft_in[j]=0.;
    }
  else if (iturbu==1)
    // 1 : Turbulence activated
    {
      if (ilm==0)
	// 0 : Prandtl mixing length
	{
	  lm[0]=0.;
	  for(j=1;j<=nCell-1;j++) lm[j]=kappa*sig_in[j];
	}
      else if (ilm==1)
        // 1 : Parabolic profile (free surface flows)
        {
	  lm[0]=0.;
          for(j=1;j<=nCell-1;j++)  lm[j]=kappa*sig_in[j]*sqrt(1.-sig_in[j]/fluidHeight);
          lm[nCell-1]=0.;
        }
      else if (ilm==2)
        // 2 : Li and Sawamoto (1995) integral of concentration profile
        {
	  sum = 0.;
	  lm[0]=0.;
          for(j=1;j<=nCell-1;j++)
            {
	      alphasmid=std::min(0.5*(phiPart[j-1]+phiPart[j]),phiMax);
              sum=sum+kappa*(phiMax-alphasmid)/phiMax*dsig_in[j-1];
              lm[j]=sum;
            }
          lm[nCell-1]=lm[nCell-2];
        }
      // Compute the velocity gradient and the mixing length
      for(j=1;j<nCell-1;j++)
	{
	  dzm = 0.5*(dsig_in[j]+dsig_in[j-1]);
	  dudz=(ufn_in[j]-ufn_in[j-1])/dzm;
	  viscoft_in[j]=(1.-phiPart[j])*pow(lm[j],2)*fabs(dudz);
        }
      viscoft_in[nCell-1]=viscoft_in[nCell-2];
      // test on y+ for viscous sublayer
      ustar = sqrt(fabs(gravity[0])*sig_in[nCell-1]);
      if (viscousSubLayer==1){
	      for(j=1;j<nCell-1;j++)  if (sig_in[j]*ustar/viscof_in<11.3) viscoft_in[j]=0.;
	}
          
    }
  
}

//Resolution of tridiagonal system
void HydroForceEngine::doubleq(vector<double> ddam1,vector<double> ddam2,vector<double> ddam3, vector<double> ddbm,vector<double> ddxm)
//void HydroForceEngine::doubleq(double ddam1[],double ddam2[],double ddam3[], double ddbm[],double ddxm[])
{
  /* reosolution of tridiagonal system
       am1,2,3[n] - Tridiagonal matrix coefficients                                     
       bm[n]   - RHS vector        
       xm[n]   - Solution vector        
       em[n], fm[n) - working arrays
       n       - algebraic system size
  */
  cout<<"b"<<ddam1[10]<<"  "<<ddam2[10]<<"  "<<ddam3[10]<<"  "<<ddbm[10]<<"  "<<ddxm[10]<<endl;
  int i,ii;
  double dddiv;
  vector<double> ddem(nCell),ddfm(nCell);

  // downward sweep
  dddiv=ddam2[0];
  ddem[0]=-ddam3[0]/dddiv;
  ddfm[0]=ddbm[0]/dddiv;
  
  for (i=1;i<=nCell-2;i++)
    {
      dddiv=ddam2[i]+ddam1[i]*ddem[i-1];
      ddem[i]=-ddam3[i]/dddiv;
      ddfm[i]=(ddbm[i]-ddam1[i]*ddfm[i-1])/dddiv;
    }
    // upward sweep
  dddiv=ddam2[nCell-1]+ddam1[nCell-1]*ddem[nCell-2];
  ddfm[nCell-1]=(ddbm[nCell-1]-ddam1[nCell-1]*ddfm[nCell-2])/dddiv;
  ddxm[nCell-1]=ddfm[nCell-1];
  for (ii=2;ii<=nCell;ii++)
    {
      i=nCell-ii;
      ddxm[i]=ddem[i]*ddxm[i+1]+ddfm[i];
    }  
  cout<<"c"<<ddxm[10]<<endl;
}

//  Create taufsi = dragTerm/(rhof(vf-vxp)) for the fluid resolution
void HydroForceEngine::computeTaufsi(double dt_in)
{
  //Resize and initialize taufsi
  taufsi.resize(nCell);
  memset(& taufsi[0],0,nCell);

  //Initialization
  double lim = 1e-5, dragTerm=0., partVolume=1., partVolume1=1., partVolume2=1.;
  // Evaluate particles volume
  if (twoSize==true){
	partVolume1 = 4./3.*Mathr::PI*pow(radiusPart1,3);
	partVolume2 = 4./3.*Mathr::PI*pow(radiusPart2,3);
  }
  else 	partVolume = 4./3.*Mathr::PI*pow(radiusPart,3);
  // Compute taufsi
  taufsi[0] = 0.;
  for(int i=1;i<nCell;i++)
	{
	if (twoSize==true){
		dragTerm = phiPart1[i]/partVolume1*averageDrag1[i] + phiPart2[i]/partVolume2*averageDrag2[i];
	}
	else {
		dragTerm = phiPart[i]/partVolume*averageDrag[i];
	}

	double urel = std::abs(vxFluid[i] - vxPart[i]);
	double urel_bound = std::max(urel,lim); //limit the value to avoid division by 0
	taufsi[i] = std::max(0.,std::min(dragTerm/urel_bound/densFluid,pow(10*dt_in,-1.))); //limit the max value of taufsi to the fluid resolution limit, i.e. 1/(10dt) and required positive (charact. time)
	}
}

//Evaluate the wall friction coefficient at each elevation, solving the Graf & Altinakar 1993 formulation. 
void HydroForceEngine::calWallFriction(vector<double> ufn_in,double channelWidth_in,double viscof_in,vector<double> wallFriction_in){
   int  maxiter,q,j;
   double Re, epsilon, ff, ffold,delta;
   maxiter = 100;	//Maximum number iteration for the resolution
   epsilon = 1e-3;	//Tolerance for the equation resolution
   for (j=0;j<=nCell-1;j++){
        Re = max(1e-10,fabs(ufn_in[j])*channelWidth_in/viscof_in);
  	ffold=pow(0.32,-2);	//Initial guess of the friction factor
     	delta=1e10;	//Initialize at a random value greater than epsilon
	q=0;
	while ((delta>=epsilon)&&(q<maxiter)){ //Loop while the required precision is reached or the  maximum iteration number is overpassed
		q+=1;
		//Graf and Altinakar 1993 formulation of the friction factor
		ff = pow(2.*log(Re*sqrt(ffold))+0.32,-2);
		delta = fabs(ff-ffold)/ffold;
		ffold = ff;
		}
	if (q==maxiter) ff=0.;
	wallFriction_in[j] = ff;
   }
}

