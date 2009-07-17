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

class ColorizedLayerFilter : public FilterEngine {
	private:
		Vector3r far;
	public :
		Vector3r near;
		Vector3r normal;
		Real thickness;
		Vector3r diffuseColor;
		int interval;
		ColorizedLayerFilter();
		virtual ~ColorizedLayerFilter();
	
		virtual bool isActivated(MetaBody*);
		virtual void applyCondition(MetaBody*);
	
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(FilterEngine,(near)(normal)(thickness)(diffuseColor)(interval));
	REGISTER_CLASS_NAME(ColorizedLayerFilter);
	REGISTER_BASE_CLASS_NAME(FilterEngine);
};

REGISTER_SERIALIZABLE(ColorizedLayerFilter);


