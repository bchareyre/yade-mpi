/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PartialEngine.hpp>


class MomentEngine : public PartialEngine 
{
	public :
		Vector3r		moment;

		MomentEngine();
		virtual ~MomentEngine();
	
		virtual void applyCondition(Scene*);
	REGISTER_ATTRIBUTES(PartialEngine,(moment));
	REGISTER_CLASS_NAME(MomentEngine);
	REGISTER_BASE_CLASS_NAME(PartialEngine);
};

REGISTER_SERIALIZABLE(MomentEngine);


