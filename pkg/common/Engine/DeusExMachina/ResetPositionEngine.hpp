/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef RESETPOSITONENGINE_HPP
#define RESETPOSITONENGINE_HPP

#include<yade/core/DeusExMachina.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

/* Resets spatial position for all subscribed bodies to the desired value. */
class ResetPositionEngine : public DeusExMachina {
	public:
		void applyCondition(MetaBody*);
		bool isActivated()
				{return ((Omega::instance().getCurrentIteration() % interval == 0));}
		ResetPositionEngine();

		int interval;
		Real Y_min;
		/// import/export initial positions
		std::string fileName; 
		std::vector<Vector3r> initial_positions; // for serialization
		bool onlyDynamic;
		
		DECLARE_LOGGER;
	protected:
		virtual void postProcessAttributes(bool);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(ResetPositionEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
	private:
		std::vector<Vector3r> ini_pos;
		std::vector<long int> subscrBodies;
		bool first;
		void initialize(MetaBody * ncb);
};

REGISTER_SERIALIZABLE(ResetPositionEngine,false);

#endif //  RESETPOSITONENGINE_HPP

