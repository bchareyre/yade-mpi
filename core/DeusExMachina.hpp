/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef KINEMATICENGINE_HPP
#define KINEMATICENGINE_HPP

#include <vector>

#include<yade/core/Engine.hpp>
#include<yade/core/Body.hpp>

/*! \brief Abstract interface for all dii ex machina.

	All kinematic engines must derived from this class. A kinematic engine is used to modify the state of an object
	(position,veloity ...) according to a predefined law (mathematical function, stored data ...) and not according
	to a dynamic law as dynamic engines do. A kinematic engine contains a list of bodies to act on, and bodies can
	subscribe to several dii ex machina.
*/

class DeusExMachina : public Engine
{
	public :
		std::vector<int> subscribedBodies; /// Lists of bodies whose state will be modified by deux ex machina
		DeusExMachina ();
		virtual void action(MetaBody*);
		virtual void applyCondition(MetaBody*) { throw; };
	//	virtual bool isActivated();
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(DeusExMachina);
	REGISTER_BASE_CLASS_NAME(Engine);
};

REGISTER_SERIALIZABLE(DeusExMachina,false);

#endif // __KINEMATICENGINE_H__

