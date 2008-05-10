/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef EDGE2SPHERE4SPHERESCONTACTGEOMETRY_HPP
#define EDGE2SPHERE4SPHERESCONTACTGEOMETRY_HPP

#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

class InteractingEdge2InteractingSphere4SpheresContactGeometry : public InteractionGeometryEngineUnit
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
					
		InteractingEdge2InteractingSphere4SpheresContactGeometry();		
					
	REGISTER_CLASS_NAME(InteractingEdge2InteractingSphere4SpheresContactGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);

	FUNCTOR2D(InteractingEdge,InteractingSphere);

	DEFINE_FUNCTOR_ORDER_2D(InteractingEdge,InteractingSphere);
	
	protected :
		virtual void registerAttributes();
};

REGISTER_SERIALIZABLE(InteractingEdge2InteractingSphere4SpheresContactGeometry,false);

#endif //  EDGE2SPHERE4SPHERESCONTACTGEOMETRY_HPP

