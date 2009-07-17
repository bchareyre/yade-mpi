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

class ColorizedVelocityFilter : public FilterEngine {
	protected:
		vector<Vector3r> prevPositions;
		long int prevIteration;
		Real dt;
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

		Real posX,posY;
		Real width,height;
		string title;

		ColorizedVelocityFilter();
		virtual ~ColorizedVelocityFilter();
	
		virtual bool isActivated(MetaBody*);
		virtual void applyCondition(MetaBody*);
	
	DECLARE_LOGGER;
	REGISTER_ATTRIBUTES(FilterEngine,(autoScale)(onlyDynamic)(minValue)(maxValue)(posX)(posY)(width)(height)(title));
	REGISTER_CLASS_NAME(ColorizedVelocityFilter);
	REGISTER_BASE_CLASS_NAME(FilterEngine);
};

REGISTER_SERIALIZABLE(ColorizedVelocityFilter);


