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

// class TriaxialCompressionEngine;
// class FlowBoundingSphere;
// class Tesselation;

class FlowEngine : public PartialEngine
{
	private:
		shared_ptr<TriaxialCompressionEngine> triaxialCompressionEngine;
		shared_ptr<CGT::FlowBoundingSphere> flow;
// 		FlowBoundingSphere* flow;
		//Tesselation* Tes;
	public :

		Vector3r gravity;
		bool isActivated;
		bool first;

		int PermuteInterval,
		current_state;
		double permeability_factor;
		int previous_state;
		bool currentTes
		,compute_K
		,unload
		,tess_based_force;
		Real loadFactor;
		int cons;
		
		
		Real wall_thickness;

		double P_zero;
		
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
		
		FlowEngine();
		virtual ~FlowEngine();
	
		virtual void applyCondition(Scene*);
		
		REGISTER_ATTRIBUTES(PartialEngine,(isActivated)(first)(currentTes)(P_zero)(PermuteInterval)(compute_K)(permeability_factor)(loadFactor)(unload)(tess_based_force));
	
	protected :
	REGISTER_CLASS_NAME(FlowEngine);
	REGISTER_BASE_CLASS_NAME(PartialEngine);
	
	DECLARE_LOGGER;
};


REGISTER_SERIALIZABLE(FlowEngine);


