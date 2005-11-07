/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FORCE_ENGINE_HPP
#define FORCE_ENGINE_HPP

#include <yade/yade-core/DeusExMachina.hpp>

class Force;

class ForceEngine : public DeusExMachina 
{
	private :
		shared_ptr<Force>	actionParameterForce;
	public :
		Vector3r		force;

		ForceEngine();
		virtual ~ForceEngine();
	
		virtual void applyCondition(Body*);
	
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(ForceEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(ForceEngine,false);

#endif // FORCE_ENGINE_HPP

