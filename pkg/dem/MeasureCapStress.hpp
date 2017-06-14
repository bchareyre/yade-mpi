// jerome.duriez@ucalgary.ca

#pragma once

#include <pkg/common/PeriodicEngines.hpp>

class MeasureCapStress : public PeriodicEngine {
  protected:
    Matrix3r matA_BodyGlob(Real, Real, Vector3r); // particle-scale matrix A (see (3.48) p.65 [Khosravani2014]) describing the orientation of wetted surface, expressed in global axis
    Matrix3r matBp_BodyGlob(Real, Real, Real, Real, Vector3r); // particle-scale matrix B' (from (3.49) p.65 [Khosravani2014]) relative to the contact line, expressed in global axis
    Matrix3r matLG_bridgeGlob(Real, Real, Real, Vector3r); // = surface tension * integral over Snw (deltaIJ - nI nJ) dS, expressed in global axis
    Matrix3r matGlobToLoc(Vector3r vecN); // Change of basis matrix, from local basis with vecN = z, see Fig 3.18 p.65 [Khosravani2014]; to global basis X,Y,Z, where vecN is identified by (theta,phi) angles, see Fig 3.8 p.54 [Khosravani2014]. Upon execution of this function, vec N is basically the meniscus' axis of symmetry.
    
  
  public:
    void action();
    YADE_CLASS_BASE_DOC_ATTRS(MeasureCapStress,PeriodicEngine,"Post-processing engine giving :yref:`the capillary stress tensor<MeasureCapStress.capStrTens>` (the fluids mixture contribution to the total stress in unsaturated, i.e. triphasic, conditions) according to the expression detailled in [Duriez2016b]_, [Duriez2017c]_. Although this expression differs in nature from the one of utils.getCapillaryStress (consideration of distributed integrals herein, vs resultant capillary force therein), both are equivalent [Duriez2016b]_, [Duriez2017]_, [Duriez2017c]_.\n The REV volume $V$ entering the expression is automatically measured, from the :yref:`Cell` for periodic conditions, or from :yref:`aabbExtrema<yade.utils.aabbExtrema>` function otherwise.",
			      ((Matrix3r,capStrTens,Matrix3r::Zero(),Attr::readonly,"The capillary stress tensor itself, equal to :yref:`capStrTens1<MeasureCapStress.capStrTens1>` + :yref:`capStrTens2<MeasureCapStress.capStrTens2>` + :yref:`capStrTens3<MeasureCapStress.capStrTens3>` + :yref:`capStrTens4<MeasureCapStress.capStrTens4>`. |yupdate|"))
			      ((Matrix3r,capStrTens1,Matrix3r::Zero(),Attr::readonly,"Wetting fluid volume spherical stress contribution to :yref:`capStrTens<MeasureCapStress.capStrTens>`: $u_c / V \\boldsymbol{\\mu_{Vw}}$ in [Duriez2017c]_. |yupdate|"))
			      ((Matrix3r,capStrTens2,Matrix3r::Zero(),Attr::readonly,"Wetted solid surfaces stress contribution to :yref:`capStrTens<MeasureCapStress.capStrTens>`: $u_c / V \\boldsymbol{\\mu_{Ssw}}$ in [Duriez2017c]_. |yupdate|"))
			      ((Matrix3r,capStrTens3,Matrix3r::Zero(),Attr::readonly,"Wetting/non-wetting interface (e.g. liquid-gas) stress contribution to :yref:`capStrTens<MeasureCapStress.capStrTens>`: $\\gamma_{nw} / V \\boldsymbol{\\mu_{Snw}}$ in [Duriez2017c]_. |yupdate|"))
			      ((Matrix3r,capStrTens4,Matrix3r::Zero(),Attr::readonly,"Contact lines stress contribution to :yref:`capStrTens<MeasureCapStress.capStrTens>`: $\\gamma_{nw} / V \\boldsymbol{\\mu_{\\Gamma}}$ in [Duriez2017c]_. |yupdate|"))
			      ((Real,wettAngle,0,,"Wetting, i.e. contact, angle value (radians). To be defined consistenly with the value upon which the capillary files (used by :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`) are founded."))
			      ((Real,capillaryPressure,0,,"Capillary pressure $u_c$, to be defined equal to :yref:`Law2_ScGeom_CapillaryPhys_Capillarity.capillaryPressure` (Pa)."))
			      ((Real,surfaceTension,0.073,,"Fluid-fluid surface tension $\\gamma_{nw}$, to be defined equal to :yref:`Law2_ScGeom_CapillaryPhys_Capillarity.surfaceTension` (N/m)."))
			      ((bool,debug,0,,"To output some debugging messages."))
			      ((Real,vLiq,0,Attr::readonly,"Wetting fluid volume (m3), summing :yref:`menisci volumes<CapillaryPhys.vMeniscus>` (faster here than through python loops). |yupdate|"))
    );
  
};

REGISTER_SERIALIZABLE(MeasureCapStress);