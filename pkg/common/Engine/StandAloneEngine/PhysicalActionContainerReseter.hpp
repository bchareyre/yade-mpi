/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/StandAloneEngine.hpp>

class Body;

class PhysicalActionContainerReseter : public StandAloneEngine
{
	public :
		PhysicalActionContainerReseter();
		virtual ~PhysicalActionContainerReseter();
		virtual void action(World*);

	REGISTER_CLASS_AND_BASE(PhysicalActionContainerReseter,StandAloneEngine);
};
REGISTER_SERIALIZABLE(PhysicalActionContainerReseter);

class BexResetter: public StandAloneEngine{
	public:
		virtual void action(World*);
	REGISTER_CLASS_AND_BASE(BexResetter,StandAloneEngine);
};
REGISTER_SERIALIZABLE(BexResetter);


