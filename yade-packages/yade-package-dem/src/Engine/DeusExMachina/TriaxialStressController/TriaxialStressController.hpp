/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TRIAXIAL_STRESS_CONTROLLER_HPP
#define TRIAXIAL_STRESS_CONTROLLER_HPP

#include <yade/yade-core/DeusExMachina.hpp>
#include <Wm3Math.h>
#include <yade/yade-lib-base/yadeWm3.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>

class PhysicalAction;
class MetaBody;
class PhysicalParameters;


/*! \brief Controls the stress on the boundaries of a box

	detailed description...
*/

class TriaxialStressController : public DeusExMachina 
{
	private :
		shared_ptr<PhysicalAction> actionParameterStiffnessMatrix;
		shared_ptr<PhysicalAction> actionParameterForce;
		int StiffnessMatrixClassIndex, ForceClassIndex;
		
		
			
	public :
		unsigned int interval;
		//! index values for retrieving walls
		int wall_bottom, wall_top, wall_left, wall_right, wall_front, wall_back;
		//! Defines the prescibed resultant force 
		Vector3r		force;	
		//! Stores the value of the translation at the previous time step, stiffness, and normal
		Vector3r	previoustranslation [6];
		//! The value of stiffness (updated according to interval) 
		Real		stiffness [6];
		Vector3r	normal [6];
		Vector3r	stress [6];
		int 		wall_id [6];
				
		//! damping coefficient - damping=0 implies a "perfect" control of the resultant force, damping=1 means no movement
		Real			damping;
		//! maximum displacement/cycle (usefull to prevent explosions when stiffness is very low...) 
		Real			max_vel;
		
		int &wall_bottom_id, &wall_top_id, &wall_left_id, &wall_right_id, &wall_front_id, &wall_back_id;
		bool wall_bottom_activated, wall_top_activated, wall_left_activated, wall_right_activated, wall_front_activated, wall_back_activated;
		Real height, width, depth;
		Real thickness; // FIXME should retrieve "extents" of a InteractingBox
		Real sigma_iso;
		

		TriaxialStressController();
		virtual ~TriaxialStressController();
	
		virtual void applyCondition(Body*);
		void controlStress(int wall, MetaBody* ncb, int id, Vector3r resultantForce, PhysicalParameters* p, Real wall_max_vel);
		void updateStiffness(MetaBody* ncb);
		
	
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(TriaxialStressController);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(TriaxialStressController,false);

#endif // TRIAXIAL_STRESS_CONTROLLER_HPP

