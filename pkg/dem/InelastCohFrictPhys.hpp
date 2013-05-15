#pragma once

#include "CohFrictPhys.hpp"


class InelastCohFrictPhys : public CohFrictPhys
{
	public :
		virtual ~InelastCohFrictPhys();
		void SetBreakingState ();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(InelastCohFrictPhys,CohFrictPhys,"",
		((bool,cohesionBroken,false,,"is cohesion active? will be set false when a fragile contact is broken"))
		((bool,fragile,false,,"do cohesion disapear when contact strength is exceeded?"))
		((Real,knT,0,,"tension stiffness [N.m/rad]"))
		((Real,knC,0,,"compression stiffness [N.m/rad]"))
		((Real,kt,0,,"twist shear stiffness [N.m/rad]"))
		((Real,ks,0,,"shear stiffness [N.m/rad]"))
		((Real,kr,0,,"bending stiffness [N.m/rad]"))
		((Real,maxElastB,0.0,,"max. elastic Bending, zero if non elasto-plastic behavour "))
		((Real,maxElastTw,0.0,,"max. elastic Twist, zero if non elasto-plastic behavour "))
		((Real,dElT,0.0,,"Max elastic displacement Tension"))
		((Real,dElC,0.0,,"Max elastic displacement Compression"))
		((Real,crpT,0.0,,"Bending creep"))
		((Real,crpB,0.0,,"Tension crepp"))
		((Real,crpTw,0.0,,"Twist creep "))
		((Real,epsMaxT,0.0,,"Maximal plastic strain Tension"))
		((Real,epsMaxC,0.0,,"Maximal plastic strain compression"))
		((Real,phBMax,0.0,,"Maximal plastic bending strain"))
		((Real,phTwMax,0.0,,"Maximal plastic bending strain"))
		((Real,unldT,0.0,,"Tension/compression plastic unload"))
		((Real,unldB,0.0,,"bending plastic unload"))
		((Real,unldTw,0.0,,"twist plastic unload"))
		((bool,isBrokenB,false,,"true if bend plastic fracture achieved"))
		((bool,isBrokenT,false,,"true if Traction or compression plastic fracture achieved"))
		((bool,isBrokenTw,false,,"true if twist plastic fracture achieved"))
		((bool,unloadedT,false,,"true if unload in Traction plastic deformation"))
		((bool,unloadedC,false,,"true if unload in compression plastic deformation"))
		((bool,unloadedB,false,,"true if unload in bending plastic deformation"))
		((bool,unloadedTw,false,,"true if unload in twist plastic deformation"))
		((Real,normalAdhesion,0,,"tensile strength"))
		((Real,shearAdhesion,0,,"cohesive part of the shear strength (a frictional term might be added depending on :yref:`Law2_ScGeom6D_CohFrictPhys_CohesionMoment::always_use_moment_law`)"))
		((Real,unp,0,,"plastic normal displacement, only used for tensile behaviour and if :yref:`CohFrictPhys::fragile`=false."))
		((bool,onPlastic,false,,"true if plastic behaviour achieved, for creeping calculation"))
		// internal attributes
		,
		createIndex();
	);
/// Indexable
	REGISTER_CLASS_INDEX(InelastCohFrictPhys,CohFrictPhys);

};

REGISTER_SERIALIZABLE(InelastCohFrictPhys);
