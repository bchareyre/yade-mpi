#pragma once

#include<yade/core/Material.hpp>
#include<yade/pkg/dem/FrictPhys.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/dem/ScGeom.hpp>


class LudingMat : public Material {
  public:
    virtual ~LudingMat();
  YADE_CLASS_BASE_DOC_ATTRS_CTOR(LudingMat,Material,"Material for simple Ludning`s model of contact.\n",
    ((Real,k1,NaN,,"Slope of loading plastic branch"))
    ((Real,kp,NaN,,"Slope of unloading and reloading limit elastic branch"))
    ((Real,kc,NaN,,"Slope of irreversible, tensile adhesive branch"))
    ((Real,PhiF,NaN,,"Dimensionless plasticity depth"))
    ((Real,G0,NaN,,"Viscous damping"))
    ((Real,frictionAngle,NaN,,"Friction angle [rad]")),
    createIndex();
  );
  REGISTER_CLASS_INDEX(LudingMat,Material);
};
REGISTER_SERIALIZABLE(LudingMat);

class LudingPhys : public FrictPhys{
	public:
		virtual ~LudingPhys();
		Real R;
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(LudingPhys,FrictPhys,"IPhys created from :yref:`LudingMat`, for use with :yref:`Law2_ScGeom_LudingPhys_Basic`.",
		((Real,k1,NaN,,"Slope of loading plastic branch"))
		((Real,k2,NaN,,"Slope of unloading and reloading elastic branch"))
		((Real,kp,NaN,,"Slope of unloading and reloading limit elastic branch"))
		((Real,kc,NaN,,"Slope of irreversible, tensile adhesive branch"))
		((Real,PhiF,NaN,,"Dimensionless plasticity depth"))
		((Real,ThetMax,NaN,,"Maximum overlap between particles for a collision"))
		((Real,ThetPMax,NaN,,"Maximum overlap between particles for the limit case"))
		((Real,ThetNull,NaN,,"Force free overlap, plastic contact deformation"))
		((Real,G0,NaN,,"Viscous damping")),
		createIndex();
	)
};
REGISTER_SERIALIZABLE(LudingPhys);

class Ip2_LudingMat_LudingMat_LudingPhys: public IPhysFunctor {
  public :
    virtual void go(const shared_ptr<Material>& b1,
          const shared_ptr<Material>& b2,
          const shared_ptr<Interaction>& interaction);
  YADE_CLASS_BASE_DOC(Ip2_LudingMat_LudingMat_LudingPhys,IPhysFunctor,"Convert 2 instances of :yref:`LudingMat` to :yref:`LudingPhys` using the rule of consecutive connection.");
  FUNCTOR2D(LudingMat,LudingMat);
  private:
    Real reduced(Real, Real);

};
REGISTER_SERIALIZABLE(Ip2_LudingMat_LudingMat_LudingPhys);

class Law2_ScGeom_LudingPhys_Basic: public LawFunctor {
  public :
    virtual void go(shared_ptr<IGeom>&, shared_ptr<IPhys>&, Interaction*);
  private:
    Real calculateCapillarForce(const ScGeom& geom, LudingPhys& phys);
  FUNCTOR2D(ScGeom,LudingPhys);
  YADE_CLASS_BASE_DOC(Law2_ScGeom_LudingPhys_Basic,LawFunctor,"Linear viscoelastic model operating on :yref:`ScGeom` and :yref:`LudingPhys`.");
};
REGISTER_SERIALIZABLE(Law2_ScGeom_LudingPhys_Basic);
