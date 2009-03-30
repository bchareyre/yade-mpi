/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/DeusExMachina.hpp>


class MomentEngine : public DeusExMachina 
{
	public :
		Vector3r		moment;

		MomentEngine();
		virtual ~MomentEngine();
	
		virtual void applyCondition(MetaBody*);
	
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(MomentEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(MomentEngine);


