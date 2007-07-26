/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TRIAXIAL_STRESS_CONTROLLER_HPP
#define TRIAXIAL_STRESS_CONTROLLER_HPP

#include<yade/core/DeusExMachina.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>


class PhysicalAction;
class MetaBody;
class PhysicalParameters;


/*! \brief Controls the stress on the boundaries of a box

	detailed description...
*/

class TriaxialStressController : public DeusExMachina 
{
	private :
		shared_ptr<PhysicalAction> actionParameterForce;
		int ForceClassIndex;
		Real previousStress, previousMultiplier; //previous mean stress and size multiplier		
		
			
	public :
		unsigned int stiffnessUpdateInterval, computeStressStrainInterval, radiusControlInterval;
		//! internal index values for retrieving walls
		int wall_bottom, wall_top, wall_left, wall_right, wall_front, wall_back;
		//! Defines the prescibed resultant force 
		Vector3r		force;	
		//! Stores the value of the translation at the previous time step, stiffness, and normal
		Vector3r	previousTranslation [6];
		//! The value of stiffness (updated according to stiffnessUpdateInterval) 
		Real		stiffness [6];
		Real 		strain [3];
		Vector3r	normal [6];
		Vector3r	stress [6];
		int 		wall_id [6];
		Real		meanStress;
		
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
		Real thickness; // FIXME should retrieve "extents" of a InteractingBox
		Real sigma_iso;
		Real max_vel;

		TriaxialStressController();
		virtual ~TriaxialStressController();
	
		virtual void applyCondition(Body*);
		void controlExternalStress(int wall, MetaBody* ncb, int id, Vector3r resultantForce, PhysicalParameters* p, Real wall_max_vel);
		void controlInternalStress(MetaBody* ncb, Real multiplier);
		void updateStiffness(MetaBody* ncb);
		Real computeStressStrain(MetaBody* ncb); //Compute stresses on walls and store the values in "Vector3r stress[6]", return mean stress
		//! Compute the mean/max unbalanced force in the assembly (normalized by mean contact force)
    		Real ComputeUnbalancedForce(Body * body, bool maxUnbalanced=false);
		
	
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(TriaxialStressController);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(TriaxialStressController,false);

#endif // TRIAXIAL_STRESS_CONTROLLER_HPP

