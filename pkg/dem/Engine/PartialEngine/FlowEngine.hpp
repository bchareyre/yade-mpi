/*************************************************************************
*  Copyright (C) 2009 by Emanuele Catalano                               *
*  emanuele.catalanog@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include<yade/lib-triangulation/FlowBoundingSphere.h>

#ifdef FLOW_ENGINE

class FlowEngine : public PartialEngine
{
	private:
		shared_ptr<TriaxialCompressionEngine> triaxialCompressionEngine;
		shared_ptr<CGT::FlowBoundingSphere> flow;
	public :

		Vector3r gravity;
// 		bool first;

		int current_state
		,previous_state
		,cons;
		
		Real wall_thickness;
		
		void Triangulate ( Scene* ncb );
		void AddBoundary ( Scene* ncb );
		void Initialize ( Scene* ncb, double P_zero );
		void UpdateVolumes ( Scene* ncb );
		void Initialize_volumes ( Scene* ncb );
		Real Volume_cell_single_fictious (CGT::Cell_handle cell, Scene* ncb);
		Real Volume_cell_double_fictious (CGT::Cell_handle cell, Scene* ncb);
		Real Volume_cell_triple_fictious (CGT::Cell_handle cell, Scene* ncb);
		Real Volume_cell (CGT::Cell_handle cell, Scene* ncb);
		void NewTriangulation ( Scene* ncb );
		void Oedometer_Boundary_Conditions();
		
		virtual ~FlowEngine();
	
		virtual void applyCondition(Scene*);
		
		YADE_CLASS_BASE_DOC_ATTRS(FlowEngine,PartialEngine,"An engine to solve the flow problem in saturated granular media",
					((bool,isActivated,true,"Activates Flow Engine "))
					((bool,first,true,"Controls the initialization/update phases"))
					((bool, damped, true, ""))
					((bool, slip_boundary, false, "Controls friction condition on lateral walls"))
					((bool,currentTes,false,"Identifies the current triangulation/tesselation of pore space"))
					((double,P_zero,0,"Initial internal pressure for oedometer test"))
					((int,PermuteInterval,100000,"Pore space re-triangulation period"))
					((bool,compute_K,true,"Activates permeability measure within a granular sample"))
					((double,permeability_factor,1.0,"a permability multiplicator"))
					((Real,loadFactor,1.5,"Load multiplicator for oedometer test"))
					((bool,unload,false,"Remove the load in oedometer test"))
					((bool,tess_based_force,true,"true=force computation based on tessalation, false=force computation based on triangulation")));
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(FlowEngine);

#endif