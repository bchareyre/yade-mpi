#ifdef YADE_POTENTIAL_BLOCKS

#include"KnKsPBLaw.hpp"
#include<core/Scene.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<core/Omega.hpp>
#include<pkg/dem/PotentialBlock.hpp>

YADE_PLUGIN((Law2_SCG_KnKsPBPhys_KnKsPBLaw)(Ip2_FrictMat_FrictMat_KnKsPBPhys)(KnKsPBPhys)

);


/********************** Law2_Dem3DofGeom_RockPMPhys_Rpm ****************************/
CREATE_LOGGER(Law2_SCG_KnKsPBPhys_KnKsPBLaw);



bool Law2_SCG_KnKsPBPhys_KnKsPBLaw::go(shared_ptr<IGeom>& ig /* contact geometry */, shared_ptr<IPhys>& ip /* contact physics */, Interaction* contact){
	const Real& dt = scene->dt; /* size of time step */
	int id1 = contact->getId1();  /* id of Body1 */
	int id2 = contact->getId2(); /* id of Body2 */
	ScGeom*    geom= static_cast<ScGeom*>(ig.get()); /* contact geometry */
	KnKsPBPhys* phys = static_cast<KnKsPBPhys*>(ip.get()); /* contact physics */
	State* de1 = Body::byId(id1,scene)->state.get(); /* pointer to Body1 */
	State* de2 = Body::byId(id2,scene)->state.get(); /* pointer to Body2 */
	Shape* shape1 = Body::byId(id1,scene)->shape.get(); /* pointer to Shape1 */
	Shape* shape2 = Body::byId(id2,scene)->shape.get(); /* pointer to Shape2 */
	PotentialBlock *s1=static_cast<PotentialBlock*>(shape1);
	PotentialBlock *s2=static_cast<PotentialBlock*>(shape2);

	Vector3r& shearForce = phys->shearForce; /* shear force at previous timestep */
	Real un=geom->penetrationDepth; /* overlap distance */
	TRVAR3(geom->penetrationDepth,de1->se3.position,de2->se3.position);
	
	/* ERASE CONTACT OR RESET PARAMETERS IF NO OVERLAP */
	if(geom->penetrationDepth <0.0 ){
		if (neverErase) {
			phys->shearForce = Vector3r::Zero();
			phys->normalForce = Vector3r::Zero();
			phys->normalViscous = Vector3r::Zero();
			geom->normal = Vector3r::Zero();
			phys->tensionBroken = true;
		}else{
			scene->interactions->requestErase(id1,id2);
			return false;
		}
	  	return true;
	}


	Vector3r shiftVel = Vector3r(0,0,0); //scene->isPeriodic ? (Vector3r)((scene->cell->velGrad*scene->cell->Hsize)*Vector3r((Real) contact->cellDist[0],(Real) contact->cellDist[1],(Real) contact->cellDist[2])) : Vector3r::Zero();
	geom->rotateNonSpherical(shearForce); /*rotate shear force according to new contact plane (normal) */ 
	Vector3r oriShear = shearForce; 
	Vector3r oriNormalF = phys->normalForce;

	/* CALCULATE SHEAR INCREMENT */
	Vector3r shift2(0,0,0);
	Vector3r incidentV = geom->getIncidentVel(de1, de2, scene->dt, shift2, shiftVel, /*preventGranularRatcheting*/false );	/* get relative velocity */
	Vector3r incidentVn = geom->normal.dot(incidentV)*geom->normal;  /* get normal relative velocity */
	Vector3r incidentVs = incidentV-incidentVn; /* get shear relative velocity */
	Vector3r shearIncrement=incidentVs*scene->dt; /* calculate shear increment from shear velocity */

	double du = shearIncrement.norm();  /* magnitude of shear increment */
	phys->shearDir = shearIncrement; /* get shear direction */
	if(phys->shearDir.norm() > pow(10,-15)){
		phys->shearDir.normalize();
	}

	if(phys->twoDimension) { phys->contactArea = phys->unitWidth2D*phys->jointLength;} /* contact area in 2D */
	if(s1->isBoundary == true || s2->isBoundary==true){phys->tensionBroken = true; phys->cohesionBroken = true;} /* no cohesion and tension if the contact is a boundary*/
	
	/* NORMAL CONTACT FORCE  */
	double normalStiffness = phys->kn; /* use default stiffness values */
	if(Talesnick){
		//phys->prevSigma = phys->kn*std::max(un,(Real) 0.0);
		//phys->normalForce=phys->prevSigma*phys->normal*std::max(pow(10,-15),phys->contactArea); 
		//#if 0
		//#if 0
		/* linear */
		//un = un - 8.0*pow(10.0,-5);
		double A = 0.5*4.0*pow(10.0,9);//*pow(10.0,9);
		double B = 0.5*7.4*pow(10.0,4);
		double expTerm = B*(std::max(un,0.0)) + std::log(A) ;
		phys->prevSigma = std::max(( (std::exp(expTerm)-A)) / B, 0.0);
		double Fn = phys->prevSigma*std::max(pow(10,-15),phys->contactArea);
		phys->normalForce = Fn*geom->normal; 
		phys->kn  = (A+ B*phys->prevSigma);	
		//#endif
		#if 0
		/* power */phys->h
		double Fn = pow(525000*std::max(un,(Real) 0),1.0/0.25)*std::max(pow(10,-11),phys->contactArea);
		phys->kn  =  2.1*pow(10.0,6)*pow(phys->prevSigma,0.75);
		#endif

		#if 0
		double A = 7.0*pow(10.0,13);//*pow(10.0,9);
		double B = 1.0*pow(10.0,6);
		phys->prevSigma = (A*un)*un - B*un;
		double Fn = phys->prevSigma*std::max(pow(10,-15),phys->contactArea);
		phys->normalForce = Fn*geom->normal; 
		phys->kn  = A*2.0*un - pow(10,6);
		#endif

		#if 0
		//power
		double A = 2.1*pow(10.0,6); //4.2*pow(10.0,5);
		double B = 0.75; //0.88;
		phys->prevSigma = pow( (1.0-B)*A*std::max(un, 0.0),1.0/(1.0-B) );
		double Fn = phys->prevSigma*std::max(pow(10,-14),phys->contactArea);
		phys->kn  =  A*pow(phys->prevSigma,B); //1.0/(1.0-B)*pow( (1.0-B)*A,1.0/(1.0-B) ) * pow(std::max(un,0.0), B/(1.0-B)); //A*pow(phys->prevSigma,B); //
		phys->normalForce = Fn*geom->normal; 
		#endif	

		if (phys->prevSigma > pow(10.0,15) /* || Fn < 0.0 */){
			std::cout<<"prevSigma: "<<phys->prevSigma/* <<", Fn: "<<Fn*/<<endl;
			while(1){	
			}

		}
		phys->Knormal_area = phys->kn*std::max(pow(10,-15),phys->contactArea);
		
	}else{
		if(s1->isBoundary == true || s2->isBoundary==true|| s1->isEastBoundary == true || s2->isEastBoundary==true ){
			normalStiffness = phys->kn_i; /* use special stiffness for boundaries */
			phys->prevSigma = normalStiffness*un;
			phys->normalForce = phys->prevSigma*std::max(pow(10,-15),phys->contactArea)*geom->normal; //std::max(pow(10,-15),phys->contactArea)*
			phys->Knormal_area = normalStiffness*std::max(pow(10,-15),phys->contactArea);
		}else{
			if(s1->isLining==true){normalStiffness=s1->liningStiffness; un = un-s1->liningTensionGap; phys->prevSigma = normalStiffness*un;}
			else if(s2->isLining==true){normalStiffness=s2->liningStiffness; un = un-s2->liningTensionGap; phys->prevSigma = normalStiffness*un;}
			else{ 
				un = un-initialOverlapDistance; 
				if(phys->tensionBroken == true){
					//if(allowBreakage == false && un > 0.0){phys->tensionBroken = false;}
					phys->prevSigma = normalStiffness*std::max(un,0.0);
				}else{
					phys->prevSigma = normalStiffness*un;
				}
			} /* GENERAL CASE - initialOverlapDistance is the offset distance for tension overlap, i.e. negative overlap */
			
			phys->normalForce = phys->prevSigma*std::max(pow(10,-15),phys->contactArea)*geom->normal;
			phys->Knormal_area = normalStiffness*std::max(pow(10,-15),phys->contactArea);
		}
		
	}
	

	/* ERASE CONTACT IF TENSION IS BROKEN */
	if((un <0.0 && fabs(phys->prevSigma)>phys->tension && phys->tensionBroken == false /* first time tension is broken */) || (un<0.0 && phys->tensionBroken==true)){
		if (neverErase) {
			phys->shearForce = Vector3r::Zero();
			phys->normalForce = Vector3r::Zero();
			phys->normalViscous = Vector3r::Zero();
			//geom->normal = Vector3r::Zero();
			phys->tensionBroken = true;
		}else 	{
			return false;
		}
	  	return true;
	}

	
	
	/* SHEAR CONTACT FORCE */

	double Ks = 0.0;
	if(Talesnick){
		//shearForce -= phys->ks*shearIncrement*std::max(pow(10,-15),phys->contactArea); 
		#if 0
		/* TALESNICK */
		/* linear law */
		double shearStiffness = 1.0*pow(10.0,8) + 9.7*pow(10.0,4)*phys->prevSigma; /* current sigmaN from above */ 
		/* power law */		
		//double shearStiffness = 0.95*pow(10.0,6)*pow(phys->prevSigma,0.7); /* current sigmaN from above */ 
		//double shearStiffness = 3.3*pow(10.0,5)*pow(phys->prevSigma,0.88); /* current sigmaN from above */ 
		phys->ks = shearStiffness; 
		Ks = shearStiffness;
		shearForce -= shearStiffness*shearIncrement*std::max(pow(10,-15),phys->contactArea);
		#endif

		//#if 0
		phys->ks = 1.9*pow(10.0,6)*pow(phys->prevSigma,0.7);
		shearForce -= phys->ks*shearIncrement*std::max(pow(10,-14),phys->contactArea);
		//#endif
		phys->Kshear_area = phys->ks*std::max(pow(10,-15),phys->contactArea);
	}else{
		Ks= phys->ks; /* use default values */
		if(s1->isBoundary == true || s2->isBoundary==true || s1->isEastBoundary == true || s2->isEastBoundary==true){
			Ks = phys->ks_i;
			shearForce -= Ks*shearIncrement*std::max(pow(10,-15),phys->contactArea);
			phys->Kshear_area = Ks*std::max(pow(10,-15),phys->contactArea); 
		}else{
			if(s1->isLining==true){Ks=s1->liningStiffness;}
			if(s2->isLining==true){Ks=s2->liningStiffness;}
			shearForce -= Ks*shearIncrement*std::max(pow(10,-15),phys->contactArea); /* GENERAL CASE */
			phys->Kshear_area = Ks*std::max(pow(10,-15),phys->contactArea); 
		}
	}
	

	/* CONTACT DAMPING */
	Real mass1 = 0.0;
	Real mass2 = 0.0;
	const shared_ptr<Body>& b1=Body::byId(id1,scene); 
	const shared_ptr<Body>& b2=Body::byId(id2,scene); 
	if (b1->isClumpMember() == true){
		State* stateClump = Body::byId(b1->clumpId,scene)->state.get();
		mass1 = stateClump->mass;
	}
	if (b2->isClumpMember() == true){
		State* stateClump = Body::byId(b2->clumpId,scene)->state.get();
		mass2 = stateClump->mass;
	}
	if (b1->isClumpMember() == false && b2->isClumpMember() == false){
		mass1 = de1->mass;
		mass2 = de2->mass;
	}
	Real mbar = (!b1->isDynamic() && b2->isDynamic()) ? mass2 : ((!b2->isDynamic() && b1->isDynamic()) ? mass1 : (mass1*mass2 / (mass1 + mass2))); // get equivalent mass 
	Real Cn_crit = 2.*sqrt(mbar*phys->Knormal_area); // Knormal_area Critical damping coefficient (normal direction) 2.*sqrt(mbar*std::min(phys->Knormal_area,phys->Kshear_area))
	Real Cs_crit = Cn_crit ; //2.*sqrt(mbar*phys->Kshear_area); // Kshear_area Critical damping coefficient (shear direction)
	// Note: to compare with the analytical solution you provide cn and cs directly (since here we used a different method to define c_crit)
	double cn = Cn_crit*phys->viscousDamping; // Damping normal coefficient
	double cs = Cs_crit*phys->viscousDamping; // Damping tangential coefficient

	/* Add normal viscous component if damping is included */
	phys->normalViscous = cn*incidentVn;
	phys->normalForce -= phys->normalViscous;
	



	/* FRICTION LIMIT */

	const double PI = std::atan(1.0)*4;
	double tan_effective_phi = 0.0;
	bool useIterativeMethod = false;
	if(Talesnick){
		phys->cumulative_us = phys->cumulative_us + fabs(du);
		phys->effective_phi = phys->phi_b;
		tan_effective_phi = tan(phys->effective_phi/180.0*PI);
		#if 0
		double upeak = 2.0*pow(10.0,-6)*pow(phys->prevSigma,0.213); 
		double delta_miu = phys->ks/phys->prevSigma*(1.0 - std::min(fabs(phys->cumulative_us)/upeak, 1.0) ); if (isnan(delta_miu)){delta_miu = phys->ks/phys->prevSigma;}
		if (shearForce.norm() > phys->normalForce.norm()*phys->effective_phi){
			phys->effective_phi = phys->effective_phi + delta_miu*fabs(du);
		}
		tan_effective_phi = phys->effective_phi;
		#endif
		
	}else{

		if(s1->isBoundary==true || s2->isBoundary == true || phys->jointType==2 ){ // clay layer at boundary;
			useIterativeMethod=false; //fixme			
			//if (allowBreakage == true) {useIterativeMethod = true;}else{
			//	useIterativeMethod = false;
			//	phys->effective_phi = phys->phi_b; // - 3.25*(1.0-exp(-fabs(phys->cumulative_us)/0.4));
			//	tan_effective_phi = tan(phys->effective_phi/180.0*PI);
			//}
		}else if(s1->isLining==true){
			phys->effective_phi = s1->liningFriction;
			tan_effective_phi = tan(phys->effective_phi/180.0*PI);
		}else if(s2->isLining==true){
			phys->effective_phi = s2->liningFriction;
			tan_effective_phi = tan(phys->effective_phi/180.0*PI);
		}else{
			phys->effective_phi = phys->phi_b;
			//if(s1->isEastBoundary==true || s2->isEastBoundary==true){phys->effective_phi = 0.0;}
			tan_effective_phi = tan(phys->effective_phi/180.0*PI);
		}
	}
	
	
	/* SHEAR CORRECTION */
	Vector3r dampedShearForce = shearForce; 
	double cohesiveForce = phys->cohesion*std::max(pow(10,-15),phys->contactArea);
	Real maxFs = 0.0; double maxShear = 0.0;
	if (useIterativeMethod == false){
	
			double fN = phys->normalForce.dot(geom->normal);
			if(std::isnan(fN)){fN=0.0;} 
			if(phys->intactRock == true){
				if(allowBreakage == false || phys->cohesionBroken == false){
					maxFs = cohesiveForce+ fN*tan_effective_phi; 
				}else{
					maxFs = std::max( fN,0.0)*tan_effective_phi; 
				}
			}else{
				maxFs = std::max( fN,0.0)*tan_effective_phi; 
			}
		
		if( shearForce.norm() > maxFs ){
			Real ratio = maxFs / shearForce.norm();
			shearForce *= ratio; 
			shearForce = shearForce;
			if(allowBreakage == true){phys->cohesionBroken = true;}
			dampedShearForce = shearForce;
			phys->shearViscous = Vector3r(0,0,0);
		}else{ /* no damping when it slides */
			phys->shearViscous = Vector3r(0,0,0); //cs*incidentVs; //
			dampedShearForce = shearForce - phys->shearViscous;
		}
	}else{
		Vector3r Fs_prev = oriShear; /* shear force before stress update */
		Vector3r delta_us = -shearIncrement; /* increment of shear displacement */
		double beta = 0.0; /* rate of plastic multiplier */
		double beta_prev = phys->cumulative_us; /* accumulated plastic mutliplier before stress update */
		double fN = phys->normalForce.norm(); //std::max(pow(10,-15),phys->contactArea);
		if(std::isnan(fN)){fN=0.0;} 
		double phi =phys->phi_b;
		Vector3r newFs (0,0,0);
		double plasticDisp = 0.0;
		if(!Talesnick){
			plasticDisp = stressUpdateVec(ip /*contact physics */, Fs_prev, delta_us, beta_prev, phys->Kshear_area /*shear stiffness */,fN,  phi, newFs);
		}else{
			double upeak = 2.0*pow(10.0,-6)*pow(phys->prevSigma,0.213); 
			plasticDisp = stressUpdateVecTalesnick(ip /*contact physics */, Fs_prev, delta_us, beta_prev, phys->Kshear_area /*shear stiffness */,fN,  phi, newFs, upeak);
		}
		shearForce = newFs;
		dampedShearForce = newFs;
		phys->cumulative_us = phys->cumulative_us + plasticDisp; //beta*shearIncrement; /* add plastic displacements */
		double miu_peak = tan(phys->phi_b/180.0*PI);
		double delta_miu = 0.059266;
		tan_effective_phi = miu_peak - delta_miu*(1.0-exp(-phys->cumulative_us/0.35)); 
		phys->effective_phi = atan(tan_effective_phi)/PI*180.0;
		maxFs = fN*tan_effective_phi;
		//if(shearForce.norm()/maxFs > 1.02) {std::cout<<"shearForce.norm()/maxFs: "<<shearForce.norm()/maxFs<<", shearForce-maxFs:"<<shearForce.norm()-maxFs<<", maxFs: "<<maxFs<<", shearForce: "<<shearForce<<endl;}
		if (plasticDisp < pow(10,-15)){ /*elastic*/
			phys->shearViscous = Vector3r(0,0,0); //cs*incidentVs; //
			dampedShearForce = shearForce - phys->shearViscous;
		}
	}


	/* APPLY FORCES */
	Vector3r c1x = geom->contactPoint - de1->pos; 
	Vector3r c2x = geom->contactPoint - de2->pos;
	Vector3r force = -phys->normalForce-dampedShearForce;
	scene->forces.addForce(id1,force);
	scene->forces.addForce(id2,-force);
	Vector3r normal = geom->normal;
	scene->forces.addTorque(id1,c1x.cross(force));
	scene->forces.addTorque(id2,-(c2x).cross(force));
	phys->prevNormal = geom->normal;



	/* RECORDING VALUES AND DEBUGGING */
	if(shearForce.norm() < pow(10,-11) ){phys->mobilizedShear = 1.0;}else{phys->mobilizedShear = shearForce.norm()/maxFs;}
	if(s1->isLining==true){s1->liningTotalPressure = 1.0/s1->liningLength*force; s1->liningNormalPressure=-1.0/s1->liningLength*phys->normalForce; }
	else if(s2->isLining==true){s2->liningTotalPressure = -1.0/s2->liningLength*force; s2->liningNormalPressure=1.0/s2->liningLength*phys->normalForce; }
	if(std::isnan(force.norm())){
		std::cout<<"shearForce: "<<shearForce<<", normalForce: "<<phys->normalForce<<", viscousNormal: "<<phys->normalViscous<<", viscousShear: "<<phys->shearViscous<<", normal: "<<phys->normal<<", geom normal: "<<geom->normal<<", effective_phi: "<<phys->effective_phi<<", shearIncrement: "<<shearIncrement<<", cs: "<<cs<<", incidentVs: "<<incidentVs<<", id1: "<<id1<<", id2: "<<id2<<", debugShear: "<<oriShear<</* " cyF: "<<cyF<<", cyR: "<<cyR<< */", phys->mobilizedShear: "<<phys->mobilizedShear<<endl;
	}
	return true;

}




