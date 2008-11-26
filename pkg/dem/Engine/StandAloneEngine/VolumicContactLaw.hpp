/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef VOLUMIC_CONTACT_LAW_HPP
#define VOLUMIC_CONTACT_LAW_HPP

#include<yade/core/InteractionSolver.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>

class PhysicalAction;

class VolumicContactLaw : public InteractionSolver
{
/// Attributes
	private :
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;
		int actionForceIndex;
		int actionMomentumIndex;
		
	public :
		int sdecGroupMask;
		bool momentRotationLaw;
		vector<double> compacite_init;
		Real rigidite_max_enreg;
		Real max_force_enreg;
		Real compacity_init;
		Real compacity;
		Real compacity_max;
		Real interpenetration_max;
		Real delta_interpenetration_max;
		Real rigidite_max_1;
		Real rigidite_max_2;
		Real rigidite_max_3;
		Real info;


	
		VolumicContactLaw();
		void action(MetaBody*);
		void speedTest(MetaBody* ncb);

	protected :
		void registerAttributes();
	NEEDS_BEX("Force","Momentum");
	REGISTER_CLASS_NAME(VolumicContactLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(VolumicContactLaw);

#endif // VOLUMIC_CONTACT_LAW_HPP

