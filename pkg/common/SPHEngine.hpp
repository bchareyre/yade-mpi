#ifdef YADE_SPH
#pragma once

#include<core/PartialEngine.hpp>
#include<pkg/dem/ScGeom.hpp>

typedef Real (* KernelFunction)(const double & r, const double & h);

enum KernFunctions {Lucy=1,BSpline1=2,BSpline2=3};
#define KERNELFUNCDESCR throw runtime_error("Type of kernel function undefined! The following kernel functions are available: Lucy=1 ([Lucy1977]_ (27)), BSpline1=2 ([Monaghan1985]_ (21)), BSpline2=3 ([Monaghan1985]_ (22)).");

enum typeKernFunctions {Norm, Grad, Lapl};
class SPHEngine: public PartialEngine{
  public:
    void calculateSPHRho(const shared_ptr<Body>& b);
    virtual void action();
  YADE_CLASS_BASE_DOC_ATTRS(SPHEngine,PartialEngine,"Apply given torque (momentum) value at every subscribed particle, at every step. ",
    ((int, mask,-1,, "Bitmask for SPH-particles."))
    ((Real,k,-1,,    "Gas constant for SPH-interactions (only for SPH-model). See Mueller [Mueller2003]_ .")) // [Mueller2003], (11)
    ((Real,rho0,-1,, "Rest density. See Mueller [Mueller2003]_ ."))                                           // [Mueller2003], (1)
    ((Real,h,-1,,    "Core radius. See Mueller [Mueller2003]_ ."))                                            // [Mueller2003], (1)
    ((int,KernFunctionDensity, Lucy,, "Kernel function for density calculation (by default - Lucy). The following kernel functions are available: Lucy=1 ([Lucy1977]_ (27)), BSpline1=2 ([Monaghan1985]_ (21)), BSpline2=3 ([Monaghan1985]_ (22))."))
  );
};
REGISTER_SERIALIZABLE(SPHEngine);
Real smoothkernelLucy(const double & r, const double & h);         
Real smoothkernelLucyGrad(const double & r, const double & h);     
Real smoothkernelLucyLapl(const double & r, const double & h);
Real smoothkernelBSpline1(const double & r, const double & h);         
Real smoothkernelBSpline1Grad(const double & r, const double & h);     
Real smoothkernelBSpline1Lapl(const double & r, const double & h);
Real smoothkernelBSpline2(const double & r, const double & h);         
Real smoothkernelBSpline2Grad(const double & r, const double & h);     
Real smoothkernelBSpline2Lapl(const double & r, const double & h);

KernelFunction returnKernelFunction(const int a, const int b, const typeKernFunctions typeF);
KernelFunction returnKernelFunction(const int a, const typeKernFunctions typeF);

bool computeForceSPH(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I, Vector3r & force);
#endif