CREATE_LOGGER(Ip2_FrictMat_FrictMat_KnKsPBPhys);



double Law2_SCG_KnKsPBPhys_KnKsPBLaw::stressUpdateVec(shared_ptr<IPhys>& ip, const Vector3r Fs_prev /*prev shear force*/ , const Vector3r du /*shear displacement increment*/, const double beta_prev /* prev plastic displacements*/, const double Ks /*shear stiffness */,const double fN /*normal force*/, const double phi_b /*peak friction angle*/, Vector3r & newFs /*new shear force*/){

// FUNCTION RETURNS PLASTIC MULTIPLIER RATE (beta) AND CURRENT SHEAR FORCE

	newFs = Vector3r(0,0,0);	
	double maxFs = 0.0;	
	const double PI = std::atan(1.0)*4;
	
	// Define beta_prev as the cumulated plastic multiplier
	// Define beta as the rate of plastic multiplier at the current time step
	// Fs_new = Fs_prev + dF
	// dF = Ks*du_elastic = Ks*(du - du_plastic) = Ks*(du - beta*du_p)  
	double beta = 0.0; //beta is the plastic multiplier
	double effective_phi = phi_b;
	double tan_effective_phi = tan(effective_phi/180.0*PI);
	double miu_peak = tan_effective_phi ;
	double delta_miu = 0.059266;
	double function = 0.0;
	double lambda = 0.0;
	
	newFs = Fs_prev + Ks*du;
	maxFs = fN*(miu_peak-delta_miu*(1.0-exp(-1.0*beta_prev/0.35)));
	
	//If new stress after elastic update is outside the previous yield surface
	
	if (newFs.norm() - maxFs >pow(10,-11) && fN>pow(10,-11) && (Ks*du).norm()>pow(10,-11) ){

		// Fs_new = Fs_prev + dF
		// dF = Ks*du_elastic = Ks*(du - du_plastic) = Ks*(du - beta*du_p)  
		// where du_p is a unit vector whose sign is Sign((Fs_prev + Ks*du).dot(du))*Sign(du);

		/* EQUATION TO SOLVE */
		/* Solve for beta */
		// Fs + Ks*(du - beta*du_p) = N*( miu_peak-delta_miu*(1-exp(beta_prev + beta) ) )
		beta = 0.0;

		

		//////* ESTABLISH LOWER AND UPPER BOUNDS (positive and negative) FOR BRACKETING beta (plastic multiplier) *//////

		/* Establish lower bound for lambda*/
		/* Lower bound = 0.0, i.e. fully elastic */
		/* f_lower_bound <0.0, because it is outside the yield surface */
		double lowerBound = 0.0; 
		Vector3r termA = (Fs_prev + Ks*du)/(Ks*lowerBound + 1.0);
		double beta = beta_prev + lowerBound*termA.norm();
		double f_lower_bound  = termA.norm() - fN*(miu_peak - delta_miu*(1.0 - exp(-1.0*beta/0.35)));
		

		
		/* Establish upper bound for lambda*/
		double upperBound = du.norm()/Fs_prev.norm(); if(std::isnan(upperBound)==true){upperBound=1.0;} if(std::isinf(upperBound)==true || upperBound>pow(10.0,12) ){upperBound=pow(10.0,12);} 
		termA = (Fs_prev + Ks*du)/(Ks*upperBound + 1.0);
		beta = beta_prev + upperBound*termA.norm();
		double f_upper_bound= termA.norm() - fN*(miu_peak - delta_miu*(1.0 - exp(-1.0*beta/0.35)));
		int iterUpper = 0;
		while(Mathr::Sign(f_upper_bound)*Mathr::Sign(f_lower_bound)>0.0){	
			upperBound=5.0*upperBound;
			termA = (Fs_prev + Ks*du)/(Ks*upperBound + 1.0);
			beta = beta_prev + upperBound*termA.norm();
			f_upper_bound= termA.norm() - fN*(miu_peak - delta_miu*(1.0 - exp(-1.0*beta/0.35)));
			iterUpper++;
			if(iterUpper>1000){
				std::cout<<"iterUpper: "<<iterUpper<<endl;
			}		
		}

		double oriUpperBound = upperBound;
		double orif_upper_bound = f_upper_bound;
		double orif_lower_bound = f_lower_bound;
		double midTrial = 0.5*(lowerBound+upperBound);

		int iter = 0;
		function = 1.0;
		/* Bisection to find beta*/
		while(fabs(function) > pow(10,-14)  && fabs(lowerBound-upperBound)> pow(10,-14) ) {
			midTrial = 0.5*(lowerBound+upperBound);
			lambda = midTrial;
			termA = (Fs_prev + Ks*du)/(Ks*lambda + 1.0);
			beta = beta_prev + lambda*termA.norm();
			function = termA.norm() - fN*(miu_peak - delta_miu*(1.0 - exp(-1.0*beta/0.35)));
			double Fmid = function;
			if (Mathr::Sign(Fmid) == Mathr::Sign(f_lower_bound)){ lowerBound = midTrial; f_lower_bound=function;}else{upperBound = midTrial;f_upper_bound=function;}
			iter++;
			if(iter > 98){
				if ( fabs(function) > pow(10,-6)  && fabs(lowerBound-upperBound)> pow(10,-6) ){
					std::cout<<"iter: "<<iter<<", Fs_prev:"<<Fs_prev<<", beta: "<<beta<<", function: "<<function<<", fN: "<<fN<<", beta_prev: "<<beta_prev<<", lowerBound: "<<lowerBound<<", upperBound: "<<upperBound<<", lowerBound-upperBound: "<<lowerBound-upperBound<<", f_lower_bound: "<<f_lower_bound<<", f_upper_bound: "<<f_upper_bound<<", oriUpperBound: "<<oriUpperBound<<", orif_upper_bound: "<<orif_upper_bound<<", orif_lower_bound: "<<orif_lower_bound<<endl;
				}
				break;
			}	
		}
		
		
		newFs = termA;
		maxFs = fN*(miu_peak - delta_miu*(1.0 - exp(-1.0*beta/0.35)));

		if( newFs.norm()/maxFs > 1.05){
			std::cout<<"newFs.norm()/maxFs: "<<newFs.norm()/maxFs<<", newFs-maxFs: "<<newFs.norm()-maxFs<<", newFs: "<<newFs<<", maxFs: "<<maxFs<<", beta_prev: "<<beta_prev<<", newFs.dotFsprev: "<<newFs.dot(Fs_prev)<<"f_upper_bound: "<<f_upper_bound<<", f_lower_bound: "<<f_lower_bound<<", upperBound: "<<upperBound<<", lowerBound: "<<lowerBound<<", du: "<<du.norm()<<", Ks (GPa): "<<Ks*pow(10,-9)<<endl;
		}
		if( std::isnan(beta) ==true ){std::cout<<"beta: "<<beta<<", oriUpperBound: "<<oriUpperBound<<", lambda: "<<lambda<<", lowerBound: "<<lowerBound<<", upperBound: "<<upperBound<<", termA,: "<<termA<<", beta_prev: "<<beta_prev<<", orif_upper_bound: "<<orif_upper_bound<<", orif_lower_bound: "<<orif_lower_bound<<endl;}
		return beta-beta_prev;
	}else{
		// CASE1: FULLY ELASTIC
		newFs = Fs_prev + Ks*du;
		return 0.0;
	}

}



