/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/DeusExMachina.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/pkg-common/Force.hpp>


#define TR {if (Omega::instance().getCurrentIteration()%100==0) TRACE; }
class PhysicalAction;
class MetaBody;
class PhysicalParameters;


/*! \brief Controls the stress on the boundaries of a box and compute strain-like and stress-like quantities for the packing

	detailed description...
*/

class TriaxialStressController : public DeusExMachina 
{
	private :
		//shared_ptr<PhysicalAction> actionParameterForce;
		//int cachedForceClassIndex;
		int ForceClassIndex;
		Real previousStress, previousMultiplier; //previous mean stress and size multiplier		
		bool firstRun;
		inline const Vector3r getForce(MetaBody* rb, body_id_t id){
			#ifdef BEX_CONTAINER
				return rb->bex.getForce(id); // needs sync, which is done at the beginning of applyCondition
			#else
				return static_cast<Force*>(rb->physicalActions->find(id,ForceClassIndex).get())->force;
			#endif
		}
		
		 	
	public :
		unsigned int stiffnessUpdateInterval, computeStressStrainInterval, radiusControlInterval;
		//! internal index values for retrieving walls
		enum { wall_bottom=0, wall_top, wall_left, wall_right, wall_front, wall_back };
		//! real index values of walls in the MetaBody
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
		Real max_vel;
		Real position_top;
		Real position_bottom;
		Real position_right;
		Real position_left;
		Real position_front;
		Real position_back;

		TriaxialStressController();
		virtual ~TriaxialStressController();
	
		virtual void applyCondition(MetaBody*);
		//! Regulate the stress applied on walls with flag wall_XXX_activated = true
		void controlExternalStress(int wall, MetaBody* ncb, Vector3r resultantForce, PhysicalParameters* p, Real wall_max_vel);
		void controlInternalStress(MetaBody* ncb, Real multiplier);
		void updateStiffness(MetaBody* ncb);
		void computeStressStrain(MetaBody* ncb); //Compute stresses on walls as "Vector3r stress[6]", compute meanStress, strain[3] and mean strain
		//! Compute the mean/max unbalanced force in the assembly (normalized by mean contact force)
    	Real ComputeUnbalancedForce(MetaBody * ncb, bool maxUnbalanced=false);

		DECLARE_LOGGER;
		
	
	protected :
		virtual void registerAttributes();
	NEEDS_BEX("Force");
	REGISTER_CLASS_NAME(TriaxialStressController);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(TriaxialStressController);


