/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/lib-base/yadeWm3.hpp>

class TranslationEngineEigen : public PartialEngine {
	public:
		Real velocity;
		Vector3r translationAxis;
		virtual void applyCondition(Scene *);
		virtual void postProcessAttributes(bool deserializing){ if(deserializing) translationAxis.Normalize(); }
	REGISTER_ATTRIBUTES(PartialEngine,(velocity)(translationAxis));
	REGISTER_CLASS_AND_BASE(TranslationEngineEigen,PartialEngine);
};
REGISTER_SERIALIZABLE(TranslationEngineEigen);

YADE_REQUIRE_FEATURE(eigen)

