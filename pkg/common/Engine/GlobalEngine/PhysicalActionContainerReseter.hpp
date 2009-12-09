/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GlobalEngine.hpp>

class Body;

class PhysicalActionContainerReseter : public GlobalEngine
{
	public :
		PhysicalActionContainerReseter();
		virtual ~PhysicalActionContainerReseter();
		virtual void action(Scene*);

	REGISTER_CLASS_AND_BASE(PhysicalActionContainerReseter,GlobalEngine);
};
REGISTER_SERIALIZABLE(PhysicalActionContainerReseter);

class BexResetter: public GlobalEngine{
	public:
		virtual void action(Scene*);
	REGISTER_CLASS_AND_BASE(BexResetter,GlobalEngine);
};
REGISTER_SERIALIZABLE(BexResetter);


