/*************************************************************************
*  Copyright (C) 2009 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
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

		int PermuteInterval;
		int current_state;
		int previous_state;
		bool currentTes;
		
		Real wall_thickness;
		
		bool Pressures_Initialized;

		double P_zero;
		
		
		
		void Triangulate ( Scene* ncb );
		void Initialize ( Scene* ncb, double P_zero );
		void UpdateVolumes ( Scene* ncb );
		void Initialize_volumes ( Scene* ncb );
		Real Volume_cell_single_fictious (CGT::Cell_handle cell, Scene* ncb);
		Real Volume_cell_double_fictious (CGT::Cell_handle cell, Scene* ncb);
		Real Volume_cell_triple_fictious (CGT::Cell_handle cell, Scene* ncb);
		Real Volume_cell (CGT::Cell_handle cell, Scene* ncb);
// 		void NewTriangulation ( Scene* ncb, Tesselation& t1, Tesselation& t2, int currentTes );
		
		FlowEngine();
		virtual ~FlowEngine();
	
		virtual void applyCondition(Scene*);
	
	protected :
	REGISTER_ATTRIBUTES(PartialEngine,/*(gravity)*/(isActivated));
	REGISTER_CLASS_NAME(FlowEngine);
	REGISTER_BASE_CLASS_NAME(PartialEngine);
	
	DECLARE_LOGGER;
};


REGISTER_SERIALIZABLE(FlowEngine);


