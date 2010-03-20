/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include <yade/core/Collider.hpp>
#include <yade/core/InteractionContainer.hpp>
#include <vector>

using namespace std;

class SpatialQuickSortCollider : public Collider {
    protected:

	struct AABBBound {
	    Vector3r min,max;
	    int id;
	};

	class xBoundComparator {
	    public:
	      bool operator() (shared_ptr<AABBBound> b1, shared_ptr<AABBBound> b2)
	      {
		 return b1->min[0] < b2->min[0];
	      }
	};

	vector<shared_ptr<AABBBound> > rank;

   public:
		virtual void action();
	YADE_CLASS_BASE_DOC(SpatialQuickSortCollider,Collider,"Collider using quicksort along axes at each step, using :yref:`Aabb` bounds. \n\n Its performance is lower than that of :yref:`InsertionSortCollider` (see `Colliders' performance <https://yade-dem.org/index.php/Colliders_performace>`_), but the algorithm is simple enought to make it good for checking other collider's correctness.");
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(SpatialQuickSortCollider);