double Law2_SCG_KnKsPBPhys_KnKsPBLaw::stressUpdateVecTalesnick(shared_ptr<IPhys>& ip, const Vector3r Fs_prev /*prev shear force*/ , const Vector3r du /*shear displacement increment*/, const double beta_prev /* prev plastic displacements*/, const double Ks /*shear stiffness */,const double fN /*normal force*/, const double phi_b /*peak friction angle*/, Vector3r & newFs /*new shear force*/, const double upeak){

// FUNCTION RETURNS PLASTIC MULTIPLIER RATE (beta) AND CURRENT SHEAR FORCE

	newFs = Vector3r(0,0,0);	
	double maxFs = 0.0;	
	const double PI = std::atan(1.0)*4;
	
	// Define beta_prev as the cumulated plastic multiplier
	// Define beta as the rate of plastic multiplier at the current time step
	// Fs_new = Fs_prev + dF
	// dF = Ks*du_elastic = Ks*(du - du_plastic) = Ks*(du - beta*du_p)  
	double beta = 0.0; //beta is the plastic multiplier
	double effective_phi = phi_b;
	double tan_effective_phi = tan(effective_phi/180.0*PI);
	double miu_peak = tan_effective_phi ;
	double function = 0.0;
	double lambda = 0.0;
	
	newFs = Fs_prev + Ks*du;
	maxFs = fN*(miu_peak*(1.0-exp(-1.0*beta_prev/upeak)));
	
	//If new stress after elastic update is outside the previous yield surface
	
	if (newFs.norm() - maxFs >pow(10,-11) && fN>pow(10,-11) && (Ks*du).norm()>pow(10,-11) ){

		// Fs_new = Fs_prev + dF
		// dF = Ks*du_elastic = Ks*(du - du_plastic) = Ks*(du - beta*du_p)  
		// where du_p is a unit vector whose sign is Sign((Fs_prev + Ks*du).dot(du))*Sign(du);

		/* EQUATION TO SOLVE */
		/* Solve for beta */
		// Fs + Ks*(du - beta*du_p) = N*( miu_peak-delta_miu*(1-exp(beta_prev + beta) ) )
		beta = 0.0;

		

		//////* ESTABLISH LOWER AND UPPER BOUNDS (positive and negative) FOR BRACKETING beta (plastic multiplier) *//////

		/* Establish lower bound for lambda*/
		/* Lower bound = 0.0, i.e. fully elastic */
		/* f_lower_bound <0.0, because it is outside the yield surface */
		double lowerBound = 0.0; 
		Vector3r termA = (Fs_prev + Ks*du)/(Ks*lowerBound + 1.0);
		double beta = beta_prev + lowerBound*termA.norm();
		double f_lower_bound  = termA.norm() - fN*(miu_peak*(1.0-exp(-1.0*beta_prev/upeak)));
		

		
		/* Establish upper bound for lambda*/
		double upperBound = du.norm()/Fs_prev.norm(); if(std::isnan(upperBound)==true){upperBound=1.0;} if(std::isinf(upperBound)==true || upperBound>pow(10.0,12) ){upperBound=pow(10.0,12);} 
		termA = (Fs_prev + Ks*du)/(Ks*upperBound + 1.0);
		beta = beta_prev + upperBound*termA.norm();
		double f_upper_bound= termA.norm() - fN*(miu_peak*(1.0-exp(-1.0*beta_prev/upeak)));
		int iterUpper = 0;
		while(Mathr::Sign(f_upper_bound)*Mathr::Sign(f_lower_bound)>0.0){	
			upperBound=5.0*upperBound;
			termA = (Fs_prev + Ks*du)/(Ks*upperBound + 1.0);
			beta = beta_prev + upperBound*termA.norm();
			f_upper_bound= termA.norm() - fN*(miu_peak*(1.0-exp(-1.0*beta_prev/upeak)));
			iterUpper++;
			if(iterUpper>1000){
				std::cout<<"iterUpper: "<<iterUpper<<endl;
			}		
		}

		double oriUpperBound = upperBound;
		double orif_upper_bound = f_upper_bound;
		double orif_lower_bound = f_lower_bound;
		double midTrial = 0.5*(lowerBound+upperBound);

		int iter = 0;
		function = 1.0;
		/* Bisection to find beta*/
		while(fabs(function) > pow(10,-14)  && fabs(lowerBound-upperBound)> pow(10,-14) ) {
			midTrial = 0.5*(lowerBound+upperBound);
			lambda = midTrial;
			termA = (Fs_prev + Ks*du)/(Ks*lambda + 1.0);
			beta = beta_prev + lambda*termA.norm();
			function = termA.norm() - fN*(miu_peak*(1.0-exp(-1.0*beta_prev/upeak)));
			double Fmid = function;
			if (Mathr::Sign(Fmid) == Mathr::Sign(f_lower_bound)){ lowerBound = midTrial; f_lower_bound=function;}else{upperBound = midTrial;f_upper_bound=function;}
			iter++;
			if(iter > 98){
				if ( fabs(function) > pow(10,-6)  && fabs(lowerBound-upperBound)> pow(10,-6) ){
					std::cout<<"iter: "<<iter<<", Fs_prev:"<<Fs_prev<<", beta: "<<beta<<", function: "<<function<<", fN: "<<fN<<", beta_prev: "<<beta_prev<<", lowerBound: "<<lowerBound<<", upperBound: "<<upperBound<<", lowerBound-upperBound: "<<lowerBound-upperBound<<", f_lower_bound: "<<f_lower_bound<<", f_upper_bound: "<<f_upper_bound<<", oriUpperBound: "<<oriUpperBound<<", orif_upper_bound: "<<orif_upper_bound<<", orif_lower_bound: "<<orif_lower_bound<<endl;
				}
				break;
			}	
		}
		
		
		newFs = termA;
		maxFs = fN*(miu_peak*(1.0-exp(-1.0*beta_prev/upeak)));

		if( newFs.norm()/maxFs > 1.05){
			std::cout<<"newFs.norm()/maxFs: "<<newFs.norm()/maxFs<<", newFs-maxFs: "<<newFs.norm()-maxFs<<", newFs: "<<newFs<<", maxFs: "<<maxFs<<", beta_prev: "<<beta_prev<<", newFs.dotFsprev: "<<newFs.dot(Fs_prev)<<"f_upper_bound: "<<f_upper_bound<<", f_lower_bound: "<<f_lower_bound<<", upperBound: "<<upperBound<<", lowerBound: "<<lowerBound<<endl;
		}
		if( std::isnan(beta) ==true ){std::cout<<"beta: "<<beta<<", oriUpperBound: "<<oriUpperBound<<", lambda: "<<lambda<<", lowerBound: "<<lowerBound<<", upperBound: "<<upperBound<<", termA,: "<<termA<<", beta_prev: "<<beta_prev<<", orif_upper_bound: "<<orif_upper_bound<<", orif_lower_bound: "<<orif_lower_bound<<endl;}
		return beta-beta_prev;
	}else{
		// CASE1: FULLY ELASTIC
		newFs = Fs_prev + Ks*du;
		return 0.0;
	}

}





