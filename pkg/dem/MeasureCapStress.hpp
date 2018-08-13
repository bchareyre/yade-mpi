// jerome.duriez@irstea.fr

#pragma once

#include <pkg/common/PeriodicEngines.hpp>

class MeasureCapStress : public PeriodicEngine {
  protected:
    Matrix3r matA_BodyGlob(Real, Real, Vector3r); // particle-scale matrix A (see (3.48) p.65 [Khosravani2014]) describing the orientation of wetted surface, expressed in global axis
    Matrix3r matBp_BodyGlob(Real, Real, Real, Vector3r); // ~ particle-scale matrix B' (from (3.49) p.65 [Khosravani2014]) relative to the contact line, expressed in global axis
    Matrix3r matLG_bridgeGlob(Real, Real, Vector3r); // = integral over Snw (deltaIJ - nI nJ) dS, expressed in global axis
    Matrix3r matGlobToLoc(Vector3r vecN); // Change of basis matrix, from local basis with vecN = z, see Fig 3.18 p.65 [Khosravani2014]; to global basis X,Y,Z, where vecN is identified by (theta,phi) angles, see Fig 3.8 p.54 [Khosravani2014]. Upon execution of this function, vec N is basically the meniscus' axis of symmetry.
    
  
  public:
    void action();
    YADE_CLASS_BASE_DOC_ATTRS(MeasureCapStress,PeriodicEngine,"Post-processing engine giving :yref:`the capillary stress tensor<MeasureCapStress.sigmaCap>` (the fluids mixture contribution to the total stress in unsaturated, i.e. triphasic, conditions) according to the $\\mu$UNSAT expression detailled in [Duriez2017c]_. Although this expression differs in nature from the one of utils.getCapillaryStress (consideration of distributed integrals herein, vs resultant capillary force therein), both are equivalent [Duriez2016b]_, [Duriez2017]_, [Duriez2017c]_. The REV volume $V$ entering the expression is automatically measured, from the :yref:`Cell` for periodic conditions, or from utils.aabbExtrema function otherwise.",
			      ((Matrix3r,sigmaCap,Matrix3r::Zero(),Attr::readonly,"The capillary stress tensor $\\boldsymbol{\\sigma^{cap}}$ itself, expressed as $\\boldsymbol{\\sigma^{cap}} = 1/V \\, [ u_c (\\boldsymbol{\\mu_{Vw}} + \\boldsymbol{\\mu_{Ssw}}) + \\gamma_{nw} (\\boldsymbol{\\mu_{Snw}} + \\boldsymbol{\\mu_{\\Gamma}}) ]$ where the four microstructure tensors $\\boldsymbol{\\mu_{Vw}}, \\boldsymbol{\\mu_{Ssw}}, \\boldsymbol{\\mu_{Snw}}, \\boldsymbol{\\mu_{\\Gamma}}$ correspond to :yref:`muVw<MeasureCapStress.muVw>`, :yref:`muSsw<MeasureCapStress.muSsw>`, :yref:`muSnw<MeasureCapStress.muSnw>` and :yref:`muGamma<MeasureCapStress.muGamma>` attributes. |yupdate|"))
			      ((Matrix3r,muVw,Matrix3r::Zero(),Attr::readonly,"Tensorial contribution (spherical i.e. isotropic) to :yref:`sigmaCap<MeasureCapStress.sigmaCap>` from the wetting fluid volume: $\\boldsymbol{\\mu_{Vw}} = V_w \\, \\boldsymbol{\\delta}$ with $V_w =$ :yref:`vW<MeasureCapStress.vW>` and $\\boldsymbol{\\delta}$ the identity tensor. |yupdate|"))
			      ((Matrix3r,muSsw,Matrix3r::Zero(),Attr::readonly,"Tensorial contribution to :yref:`sigmaCap<MeasureCapStress.sigmaCap>` from the wetted solid surfaces $Ssw$: $\\boldsymbol{\\mu_{Ssw}} = \\int_{Ssw} \\vec n \\otimes \\vec x dS$ with $\\vec n$ the outward normal and $\\vec x$ the position. |yupdate|"))
			      ((Matrix3r,muSnw,Matrix3r::Zero(),Attr::readonly," Tensorial contribution to :yref:`sigmaCap<MeasureCapStress.sigmaCap>` from the wetting/non-wetting (e.g. liquid/gas) interface $S{nw}$: $\\boldsymbol{\\mu_{Snw}} = \\int_{Snw} (\\boldsymbol \\delta - \\vec n \\otimes \\vec n) dS$ with $\\vec n$ the outward normal and $\\boldsymbol{\\delta}$ the identity tensor. |yupdate|"))
			      ((Matrix3r,muGamma,Matrix3r::Zero(),Attr::readonly,"Tensorial contribution to :yref:`sigmaCap<MeasureCapStress.sigmaCap>` from the contact lines $\\Gamma$: $\\boldsymbol{\\mu_{\\Gamma}} = \\int_{\\Gamma} \\vec \\nu_{nw} \\otimes \\vec x \\, dl$ with $\\vec \\nu_{nw}$ the fluid-fluid interface conormal [Duriez2017c]_, and $\\vec x$ the position. |yupdate|"))
			      ((Real,wettAngle,0,,"Wetting, i.e. contact, angle value (radians). To be defined consistently with the value upon which the capillary files (used by :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`) rely."))
			      ((Real,capillaryPressure,0,,"Capillary pressure $u_c$, to be defined equal to :yref:`Law2_ScGeom_CapillaryPhys_Capillarity.capillaryPressure`."))
			      ((Real,surfaceTension,0.073,,"Fluid-fluid surface tension $\\gamma_{nw}$, to be defined equal to :yref:`Law2_ScGeom_CapillaryPhys_Capillarity.surfaceTension`."))
			      ((bool,debug,0,,"To output some debugging messages."))
			      ((Real,vW,0,Attr::readonly,"Wetting fluid volume, summing :yref:`menisci volumes<CapillaryPhys.vMeniscus>` (faster here than through python loops). |yupdate|"))
    );
  
};

REGISTER_SERIALIZABLE(MeasureCapStress);