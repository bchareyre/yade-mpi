/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*									 *
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMPLEVISCOELASTIC_CONTACT_LAW_HPP
#define SIMPLEVISCOELASTIC_CONTACT_LAW_HPP

#include<yade/core/InteractionSolver.hpp>

class PhysicalAction;

/// This class provides linear viscoelastic contact model
class SimpleViscoelasticContactLaw : public InteractionSolver
{
/// Attributes
	private :
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;
		int actionForceIndex;
		int actionMomentumIndex;
		
	public :
//		int sdecGroupMask;
		bool momentRotationLaw;
	
		SimpleViscoelasticContactLaw();
		void action(MetaBody*);

	protected :
		void registerAttributes();
	NEEDS_BEX("Force","Momentum");
	REGISTER_CLASS_NAME(SimpleViscoelasticContactLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(SimpleViscoelasticContactLaw);

#endif // SIMPLEVISCOELASTIC_CONTACT_LAW_HPP

