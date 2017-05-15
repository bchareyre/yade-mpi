// 2009 © Sergei Dorofeenko <sega@users.berlios.de>
// This file contains a set of classes for modelling of viscoelastic
// particles.

#pragma once

#include<pkg/common/ElastMat.hpp>
#include<pkg/dem/FrictPhys.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<pkg/dem/DemXDofGeom.hpp>
#include<pkg/common/MatchMaker.hpp>
#include<pkg/common/InteractionLoop.hpp>

#ifdef YADE_SPH
#include<pkg/common/SPHEngine.hpp>
#endif
#ifdef YADE_DEFORM
#include <core/PartialEngine.hpp>
#endif

/* Simple viscoelastic model */

/// Material
/// Note: Shop::getViscoelasticFromSpheresInteraction can get kn,cn,ks,cs from a analytical solution of a pair spheres interaction problem.
class ViscElMat : public FrictMat {
	public:
		virtual ~ViscElMat();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(ViscElMat,FrictMat,"Material for simple viscoelastic model of contact from analytical solution of a pair spheres interaction problem  [Pournin2001]_ .",
		((Real,tc,NaN,,"Contact time"))
		((Real,en,NaN,,"Restitution coefficient in normal direction"))
		((Real,et,NaN,,"Restitution coefficient in tangential direction"))
		((Real,kn,NaN,,"Normal elastic stiffness. Attention, this parameter cannot be set if tc, en or es is defined!"))
		((Real,cn,NaN,,"Normal viscous constant. Attention, this parameter cannot be set if tc, en or es is defined!"))
		((Real,ks,NaN,,"Shear elastic stiffness. Attention, this parameter cannot be set if tc, en or es is defined!"))
		((Real,cs,NaN,,"Shear viscous constant. Attention, this parameter cannot be set if tc, en or es is defined!"))
		((Real,mR,0.0,,"Rolling resistance, see [Zhou1999536]_."))
#ifdef YADE_SPH
		((bool,SPHmode,false,,"True, if SPH-mode is enabled."))
		((Real,mu,-1,, "Viscosity. See Mueller [Morris1997]_ ."))                                              // [Mueller2003], (14)
		((Real,h,-1,,  "Core radius. See Mueller [Mueller2003]_ ."))                                            // [Mueller2003], (1)
		((int,KernFunctionPressure,Lucy,, "Kernel function for pressure calculation (by default - Lucy). The following kernel functions are available: Lucy=1."))
		((int,KernFunctionVisco,   Lucy,, "Kernel function for viscosity calculation (by default - Lucy). The following kernel functions are available: Lucy=1."))
#endif
#ifdef YADE_DEFORM
		((bool,DeformEnabled,false,,"True, if particle deformation is needed. Off by default."))
#endif
		((unsigned int,mRtype,1,,"Rolling resistance type, see [Zhou1999536]_. mRtype=1 - equation (3) in [Zhou1999536]_; mRtype=2 - equation (4) in [Zhou1999536]_.")),
		createIndex();
	);
	REGISTER_CLASS_INDEX(ViscElMat,FrictMat);
};
REGISTER_SERIALIZABLE(ViscElMat);

/// Interaction physics
class ViscElPhys : public FrictPhys{
	public:
		virtual ~ViscElPhys();
		Real R;
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(ViscElPhys,FrictPhys,"IPhys created from :yref:`ViscElMat`, for use with :yref:`Law2_ScGeom_ViscElPhys_Basic`.",
		((Real,cn,NaN,,"Normal viscous constant"))
		((Real,cs,NaN,,"Shear viscous constant"))
		((Real,Fn,0.0,,"Normal force of the contact"))
		((Real,Fv,0.0,,"Viscous force of the contact"))
		((Real,mR,0.0,,"Rolling resistance, see [Zhou1999536]_."))
#ifdef YADE_SPH
		((bool,SPHmode,false,,"True, if SPH-mode is enabled."))
		((Real,h,-1,,    "Core radius. See Mueller [Mueller2003]_ ."))                                            // [Mueller2003], (1)
		((Real,mu,-1,,   "Viscosity. See Mueller [Mueller2003]_ ."))                                              // [Mueller2003], (14)
#endif
#ifdef YADE_DEFORM
		((bool,DeformEnabled,false,,"True, if particle deformation mechanism is needed."))
#endif
		((unsigned int,mRtype,1,,"Rolling resistance type, see [Zhou1999536]_. mRtype=1 - equation (3) in [Zhou1999536]_; mRtype=2 - equation (4) in [Zhou1999536]_")),
		createIndex();
	)
#ifdef YADE_SPH
		KernelFunction kernelFunctionCurrentPressure;
		KernelFunction kernelFunctionCurrentVisco;
#endif
	REGISTER_CLASS_INDEX(ViscElPhys,FrictPhys);
};
REGISTER_SERIALIZABLE(ViscElPhys);

