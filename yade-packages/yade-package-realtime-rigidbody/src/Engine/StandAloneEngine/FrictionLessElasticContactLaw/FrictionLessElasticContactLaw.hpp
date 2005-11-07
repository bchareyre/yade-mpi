/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FRICTIONLESSELASTICCONTACTLAW_HPP
#define FRICTIONLESSELASTICCONTACTLAW_HPP

#include <yade/yade-core/Engine.hpp>

class PhysicalAction;

class FrictionLessElasticContactLaw : public Engine
{
	private :
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;

	public :
		FrictionLessElasticContactLaw ();
		void action(Body* body);

	protected :
		 void registerAttributes();
	REGISTER_CLASS_NAME(FrictionLessElasticContactLaw);
	REGISTER_BASE_CLASS_NAME(Engine);

};

REGISTER_SERIALIZABLE(FrictionLessElasticContactLaw,false);

#endif // FRICTIONLESSELASTICCONTACTLAW_HPP

