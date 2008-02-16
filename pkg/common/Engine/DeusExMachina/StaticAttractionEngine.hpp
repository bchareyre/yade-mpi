/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef STATICATTRACTIONENGINE_HPP
#define STATICATTRACTIONENGINE_HPP

#include<yade/core/DeusExMachina.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class StaticAttractionEngine : public DeusExMachina
{
	public :
		void applyCondition(Body * body);
		bool active;
		StaticAttractionEngine() : active(true) {};
		virtual bool isActivated() { return active; };

	protected :
		virtual Real getMaxDisplacement(MetaBody*) {throw "do not use StaticAttractionEngine directly, use derived classes instead.";};
		virtual bool doesItApplyToThisBody(Body*) {throw "do not use StaticAttractionEngine directly, use derived classes instead.";};

		virtual void registerAttributes();
	REGISTER_CLASS_NAME(StaticAttractionEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(StaticAttractionEngine,false);

#endif //  DISPLACEMENTENGINE_HPP

