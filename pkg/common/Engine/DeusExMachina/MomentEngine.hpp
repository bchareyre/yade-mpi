/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef MOMENT_ENGINE_HPP
#define MOMENT_ENGINE_HPP

#include<yade/core/DeusExMachina.hpp>

class Momentum;

class MomentEngine : public DeusExMachina 
{
	private :
		shared_ptr<Momentum>	actionParameterMoment;
	public :
		Vector3r		moment;

		MomentEngine();
		virtual ~MomentEngine();
	
		virtual void applyCondition(Body*);
	
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(MomentEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(MomentEngine,false);

#endif // FORCE_ENGINE_HPP

