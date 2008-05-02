/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FRICTIONLESSELASTICCONTACTLAW_HPP
#define FRICTIONLESSELASTICCONTACTLAW_HPP

#include<yade/core/InteractionSolver.hpp>

class PhysicalAction;

class FrictionLessElasticContactLaw : public InteractionSolver
{
	private :
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;

	public :
		FrictionLessElasticContactLaw ();
		void action(MetaBody*);

	protected :
		 void registerAttributes();
	NEEDS_BEX("Force","Momentum");
	REGISTER_CLASS_NAME(FrictionLessElasticContactLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);

};

REGISTER_SERIALIZABLE(FrictionLessElasticContactLaw,false);

#endif // FRICTIONLESSELASTICCONTACTLAW_HPP

