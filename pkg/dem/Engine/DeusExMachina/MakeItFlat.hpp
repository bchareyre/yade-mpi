/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/DeusExMachina.hpp>

class Force;

class MakeItFlat : public DeusExMachina 
{
	private	:
		shared_ptr<Force> actionParameterForce;

	public :
		MakeItFlat();
		virtual ~MakeItFlat();
	
		virtual void applyCondition(MetaBody*);

		int direction;
		Real plane_position;
		bool reset_force;
	
	protected :
		virtual void registerAttributes();
	NEEDS_BEX("Force");
	REGISTER_CLASS_NAME(MakeItFlat);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(MakeItFlat);


