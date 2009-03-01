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

	SpatialQuickSortCollider();
	virtual ~SpatialQuickSortCollider();

	virtual void action(MetaBody*);


	//! Don't break transient interaction once bodies don't overlap anymore; material law will be responsible for breaking it.
	bool haveDistantTransient;

	void registerAttributes() {
		Collider::registerAttributes();
		REGISTER_ATTRIBUTE(haveDistantTransient);
	}

	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(SpatialQuickSortCollider);
	REGISTER_BASE_CLASS_NAME(Collider);

};
REGISTER_SERIALIZABLE(SpatialQuickSortCollider);


