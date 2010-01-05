/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/core/Scene.hpp>
#include<yade/lib-base/yadeWm3.hpp>

class Scene;
class State;


/*! \brief Controls the stress on the boundaries of a box and compute strain-like and stress-like quantities for the packing

	detailed description...
*/

class TriaxialStressController : public PartialEngine 
{
	private :
		Real previousStress, previousMultiplier; //previous mean stress and size multiplier		
		bool firstRun;
		inline const Vector3r getForce(Scene* rb, body_id_t id){ return rb->forces.getForce(id); /* needs sync, which is done at the beginning of applyCondition */ }
		
		 	
	public :
		unsigned int stiffnessUpdateInterval, computeStressStrainInterval, radiusControlInterval;
		//! internal index values for retrieving walls
		enum { wall_bottom=0, wall_top, wall_left, wall_right, wall_front, wall_back };
		//! real index values of walls in the Scene
		int wall_id [6];
		//! Defines the prescibed resultant force 
		//Vector3r		force;	
		//! Stores the value of the translation at the previous time step, stiffness, and normal
		Vector3r	previousTranslation [6];
		//! The value of stiffness (updated according to stiffnessUpdateInterval) 
		vector<Real>	stiffness;
		Real 		strain [3];
		Real volumetricStrain;
		Vector3r	normal [6];
		//! The values of stresses 
		Vector3r	stress [6];
		Vector3r	force [6];
		Real		meanStress;
		//! Value of spheres volume (solid volume)
		Real spheresVolume;
		//! Value of box volume 
		Real boxVolume;
		//! Sample porosity
		Real porosity;
				
		
		//Real UnbalancedForce;		
				
		//! wallDamping coefficient - wallDamping=0 implies a "perfect" control of the resultant force, wallDamping=1 means no movement
		Real			wallDamping;
		//! maximum displacement/cycle (usefull to prevent explosions when stiffness is very low...) 

		Real			maxMultiplier;
		Real			finalMaxMultiplier;
		//! switch between "external" (walls) and "internal" (growth of particles) compaction 
		bool internalCompaction; 
		
		
		int &wall_bottom_id, &wall_top_id, &wall_left_id, &wall_right_id, &wall_front_id, &wall_back_id;
		bool wall_bottom_activated, wall_top_activated, wall_left_activated, wall_right_activated, wall_front_activated, wall_back_activated;
		Real height, width, depth, height0, width0, depth0;
		Real thickness;
		Real sigma_iso;
		//! The three following parameters allow to perform an external stress control with different stress values for the three space directions.
		Real sigma1;
		Real sigma2;
		Real sigma3;
		//!"if (isAxisymetric)" (true by default) sigma_iso is attributed to sigma1, 2 and 3
		bool isAxisymetric;
		Real max_vel;
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

		TriaxialStressController();
		virtual ~TriaxialStressController();
	
		virtual void applyCondition(Scene*);
		//! Regulate the stress applied on walls with flag wall_XXX_activated = true
		void controlExternalStress(int wall, Scene* ncb, Vector3r resultantForce, State* p, Real wall_max_vel);
		void controlInternalStress(Scene* ncb, Real multiplier);
		void updateStiffness(Scene* ncb);
		void computeStressStrain(Scene* ncb); //Compute stresses on walls as "Vector3r stress[6]", compute meanStress, strain[3] and mean strain
		//! Compute the mean/max unbalanced force in the assembly (normalized by mean contact force)
    		Real ComputeUnbalancedForce(Scene * ncb, bool maxUnbalanced=false);

		DECLARE_LOGGER;
		
	REGISTER_ATTRIBUTES(PartialEngine,
		(stiffnessUpdateInterval)
		(radiusControlInterval)
		(computeStressStrainInterval)
		(wallDamping)
		//	(force)
		
		//(UnbalancedForce)
		(stiffness)
		(wall_bottom_id)
		(wall_top_id)
		(wall_left_id)
		(wall_right_id)
		(wall_front_id)
		(wall_back_id)
		//	(wall_id)
		(wall_bottom_activated)
		(wall_top_activated)
		(wall_left_activated)
		(wall_right_activated)
		(wall_front_activated)
		(wall_back_activated)
		
		(thickness)
		(height)
		(width)
		(depth)
		(height0)
		(width0)
		(depth0)
		
		(sigma_iso)
		(sigma1)
		(sigma2)
		(sigma3)
		(isAxisymetric)
		(maxMultiplier)
		(finalMaxMultiplier)
		(max_vel)
		(max_vel1)
		(max_vel2)
		(max_vel3)
		(previousStress)
		(previousMultiplier)
		(internalCompaction)

		// needed for access from python
		(meanStress)
		(volumetricStrain)
	);
	REGISTER_CLASS_NAME(TriaxialStressController);
	REGISTER_BASE_CLASS_NAME(PartialEngine);
};

REGISTER_SERIALIZABLE(TriaxialStressController);


