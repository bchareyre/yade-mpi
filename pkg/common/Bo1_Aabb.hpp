/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include <pkg/common/Dispatching.hpp>
#include <pkg/common/Aabb.hpp>
#include <pkg/common/Sphere.hpp>
#include <pkg/common/Facet.hpp>
#include <pkg/common/Box.hpp>

class Bo1_Sphere_Aabb : public BoundFunctor
{
	public :
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
	FUNCTOR1D(Sphere);
	YADE_CLASS_BASE_DOC_ATTRS(Bo1_Sphere_Aabb,BoundFunctor,"Functor creating :yref:`Aabb` from :yref:`Sphere`.",
		((Real,aabbEnlargeFactor,((void)"deactivated",-1),,"Relative enlargement of the bounding box; deactivated if negative.\n\n.. note::\n\tThis attribute is used to create distant interaction, but is only meaningful with an :yref:`IGeomFunctor` which will not simply discard such interactions: :yref:`Ig2_Sphere_Sphere_ScGeom::interactionDetectionFactor` should have the same value as :yref:`aabbEnlargeFactor<Bo1_Sphere_Aabb::aabbEnlargeFactor>`."))
	);
};

REGISTER_SERIALIZABLE(Bo1_Sphere_Aabb);


/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

class Bo1_Facet_Aabb : public BoundFunctor{
	public:
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body*);
	FUNCTOR1D(Facet);
	YADE_CLASS_BASE_DOC(Bo1_Facet_Aabb,BoundFunctor,"Creates/updates an :yref:`Aabb` of a :yref:`Facet`.");
};
REGISTER_SERIALIZABLE(Bo1_Facet_Aabb);


/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

class Box;
class Bo1_Box_Aabb : public BoundFunctor{
	public:
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body*);
	FUNCTOR1D(Box);
	YADE_CLASS_BASE_DOC(Bo1_Box_Aabb,BoundFunctor,"Create/update an :yref:`Aabb` of a :yref:`Box`.");
};

REGISTER_SERIALIZABLE(Bo1_Box_Aabb);



