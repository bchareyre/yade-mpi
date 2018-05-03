
#include "Lubrication.hpp"

YADE_PLUGIN((Ip2_FrictMat_FrictMat_LubricationPhys)(LubricationPhys)(Law2_ScGeom_ImplicitLubricationPhys))

LubricationPhys::~LubricationPhys()
{

}

CREATE_LOGGER(LubricationPhys);

void Ip2_FrictMat_FrictMat_LubricationPhys::go(const shared_ptr<Material> &material1, const shared_ptr<Material> &material2, const shared_ptr<Interaction> &interaction)
{
    if (interaction->phys)
        return;

    // Cast to Lubrication
    shared_ptr<LubricationPhys> phys(new LubricationPhys());
    FrictMat* mat1 = YADE_CAST<FrictMat*>(material1.get());
    FrictMat* mat2 = YADE_CAST<FrictMat*>(material2.get());
	
    /* from interaction geometry */
    GenericSpheresContact* scg = YADE_CAST<GenericSpheresContact*>(interaction->geom.get());
    Real Da = scg->refR1>0 ? scg->refR1 : scg->refR2;
    Real Db = scg->refR2;
	
    /* Physical parameters */
    Real Ea = mat1->young;
    Real Eb = mat2->young;
    Real Va = mat1->poisson;
    Real Vb = mat2->poisson;
    Real fa = mat1->frictionAngle;
    Real fb = mat2->frictionAngle;

	/* Hertz-like contact */
    /* calculate stiffness coefficients */
 //   Real Ga = Ea/(2.*(1.+Va));
 //   Real Gb = Eb/(2.*(1.+Vb));
 //   Real G = (Ga+Gb)/2.; // average of shear modulus
 //   Real V = (Va+Vb)/2.; // average of poisson's ratio
    Real E = Ea*Eb/((1.-std::pow(Va,2.))*Eb+(1.-std::pow(Vb,2.))*Ea); // Young modulus
    Real R = Da*Db/(Da+Db); // equivalent radius
    Real Kno = 4./3.*E*sqrt(R); // coefficient for normal stiffness
//    Real Kso = 2.*sqrt(4.*R)*G/(2.-V); // coefficient for shear stiffness
	
    phys->kno = Kno;
	
	/* Cundall-stack-like contact */
	Real Kn = 2.*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);
	Real Ks = 2.*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Vb);
    
	phys->kn = Kn;
	phys->ks = Ks;
	
	/* Friction */
    phys->mum = std::tan(std::min(fa,fb));
	
	/* Fluid (lubrication) */
    Real a = (Da+Db)/2.;
    phys->nun = M_PI*eta*3./2.*a*a;
    phys->eta = eta;
    phys->eps = eps;
	
	/* Integration sheme memory */
    phys->u = -1.;
    phys->prevDotU = 0.;

    interaction->phys = phys;
}
CREATE_LOGGER(Ip2_FrictMat_FrictMat_LubricationPhys);

Real Law2_ScGeom_ImplicitLubricationPhys::normalForce_NRAdimExp(LubricationPhys *phys, ScGeom* geom, Real undot, bool isNew)
{
	// Dry contact
	if(phys->nun <= 0.) {
		if(!warnedOnce) LOG_WARN("Can't solve with dimentionless-exponential method without fluid! using exact.");
		warnedOnce = true;
		return normalForce_trapezoidal(phys, geom, undot, isNew); }
	
	Real a((geom->radius1+geom->radius2)/2.);
	if(isNew) { phys->u = -geom->penetrationDepth-undot*scene->dt; phys->delta = std::log(phys->u/a); }
	
	Real d = NRAdimExp_integrate_u(-geom->penetrationDepth/a, 2.*phys->eps, 1., phys->prevDotU, scene->dt*a*phys->kn/phys->nun, phys->delta); // Dimentionless-exponential resolution!!
	
	phys->normalForce = phys->kn*(-geom->penetrationDepth-a*std::exp(d))*geom->normal;
	phys->normalContactForce = (phys->nun > 0.) ? Vector3r(-phys->kn*(std::max(2.*a*phys->eps-a*std::exp(d),0.))*geom->normal) : phys->normalForce;
	phys->normalLubricationForce = phys->normalForce - phys->normalContactForce;
	
	phys->delta = d;
	phys->u = a*std::exp(d);
	
	phys->contact = phys->normalContactForce.norm() != 0;
	phys->ue = -geom->penetrationDepth - phys->u;
	
	return phys->u;
}

