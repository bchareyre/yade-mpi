// 2014 © Raphael Maurin <raphael.maurin@irstea.fr> 

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
	if (activateAverage==true) averageProfile(); //Calculate the average solid profiles

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
					Real vRelTop = vxFluid[p+intRadius] -  b->state->vel[0]; // relative velocity of the fluid wrt the particle at the top of the particle
					Real vRelBottom = vxFluid[p-intRadius] -  b->state->vel[0]; // same at the bottom
					liftForce[2] = 0.5*densFluid*Mathr::PI*pow(sphere->radius,2.0)*Cl*(vRelTop*vRelTop-vRelBottom*vRelBottom);
				}
				//buoyant weight force calculation
				Vector3r buoyantForce = -4.0/3.0*Mathr::PI*pow(sphere->radius,3.0)*densFluid*gravity;
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
	Real deltaCenter;
	Real zInf;
	Real zSup;
	Real volPart;
	Vector3r uRel = Vector3r::Zero();
	Vector3r fDrag  = Vector3r::Zero();

	int nMax = nCell;
	vector<Real> velAverageX(nMax,0.0);
        vector<Real> velAverageY(nMax,0.0);
        vector<Real> velAverageZ(nMax,0.0);
	vector<Real> phiAverage(nMax,0.0);
	vector<Real> dragAverage(nMax,0.0);
	vector<Real> phiAverage1(nMax,0.0);
	vector<Real> dragAverage1(nMax,0.0);
	vector<Real> velAverageX1(nMax,0.0);
        vector<Real> velAverageY1(nMax,0.0);
        vector<Real> velAverageZ1(nMax,0.0);
	vector<Real> phiAverage2(nMax,0.0);
	vector<Real> dragAverage2(nMax,0.0);
	vector<Real> velAverageX2(nMax,0.0);
        vector<Real> velAverageY2(nMax,0.0);
        vector<Real> velAverageZ2(nMax,0.0);

	//Loop over the particles
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getScene()->bodies){
		shared_ptr<Sphere> s=YADE_PTR_DYN_CAST<Sphere>(b->shape); if(!s) continue;
		const Real zPos = b->state->pos[2]-zRef;
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

	//desactivate the average to avoid calculating at each step, only when asked by the user
	activateAverage=false; 
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
	static boost::normal_distribution<Real> dist(0.0, 1.0);
	static boost::variate_generator<boost::minstd_rand0&,boost::normal_distribution<Real> > rnd(randGen,dist);

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
				Real uStar2 = simplifiedReynoldStresses[p];
				if (uStar2>0.0){
					Real uStar = sqrt(uStar2);
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
        static boost::normal_distribution<Real> dist(0.0, 1.0);
        static boost::variate_generator<boost::minstd_rand0&,boost::normal_distribution<Real> > rnd(randGen,dist);

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
                        Real uStar = 0.0;
                        if (sphere){
                                Vector3r posSphere = b->state->pos;//position vector of the sphere
                                int p = floor((posSphere[2]-zRef)/deltaZ); //cell number in which the particle is
                                if (simplifiedReynoldStresses[p]>0.0) uStar = sqrt(simplifiedReynoldStresses[p]);
                                // Remove the particles outside of the flow and inside the granular bed, they are not submitted to turbulent fluctuations. 
                                if ((p<nCell)&&(posSphere[2]-zRef>bedElevation)) {
                                        rand1 = rnd();
                                        rand2 = rnd();
                                        rand3 = -rand1 + rnd(); // x and z fluctuation are correlated as measured by Nezu 1977 and as expected from the formulation of the Reynolds stress tensor. 
                                        vFluctZ[idPart] = rand1*uStar;
                                        vFluctY[idPart] = rand2*uStar;
                                        vFluctX[idPart] = rand3*uStar;
					// Limit the value to avoid the application of fluctuations in the viscous sublayer
                                        const Real zPos = max(b->state->pos[2]-zRef-bedElevation,11.6*viscoDyn/densFluid/uStar);
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
	static boost::normal_distribution<Real> dist(0.0, 1.0);
	static boost::variate_generator<boost::minstd_rand0&,boost::normal_distribution<Real> > rnd(randGen,dist);

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
				Real uStar2 = simplifiedReynoldStresses[p];
				if (uStar2>0.0){
					Real uStar = sqrt(uStar2);
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

/* function declaration */
void doubleq(double ddam1[],double ddam2[],double ddam3[],double ddbm[],double ddxm[],int n);
void calbeta(int irheolf, double alphas[], double beta[], double alphasmax, unsigned long n);
void calviscotlm(int iturbu, int ilm, double dz, double h,double ufn[], double alphas[], double alphasmax, double kappa, double lmExp, double viscoft[], unsigned long n);
void fluidModel(double h,double sig[],double dsig[],double dp,double ufn[],double alphaf[],double rhof,double viscof,double usnp[],double alphas[],double rhos,double alphasmax,double dpdx,double slope, double gra, double tfin,double dt,double cfdpYade[], double ufnp[], double viscoft[]);

void  HydroForceEngine::updateVelocity() {
	fluidModel(fluidHeight,&sig[0],&dsig[0],diameterPart,&vxFluid[0],
			&phiPart[0],//FIXME: in the py script it's in fact (1-phiPart) which is passed to nsmp, wtf? and why the redundancy wrt [#] (below)
			densFluid,
			viscoDyn /*is it really the dynamic one here? else pass viscoDyn/densFluid (awkward anyway) */,
			&vxPart[0],
			&phiPartFluid[0] /*[#] WHY PASSING IT AGAIN?!! It seems to be equal to phiPart in practice */,
			densPart, alphasmax, dpdx, slope,
			gravity[2],/*FIXME: I'm assuming that gravity is along 2-axis (??), does it mean that users have to define gravity multiple times in one single script? Newton::gravity, HydroForceEngine::gravity? ugly... :-\ */
			fluidResolPeriod,dtFluid,
			&taufsi[0],
			&vxFluid[0],&turbulentViscosity[0]); //<-------- output of the function
}


void fluidModel(double h,double sig[],double dsig[],double dp,double ufn[],double alphaf[],double rhof,double viscof,double usnp[],double alphas[],double rhos,double alphasmax,double dpdx,double slope, double gra, double tfin,double dt,double cfdpYade[], double ufnp[], double viscoft[])
{
	const unsigned& ndimz=HydroForceEngine::ndimz;
  unsigned j;
  int irheolf,idrag,iturbu,ilm,iusl,idrift;
  double dummy,dn1,ds1,dn2,ds2,dz,dzn,dzs,dzm;
  double alphafp,alphafwn,alphafws,rhop;
  double kappa,lmExp,expoRZ;
  double as,an,ap1,a[ndimz],b[ndimz],c[ndimz],s[ndimz],udrift[ndimz],beta[ndimz],cfdp[ndimz];
  double Rep,cd;
  double time=0;

  // impose constant grid size
  dz=dsig[0]*h;
  
  //
  // Option of the code
  //
  //    irheolf = 0 : Viscosite du fluide pur
  //              1 : Viscosite d'Einstein
  //              2 : Viscosite de Graham
  //              3 : Viscosite de Krieger-Dougherty / Ishii-Zuber
  //              4 : Viscosite de Boyer et al. 
  irheolf = 0;

  //     idrag = 0 : Trainee de Dallavale
  //             1 : Trainee de Schiller & Naumann
  //             2 : Trainee de Clift & Gauvin
  //             3 : Loi de Stokes
  //             4 : Loi de Darcy
  //             5 : Loi de Ergun
  //             6 : Imposed from averaged drag force (Yade) 
  idrag = 0;

  //     exposant de Richardson-Zaki (fonction d'entravement)
  expoRZ = -3.1;
          
  //     iturbu = 0 : Pas de turbulence
  //              1 : Longueur de mélange 
  //                  ilm = 0 : longueur de melange de Prandtl
  //                        1 : longueur de melange de Prandtl avec effet de Surface libre
  //                        2 : Li and Sawamoto  (1995)
  iturbu = 1;
  ilm = 2;

  kappa = 0.41;
  lmExp = 1;

  //     iusl = 0 : Condition de Dirichlet (u=0 en z=h)
  //            1 : Condition de Neumann   (dudz=0 en z=h)
  iusl = 1;     
	
  //     idrift = 0 : Sans vitesse de dispersion
  //              1 : Avec vitesse de dispersion
  idrift = 0;


  // Time loop
  while (time < tfin)
    {
      // Advance time
      time = time + dt;
      printf("t=%7.4f s\n",time);
      
      // Viscosity amplification factor
      calbeta(irheolf,alphas,beta,alphasmax,ndimz);

      // Eddy viscosity 
      calviscotlm(iturbu,ilm,dz,h,ufn,alphas,alphasmax,kappa,lmExp,viscoft,ndimz);

      // Bottom boundary condition: (always no-slip)
      j=0;
      a[j]=0.;
      b[j]=1.;
      c[j]=0.;
      
      s[j]=0.;
      
      // Top boundary condition: (0: no-slip / 1: zero gradient) 
      j=ndimz-1;
      if (iusl==0)
	{
	  a[j]=0.;
	  b[j]=1.;
	  c[j]=0.;
	}
      else if (iusl==1)
	{
	  a[j]=-1.;
          b[j]=1.;
          c[j]=0.;
	}
      s[j]=0.;
       

      //Main loop
      for(j=1;j<=ndimz-2;j++)
	{
	  // volume fraction interpolation (staggered grid)
	  alphafp = 0.5*(alphaf[j]+alphaf[j+1]);
	  if (j==1)
	    {
	      dzn = dz;
	      dzs = 1.5*dz;
	      
	      alphafwn=0.5*(alphaf[j+2]+alphaf[j+1]); 
	      alphafws=alphaf[j-1]; 
	    }
	  else if (j==ndimz-2)
	    {
	      dzn = 0.5*dz;
	      dzs = dz;
	      
	      alphafwn=alphaf[j+1]; 
	      alphafws=0.5*(alphaf[j  ]+alphaf[j-1]);
	    }
	  else
	    {
	      dzn = dz;
	      dzs = dz;
	      
	      alphafwn=0.5*(alphaf[j+2]+alphaf[j+1]); 
	      alphafws=0.5*(alphaf[j  ]+alphaf[j-1]);
	    }
	  dzm = 0.5*(dzn+dzs);

	  // Drag model
	  if (idrag==6)
	    {
	      // read from file:
	      //cfdp[j] = cfdpYade[j];
	      cfdp[j] = max(0.,cfdpYade[j]/max(fabs(usnp[j]-ufn[j]),1e-5))/rhof;
	      //printf("%u\t%12.8f\t%12.8f\t%12.8f\n", j,ufn[j],usnp[j],cfdp[j]);
	    }
	  else if (idrag==0)
	    {
	      // Dallavalle + Richardson & Zaki
	      Rep=max(fabs(ufn[j]-usnp[j])*dp/viscof,1e-8);
	      cd=(24.4/Rep+0.4)*pow(alphafp,expoRZ);
	      cfdp[j]=0.75*(1-alphafp)/dp*cd*fabs(ufn[j]-usnp[j]);
	      //printf("%u\t%12.8f\t%12.8f\t%12.8f\n", j,ufn[j],usnp[j],cfdp[j]);
	    }
	  else
	    {
	      printf("idrag value undefined");
	      break;
	    }
	  // Diffusion coefficients
	  // Eddy viscosity terms
	  dummy=dt/dzm;   
	  ds1=dummy*viscoft[j  ]/dzn;
	  dn1=dummy*viscoft[j+1]/dzs;
	  // Viscous terms
	  ds2=dummy*viscof*beta[j  ]/dz*alphafp;
	  dn2=dummy*viscof*beta[j+1]/dz*alphafp;
	  
	  // Numerical scheme coefficient (diffussion only in 1DV)
	  an=dn1+(dn2)*alphafwn;
	  as=ds1+(ds2)*alphafws;
	
	  ap1=dn1+(dn2)*alphafp+ds1+(ds2)*alphafp;

	  // LHS: algebraic system coefficients
	  a[j] = - as;
	  b[j] = alphafp + ap1 + dt*cfdp[j];
	  c[j] = - an;

	  // RHS: unsteady, gravity, drag, pressure gradient
	  s[j]= alphafp*ufn[j] + alphafp*gra*sin(slope)*dt + dt*cfdp[j]*(usnp[j]+udrift[j]) - alphafp*dpdx/rhof*dt;
	  //printf("%u\t%12.8f\t%12.8f\t%12.8f\n", j,a[j],b[j],c[j]);
	}
      // Implicit solution using tridiag (useful because of potential very high viscosities) 
      doubleq( a, b , c, s , ufnp,ndimz);
      // Update solution for next time step
      for(j=0;j<=ndimz-1;j++)
	{
	  ufn[j]=ufnp[j];
	  //printf("%u\t%12.8f\t%12.8f\t%12.8f\n", j,ufn[j],usnp[j],cfdp[j]);
	}
    }
}

void calbeta(int irheolf, double alphas[], double beta[], double alphasmax, unsigned long n)
{
	const unsigned& ndimz=HydroForceEngine::ndimz;
  int j;
  double ratio1,hsuram1;

  // viscosity amplification factor
  if (irheolf==0)
    // 0 : Viscosite du fluide pur
    {
      for(j=0;j<=ndimz;j++)
	beta[j]=1.;
    }
  else if (irheolf==1)
    // 1 : Viscosite d'Einstein
    {
      for(j=0;j<=ndimz;j++)
	beta[j]=1.+2.5*alphas[j];
    }
  else if (irheolf==2)
    // 2 : Viscosite de Graham // this one is buged
    {
      for(j=0;j<=ndimz;j++)
	{
	  ratio1=pow(min(alphas[j]/alphasmax,0.99),0.3333333);
	  hsuram1=0.5*max(ratio1/(1.-ratio1),1e-3);
	  //	  printf("%u\t%7.4f\t%7.4f\n",j,ratio1,hsura);
	  beta[j]=1.+2.5*alphas[j]+2.25*(1+0.5/hsuram1)*(hsuram1-pow(1.+1./hsuram1,-1)-pow(1+1./hsuram1,2));
	}
    }
  else if (irheolf==3)
    // 3 : Viscosite de Krieger-Dougherty / Ishii-Zuber
    {
      for(j=0;j<=ndimz;j++)
	beta[j]=pow(1.-min(alphas[j]/alphasmax,0.99),-(2.5*alphasmax));
    }
  else if (irheolf==4)
    // 4 : Viscosite de Boyer et al.
    {
      for(j=0;j<=ndimz;j++)
	beta[j]=1. + 2.5*alphas[j]*pow(1.-min(alphas[j]/alphasmax,0.99),-1);
    }
}

// ------------------------------------------------------------------------------//

void calviscotlm(int iturbu, int ilm, double dz, double h,double ufn[], double alphas[], double alphasmax, double kappa, double lmExp, double viscoft[], unsigned long n)
{
	const unsigned& ndimz=HydroForceEngine::ndimz;
  int j;
  double lm[n],dist,sum,alphasmid,dzm,dudz;

  // Eddy viscosity 
  if (iturbu==0)
    // 0 : No turbulence
    {
      for(j=0;j<=ndimz;j++)
        viscoft[j]=0.;
    }
  else if (iturbu==1)
    // 1 : Turbulence activated
    {
      if (ilm==0)
	// 0 : Prandtl mixing length
	{
	  dist = 0;
	  lm[0]=0.;
	  for(j=1;j<=ndimz-1;j++)
	    {
	      lm[j]=kappa*dist;
	      dist = dist + dz;
	    }
	}
      else if (ilm==1)
        // 1 : Parabolic profile (free surface flows)
        {
          dist = 0;
	  lm[0]=0.;
          for(j=1;j<=ndimz-1;j++)
            {
              lm[j]=kappa*dist*sqrt(1.-dist/h);
	      dist = dist + dz;
            }
          lm[ndimz-1]=0.;
        }
      else if (ilm==2)
        // 2 : Li and Sawamoto (1995) integral of concentration profile
        {
          dist = 0;
	  sum = 0.;
	  lm[0]=0.;
          for(j=1;j<=ndimz-1;j++)
            {
	      alphasmid=0.5*(alphas[j-1]+alphas[j]);
	      sum = sum + min(alphasmid/alphasmax,0.9999999)*dz;
	      lm[j]=kappa*(dist-pow(sum,lmExp));
	      //	      printf("%u\t%7.4f\t%7.4f\n",j,lm[j],dist*kappa);
              dist=dist+dz;
            }
          lm[ndimz-1]=lm[ndimz-2];
        }
      // Compute the velocity gradient and the mixing length
      for(j=1;j<ndimz-1;j++)
	{
	  if (j==1)
	    dzm=1.5*dz;
	  else if (j==ndimz-1)
	    dzm=0.5*dz;
	  else
	    dzm=dz;
	  dudz=(ufn[j]-ufn[j-1])/dz;
	  viscoft[j]=(1.-alphas[j])*pow(lm[j],2)*fabs(dudz);
	  //printf("%u\t%7.4f\t%7.4f\t%7.4f\n",j,fabs(dudz),lm[j],viscoft[j]);
	}
      viscoft[ndimz-1]=viscoft[ndimz-2];
    }
}

// ------------------------------------------------------------------------------//
void doubleq(double ddam1[],double ddam2[],double ddam3[],double ddbm[],double ddxm[],int n)
{
  /* reosolution of tridiagonal system
       am1,2,3[n] - Tridiagonal matrix coefficients                                     
       bm[n]   - RHS vector        
       xm[n]   - Solution vector        
       em[n], fm[n) - working arrays
       n       - algebraic system size
  */

  int i,ii;
  double ddem[n],ddfm[n],dddiv;

  // downward sweep
  dddiv=ddam2[0];
  ddem[0]=-ddam3[0]/dddiv;
  ddfm[0]=ddbm[0]/dddiv;
  
  for (i=1;i<=n-2;i++)
    {
      dddiv=ddam2[i]+ddam1[i]*ddem[i-1];
      ddem[i]=-ddam3[i]/dddiv;
      ddfm[i]=(ddbm[i]-ddam1[i]*ddfm[i-1])/dddiv;
    }
    // upward sweep
  dddiv=ddam2[n-1]+ddam1[n-1]*ddem[n-2];
  ddfm[n-1]=(ddbm[n-1]-ddam1[n-1]*ddfm[n-2])/dddiv;
  ddxm[n-1]=ddfm[n-1];

  for (ii=1;ii<=n-1;ii++)
    {
      i=n-1-ii;
      ddxm[i]=ddem[i]*ddxm[i+1]+ddfm[i];
    }  
}