

#include "Lubrication.hpp"

YADE_PLUGIN((Ip2_ElastMat_ElastMat_LubricationPhys)(LubricationPhys)(Law2_ScGeom_LubricationPhys)(Law2_ScGeom_ImplicitLubricationPhys))


LubricationPhys::LubricationPhys(const ViscElPhys &obj) :
    ViscElPhys(obj),
    eta(1.),
    eps(0.001),
    kno(0.),
    kso(0.),
    nun(0.),
    phic(0.),
    ue(0.),
    contact(false),
    slip(false)
{
}

LubricationPhys::~LubricationPhys()
{

}

CREATE_LOGGER(LubricationPhys);

void Ip2_ElastMat_ElastMat_LubricationPhys::go(const shared_ptr<Material> &material1, const shared_ptr<Material> &material2, const shared_ptr<Interaction> &interaction)
{
    if (interaction->phys)
        return;

    // Cast to Lubrication
    shared_ptr<LubricationPhys> phys(new LubricationPhys());
    FrictMat* mat1 = YADE_CAST<FrictMat*>(material1.get());
    FrictMat* mat2 = YADE_CAST<FrictMat*>(material2.get());
    if(otherPhysFunctor)
    {
        otherPhysFunctor->go(material1,material2,interaction);
        phys->otherPhys = interaction->phys;
    }

    // Copy from HertzMindlin
    /* from interaction physics */
    Real Ea = mat1->young;
    Real Eb = mat2->young;
    Real Va = mat1->poisson;
    Real Vb = mat2->poisson;
    Real fa = mat1->frictionAngle;
    Real fb = mat2->frictionAngle;


    /* from interaction geometry */
    GenericSpheresContact* scg = YADE_CAST<GenericSpheresContact*>(interaction->geom.get());
    Real Da = scg->refR1>0 ? scg->refR1 : scg->refR2;
    Real Db = scg->refR2;
    //Vector3r normal=scg->normal;        //The variable set but not used


    /* calculate stiffness coefficients */
    Real Ga = Ea/(2*(1+Va));
    Real Gb = Eb/(2*(1+Vb));
    Real G = (Ga+Gb)/2; // average of shear modulus
    Real V = (Va+Vb)/2; // average of poisson's ratio
    Real E = Ea*Eb/((1.-std::pow(Va,2))*Eb+(1.-std::pow(Vb,2))*Ea); // Young modulus
    Real R = Da*Db/(Da+Db); // equivalent radius
    Real a = (Da+Db)/2.;
    Real Kno = 4./3.*E*sqrt(R); // coefficient for normal stiffness
    Real Kso = 2*sqrt(4*R)*G/(2-V); // coefficient for shear stiffness
    Real frictionAngle = std::min(fa,fb);
    const Real pi(3.141596);

    phys->kno = Kno;
    phys->kso = Kso;
    phys->phic = frictionAngle;
    phys->nun = pi*eta*3./2.*a*a;

    // Get bodies properties
    Body::id_t id1 = interaction->getId1();
    Body::id_t id2 = interaction->getId2();
    const shared_ptr<Body> b1 = Body::byId(id1,scene);
    const shared_ptr<Body> b2 = Body::byId(id2,scene);
    State* s1 = b1->state.get();
    State* s2 = b2->state.get();
    phys->delta = std::log((s1->se3.position-s2->se3.position).norm()-2.*a);

    phys->eta = eta;
    phys->eps = eps;
    interaction->phys = phys;
}
CREATE_LOGGER(Ip2_ElastMat_ElastMat_LubricationPhys);