Real Law2_ScGeom_ImplicitLubricationPhys::NRAdimExp_integrate_u(Real const& un, Real const& eps, Real const& alpha, Real & prevDotU, Real const& dt, Real const& prev_d, int depth)
{
	Real d = prev_d;
	
	int i;
	Real a(0);
	
	for(i=0;i<NewtonRafsonMaxIter;i++)
	{
		a = (std::exp(d) < eps) ? alpha : 0.; // Alpha = 0 for non-contact
		
		Real ratio = (dt*(theta*(-(1.+a)*std::exp(d) + a*eps + un) + (1.-theta)*std::exp(prev_d - d)*prevDotU) - 1. + std::exp(prev_d - d))/(dt*theta*(-2.*(1.+a)*std::exp(d) + a*eps + un) - 1.);
		
 		Real F = theta*std::exp(d)*(-(1.+a)*std::exp(d) + a*eps+un) + (1.-theta)*std::exp(prev_d)*prevDotU - 1./dt*(std::exp(d) - std::exp(prev_d));
		
		d = d - ratio;
		
		if(debug) LOG_DEBUG("d " << d << " ratio " << ratio << " F " << F << " i " << i << " a " << a << " depth " << depth);
		
		if(std::abs(F) < NewtonRafsonTol)
			break;
	}
	
	if(i < NewtonRafsonMaxIter || depth > maxSubSteps) {
		if(depth > maxSubSteps) LOG_WARN("Max Substepping reach: results may be inconsistant");
		
		prevDotU = -(1.+a)*std::exp(d) + a*eps + un;
		return d;
	} else {
		// Substepping
		Real d_mid = NRAdimExp_integrate_u(un, eps, alpha, prevDotU, dt/2., prev_d, depth+1);
		return NRAdimExp_integrate_u(un, eps, alpha, prevDotU, dt/2., d_mid, depth+1);
	}
}


template <typename T> int sign(T val) {
    return (int)(T(0) < val) - (val < T(0));
}


Real Law2_ScGeom_ImplicitLubricationPhys::normalForce_NewtonRafson(LubricationPhys *phys, ScGeom* geom, Real undot, bool isNew)
{
	if(isNew) { phys->u = -geom->penetrationDepth-undot*scene->dt; }
	
	Real a((geom->radius1+geom->radius2)/2.);
	Real u = newton_integrate_u(-geom->penetrationDepth, phys->nun, scene->dt, phys->kno, phys->kn, phys->u, 2.*a*phys->eps);
	
	phys->normalForce = phys->kn*(-geom->penetrationDepth-u)*geom->normal;
	phys->normalContactForce = (phys->nun > 0.) ? Vector3r(-phys->kno*std::pow(std::max((2.*a*phys->eps-u),0.),3./2.)*geom->normal) : phys->normalForce;
	phys->normalLubricationForce = phys->normalForce - phys->normalContactForce;
	//phys->normalLubricationForce = phys->nun*(u - phys->u)/(scene->dt*u)*geom->normal;
	phys->u = u;
	phys->contact = phys->normalContactForce.norm() != 0;
	phys->ue = -geom->penetrationDepth - phys->u;
	
	return u;
}

Real Law2_ScGeom_ImplicitLubricationPhys::newton_integrate_u(Real const& un, Real const& nu, Real const& dt, Real const& k, Real const& g, Real const& u_prev, Real const& eps, int depth)
{
	Real u = u_prev;
	
	int i;
	for(i = 0;i<NewtonRafsonMaxIter;i++)
	{
		Real const keff = (u < eps) ? k*std::pow(eps-u,1./2.) : 0.;
		
		Real F = u*(g*(un-u) + keff*(eps-u)) - nu*(u-u_prev)/dt;
		u = u - F/((g*(un-u) + keff*(eps-u)) - u*(g+3./2.*keff) - nu/dt);
		
		if(debug) LOG_DEBUG(" u " << u << " F " << F << " contact  " << (u < eps) << " i " << i << " depth " << depth);
		
		if(std::abs(F)<NewtonRafsonTol)
			break;
	}

	if((i < NewtonRafsonMaxIter && u > 0.) || depth > maxSubSteps) {
		if(depth > NewtonRafsonMaxIter) LOG_WARN("Maximum Newton-Rafson iterations/substepping reach. Results may be inconsistant.");
		return u;
	} else {
		// Substepping
		Real u_mid = newton_integrate_u(un,nu,dt/2.,k,g,u_prev,eps,depth+1);
		return newton_integrate_u(un,nu,dt/2.,k,g,u_mid,eps,depth+1);
	}
}


