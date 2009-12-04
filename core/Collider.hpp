/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/core/Bound.hpp>
#include "StandAloneEngine.hpp"

class Collider : public StandAloneEngine
{
	public :
		Collider();
		virtual ~Collider();
		/*! To probe the Bound on a bodies presense.
		 *
		 * returns list of body ids with which there is potential overlap.
		 */
		virtual  vector<body_id_t> probeBoundingVolume(const Bound&){throw;}
		/*! Tell whether given bodies may interact, for other than spatial reasons.
		 *
		 * Concrete collider implementations should call this function if
		 * the bodies are in potential interaction geometrically.
		 */
		bool mayCollide(const Body*, const Body*);
		/*! Handle various state transitions of Interaction (isReal, isNew).
		 *
		 * Returns whether the interaction should be preserved (true) or deleted (false).
		 */
		bool handleExistingInteraction(Interaction*);

	protected:

	REGISTER_CLASS_NAME(Collider);	
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};

REGISTER_SERIALIZABLE(Collider);

