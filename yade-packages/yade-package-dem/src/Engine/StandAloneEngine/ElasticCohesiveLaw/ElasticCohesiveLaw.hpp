/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ELASTICCOHESIVELAW_HPP
#define ELASTICCOHESIVELAW_HPP

#include <yade/yade-core/InteractionSolver.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>

class PhysicalAction;

class ElasticCohesiveLaw : public InteractionSolver
{
	private :
		bool first; // FIXME - remove that!
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;

	public :
		int sdecGroupMask;
		bool momentRotationLaw;
		
		ElasticCohesiveLaw();
		void action(Body* body);

	protected : 
		void registerAttributes();
	REGISTER_CLASS_NAME(ElasticCohesiveLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);

};

REGISTER_SERIALIZABLE(ElasticCohesiveLaw,false);

#endif // ELASTICCOHESIVELAW_HPP

