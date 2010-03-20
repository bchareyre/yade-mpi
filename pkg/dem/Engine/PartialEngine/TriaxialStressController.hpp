/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GlobalEngine.hpp>
#include<yade/core/Scene.hpp>
#include<yade/lib-base/Math.hpp>

class Scene;
class State;


/*! \brief Controls the stress on the boundaries of a box and compute strain-like and stress-like quantities for the packing

	detailed description...
*/

class TriaxialStressController : public GlobalEngine 
{
	private :
// 		Real previousStress, previousMultiplier; //previous mean stress and size multiplier		
		bool first;
		inline const Vector3r getForce(Scene* rb, body_id_t id){ return rb->forces.getForce(id); /* needs sync, which is done at the beginning of action */ }
		
		 	
	public :
// 		unsigned int stiffnessUpdateInterval, computeStressStrainInterval, radiusControlInterval;
		//! internal index values for retrieving walls
		enum { wall_bottom=0, wall_top, wall_left, wall_right, wall_front, wall_back };
		//! real index values of walls in the Scene
		int wall_id [6];
//   		vector<int> wall_id;
		//! Stores the value of the translation at the previous time step, stiffness, and normal
		Vector3r	previousTranslation [6];
		//! The value of stiffness (updated according to stiffnessUpdateInterval) 
		vector<Real>	stiffness;
		Real 		strain [3];
// 		Real volumetricStrain;
		Vector3r	normal [6];
		//! The values of stresses 
		Vector3r	stress [6];
		Vector3r	force [6];
// 		Real		meanStress;
		//! Value of spheres volume (solid volume)
		Real spheresVolume;
		//! Value of box volume 
		Real boxVolume;
		//! Sample porosity
		Real porosity;
				
		
		//Real UnbalancedForce;		
				
		//! wallDamping coefficient - wallDamping=0 implies a "perfect" control of the resultant force, wallDamping=1 means no movement
// 		Real			wallDamping;
		//! maximum displacement/cycle (usefull to prevent explosions when stiffness is very low...) 

// 		Real			maxMultiplier;
// 		Real			finalMaxMultiplier;
		//! switch between "external" (walls) and "internal" (growth of particles) compaction 
// 		bool internalCompaction; 
		
		
// 		int &wall_bottom_id, &wall_top_id, &wall_left_id, &wall_right_id, &wall_front_id, &wall_back_id;
// 		bool wall_bottom_activated, wall_top_activated, wall_left_activated, wall_right_activated, wall_front_activated, wall_back_activated;
// 		Real height, width, depth, height0, width0, depth0;
// 		Real thickness;
// 		Real sigma_iso;
		//! The three following parameters allow to perform an external stress control with different stress values for the three space directions.
// 		Real sigma1;
// 		Real sigma2;
// 		Real sigma3;
		//!"if (isAxisymetric)" (true by default) sigma_iso is attributed to sigma1, 2 and 3
// 		bool isAxisymetric;
// 		Real max_vel;
		//! The three following parameters allow to perform an external stress control with different stress values for the three space directions.
		Real max_vel1;
		Real max_vel2;
		Real max_vel3;
		Real position_top;
		Real position_bottom;
		Real position_right;
		Real position_left;
		Real position_front;
		Real position_back;

// 		TriaxialStressController();
		virtual ~TriaxialStressController();
	