Real Law2_ScGeom_ImplicitLubricationPhys::normalForce_trapezoidal(LubricationPhys *phys, ScGeom* geom, Real undot, bool isNew)
{
	Real a((geom->radius1+geom->radius2)/2.);
	
	if(isNew) { phys->prev_un= -geom->penetrationDepth-undot*scene->dt; phys->prevDotU=undot*phys->nun ; phys->u = phys->prev_un; }
	
	phys->normalForce = geom->normal*trapz_integrate_u(	phys->prevDotU, phys->prev_un  /*prev. un*/,
					phys->u, -geom->penetrationDepth, phys->nun, phys->kn, phys->kn /*should be keps, currently both are equal*/, 
					2.*phys->eps*a, scene->dt, phys->u<(2*phys->eps*a),
					isNew?(maxSubSteps+1):0/* depth = maxSubSteps+1 will trigger backward Euler for initialization*/);
	
	phys->contact = phys->u < 2.*phys->eps*a;
	phys->normalContactForce = ((phys->contact) ? phys->kn*(phys->u - 2*phys->eps*a) : 0.)*geom->normal;
	phys->normalLubricationForce = phys->normalForce - phys->normalContactForce;
	phys->ue = -geom->penetrationDepth - phys->u;
	
	return phys->u;
}


Real Law2_ScGeom_ImplicitLubricationPhys::trapz_integrate_u(Real& prevDotU, Real& un_prev, Real& u_prev, Real un_curr,
						      const Real& nu, Real k, const Real& keps, const Real& eps, 
						      Real dt, bool withContact, int depth)
{
	Real u=0;// gap distance (by which normal lubrication terms are divided)
	Real /*a=1*/b,c;
	Real keff, un_eff;//effective values, including roughness if contact
	// if contact through roughness is assumed it implies modified coefficients in the ODE compared to no-contact solution.
	// Changes of status are checked at the end
	if (withContact) {
		keff = k + keps;
		un_eff = (k*un_curr+keps*eps)/(k+keps);}
	else {keff=k; un_eff=un_curr;}
	Real w=nu/(dt*keff);
	
	if (depth<=maxSubSteps) {
		// polynomial a*u²+b*u+c=0 with a=1, integrating du/dt=k*u*(un-u)/nu with the theta method
		b=w/theta-un_eff; c=(-prevDotU*(1-theta)/keff-w*u_prev)/theta;}
	else {b= nu/dt/keff-un_eff; c = -w*u_prev; /*implicit backward Euler 1st order*/}
	Real delta = b*b-4*c;//note: a=1
	Real rr[2]={0,0};
	if (delta>=0) {rr[0]=0.5*(-b+sqrt(delta)); rr[1]=0.5*(-b-sqrt(delta));} //roots
	if (delta<0 or rr[0]<0) {// recursive calls after halving the time increment if no positive solution found (no need to check r[1], always smaller)
		if (depth<maxSubSteps) {//sub-stepping
 			//LOG_WARN("delta<0 or negative roots, sub-stepping with dt="<<dt/2.);
			Real un_mid = un_prev+0.5*(un_curr-un_prev);
 			trapz_integrate_u(prevDotU, un_prev,u_prev,un_mid,nu,k,keps,eps,dt/2.,withContact, depth+1);
			return trapz_integrate_u(prevDotU, un_prev,u_prev,un_curr,nu,k,keps,eps,dt/2.,withContact, depth+1);
		} else { // switch to backward Euler (theta = 1) by increasing depth again (see above)
			if (!warnedOnce) {LOG_WARN("minimal sub-step reached (depth="<<maxSubSteps<<"), the result may be innacurate. Increase maxSubSteps?"); /*warnedOnce=true;*/}
			return trapz_integrate_u(prevDotU, un_prev,u_prev,un_curr,nu,k,keps,eps,dt,withContact, depth+1);
		}
	} else {	// normal case, keep the positive solution closest to the previous one, and check contact status
		// select the nearest strictly positive solution, keep 0 only if there is no positive solution
		if ((std::abs(rr[0]-u_prev)<std::abs(rr[1]-u_prev) and rr[0]>0) or rr[1]<=0)
			u = rr[0];
		else u = rr[1];
		bool hasContact = u<eps;
		// if contact appeared/disappeared recalculate with different coefficients (another recursion)
		if (withContact and not hasContact) {
// 			LOG_WARN("withContact and not hasContact");
// 			prevDotU*=keff/k;
// 			keff=k; un_eff=un_curr;// for calculating the relevant prevDotU later
			return trapz_integrate_u(prevDotU, un_prev,u_prev,un_curr,nu,k,keps,eps,dt,false,depth);
		} else if (not withContact and hasContact) {
// 			LOG_WARN("withContact=false and hasContact");
// 			prevDotU*=k/keff;
			return trapz_integrate_u(prevDotU, un_prev,u_prev,un_curr,nu,k,keps,eps,dt,true,depth);}
			
		// The normal case non-recursive case, finally.
		// After a successful integration update the variables and return total force
		prevDotU = keff*u*(un_eff-u);//set for next iteration
		un_prev = un_curr;
		u_prev = u;
		return k*(un_curr-u);}
}

