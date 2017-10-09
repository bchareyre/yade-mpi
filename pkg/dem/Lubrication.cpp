

#include "Lubrication.hpp"

YADE_PLUGIN((Ip2_ElastMat_ElastMat_LubricationPhys)(LubricationPhys)(Law2_ScGeom_LubricationPhys)(Law2_ScGeom_ImplicitLubricationPhys))


LubricationPhys::LubricationPhys(const ViscElPhys &obj) :
    ViscElPhys(obj),
    eta(1.),
    eps(0.001),
    ue(0.),
    kno(0.),
    kso(0.),
    nun(0.),
    phic(0.),
    NormalForce(Vector3r::Zero()),
    TangentForce(Vector3r::Zero())
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
    Real h((s1->se3.position-s2->se3.position).norm()-2.*a);
    const Real pi(3.141596);

    // Speeds
    Vector3r normVel((s2->vel-s1->vel).dot(norm)*norm);
    Vector3r normRot((s2->vel-s1->vel).cross(norm)/(2.*a+h));
    Vector3r shearVel((geom->radius1*(s1->angVel-normRot)+geom->radius2*(s2->angVel-normRot)).cross(norm));


    // Forces and torques
    Vector3r FLn = Vector3r::Zero();
    if(activateNormalLubrication) FLn = pi*phys->eta*3./2.*a*a/h*normVel;
    Vector3r FLs = Vector3r::Zero();
    if(activateTangencialLubrication) FLs = pi*phys->eta/2.*(-2.*a+(2.*a+h)*ln((2.*a+h)/h))*shearVel;
    Vector3r Cr  = Vector3r::Zero();
    if(activateRollLubrication) Cr = pi*phys->eta*a*a*a*(3./2.*ln(a/h)+63./500.*h/a*ln(a/h))*((s1->angVel-s2->angVel)-((s1->angVel-s2->angVel).dot(norm))*norm);
    Vector3r Ct  = Vector3r::Zero();
    if(activateTwistLubrication) Ct = pi*phys->eta*a*h*ln(a/h)*(s1->angVel-s2->angVel).dot(norm)*norm;

    // total torque
    Vector3r C1 = (geom->radius1+h/2.)*FLs.cross(norm)+Cr+Ct;
    Vector3r C2 = (geom->radius2+h/2.)*FLs.cross(norm)-Cr-Ct;

    //cout << "FL (" << FLn << ") FLs (" << FLs << ") Cr ("<< Cr << ") Ct (" << Ct << ")\n";

    // Apply!
    scene->forces.addForce(id1,FLn+FLs);
    scene->forces.addTorque(id1,C1);

    scene->forces.addForce(id2,-FLn-FLs);
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
    Real u((s1->se3.position-s2->se3.position).norm()-2.*a);
    const Real pi(3.141596);

    // Speeds
    Vector3r shiftVel=scene->isPeriodic ? Vector3r(scene->cell->velGrad*scene->cell->hSize*interaction->cellDist.cast<Real>()) : Vector3r::Zero();
    Vector3r shift2 = scene->isPeriodic ? Vector3r(scene->cell->hSize*interaction->cellDist.cast<Real>()): Vector3r::Zero();

    Vector3r relV = geom->getIncidentVel(s1, s2, scene->dt, shift2, shiftVel, false );
    Vector3r relVN = relV.dot(norm)*norm; // Normal velocity
    Vector3r relVT = relV - relVN; // Tangeancial velocity
    Real udot = relVN.norm(); // Normal velocity norm

    // Normal force
    Vector3r Fn(Vector3r::Zero());
    Real ue(0.);
    Real g = 3./2.*phys->kno*std::pow(std::abs(phys->ue),0.5); // Stiffness for normal surface deflection
    bool contact = (u + phys->NormalForce.norm()/g) < phys->eps;
    Real kn = (contact) ? g : 0.;

    if(activateNormalLubrication)
    {
        Real A = (g + kn)/phys->nun;
        Real B = -1./scene->dt - g/phys->nun*u - 2*kn/phys->nun*u+kn*phys->eps*a;
        Real C = phys->ue/scene->dt + udot + kn/phys->nun*u*u - kn/phys->nun*u*phys->eps*a;

        Real rho = B*B-4*A*C;

        Real u1 = (-B+std::sqrt(rho))/(2.*A);
        Real u2 = (-B-std::sqrt(rho))/(2.*A);

        ue = (std::abs(phys->ue - u1) < std::abs(phys->ue-u2)) ? u1 : u2;
        Fn = -ue*g*norm;
    }

    // Tangencial force
    Vector3r Ft(Vector3r::Zero());

    if(activateTangencialLubrication)
    {
        Vector3r Ft_ = geom->rotate(phys->TangentForce);
        Real kt = phys->kso*std::pow(ue,0.5);
        Real nut = pi*phys->eta/2.*(-2.*a+(2.*a+u)*ln((2.*a+u)/u));


        if(contact)
        {
            Ft = Ft_ - scene->dt*kt*relVT;
            Real Feps = std::abs(-kn*(u-ue-phys->eps));

            if(Ft.norm() > Feps*std::tan(phys->phic)) // If slip
            {
                Ft = (Ft_ - scene->dt*kt*relVT*(1.+Feps*std::tan(phys->phic)/relVT.norm()))/(1.+kt/nut*scene->dt);
            }
        }
        else
        {
            Ft = (Ft_ - scene->dt*kt*relVT)/(1.+kt/nut*scene->dt);
        }
    }


    // Update Physics memory
    phys->NormalForce = Fn;
    phys->TangentForce = Ft;
    phys->ue = ue;

    // Rolling and twist torques
    Vector3r relAngularVelocity = s1->angVel-s2->angVel;
    Vector3r relTwistVelocity = relAngularVelocity.dot(norm)*norm;
    Vector3r relRollVelocity = relAngularVelocity - relTwistVelocity;

    Vector3r Cr = Vector3r::Zero();
    if(activateRollLubrication) Cr = -pi*phys->eta*a*a*a*(3./2.*ln(a/u)+63./500.*u/a*ln(a/u))*relRollVelocity;
    Vector3r Ct = Vector3r::Zero();
    if (activateTwistLubrication) Ct = -pi*phys->eta*a*u*ln(a/u)*relTwistVelocity;

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