bool Law2_ScGeom_LubricationPhys::go(shared_ptr<IGeom> &iGeom, shared_ptr<IPhys> &iPhys, Interaction *interaction)
{
    // Physic
    LubricationPhys* phys=static_cast<LubricationPhys*>(iPhys.get());

    otherLawFunctor->scene = scene;

    // If not activated, only compute other law
    /*if(!activateLubrication)
            return (otherLawFunctor) ? otherLawFunctor->go(iGeom,phys->otherPhys,interaction) : false;//*/

    // Geometry
    ScGeom* geom=static_cast<ScGeom*>(iGeom.get());
    Vector3r norm = geom->normal/geom->normal.norm();

    // Get bodies properties
    Body::id_t id1 = interaction->getId1();
    Body::id_t id2 = interaction->getId2();
    const shared_ptr<Body> b1 = Body::byId(id1,scene);
    const shared_ptr<Body> b2 = Body::byId(id2,scene);
    State* s1 = b1->state.get();
    State* s2 = b2->state.get();

    // geometric parameters
    Real a((geom->radius1+geom->radius2)/2.);
    Real h(geom->penetrationDepth);
    const Real pi(3.141596);

    // Speeds
    Vector3r normVel((s2->vel-s1->vel).dot(norm)*norm);
    Vector3r normRot((s2->vel-s1->vel).cross(norm)/(2.*a+h));
    Vector3r shearVel((geom->radius1*(s1->angVel-normRot)+geom->radius2*(s2->angVel-normRot)).cross(norm));


    // Forces and torques
    Vector3r Fln = Vector3r::Zero();
    if(activateNormalLubrication) Fln = pi*phys->eta*3./2.*a*a/h*normVel;
    Vector3r FLs = Vector3r::Zero();
    if(activateTangencialLubrication) FLs = pi*phys->eta/2.*(-2.*a+(2.*a+h)*std::log((2.*a+h)/h))*shearVel;
    Vector3r Cr  = Vector3r::Zero();
    if(activateRollLubrication) Cr = pi*phys->eta*a*a*a*(3./2.*std::log(a/h)+63./500.*h/a*std::log(a/h))*((s1->angVel-s2->angVel)-((s1->angVel-s2->angVel).dot(norm))*norm);
    Vector3r Ct  = Vector3r::Zero();
    if(activateTwistLubrication) Ct = pi*phys->eta*a*h*std::log(a/h)*(s1->angVel-s2->angVel).dot(norm)*norm;

    // total torque
    Vector3r C1 = (geom->radius1+h/2.)*FLs.cross(norm)+Cr+Ct;
    Vector3r C2 = (geom->radius2+h/2.)*FLs.cross(norm)-Cr-Ct;

    //cout << "FL (" << Fln << ") FLs (" << FLs << ") Cr ("<< Cr << ") Ct (" << Ct << ")\n";

    // Apply!
    scene->forces.addForce(id1,Fln+FLs);
    scene->forces.addTorque(id1,C1);

    scene->forces.addForce(id2,-Fln-FLs);
    scene->forces.addTorque(id2,C2);

    // Compute other law

    if(otherLawFunctor)
        otherLawFunctor->go(iGeom, phys->otherPhys,interaction);

    NormShearPhys* ophys=YADE_CAST<NormShearPhys*>(phys->otherPhys.get());

    if(ophys)
    {
        phys->kn = ophys->kn;
        phys->ks = ophys->ks;
    }

    ViscElPhys* ophysel=YADE_CAST<ViscElPhys*>(phys->otherPhys.get());

    if(ophysel)
    {
        phys->cn += ophysel->cn;
        phys->cs += ophysel->cs;
    }

    return true;
}
CREATE_LOGGER(Law2_ScGeom_LubricationPhys);

