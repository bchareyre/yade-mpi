/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <yade/pkg-dem/StaticSpheresAttractionEngine.hpp>
#include <yade/pkg-dem/ElasticContactInteraction.hpp>
#include <yade/pkg-common/Sphere.hpp>
#include <yade/core/MetaBody.hpp>

void StaticSpheresAttractionEngine::registerAttributes()
{
	StaticAttractionEngine::registerAttributes();
	// REGISTER_ATTRIBUTE(applies_to_what);
	// REGISTER_ATTRIBUTE(divide_radius);
	REGISTER_ATTRIBUTE(max_displacement);
}

Real StaticSpheresAttractionEngine::getMaxDisplacement(MetaBody* ncb)
{
	// calculation based on max normal force - FIXME not working satifactionary, currently
	/*
	Real penetration=0;
	InteractionContainer::iterator in     = ncb->transientInteractions->begin();
	InteractionContainer::iterator in_end = ncb->transientInteractions->end();
	for( ; in!=in_end ; ++in )
		if(	   doesItApplyToThisBody((*(ncb->bodies))[ (*in)->getId1() ].get()) 
			&& doesItApplyToThisBody((*(ncb->bodies))[ (*in)->getId2() ].get()))
		{ // FIXME - it should work with more than ElasticContactInteraction
			ElasticContactInteraction* el = dynamic_cast<ElasticContactInteraction*>((*in)->interactionPhysics.get());
			if(el)
				penetration = std::max(el->normalForce.Length() / el->initialKn , penetration);
		}

	max_displacement = penetration / 10.0;
	*/

	if(max_displacement == 0) // OK, so try radius based approach
	{
		// simple calculation based on max radius
		Real max_radius=0;
		BodyContainer::iterator bi    = ncb->bodies->begin();
		BodyContainer::iterator biEnd = ncb->bodies->end();
		for( ; bi!=biEnd ; ++bi )
			if(doesItApplyToThisBody((*bi).get()))
				max_radius = std::max(static_cast<Sphere*>((*bi)->geometricalModel.get())->radius , max_radius);
		max_displacement = max_radius / 50;
	}
	return max_displacement;
};

bool StaticSpheresAttractionEngine::doesItApplyToThisBody(Body* b)
{
	if(b->isDynamic && b->geometricalModel->getClassName()=="Sphere")
		return true; 
	return false;
};


YADE_PLUGIN();
