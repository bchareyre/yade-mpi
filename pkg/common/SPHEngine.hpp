#ifdef YADE_SPH
#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/pkg/dem/ScGeom.hpp>

class SPHEngine: public PartialEngine{
  public:
    void calculateSPHRho(const shared_ptr<Body>& b);
    virtual void action();
  YADE_CLASS_BASE_DOC_ATTRS(SPHEngine,PartialEngine,"Apply given torque (momentum) value at every subscribed particle, at every step.",
    ((int, mask,-1,, "Bitmask for SPH-particles."))
    ((Real,k,-1,,    "Gas constant for SPH-interactions (only for SPH-model). See Mueller [Mueller2003]_ .")) // [Mueller2003], (11)
    ((Real,rho0,-1,, "Rest density. See Mueller [Mueller2003]_ ."))                                           // [Mueller2003], (1)
  );
};
REGISTER_SERIALIZABLE(SPHEngine);
Real smoothkernelPoly6(const double & rrj, const double & h);       // [Mueller2003] (20)
Real smoothkernelSpiky(const double & rrj, const double & h);       // [Mueller2003] (21)
Real smoothkernelVisco(const double & rrj, const double & h);       // [Mueller2003] (22)
Real smoothkernelViscoLapl(const double & rrj, const double & h);   // [Mueller2003] (22+)

void computeForceSPH(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I, Vector3r & force);

#endif

