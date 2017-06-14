

#include "Lubrication.hpp"

YADE_PLUGIN((Ip2_ElastMat_ElastMat_LubricationPhys)(LubricationPhys)(Law2_ScGeom_LubricationPhys))


LubricationPhys::LubricationPhys(const NormShearPhys &obj) :
    NormShearPhys(obj),
    eta(1.)
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

    if(otherPhysFunctor)
    {
        otherPhysFunctor->go(material1,material2,interaction);
        phys->otherPhys = interaction->phys;
    }

    phys->eta = eta;
    interaction->phys = phys;
}
CREATE_LOGGER(Ip2_ElastMat_ElastMat_LubricationPhys);

bool Law2_ScGeom_LubricationPhys::go(shared_ptr<IGeom> &iGeom, shared_ptr<IPhys> &iPhys, Interaction *interaction)
{
    // Physic
    LubricationPhys* phys=static_cast<LubricationPhys*>(iPhys.get());

    otherLawFunctor->scene = scene;

    // If not activated, only compute other law
    if(!activateLubrication)
            return (otherLawFunctor) ? otherLawFunctor->go(iGeom,phys->otherPhys,interaction) : false;

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
    Vector3r FLn = pi*phys->eta*3./2.*a*a/h*normVel;
    Vector3r FLs = pi*phys->eta/2.*(-2.*a+(2.*a+h)*ln((2.*a+h)/h))*shearVel;
    Vector3r Cr  = pi*phys->eta*a*a*a*(3./2.*ln(a/h)+63./500.*h/a*ln(a/h))*((s1->angVel-s2->angVel)-((s1->angVel-s2->angVel).dot(norm))*norm);
    Vector3r Ct  = pi*phys->eta*a*h*ln(a/h)*(s1->angVel-s2->angVel).dot(norm)*norm;

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

    return true;
}
CREATE_LOGGER(Law2_ScGeom_LubricationPhys);