/// Convert material to interaction physics.
// Uses the rule of consecutively connection.
class Ip2_ViscElMat_ViscElMat_ViscElPhys: public IPhysFunctor {
	public :
		static Real epsilon;
		virtual void go(const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_ViscElMat_ViscElMat_ViscElPhys,IPhysFunctor,"Convert 2 instances of :yref:`ViscElMat` to :yref:`ViscElPhys` using the rule of consecutive connection.",
 		((shared_ptr<MatchMaker>,tc,,,"Instance of :yref:`MatchMaker` determining contact time"))
		((shared_ptr<MatchMaker>,en,,,"Instance of :yref:`MatchMaker` determining restitution coefficient in normal direction"))
		((shared_ptr<MatchMaker>,et,,,"Instance of :yref:`MatchMaker` determining restitution coefficient in tangential direction"))
		((shared_ptr<MatchMaker>,frictAngle,,,"Instance of :yref:`MatchMaker` determining how to compute interaction's friction angle. If ``None``, minimum value is used."))
		);
	virtual void Calculate_ViscElMat_ViscElMat_ViscElPhys(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction, shared_ptr<ViscElPhys> phys);
	FUNCTOR2D(ViscElMat,ViscElMat);
};
REGISTER_SERIALIZABLE(Ip2_ViscElMat_ViscElMat_ViscElPhys);

/// Constitutive law
/// This class provides linear viscoelastic contact model
class Law2_ScGeom_ViscElPhys_Basic: public LawFunctor {
	public :
		virtual bool go(shared_ptr<IGeom>&, shared_ptr<IPhys>&, Interaction*);
	public :
	FUNCTOR2D(ScGeom,ViscElPhys);
	YADE_CLASS_BASE_DOC(Law2_ScGeom_ViscElPhys_Basic,LawFunctor,"Linear viscoelastic model operating on ScGeom and ViscElPhys. "
      "The contact law is visco-elastic in the normal direction, and visco-elastic frictional in the tangential direction. "
      "The normal contact is modelled as a spring of equivalent stiffness $k_n$, placed in parallel with a viscous damper "
      "of equivalent viscosity $c_n$. As for the tangential contact, it is made of a spring-dashpot system (in parallel "
      "with equivalent stiffness $k_s$ and viscosity $c_s$) in serie with a slider of friction coefficient "
      "$\\mu  = \\tan \\phi$.\n\nThe friction coefficient $\\mu  = \\tan \\phi$ is always evaluated as "
      "$\\tan(\\min(\\phi_1,\\phi_2))$, where $\\phi_1$ and $\\phi_2$ are respectively the friction angle of particle 1 "
      "and 2. For the other parameters, depending on the material input, the equivalent parameters of the contact "
      "($K_n$,$C_n$,$K_s$,$C_s$,$\\phi$) are evaluated differently. In the following, the quantities in parenthesis are "
      "the material constant which are precised for each particle. They are then associated to particle 1 and 2 (e.g. "
      "$kn_1$,$kn_2$,$cn_1$...), and should not be confused with the equivalent parameters of the contact "
      "($K_n$,$C_n$,$K_s$,$C_s$,$\\phi$). \n\n - If contact time (tc), normal and tangential restitution coefficient "
      "(en,et) are precised, the equivalent parameters are evaluated following the formulation of "
      "Pournin [Pournin2001]_.\n\n - If normal and tangential stiffnesses (kn, ks) and damping constant (cn,cs) "
      "of each particle are precised, the equivalent stiffnesses and damping constants of each contact made of "
      "two particles 1 and 2 is made $A = 2\\frac{a_1 a_2}{a_1 + a_2}$, where A is $K_n$, $K_s$, $C_n$ and $C_s$, "
      "and 1 and 2 refer to the value associated to particle 1 and 2.\n\n - Alternatively it is possible to precise "
      "the Young modulus (young) and poisson's ratio (poisson) instead of the normal and spring constant (kn and ks). "
      "In this case, the equivalent parameters are evaluated the same way as the previous case with $kn_x = E_x d_x$, "
      "$ks_x = v_x kn_x$, where $E_x$, $v_x$ and $d_x$ are Young modulus, poisson's ratio and diameter of particle x. "
      "\n\n - If Yound modulus (young), poisson's ratio (poisson), normal and tangential restitution coefficient (en,et) "
      "are precised, the equivalent stiffnesses are evaluated as previously:  $K_n = 2\\frac{kn_1 kn_2}{kn_1 + kn_2}$, "
      "$kn_x = E_x d_x$, $K_s = 2(ks_1 ks_2)/(ks_1 + ks_2)$, $ks_x = v kn_x$. The damping constant is computed at each "
      "contact in order to fulfill the normal restitution coefficient $e_n = (en_1 en_2)/(en_1 + en_2)$. This is "
      "achieved resolving numerically equation 21 of [Schwager2007]_ (There is in fact a mistake in the article from "
      "equation 18 to 19, so that there is a change in sign).  Be careful in this configuration the tangential "
      "restitution coefficient is set to 1 (no tangential damping). This formulation imposes directly the normal "
      "restitution coefficient of the collisions instead of the damping constant.");
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_ViscElPhys_Basic);

Real contactParameterCalculation(const Real& l1,const Real& l2);
bool computeForceTorqueViscEl(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I, Vector3r & force, Vector3r & torque1, Vector3r & torque2);

Real get_en_from_cn(const Real& cn, const Real& m, const Real& kn);
Real find_cn_from_en(const Real& en, const Real& m, const Real& kn, const shared_ptr<Interaction>& interaction);



#ifdef YADE_DEFORM
class DeformControl: public PartialEngine{
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(DeformControl,PartialEngine,"This engine implements particle deformation with const. volume . ",
		// Attrs
		,/* ctor */
		,/* py */
  );
};

REGISTER_SERIALIZABLE(DeformControl);
#endif