		virtual void action();
		//! Regulate the stress applied on walls with flag wall_XXX_activated = true
		void controlExternalStress(int wall, Vector3r resultantForce, State* p, Real wall_max_vel);
		//! Regulate the mean stress by changing spheres size, WARNING : this function assumes that all dynamic bodies in the problem are spheres
		void controlInternalStress(Real multiplier);
		void updateStiffness();
		void computeStressStrain(); //Compute stresses on walls as "Vector3r stress[6]", compute meanStress, strain[3] and mean strain
		//! Compute the mean/max unbalanced force in the assembly (normalized by mean contact force)
    		Real ComputeUnbalancedForce(bool maxUnbalanced=false);
		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(
		TriaxialStressController,GlobalEngine,"An engine maintaining constant stresses on some boundaries of a parallepipedic packing."
		,
   		((unsigned int,stiffnessUpdateInterval,10,"target strain rate (./s)"))
   		((unsigned int,radiusControlInterval,10,""))
		((unsigned int,computeStressStrainInterval,10,""))
		((Real,wallDamping,0.25,"wallDamping coefficient - wallDamping=0 implies a (theoretical) perfect control, wallDamping=1 means no movement"))
		((Real,thickness,-1,""))
// 		((vector<int>,wall_id,vector<int>(6,0),"Real index values of walls in the scene."))
		((int,wall_bottom_id,0,"id of boundary ; coordinate 1-"))
		((int,wall_top_id,0,"id of boundary ; coordinate 1+"))
		((int,wall_left_id,0,"id of boundary ; coordinate 0-"))
		((int,wall_right_id,0,"id of boundary ; coordinate 0+"))
		((int,wall_front_id,0,"id of boundary ; coordinate 2+"))
		((int,wall_back_id,0,"id of boundary ; coordinate 2-"))
		((bool,wall_bottom_activated,true,""))
		((bool,wall_top_activated,true,""))
		((bool,wall_left_activated,true,""))
		((bool,wall_right_activated,true,""))
		((bool,wall_front_activated,true,""))
		((bool,wall_back_activated,true,""))		
		((Real,height,0,""))
		((Real,width,0,""))
		((Real,depth,0,""))
		((Real,height0,0,""))
		((Real,width0,0,""))
		((Real,depth0,0,""))		
		((Real,sigma_iso,0,"applied confining stress (see :yref:'TriaxialStressController::isAxisymetric')"))
		((Real,sigma1,0,"applied stress on axis 1 (see :yref:'TriaxialStressController::isAxisymetric') |ycomp|"))
		((Real,sigma2,0,"applied stress on axis 2 (see :yref:'TriaxialStressController::isAxisymetric') |ycomp|"))
		((Real,sigma3,0,"applied stress on axis 3 (see :yref:'TriaxialStressController::isAxisymetric') |ycomp|"))
		((bool,isAxisymetric,true,"if true, sigma_iso is assigned to sigma1, 2 and 3"))
		((Real,maxMultiplier,1.001,""))
		((Real,finalMaxMultiplier,1.00001,""))
		((Real,max_vel,0.001,"max walls velocity [m/s]"))
		((Real,previousStress,0,""))
		((Real,previousMultiplier,1,""))
		((bool,internalCompaction,true,"Switch between 'external' (walls) and 'internal' (growth of particles) compaction."))
		((Real,meanStress,0,""))
		((Real,volumetricStrain,0,""))
 		,
//    		/* extra initializers */
//    		//((wall_id,vector<int>(6,0)))
//    		((wall_bottom_id,wall_id[0]))
// 		((wall_top_id,wall_id[1]))
// 		((wall_left_id,wall_id[2]))
// 		((wall_right_id,wall_id[3]))
// 		((wall_front_id,wall_id[4]))
// 		((wall_back_id,wall_id[5]))
		,
   		/* constructor */
   		first = true;
		stiffness.resize(6);
		for (int i=0; i<6; ++i){
// 			wall_id[i] = 0;
			previousTranslation[i] = Vector3r::ZERO;
			stiffness[i] = 0;
			normal[i] = Vector3r::ZERO;}
		for (int i=0; i<3; ++i) strain[i] = 0;
		normal[wall_bottom].Y()=1;
		normal[wall_top].Y()=-1;
		normal[wall_left].X()=1;
		normal[wall_right].X()=-1;
		normal[wall_front].Z()=-1;
		normal[wall_back].Z()=1;	
		porosity=1;
		,
 		.def_readonly("porosity",&TriaxialStressController::porosity,"")
		.def_readonly("boxVolume",&TriaxialStressController::boxVolume,"")
		.def_readonly("max_vel1",&TriaxialStressController::max_vel1,"|ycomp|")
		.def_readonly("max_vel2",&TriaxialStressController::max_vel2,"|ycomp|")
		.def_readonly("max_vel3",&TriaxialStressController::max_vel3,"|ycomp|")
		//.def("setContactProperties",&TriaxialCompressionEngine::setContactProperties,"Assign a new friction angle (degrees) to dynamic bodies and relative interactions")
		 )
		DECLARE_LOGGER;	
};

REGISTER_SERIALIZABLE(TriaxialStressController);


