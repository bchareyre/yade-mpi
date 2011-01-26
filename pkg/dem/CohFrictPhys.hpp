/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg/common/NormShearPhys.hpp>
#include<yade/pkg/dem/FrictPhys.hpp>

class CohFrictPhys : public FrictPhys
{
	public :
		virtual ~CohFrictPhys();
		void SetBreakingState ();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CohFrictPhys,FrictPhys,"",
		((bool,cohesionDisablesFriction,false,,"is shear strength the sum of friction and adhesion or only adhesion?"))
		((bool,cohesionBroken,true,,"is cohesion active? will be set false when a fragile contact is broken"))
		((bool,fragile,true,,"do cohesion disapear when contact strength is exceeded?"))
		((Real,kr,0,,"rotational stiffness [N.m/rad]"))
		((Real,ktw,0,,"twist stiffness [N.m/rad]"))
		((Real,maxRollPl,0.0,,"Coefficient to determine the maximum plastic rolling moment."))
		((Vector3r,maxTwistMoment,Vector3r::Zero(),,"Maximum elastic value for the twisting moment (if zero, plasticity will not be applied). In CohFrictMat a parameter should be added to decide what value should be attributed to this threshold value."))
		((Real,normalAdhesion,0,,"tensile strength"))
		((Real,shearAdhesion,0,,"cohesive part of the shear strength (a frictional term might be added depending on :yref:`Law2_ScGeom6D_CohFrictPhys_CohesionMoment::always_use_moment_law`)"))
		((Real,unp,0,,"plastic normal displacement, only used for tensile behaviour and if :yref:`CohFrictPhys::fragile`=false."))
		((Real,unpMax,0,,"maximum value of plastic normal displacement, after that the interaction breaks even if :yref:`CohFrictPhys::fragile`=false. The default value (0) means no maximum."))
		((bool,momentRotationLaw,false,,"use bending/twisting moment at contacts. See :yref:`CohFrictPhys::cohesionDisablesFriction` for details."))
		((Real,creep_viscosity,-1,,"creep viscosity [Pa.s/m]."))
		// internal attributes
		((Vector3r,moment_twist,Vector3r(0,0,0),(Attr::noSave | Attr::readonly),"Twist moment"))
		((Vector3r,moment_bending,Vector3r(0,0,0),(Attr::noSave | Attr::readonly),"Bending moment"))
		,
		createIndex();
	);
/// Indexable
	REGISTER_CLASS_INDEX(CohFrictPhys,FrictPhys);

};

REGISTER_SERIALIZABLE(CohFrictPhys);