void Law2_ScGeom_ImplicitLubricationPhys::shearForce_firstOrder(LubricationPhys* phys, ScGeom* geom)
{
	Vector3r Ft(Vector3r::Zero());
    Vector3r Ft_ = geom->rotate(phys->shearForce);
	Real a((geom->radius1+geom->radius2)/2.);
    const Vector3r& dus = geom->shearIncrement();
    Real kt = phys->ks;
    Real nut = (phys->eta > 0.) ? M_PI*phys->eta/2.*(-2.*a+(2.*a+phys->u)*std::log((2.*a+phys->u)/phys->u)) : 0.;
    
	phys->shearForce = Vector3r::Zero();
    phys->shearLubricationForce = Vector3r::Zero();
    phys->shearContactForce = Vector3r::Zero();
    phys->cs = nut;
    
	phys->slip = false;
	
	// Also work without fluid (nut == 0)
	if(phys->contact)
	{
		Ft = Ft_ + kt*dus; // Trial force
		phys->shearContactForce = Ft; // If no slip: no lubrication!
#if 1
		if(Ft.norm() > phys->normalContactForce.norm()*std::max(0.,phys->mum)) // If slip
		{
			//LOG_INFO("SLIP");
			Ft *= phys->normalContactForce.norm()*std::max(0.,phys->mum)/Ft.norm();
			phys->shearContactForce = Ft;
			Ft = (Ft*kt*scene->dt + Ft_*nut + dus*kt*nut)/(kt*scene->dt+nut);
			phys->slip = true;
			phys->shearLubricationForce = nut*dus/scene->dt;
		}
#endif
	}
	else
	{
		Ft = (Ft_ + dus*kt)*nut/(nut+kt*scene->dt);
		phys->shearLubricationForce = Ft;
	}
	
	phys->shearForce = Ft;
}

void Law2_ScGeom_ImplicitLubricationPhys::shearForce_firstOrder_log(LubricationPhys* phys, ScGeom* geom)
{
	Vector3r Ft(Vector3r::Zero());
    Vector3r Ft_ = geom->rotate(phys->shearForce);
	Real a((geom->radius1+geom->radius2)/2.);
    const Vector3r& dus = geom->shearIncrement();
    Real kt = phys->ks;
    Real nut = (phys->eta > 0.) ? M_PI*phys->eta/2.*a*(-2.+(2.+std::exp(phys->delta))*(std::log(2.+std::exp(phys->delta)) - phys->delta)) : 0.;
    
	phys->shearForce = Vector3r::Zero();
    phys->shearLubricationForce = Vector3r::Zero();
    phys->shearContactForce = Vector3r::Zero();
    phys->cs = nut;
    
	phys->slip = false;
	
	// Also work without fluid (nut == 0)
	if(phys->contact)
	{
		Ft = Ft_ + kt*dus; // Trial force
		phys->shearContactForce = Ft; // If no slip: no lubrication!
#if 1
		if(Ft.norm() > phys->normalContactForce.norm()*std::max(0.,phys->mum)) // If slip
		{
			//LOG_INFO("SLIP");
			Ft *= phys->normalContactForce.norm()*std::max(0.,phys->mum)/Ft.norm();
			phys->shearContactForce = Ft;
			Ft = (Ft*kt*scene->dt + Ft_*nut + dus*kt*nut)/(kt*scene->dt+nut);
			phys->slip = true;
			phys->shearLubricationForce = nut*dus/scene->dt;
		}
#endif
	}
	else
	{
		Ft = (Ft_ + dus*kt)*nut/(nut+kt*scene->dt);
		phys->shearLubricationForce = Ft;
	}
	
	phys->shearForce = Ft;
}


