/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ELASTIC_CONTACT_LAW_HPP
#define ELASTIC_CONTACT_LAW_HPP

#include<yade/core/InteractionSolver.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>

class PhysicalAction;

class ElasticContactLaw : public InteractionSolver
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
	
		ElasticContactLaw();
		void action(MetaBody*);

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(ElasticContactLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(ElasticContactLaw,false);

#endif // ELASTIC_CONTACT_LAW_HPP

