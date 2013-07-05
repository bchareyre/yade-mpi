/*************************************************************************
*  Copyright (C) 2012 by Ignacio Olmedo nolmedo.manich@gmail.com         *
*  Copyright (C) 2012 by François Kneib   francois.kneib@gmail.com       *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once

#include "FrictPhys.hpp"


class InelastCohFrictPhys : public FrictPhys
{
	public :
		virtual ~InelastCohFrictPhys();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(InelastCohFrictPhys,FrictPhys,"",
		((bool,cohesionBroken,false,,"is cohesion active? will be set false when a fragile contact is broken"))
		
		((Real,knT,0,,"tension stiffness"))
		((Real,knC,0,,"compression stiffness"))
		((Real,ktw,0,,"twist shear stiffness"))
		((Real,ks,0,,"shear stiffness"))
		((Real,kr,0,,"bending stiffness"))
		
		((Real,maxElB,0.0,,"Maximum bending elastic moment."))
		((Real,maxElTw,0.0,,"Maximum twist elastic moment."))
		((Real,maxElT,0.0,,"Maximum tension elastic force."))
		((Real,maxElC,0.0,,"Maximum compression elastic force."))
		((Real,shearAdhesion,0,,"Maximum elastic shear force (cohesion)."))
		
		((Real,kTCrp,0.0,,"Tension/compression creep stiffness"))
		((Real,kRCrp,0.0,,"Bending creep stiffness"))
		((Real,kTwCrp,0.0,,"Twist creep stiffness"))
		
		((Real,kTUnld,0.0,,"Tension/compression plastic unload stiffness"))
		((Real,kRUnld,0.0,,"Bending plastic unload stiffness"))
		((Real,kTwUnld,0.0,,"Twist plastic unload stiffness"))
		
		((Real,maxExten,0.0,,"Plastic failure extension (stretching)."))
		((Real,maxContract,0.0,,"Plastic failure contraction (shrinkage)."))
		((Real,maxBendMom,0.0,,"Plastic failure bending moment."))
		((Real,maxTwist,0.0,,"Plastic failure twist angle"))
		
		((bool,isBroken,false,,"true if compression plastic fracture achieved"))

		((Real,unp,0,,"plastic normal penetration depth describing the equilibrium state."))
		((Real,twp,0,,"plastic twist penetration depth describing the equilibrium state."))
		
		((bool,onPlastB,false,Attr::readonly,"true if plasticity achieved on bending"))
		((bool,onPlastTw,false,Attr::readonly,"true if plasticity achieved on twisting"))
		((bool,onPlastT,false,Attr::readonly,"true if plasticity achieved on traction"))
		((bool,onPlastC,false,Attr::readonly,"true if plasticity achieved on compression"))
		
		((Vector2r,maxCrpRchdT,Vector2r(0,0),Attr::readonly,"maximal extension reached on plastic deformation. maxCrpRchdT[0] stores un and maxCrpRchdT[1] stores Fn."))
		((Vector2r,maxCrpRchdC,Vector2r(0,0),Attr::readonly,"maximal compression reached on plastic deformation. maxCrpRchdC[0] stores un and maxCrpRchdC[1] stores Fn."))
		((Vector2r,maxCrpRchdTw,Vector2r(0,0),Attr::readonly,"maximal twist reached on plastic deformation. maxCrpRchdTw[0] stores twist angle and maxCrpRchdTw[1] stores twist moment."))
		((Vector3r,maxCrpRchdB,Vector3r(0,0,0),Attr::readonly,"maximal bending moment reached on plastic deformation."))
		
		((Vector3r,moment_twist,Vector3r(0,0,0),(Attr::readonly),"Twist moment"))
		((Vector3r,moment_bending,Vector3r(0,0,0),(Attr::readonly),"Bending moment"))
		((Vector3r,pureCreep,Vector3r(0,0,0),(Attr::readonly),"Pure creep curve, used for comparison in calculation."))
		((Real,kDam,0,(Attr::readonly),"Damage coefficient on bending, computed from maximum bending moment reached and pure creep behaviour. Its values will vary between :yref:`InelastCohFrictPhys::kr` and :yref:`InelastCohFrictPhys::kRCrp` ."))
		// internal attributes
		,
		createIndex();
	);
/// Indexable
	REGISTER_CLASS_INDEX(InelastCohFrictPhys,FrictPhys);

};

REGISTER_SERIALIZABLE(InelastCohFrictPhys);
