// Copyright (C) 2004 by Olivier Galizzi <olivier.galizzi@imag.fr>
// Copyright (C) 2004 by Janek Kozicki <cosurgi@berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>

class PhysicalParameters : public Serializable, public Indexable
{
	public:
			PhysicalParameters(): se3(Vector3r(0,0,0),Quaternionr(1,0,0,0)), refSe3(Vector3r(0,0,0),Quaternionr(1,0,0,0)), isDisplayed(true) {}
		Se3r se3; //! Se3 of the body in simulation space

		Se3r refSe3; //! Reference Se3 of body in display space
		// the following two are recomputed at every renderering cycle, no need to save/load them via registerAttributes
		Se3r dispSe3; //! Se3 of body in display space (!=se3 if scaling positions/orientations)
		bool isDisplayed; //! False if the body is not displayed in this cycle (clipped, for instance)
	protected :
		void registerAttributes(){
			REGISTER_ATTRIBUTE(se3);
			REGISTER_ATTRIBUTE(refSe3);
		}
	REGISTER_CLASS_NAME(PhysicalParameters);
	REGISTER_BASE_CLASS_NAME(Serializable Indexable);
	REGISTER_INDEX_COUNTER(PhysicalParameters);
};

REGISTER_SERIALIZABLE(PhysicalParameters,false);
