/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef COLORIZED_LAYER_FILTER_HPP
#define COLORIZED_LAYER_FILTER_HPP 

#include<yade/pkg-common/FiltrEngine.hpp>
#include<yade/core/MetaBody.hpp>

class ColorizedLayerFilter : public FiltrEngine {
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
	
		virtual bool isActivated();
		virtual void applyCondition(MetaBody*);
	
		virtual void registerAttributes();
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(ColorizedLayerFilter);
	REGISTER_BASE_CLASS_NAME(FiltrEngine);
};

REGISTER_SERIALIZABLE(ColorizedLayerFilter,false);

#endif // COLORIZED_LAYER_FILTER_HPP 

