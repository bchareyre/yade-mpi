/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include <yade/yade-core/DeusExMachina.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>

class TranslationEngine : public DeusExMachina
{
	public :
		Real velocity;
		Vector3r translationAxis;
		void applyCondition(Body * body);

	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(TranslationEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(TranslationEngine,false);

#endif // TRANSLATOR_HPP