bool Law2_ScGeom_ImplicitLubricationPhys::go(shared_ptr<IGeom> &iGeom, shared_ptr<IPhys> &iPhys, Interaction *interaction)
{

    // Physic
    LubricationPhys* phys=static_cast<LubricationPhys*>(iPhys.get());

    // Geometry
    ScGeom* geom=static_cast<ScGeom*>(iGeom.get());

    // Get bodies properties
    Body::id_t id1 = interaction->getId1();
    Body::id_t id2 = interaction->getId2();
    const shared_ptr<Body> b1 = Body::byId(id1,scene);
    const shared_ptr<Body> b2 = Body::byId(id2,scene);
    State* s1 = b1->state.get();
    State* s2 = b2->state.get();

    // geometric parameters
    Real a((geom->radius1+geom->radius2)/2.);
    bool isNew=false;
	
	// Speeds
    Vector3r shiftVel=scene->isPeriodic ? Vector3r(scene->cell->velGrad*scene->cell->hSize*interaction->cellDist.cast<Real>()) : Vector3r::Zero();
    Vector3r shift2 = scene->isPeriodic ? Vector3r(scene->cell->hSize*interaction->cellDist.cast<Real>()): Vector3r::Zero();

    Vector3r relV = geom->getIncidentVel(s1, s2, scene->dt, shift2, shiftVel, false );
	//    Vector3r relVN = relV.dot(norm)*norm; // Normal velocity
	//    Vector3r relVT = relV - relVN; // Tangeancial velocity
	Real undot = relV.dot(geom->normal); // Normal velocity norm
    
    if(-geom->penetrationDepth > a)
    {
           //FIXME: it needs to go to potential always based on distance, the "undot < 0" here is dangerous (let the collider do its job), ex: if true is returned here the interaction is still alive and it will be included in the stress
//         return undot < 0; // Only go to potential if distance is increasing 
	    return false;
    }

    // inititalization
	if(phys->u == -1. ) {phys->u = -geom->penetrationDepth; isNew=true;}
	
	// reset forces
	phys->normalForce = Vector3r::Zero();
    phys->normalContactForce = Vector3r::Zero();
    phys->normalLubricationForce = Vector3r::Zero();
	
    if(activateNormalLubrication)
    {
		switch(resolution) {
			case 0: normalForce_trapezoidal(phys,geom, undot, isNew); break;
			case 1: normalForce_NRAdimExp(phys, geom, undot, isNew); break;
			case 2: normalForce_NewtonRafson(phys, geom, undot, isNew); break;
			default:
			LOG_WARN("Nonexistant resolution method. Using exact (0).");
			normalForce_trapezoidal(phys,geom, undot, isNew); break;
		}
	}
	else
	{
		phys->u = -geom->penetrationDepth;
	}
	
	Vector3r C1 = Vector3r::Zero();
	Vector3r C2 = Vector3r::Zero();
	
	if(resolution == 1)
		computeShearForceAndTorques_log(phys, geom, s1, s2, C1, C2);
	else
		computeShearForceAndTorques(phys, geom, s1, s2, C1, C2);
	
    // Apply!
    scene->forces.addForce(id1,phys->normalForce+phys->shearForce);
    scene->forces.addTorque(id1,C1);

    scene->forces.addForce(id2,-(phys->normalForce+phys->shearForce));
    scene->forces.addTorque(id2,C2);

    return true;
}

