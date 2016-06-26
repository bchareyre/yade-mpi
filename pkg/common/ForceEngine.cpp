// 2004 © Janek Kozicki <cosurgi@berlios.de> 
// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 
// 2014 © Raphael Maurin <raphael.maurin@irstea.fr> 

#include"ForceEngine.hpp"
#include<core/Scene.hpp>
#include<pkg/common/Sphere.hpp>
#include<lib/smoothing/LinearInterpolate.hpp>
#include<pkg/dem/Shop.hpp>

#include<core/IGeom.hpp>
#include<core/IPhys.hpp>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

YADE_PLUGIN((ForceEngine)(InterpolatingDirectedForceEngine)(RadialForceEngine)(DragEngine)(LinearDragEngine)(HydroForceEngine));

void ForceEngine::action(){
	FOREACH(Body::id_t id, ids){
		if (!(scene->bodies->exists(id))) continue;
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
		if (!(scene->bodies->exists(id))) continue;
		const Vector3r& pos=Body::byId(id,scene)->state->pos;
		Vector3r radial=(pos - (axisPt+axisDir * /* t */ ((pos-axisPt).dot(axisDir)))).normalized();
		if(radial.squaredNorm()==0) continue;
		scene->forces.addForce(id,fNorm*radial);
	}
}

void DragEngine::action(){
	FOREACH(Body::id_t id, ids){
		Body* b=Body::byId(id,scene).get();
		if (!b) continue;
		if (!(scene->bodies->exists(id))) continue;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
		if (sphere){
			Real A = sphere->radius*sphere->radius*Mathr::PI;	//Crossection of the sphere
			Vector3r velSphTemp = b->state->vel;
			Vector3r dragForce = Vector3r::Zero();
			
			if (velSphTemp != Vector3r::Zero()) {
				dragForce = -0.5*Rho*A*Cd*velSphTemp.squaredNorm()*velSphTemp.normalized();
			}
			scene->forces.addForce(id,dragForce);
		}
	}
}

void LinearDragEngine::action(){
	FOREACH(Body::id_t id, ids){
		Body* b=Body::byId(id,scene).get();
		if (!b) continue;
		if (!(scene->bodies->exists(id))) continue;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
		if (sphere){
			Vector3r velSphTemp = b->state->vel;
			Vector3r dragForce = Vector3r::Zero();
			
			Real b = 6*Mathr::PI*nu*sphere->radius;
			
			if (velSphTemp != Vector3r::Zero()) {
				dragForce = -b*velSphTemp;
			}
			scene->forces.addForce(id,dragForce);
		}
	}
}

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
			if ((p<nCell)&&(p>0)) {
				Vector3r liftForce = Vector3r::Zero();
				Vector3r dragForce = Vector3r::Zero();
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
				//add the hydro forces to the particle
				scene->forces.addForce(id,dragForce+liftForce+buoyantForce);		
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




