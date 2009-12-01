/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/DeusExMachina.hpp>
#include<yade/lib-base/yadeWm3.hpp>

class TranslationEngine : public DeusExMachina {
	public:
		Real velocity;
		Vector3r translationAxis;
		virtual void applyCondition(World *);
		virtual void postProcessAttributes(bool deserializing){ if(deserializing) translationAxis.Normalize(); }
	REGISTER_ATTRIBUTES(DeusExMachina,(velocity)(translationAxis));
	REGISTER_CLASS_AND_BASE(TranslationEngine,DeusExMachina);
};
REGISTER_SERIALIZABLE(TranslationEngine);