void Law2_ScGeom_ImplicitLubricationPhys::computeShearForceAndTorques(LubricationPhys *phys, ScGeom* geom, State * s1, State *s2, Vector3r & C1, Vector3r & C2)
{
	Real a((geom->radius1+geom->radius2)/2.);
	if(phys->eta <= 0. || phys->u > 0.)
	{
		if(activateTangencialLubrication) shearForce_firstOrder(phys,geom);
		else {phys->shearForce = Vector3r::Zero(); phys->shearContactForce = Vector3r::Zero(); phys->shearLubricationForce = Vector3r::Zero();}
		
		if (phys->nun > 0.) phys->cn = phys->nun/phys->u;
		
		Vector3r Cr = Vector3r::Zero();
		Vector3r Ct = Vector3r::Zero();

		// Rolling and twist torques
		Vector3r relAngularVelocity = geom->getRelAngVel(s1,s2,scene->dt);
		Vector3r relTwistVelocity = relAngularVelocity.dot(geom->normal)*geom->normal;
		Vector3r relRollVelocity = relAngularVelocity - relTwistVelocity;

		if(activateRollLubrication && phys->eta > 0.) Cr = M_PI*phys->eta*a*a*a*(3./2.*std::log(a/phys->u)+63./500.*phys->u/a*std::log(a/phys->u))*relRollVelocity;
		if (activateTwistLubrication && phys->eta > 0.) Ct = M_PI*phys->eta*a*a*phys->u*std::log(a/phys->u)*relTwistVelocity;
		
	    // total torque
		C1 = -(geom->radius1-geom->penetrationDepth/2.)*phys->shearForce.cross(geom->normal)+Cr+Ct;
		C2 = -(geom->radius2-geom->penetrationDepth/2.)*phys->shearForce.cross(geom->normal)-Cr-Ct;
	}
	else
	{
		LOG_WARN("Gap is negative or null with lubrication: inconsistant results: skip shear force and torques calculation");
	}
}


void Law2_ScGeom_ImplicitLubricationPhys::computeShearForceAndTorques_log(LubricationPhys *phys, ScGeom* geom, State * s1, State *s2, Vector3r & C1, Vector3r & C2)
{
	Real a((geom->radius1+geom->radius2)/2.);
	if(resolution != 1 && !warnedOnce) { LOG_DEBUG("This method use log(u/a) for shear and torque component calculation. Make sure phys->delta is set before calling this method."); warnedOnce = true; }
		
	if(activateTangencialLubrication) shearForce_firstOrder_log(phys,geom);
	else {phys->shearForce = Vector3r::Zero(); phys->shearContactForce = Vector3r::Zero(); phys->shearLubricationForce = Vector3r::Zero();}
	
	if (phys->nun > 0.) phys->cn = phys->nun/phys->u;
	
	Vector3r Cr = Vector3r::Zero();
	Vector3r Ct = Vector3r::Zero();

	// Rolling and twist torques
	Vector3r relAngularVelocity = geom->getRelAngVel(s1,s2,scene->dt);
	Vector3r relTwistVelocity = relAngularVelocity.dot(geom->normal)*geom->normal;
	Vector3r relRollVelocity = relAngularVelocity - relTwistVelocity;

	if(activateRollLubrication && phys->eta > 0.) Cr = -M_PI*phys->eta*a*a*a*(3./2.+63./500.*std::exp(phys->delta))*phys->delta*relRollVelocity;
	if (activateTwistLubrication && phys->eta > 0.) Ct = -M_PI*phys->eta*a*a*a*std::exp(phys->delta)*phys->delta*relTwistVelocity;
	
	// total torque
	C1 = -(geom->radius1-geom->penetrationDepth/2.)*phys->shearForce.cross(geom->normal)+Cr+Ct;
	C2 = -(geom->radius2-geom->penetrationDepth/2.)*phys->shearForce.cross(geom->normal)-Cr-Ct;
}

CREATE_LOGGER(Law2_ScGeom_ImplicitLubricationPhys);

