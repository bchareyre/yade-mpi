/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include<yade/pkg-common/Collider.hpp>
#include<yade/core/InteractionContainer.hpp>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include<yade/pkg-dem/TesselationWrapper.hpp>

/*! \brief Collision detection engine based on regular triangulation.
 
 	This engine is using CGAL library (see http://www.cgal.org/)
 	It is still experimental.
 	Uncoment lines in sconscript file to compile it and just change the name of the collider in an existing xml and it will (should) work.
 	Also needed : uncommenting lines in core/Interaction.cpp and core/Interaction.hpp (see NOTE:TriangulationCollider
 */

class TesselationWrapper;


class PersistentTriangulationCollider : public Collider
{
	private :	
		TesselationWrapper* Tes;	

	public :		
		virtual ~PersistentTriangulationCollider();
		/// return a list "interactions" of pairs of Body which bounding spheres are overlapping
		void action();
		//! this flag is used to check if the packing has been triangulated
		bool isTriangulated;
		shared_ptr<InteractionContainer> interactions;
		
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(
		PersistentTriangulationCollider,Collider,"Collision detection engine based on regular triangulation. Handles spheres and flat boundaries (considered as infinite-sized bounding spheres).",
		((bool,haveDistantTransient,false,"Keep distant interactions? If True, don't delete interactions once bodies don't overlap anymore; constitutive laws will be responsible for requesting deletion. If False, delete as soon as there is no object penetration."))
		,
		isTriangulated = false;
		Tes = new (TesselationWrapper);
  	,);
};

REGISTER_SERIALIZABLE(PersistentTriangulationCollider);
