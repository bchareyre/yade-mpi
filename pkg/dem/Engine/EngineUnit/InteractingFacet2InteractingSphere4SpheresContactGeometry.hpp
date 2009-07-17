/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

class InteractingFacet2InteractingSphere4SpheresContactGeometry : public InteractionGeometryEngineUnit
{
	public :
		virtual bool go(	const shared_ptr<InteractingGeometry>& cm1,
					const shared_ptr<InteractingGeometry>& cm2,
					const Se3r& se31,
					const Se3r& se32,
					const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<InteractingGeometry>& cm1,
					const shared_ptr<InteractingGeometry>& cm2,
					const Se3r& se31,
					const Se3r& se32,
					const shared_ptr<Interaction>& c);
					
		InteractingFacet2InteractingSphere4SpheresContactGeometry();		
					
	REGISTER_CLASS_NAME(InteractingFacet2InteractingSphere4SpheresContactGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);
	REGISTER_ATTRIBUTES(InteractionGeometryEngineUnit,(shrinkFactor));

	DECLARE_LOGGER;

	FUNCTOR2D(InteractingFacet,InteractingSphere);

	DEFINE_FUNCTOR_ORDER_2D(InteractingFacet,InteractingSphere);

	// The radius of the inscribed circle of the facet is decreases by the
	// value of the sphere's radius multipled by shrinkFactor. So, at
	// definition of a contact point on the surface made of facets, the given
	// surface is not continuous and is similar to a surface covered with a
	// tile, with a gap between the separate tiles equal to the sphere's radius
	// multiplied by 2*shrinkFactor. By default shrinkFactor=0.
	Real shrinkFactor; 
};

REGISTER_SERIALIZABLE(InteractingFacet2InteractingSphere4SpheresContactGeometry);