void Law2_ScGeom_ImplicitLubricationPhys::getStressForEachBody(vector<Matrix3r>& NCStresses, vector<Matrix3r>& SCStresses, vector<Matrix3r>& NLStresses, vector<Matrix3r>& SLStresses)
{
  	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	NCStresses.resize(scene->bodies->size());
	SCStresses.resize(scene->bodies->size());
	NLStresses.resize(scene->bodies->size());
	SLStresses.resize(scene->bodies->size());
	for (size_t k=0;k<scene->bodies->size();k++)
	{
		NCStresses[k]=Matrix3r::Zero();
		SCStresses[k]=Matrix3r::Zero();
		NLStresses[k]=Matrix3r::Zero();
		SLStresses[k]=Matrix3r::Zero();
	}
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		LubricationPhys* phys=YADE_CAST<LubricationPhys*>(I->phys.get());
		
		if(phys)
		{
			Vector3r lV1 = (3.0/(4.0*Mathr::PI*pow(geom->refR1,3)))*((geom->contactPoint-Body::byId(I->getId1(),scene)->state->pos));
			Vector3r lV2 = Vector3r::Zero();
			if (!scene->isPeriodic)
				lV2 = (3.0/(4.0*Mathr::PI*pow(geom->refR2,3)))*((geom->contactPoint- (Body::byId(I->getId2(),scene)->state->pos)));
			else
				lV2 = (3.0/(4.0*Mathr::PI*pow(geom->refR2,3)))*((geom->contactPoint- (Body::byId(I->getId2(),scene)->state->pos + (scene->cell->hSize*I->cellDist.cast<Real>()))));

			NCStresses[I->getId1()] += phys->normalContactForce*lV1.transpose();
			NCStresses[I->getId2()] -= phys->normalContactForce*lV2.transpose();
			SCStresses[I->getId1()] += phys->shearContactForce*lV1.transpose();
			SCStresses[I->getId2()] -= phys->shearContactForce*lV2.transpose();
			NLStresses[I->getId1()] += phys->normalLubricationForce*lV1.transpose();
			NLStresses[I->getId2()] -= phys->normalLubricationForce*lV2.transpose();
			SLStresses[I->getId1()] += phys->shearLubricationForce*lV1.transpose();
			SLStresses[I->getId2()] -= phys->shearLubricationForce*lV2.transpose();
		}
	}
}

py::tuple Law2_ScGeom_ImplicitLubricationPhys::PyGetStressForEachBody()
{
	py::list nc, sc, nl, sl;
	vector<Matrix3r> NCs, SCs, NLs, SLs;
	getStressForEachBody(NCs, SCs, NLs, SLs);
	FOREACH(const Matrix3r& m, NCs) nc.append(m);
	FOREACH(const Matrix3r& m, SCs) sc.append(m);
	FOREACH(const Matrix3r& m, NLs) nl.append(m);
	FOREACH(const Matrix3r& m, SLs) sl.append(m);
	return py::make_tuple(nc, sc, nl, sl);
}

void Law2_ScGeom_ImplicitLubricationPhys::getTotalStresses(Matrix3r& NCStresses, Matrix3r& SCStresses, Matrix3r& NLStresses, Matrix3r& SLStresses)
{
	vector<Matrix3r> NCs, SCs, NLs, SLs;
	getStressForEachBody(NCs, SCs, NLs, SLs);
    
  	const shared_ptr<Scene>& scene=Omega::instance().getScene();
    
    if(!scene->isPeriodic)
    {
        LOG_ERROR("This method can only be used in periodic simulations");
        return;
    }
    
    for(unsigned int i(0);i<NCs.size();i++)
    {
        Sphere * s = YADE_CAST<Sphere*>(Body::byId(i,scene)->shape.get());
        
        if(s)
        {
            Real vol = 4./3.*M_PI*pow(s->radius,3);
            
            NCStresses += NCs[i]*vol;
            SCStresses += SCs[i]*vol;
            NLStresses += NLs[i]*vol;
            SLStresses += SLs[i]*vol;
        }
    }
    
    NCStresses /= scene->cell->getVolume();
    SCStresses /= scene->cell->getVolume();
    NLStresses /= scene->cell->getVolume();
    SLStresses /= scene->cell->getVolume();
}

py::tuple Law2_ScGeom_ImplicitLubricationPhys::PyGetTotalStresses()
{
	Matrix3r nc(Matrix3r::Zero()), sc(Matrix3r::Zero()), nl(Matrix3r::Zero()), sl(Matrix3r::Zero());

    getTotalStresses(nc, sc, nl, sl);
	return py::make_tuple(nc, sc, nl, sl);
}


