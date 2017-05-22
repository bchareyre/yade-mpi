

#include "Lubrication.hpp"

YADE_PLUGIN((Ip2_FrictMat_FrictMat_LubricationPhys)(LubricationPhys)(Law2_ScGeom_LubricationPhys))


LubricationPhys::LubricationPhys(const NormShearPhys &obj) :
    NormShearPhys(obj),
    eta(1.)
{
    createIndex();
}

LubricationPhys::~LubricationPhys()
{

}

CREATE_LOGGER(LubricationPhys);

void Ip2_FrictMat_FrictMat_LubricationPhys::go(const shared_ptr<Material> &material1, const shared_ptr<Material> &material2, const shared_ptr<Interaction> &interaction)
{
    // Cast to Lubrication
    shared_ptr<LubricationPhys> phys(new LubricationPhys());
    phys->eta = eta;
    interaction->phys = phys;
}
CREATE_LOGGER(Ip2_FrictMat_FrictMat_LubricationPhys);

bool Law2_ScGeom_LubricationPhys::go(shared_ptr<IGeom> &iGeom, shared_ptr<IPhys> &iPhys, Interaction *interaction)
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

    Real a((geom->radius1+geom->radius2)/2.);
    Real h((s1->se3.position-s2->se3.position).norm());

    Vector3r relvel(b1->state.get()->vel-b2->state.get()->vel);

    relvel = relvel.dot(norm)*norm; // projection to normal

    Vector3r normalForce = 3./2.*3.141596*phys->eta*a*a/h*relvel;

    m_force = normalForce;
    m_speed = relvel;
    m_speed1 = s1->vel;
    m_speed2 = s2->vel;

    if (!scene->isPeriodic) {
            applyForceAtContactPoint(normalForce, geom->contactPoint , id1, s1->se3.position, id2, s2->se3.position);
    } else {
            scene->forces.addForce(id1,normalForce);
            scene->forces.addForce(id2,-normalForce);
    }
    return true;
}
CREATE_LOGGER(Law2_ScGeom_LubricationPhys);


