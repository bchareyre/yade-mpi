/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractionSolver.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>

class PhysicalAction;


class GlobalStiffnessCounter : public InteractionSolver
{
/// Attributes
	private :
		int actionForceIndex;
		int actionMomentumIndex;
		int actionStiffnessIndex;

		bool getInteractionParameters(const shared_ptr<Interaction>& contact, Vector3r& normal, Real& kn, Real& ks, Real& radius1, Real& radius2);
		bool getSphericalElasticInteractionParameters(const shared_ptr<Interaction>& contact, Vector3r& normal, Real& kn, Real& ks, Real& radius1, Real& radius2);
		void traverseInteractions(MetaBody* ncb, const shared_ptr<InteractionContainer>& interactions);

	public :
		unsigned int interval;
		//int sdecGroupMask;
	
		GlobalStiffnessCounter();
		void action(MetaBody*);
		virtual bool isActivated();

	protected :
		void registerAttributes();
	NEEDS_BEX("Force","Momentum","GlobalStiffness");
	REGISTER_CLASS_NAME(GlobalStiffnessCounter);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(GlobalStiffnessCounter);