void Ip2_FrictMat_FrictMat_KnKsPBPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction){
	
	const double PI = 3.14159265358979323846;
	if(interaction->phys) return; 

	ScGeom* scg=YADE_CAST<ScGeom*>(interaction->geom.get());
			
	assert(scg);

	const shared_ptr<FrictMat>& sdec1 = YADE_PTR_CAST<FrictMat>(b1);
	const shared_ptr<FrictMat>& sdec2 = YADE_PTR_CAST<FrictMat>(b2);
			
	shared_ptr<KnKsPBPhys> contactPhysics(new KnKsPBPhys()); 
	//interaction->interactionPhysics = shared_ptr<MomentPhys>(new MomentPhys());		
	//const shared_ptr<MomentPhys>& contactPhysics = YADE_PTR_CAST<MomentPhys>(interaction->interactionPhysics);

	/* From interaction physics */
	Real fa 	= sdec1->frictionAngle;
	Real fb 	= sdec2->frictionAngle;
				
	
	/* calculate stiffness */
	Real Kn= Knormal;
	Real Ks= Kshear;

	/* Pass values calculated from above to CSPhys */
	contactPhysics->viscousDamping = viscousDamping;
	contactPhysics->useOverlapVol = useOverlapVol;
	contactPhysics->kn = Kn;
	contactPhysics->ks = Ks;
	contactPhysics->kn_i = kn_i;
	contactPhysics->ks_i = ks_i;
	contactPhysics->u_peak = u_peak;
	contactPhysics->maxClosure = maxClosure;
	contactPhysics->cohesionBroken = cohesionBroken;
	contactPhysics->tensionBroken = tensionBroken;
	contactPhysics->unitWidth2D = unitWidth2D;
	contactPhysics->frictionAngle		= std::min(fa,fb); 
	if(!useFaceProperties){
		contactPhysics->phi_r = std::min(fa,fb)/PI*180.0;
		contactPhysics->phi_b = contactPhysics->phi_r;
	}
	contactPhysics->tanFrictionAngle	= std::tan(contactPhysics->frictionAngle); 
	//contactPhysics->initialOrientation1	= Body::byId(interaction->getId1())->state->ori;
	//contactPhysics->initialOrientation2	= Body::byId(interaction->getId2())->state->ori;
	contactPhysics->prevNormal 		= scg->normal; //This is also done in the Contact Law.  It is not redundant because this class is only called ONCE!
	contactPhysics->calJointLength = calJointLength;
	contactPhysics->twoDimension = twoDimension;
	contactPhysics->useFaceProperties = useFaceProperties;
	contactPhysics->brittleLength = brittleLength;
	interaction->phys = contactPhysics;
}

CREATE_LOGGER(KnKsPBPhys);
/* KnKsPBPhys */		
KnKsPBPhys::~KnKsPBPhys(){}

#endif // YADE_POTENTIAL_BLOCKS
