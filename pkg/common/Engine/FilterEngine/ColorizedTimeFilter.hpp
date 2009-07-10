/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/FilterEngine.hpp>
#include<yade/core/MetaBody.hpp>

class ColorScale;

// Assign a sphere's color with the time when it cross the plane.
class ColorizedTimeFilter : public FilterEngine {
	protected:
		Real prevSimulTime;
		vector<Real> values;
		vector<long int> subscrBodies;
		bool first;
		void initialize(MetaBody*);
		void makeScale();
		void updateColorScale();
		Vector3r getColor4Value(Real v);
		shared_ptr<Body> widget;
		shared_ptr<ColorScale> legend;
		Real cur_minValue;
		Real cur_maxValue;
	public :
		bool autoScale;
		bool onlyDynamic;
		Real minValue;
		Real maxValue;

		// plane 
		Vector3r point;
		Vector3r normal;

		// legend
		Real posX,posY;
		Real width,height;
		string title;

		ColorizedTimeFilter();
		virtual ~ColorizedTimeFilter();
	
		virtual bool isActivated(MetaBody*);
		virtual void applyCondition(MetaBody*);
	
		virtual void registerAttributes();
		DECLARE_LOGGER;
	REGISTER_CLASS_NAME(ColorizedTimeFilter);
	REGISTER_BASE_CLASS_NAME(FilterEngine);
};

REGISTER_SERIALIZABLE(ColorizedTimeFilter);


