/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef COLORIZED_VELOCITY_FILTER_HPP
#define COLORIZED_VELOCITY_FILTER_HPP 

#include<yade/core/FiltrEngine.hpp>
#include<yade/core/MetaBody.hpp>

class ColorizedVelocityFilter : public FiltrEngine {
	protected:
		Real midValue;
		vector<Vector3r> prevPositions;
		long int prevIteration;
		Real dt;
		vector<Real> values;
		vector<long int> subscrBodies;
		bool first;
		void initialize(MetaBody*);
		void makeScale();
		Vector3r getColor4Value(Real v);
	public :
		bool autoScale;
		bool onlyDynamic;
		Real minValue;
		Real maxValue;

		ColorizedVelocityFilter();
		virtual ~ColorizedVelocityFilter();
	
		virtual bool isActivated();
		virtual void applyCondition(MetaBody*);
	
		virtual void registerAttributes();
		//virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(ColorizedVelocityFilter);
	REGISTER_BASE_CLASS_NAME(FiltrEngine);
};

REGISTER_SERIALIZABLE(ColorizedVelocityFilter,false);

#endif // COLORIZED_VELOCITY_FILTER_HPP 

