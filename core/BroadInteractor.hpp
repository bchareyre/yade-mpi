/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/core/BoundingVolume.hpp>
#include "StandAloneEngine.hpp"

class BroadInteractor : public StandAloneEngine
{
	public :
		BroadInteractor();
		virtual ~BroadInteractor();
		virtual  bool probeBoundingVolume(const BoundingVolume&){throw;}
		vector<body_id_t> probedBodies;

	protected:

	REGISTER_CLASS_NAME(BroadInteractor);	
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};

REGISTER_SERIALIZABLE(BroadInteractor);

