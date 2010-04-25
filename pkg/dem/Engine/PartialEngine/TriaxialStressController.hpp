/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/BoundaryController.hpp>
#include<yade/core/Scene.hpp>
#include<yade/lib-base/Math.hpp>

#include<boost/array.hpp>

class Scene;
class State;


/*! \brief Controls the stress on the boundaries of a box and compute strain-like and stress-like quantities for the packing
*/

class TriaxialStressController : public BoundaryController
{
	private :
		bool first;
		inline const Vector3r getForce(Scene* rb, body_id_t id){ return rb->forces.getForce(id); /* needs sync, which is done at the beginning of action */ }
	public :
		//! internal index values for retrieving walls
		enum { wall_bottom=0, wall_top, wall_left, wall_right, wall_front, wall_back };
		//! real index values of walls in the Scene
		int wall_id [6];
		//! Stores the value of the translation at the previous time step, stiffness, and normal
		boost::array<Vector3r,6> previousTranslation;
		//! The value of stiffness (updated according to stiffnessUpdateInterval) 
		vector<Real>	stiffness;
		Real 		strain [3];
		Vector3r	normal [6];
		//! The values of stresses 
		Vector3r	stress [6];
		Vector3r	force [6];
		//! Value of spheres volume (solid volume)
		Real spheresVolume;
		//! Value of box volume 
		Real boxVolume;
		//! Sample porosity
		Real porosity;
		
		Real max_vel1;
		Real max_vel2;
		Real max_vel3;
		Real position_top;
		Real position_bottom;
		Real position_right;
		Real position_left;
		Real position_front;
		Real position_back;

		virtual ~TriaxialStressController();
	
		virtual void action();
		//! Regulate the stress applied on walls with flag wall_XXX_activated = true
		void controlExternalStress(int wall, Vector3r resultantForce, State* p, Real wall_max_vel);
		//! Regulate the mean stress by changing spheres size, WARNING : this function assumes that all dynamic bodies in the problem are spheres
		void controlInternalStress(Real multiplier);
		//! update the stiffness of boundary-packing interaction (sum of contacts stiffness on the boundary)
		void updateStiffness();
		//! Compute stresses on walls as "Vector3r stress[6]", compute meanStress, strain[3] and mean strain
		void computeStressStrain();
		//! Compute the mean/max unbalanced force in the assembly (normalized by mean contact force)
    		Real ComputeUnbalancedForce(bool maxUnbalanced=false);
		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(
		TriaxialStressController,BoundaryController,"An engine maintaining constant stresses on some boundaries of a parallepipedic packing."
		,
   		((unsigned int,stiffnessUpdateInterval,10,"target strain rate (./s)"))
   		((unsigned int,radiusControlInterval,10,""))
		((unsigned int,computeStressStrainInterval,10,""))
		((Real,wallDamping,0.25,"wallDamping coefficient - wallDamping=0 implies a (theoretical) perfect control, wallDamping=1 means no movement"))
		((Real,thickness,-1,""))
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
		((Real,sigma_iso,0,"applied confining stress (see :yref:`TriaxialStressController::isAxisymetric`)"))
		((Real,sigma1,0,"applied stress on axis 1 (see :yref:`TriaxialStressController::isAxisymetric`)"))
		((Real,sigma2,0,"applied stress on axis 2 (see :yref:`TriaxialStressController::isAxisymetric`)"))
		((Real,sigma3,0,"applied stress on axis 3 (see :yref:`TriaxialStressController::isAxisymetric`)"))
		((bool,isAxisymetric,true,"if true, sigma_iso is assigned to sigma1, 2 and 3"))
		((Real,maxMultiplier,1.001,"max multiplier of diameters during internal compaction (initial fast increase - :yref:`TriaxialStressController::finalMaxMultiplier` is used in a second stage)"))
		((Real,finalMaxMultiplier,1.00001,"max multiplier of diameters during internal compaction (secondary precise adjustment - :yref:`TriaxialStressController::maxMultiplier` is used in the initial stage)"))	
		((Real,max_vel,0.001,"Maximum allowed walls velocity [m/s]. This value superseeds the one assigned by the stress controller if the later is higher. max_vel can be set to infinity in many cases, but sometimes helps stabilizing packings. Based on this value, different maxima are computed for each axis based on the dimensions of the sample, so that if each boundary moves at its maximum velocity, the strain rate will be isotropic (see e.g. :yref:`TriaxialStressController::max_vel1`)."))
		((Real,previousStress,0,""))
		((Real,previousMultiplier,1,""))
		((bool,internalCompaction,true,"Switch between 'external' (walls) and 'internal' (growth of particles) compaction."))
		((Real,meanStress,0,"Mean stress in the packing."))
		((Real,volumetricStrain,0,"Volumetric strain (see :yref:`TriaxialStressController::strain`)."))
 		,
		/* extra initializers */
		,
   		/* constructor */
   		first = true;
		stiffness.resize(6);
		previousTranslation.assign(Vector3r::Zero());
		for (int i=0; i<6; ++i){normal[i]=stress[i]=force[i]=Vector3r::Zero();stiffness[i]=0;}
		normal[wall_bottom].Y()=1;
		normal[wall_top].Y()=-1;
		normal[wall_left].X()=1;
		normal[wall_right].X()=-1;
		normal[wall_front].Z()=-1;
		normal[wall_back].Z()=1;	
		porosity=1;
		,
		.def_readonly("strain",&TriaxialStressController::strain,"Current strain (logarithmic).")
 		.def_readonly("porosity",&TriaxialStressController::porosity,"Pososity of the packing.")
		.def_readonly("boxVolume",&TriaxialStressController::boxVolume,"Total packing volume.")
		.def_readonly("max_vel1",&TriaxialStressController::max_vel1,"see :yref:`TriaxialStressController::max_vel` |ycomp|")
		.def_readonly("max_vel2",&TriaxialStressController::max_vel2,"see :yref:`TriaxialStressController::max_vel` |ycomp|")
		.def_readonly("max_vel3",&TriaxialStressController::max_vel3,"see :yref:`TriaxialStressController::max_vel` |ycomp|")
		 )
		DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(TriaxialStressController);
