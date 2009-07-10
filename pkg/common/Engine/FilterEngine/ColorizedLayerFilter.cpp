/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ColorizedLayerFilter.hpp"

ColorizedLayerFilter::ColorizedLayerFilter() : FilterEngine() 
{
	near = Vector3r(0,0,0);
	normal = Vector3r(0,1,0);
	thickness = 1;
	far = near + thickness*normal;
	diffuseColor = Vector3r(0.5,0.5,0.5);
	interval = 1;
}


ColorizedLayerFilter::~ColorizedLayerFilter()
{
}

bool ColorizedLayerFilter::isActivated(MetaBody* rootBody)
{
	return FilterEngine::isActivated(rootBody) 
		&& ((Omega::instance().getCurrentIteration() % interval == 0));
}

void ColorizedLayerFilter::registerAttributes()
{
	FilterEngine::registerAttributes();
	REGISTER_ATTRIBUTE(near);
	REGISTER_ATTRIBUTE(normal);
	REGISTER_ATTRIBUTE(thickness);
	REGISTER_ATTRIBUTE(diffuseColor);
	REGISTER_ATTRIBUTE(interval);
}

void ColorizedLayerFilter::postProcessAttributes(bool deserializing)
{
	if (deserializing){
		far = near + thickness*normal;
		normal.Normalize();
	}
}

void ColorizedLayerFilter::applyCondition(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		if (!b->isDynamic || !b->geometricalModel) continue;
		const Vector3r& pos = b->physicalParameters->se3.position;
		if ( (pos-near).Dot(normal)>0 && (pos-far).Dot(normal)<0 )
			b->geometricalModel->diffuseColor = diffuseColor;
	}
}

YADE_PLUGIN();