bool Law2_ScGeom_ImplicitLubricationPhys::go(shared_ptr<IGeom> &iGeom, shared_ptr<IPhys> &iPhys, Interaction *interaction)
{

    // Physic
    LubricationPhys* phys=static_cast<LubricationPhys*>(iPhys.get());

    // Geometry
    ScGeom* geom=static_cast<ScGeom*>(iGeom.get());
    Vector3r norm = geom->normal/geom->normal.norm();

    // Get bodies properties
    Body::id_t id1 = interaction->getId1();
    Body::id_t id2 = interaction->getId2();
    const shared_ptr<Body> b1 = Body::byId(id1,scene);
    const shared_ptr<Body> b2 = Body::byId(id2,scene);
    State* s1 = b1->state.get();
    State* s2 = b2->state.get();

    // geometric parameters
    Real a((geom->radius1+geom->radius2)/2.);
    Real u(-geom->penetrationDepth);
    const Real pi(3.141596);

    if(debug)
        LOG_INFO("INTER id1:" << id1 << " id2:" << id2 << " u:" << u << " a:" << a);
    
    if(debug)
	LOG_INFO("Called");

    if(u > a)
    {
        return false;
    }

    // Speeds
    Vector3r shiftVel=scene->isPeriodic ? Vector3r(scene->cell->velGrad*scene->cell->hSize*interaction->cellDist.cast<Real>()) : Vector3r::Zero();
    Vector3r shift2 = scene->isPeriodic ? Vector3r(scene->cell->hSize*interaction->cellDist.cast<Real>()): Vector3r::Zero();

    Vector3r relV = geom->getIncidentVel(s1, s2, scene->dt, shift2, shiftVel, false );
    Vector3r relVN = relV.dot(norm)*norm; // Normal velocity
    Vector3r relVT = relV - relVN; // Tangeancial velocity
    Real udot = relV.dot(norm); // Normal velocity norm

    // Normal force
    Vector3r Fn(Vector3r::Zero());
    Real ue(0.),D(0.),un(0.);
    Real delt = max(std::abs(phys->ue),a/100.);
    //Real delt = std::abs(phys->ue);
    
    //delt = a/100.;
    
    Real g = 3./2.*phys->kno*std::pow(delt,0.5); // Stiffness for normal surface deflection
    bool contact = (u - phys->ue) < phys->eps*a;

    if(solution == 2)
        contact = (u-(1+phys->eps)*phys->ue) < phys->eps*a;

    // Log resolution
    if(solution == 4)
        contact = std::exp(phys->delta) < phys->eps*a;

    Real kn = (contact) ? g : 0.;

    if(debug && contact && !phys->contact) LOG_INFO("CONTACT");
    if(debug && !contact && phys->contact) LOG_INFO("END OF CONTACT");

    if(activateNormalLubrication)
    {
        Real G = g/phys->nun;
        Real K = kn/phys->nun;
        Real EPS = phys->eps*a;

        // Full implicit resolution
        if(solution == 1 || solution == 2)
        {
            Real A = G + K;
            Real B = -1./scene->dt - G*u - 2.*K*u + K*EPS;
            Real C = phys->ue/scene->dt + udot + K*u*u - K*u*EPS;

            if(solution == 2)
            {
                A = -G-(1+phys->eps)*K;
                B = G*u-K*(EPS-u-u*(1+phys->eps)) - 1/scene->dt;
                C = phys->ue/scene->dt - udot - K*u*(u-EPS);
            }

            Real rho = B*B-4.*A*C;

            Real u1 = (-B+std::sqrt(rho))/(2.*A);
            Real u2 = (-B-std::sqrt(rho))/(2.*A);

            // FIXME: Very ugly...
            if(rho >= 0)
            {
                ue = (std::abs(phys->ue - u1) < std::abs(phys->ue - u2)) ? u1 : u2;
                //ue2 = (ue == u1) ? u2 : u1;
                //phys->due = ue - phys->ue;
            }
            else
            {
                //LOG_DEBUG("Unable to find a solution: rho < 0");
                ue = phys->ue;// + phys->due;
            }
        }

        // Semi-implicit resolution
        if(solution == 3)
        {
            ue = (K*(u-phys->ue)*(u-EPS)+udot+phys->ue/scene->dt)/((G+K)*(u-phys->ue)+1/scene->dt);
           // ue2 = 0.;
        }

        un = u - ue;

        //Implicit log-resolution
        if(solution == 4)
        {

// Function to be zero
#define fx(d,d_,u,g,k,eps,dt) (std::exp(2.*d)*(g+k)+std::exp(d)*(1./dt-g*u-k*eps)-std::exp(d_)/dt)
#define F(d) fx((d),phys->delta,u,G,K,EPS,scene->dt)

// Expression of F(d)/(dF/dd)(d)
#define fdf(d,d_,u,g,k,eps,dt) ((std::exp(d)*(g+k)+(1.-std::exp(d_-d))/dt-g*u-k*eps)/(2.*std::exp(d)*(g+k)+1./dt-g*u-k*eps))
#define FdF(d) fdf((d),phys->delta,u,G,K,EPS,scene->dt)

            //if(debug)
                //LOG_DEBUG("G K EPS dt u d_ " << G << " " << K << " " << EPS << " " << scene->dt << " " << u << " " << phys->delta);

            Real d = phys->delta;

            for(int i(0);i<20;i++)
            {
                //Real dd = 0.005;
                //Real df = (F(d+dd) - F(d-dd))/(2.*dd);

                //d = d - F(d)/df;

                d = d - FdF(d);

                if(std::abs(F(d)) < 1e-10)
                    break;

                //if(debug)
                    //LOG_DEBUG("d F(d) F/(dF/dd) " << d << " " << F(d) << " " << FdF(d));

                //if(debug && i == 19)
                    //LOG_DEBUG("Max Newton-Rafson steps reach");
            }

            ue = u - std::exp(d);
            un = std::exp(d);
            D = d;
        }

        // calc dtm
        //phys->dtm = (-2.*phys->ue)/(-3.*(G+K)*phys->ue*phys->ue+udot+K*u*(u-EPS)-(-G*u+K*(EPS-2.*u))*phys->ue);

        //calculate the force
        Fn = ue*g*norm;
    }

    // Tangencial force
    Vector3r Ft(Vector3r::Zero());
    Vector3r Ft_ = geom->rotate(phys->shearForce);
    Real kt = phys->kso*std::pow(delt,0.5);
    Real nut = pi*phys->eta/2.*(-2.*a+(2.*a+un)*std::log((2.*a+un)/un));
    
    if(activateTangencialLubrication)
    {

        if(solution == 4)
        {
            nut = pi*phys->eta/2.*(-2.*a+(2.*a+un)*(std::log(2.*a+un)-D));
        }

        phys->slip = false;
        //LOG_INFO("nut: " << nut);

        if(contact)
        {
            Ft = Ft_ - scene->dt*kt*relVT;
            Real Feps = std::abs(-kn*(u-ue-phys->eps*a));

            if(Ft.norm() > Feps*std::tan(phys->phic)) // If slip
            {
                //LOG_INFO("SLIP");
                Ft = (Ft_ - scene->dt*kt*relVT*(1.+Feps*std::tan(phys->phic)/relVT.norm()))/(1.+kt/nut*scene->dt);
                phys->slip = true;
            }
        }
        else
        {
            Ft = (Ft_ - scene->dt*kt*relVT)/(1.+kt/nut*scene->dt);
        }
    }


    // Update Physics memory
    phys->normalForce = Fn;
    phys->ue = ue;
    phys->delta = D;
    phys->shearForce = Ft;
    phys->contact = contact;
    
    // Compute separate forces    
    phys->normalContactForce = un*kn*norm;
    phys->normalLubricationForce = (Fn - un*kn*norm);
    phys->shearLubricationForce = -nut*(relVT + (Ft-Ft_)/(scene->dt*kt));
    phys->shearContactForce = Ft - phys->shearLubricationForce;

    // Rolling and twist torques
    Vector3r relAngularVelocity = geom->getRelAngVel(s1,s2,scene->dt);
    Vector3r relTwistVelocity = relAngularVelocity.dot(norm)*norm;
    Vector3r relRollVelocity = relAngularVelocity - relTwistVelocity;

    Vector3r Cr = Vector3r::Zero();
    if(activateRollLubrication) Cr = pi*phys->eta*a*a*a*(3./2.*std::log(a/u)+63./500.*u/a*std::log(a/u))*relRollVelocity;
    Vector3r Ct = Vector3r::Zero();
    if (activateTwistLubrication) Ct = pi*phys->eta*a*u*std::log(a/u)*relTwistVelocity;

    // total torque
    Vector3r C1 = (geom->radius1+u/2.)*Ft.cross(norm)+Cr+Ct;
    Vector3r C2 = (geom->radius2+u/2.)*Ft.cross(norm)-Cr-Ct;

    // Apply!
    scene->forces.addForce(id1,Fn+Ft);
    scene->forces.addTorque(id1,C1);

    scene->forces.addForce(id2,-Fn-Ft);
    scene->forces.addTorque(id2,C2);

    return true;
}

CREATE_LOGGER(Law2_ScGeom_ImplicitLubricationPhys);

void Law2_ScGeom_ImplicitLubricationPhys::getStressForEachBody(vector<Matrix3r>& NCStresses, vector<Matrix3r>& SCStresses, vector<Matrix3r>& NLStresses, vector<Matrix3r>& SLStresses )
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

			NCStresses[I->getId1()] -= phys->normalContactForce*lV1.transpose();
			NCStresses[I->getId2()] += phys->normalContactForce*lV2.transpose();
			SCStresses[I->getId1()] -= phys->shearContactForce*lV1.transpose();
			SCStresses[I->getId2()] += phys->shearContactForce*lV2.transpose();
			NLStresses[I->getId1()] -= phys->normalLubricationForce*lV1.transpose();
			NLStresses[I->getId2()] += phys->normalLubricationForce*lV2.transpose();
			SLStresses[I->getId1()] -= phys->shearLubricationForce*lV1.transpose();
			SLStresses[I->getId2()] += phys->shearLubricationForce*lV2.transpose();
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


