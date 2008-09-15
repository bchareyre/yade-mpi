/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SPHERE2SPHERE4MACROMICROCONTACTGEOMETRY_HPP
#define SPHERE2SPHERE4MACROMICROCONTACTGEOMETRY_HPP

#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

class InteractingSphere2InteractingSphere4SpheresContactGeometry : public InteractionGeometryEngineUnit
{
	public :
		virtual bool go(const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const Se3r& se31, const Se3r& se32, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const Se3r& se31, const Se3r& se32, const shared_ptr<Interaction>& c);
					
		InteractingSphere2InteractingSphere4SpheresContactGeometry();		
		
		/*! enlarge both radii by this factor (if >1), to permit creation of distant interactions.
		 *
		 * InteractionGeometry will be computed when interactionDetectionFactor*(rad1+rad2) > distance.
		 *
		 * @note This parameter is functionally coupled with InteractinSphere2AABB::aabbEnlargeFactor,
		 * which will create larger bounding boxes and should be of the same value. */
		double interactionDetectionFactor;
		/*! Whether we create SpheresContactGeometry with data necessary for exact rotation computation */
		bool exactRot;

	REGISTER_CLASS_NAME(InteractingSphere2InteractingSphere4SpheresContactGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);

	FUNCTOR2D(InteractingSphere,InteractingSphere);
	
	//FIXME: what is this good for?!
	DEFINE_FUNCTOR_ORDER_2D(InteractingSphere,InteractingSphere);
	
	protected :
		virtual void registerAttributes();
};

REGISTER_SERIALIZABLE(InteractingSphere2InteractingSphere4SpheresContactGeometry,false);

#endif // SPHERE2SPHERE4MACROMICROCONTACTGEOMETRY_HPP

