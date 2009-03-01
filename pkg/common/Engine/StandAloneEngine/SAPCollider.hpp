/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include<yade/core/Collider.hpp>

class SAPCollider : public Collider
{
	private :
		// represent an extrmity of an Axis ALigned bounding box
		struct AABBBound
		{
			AABBBound() {};
			char	lower;		// is it the lower or upper bound of the AABB
			int	id;		// AABB of the "id" shpere
			Real	value;		// value of the bound
		};

		// strucuture that compare 2 AABBBounds => used in the sort algorithm
		struct AABBBoundComparator
		{
			bool operator() (AABBBound* b1, AABBBound* b2)
			{
				return b1->value<b2->value;
			}
		};

	protected :
		// number of potential transientInteractions = number of colliding AABB
		int nbPotentialInteractions;
		// maximum number of object that that collider can handle
		unsigned int maxObject;
		// number of AABB
		unsigned int nbObjects;
		// AABB extremity of the sphere number "id" projected onto the X axis
		std::vector<AABBBound*> xBounds;
		// AABB extremity of the sphere number "id" projected onto the Y axis
		std::vector<AABBBound*> yBounds;
		// AABB extremity of the sphere number "id" projected onto the Z axis
		std::vector<AABBBound*> zBounds;
		// collection of AABB that are in interaction
		std::vector< std::set<unsigned int> > overlappingBB;
		// upper right corner of the AABB of the objects =>  for spheres = center[i]-radius
		Real * maximums;
		// lower left corner of the AABB of the objects =>  for spheres = center[i]+radius
		Real * minimums;
		
		void findOverlappingBB(std::vector<AABBBound*>& bounds, int nbElements);

	public :
		SAPCollider ();
		~SAPCollider ();

		// return a list "transientInteractions" of pairs of Body which Bounding volume are in potential interaction
		void action(MetaBody *);


		// Used the first time broadInteractionTest is called, to initialize and sort the xBounds, yBounds,
		// and zBounds arrays and to initialize the overlappingBB collection
		void updateIds(unsigned int nbElements);

		// Permutation sort the xBounds, yBounds, zBounds arrays according to the "value" field
		// Calls updateOverlapingBBSet every time a permutation between 2 AABB i and j occurs
		void sortBounds(std::vector<AABBBound*>& bounds, int nbElements);

		// Tests if the AABBi and AABBj really overlap.
		// If yes, adds the pair (id1,id2) to the collection of overlapping AABBs
		// If no, removes the (id1,id2) to the collection of overlapping AABBs if necessary
		void updateOverlapingBBSet(int id1,int id2);

		// update the "value" field of the xBounds, yBounds, zBounds arrays
		void updateBounds(int nbElements);

		// Used the first time broadInteractionTest is called
		// It is necessary to initialise the overlapping AABB collection because this collection is only
		// incrementally udated each time step

	REGISTER_CLASS_NAME(SAPCollider);
	REGISTER_BASE_CLASS_NAME(Collider);
};

REGISTER_SERIALIZABLE(SAPCollider);


