/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/core/Bound.hpp>
#include "GlobalEngine.hpp"

class Collider : public GlobalEngine
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

		/*! Invalidate all persistent data (if the collider has any), forcing reinitialization at next run.
		The default implementation does nothing, colliders should override it if it is applicable.

		Currently used from Shop::flipCell, which changes cell information for bodies.
		*/
		virtual void invalidatePersistentData(){}

	protected:

	REGISTER_CLASS_NAME(Collider);	
	REGISTER_BASE_CLASS_NAME(GlobalEngine);
};

REGISTER_SERIALIZABLE(Collider);

