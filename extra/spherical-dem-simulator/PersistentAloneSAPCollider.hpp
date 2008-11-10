/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PERSISTENTSAPCOLLIDER_HPP
#define PERSISTENTSAPCOLLIDER_HPP

#include "SphericalDEM.hpp"
#include "Contact.hpp"
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#ifndef  __GXX_EXPERIMENTAL_CXX0X__
#	include<boost/shared_ptr.hpp>
	using boost::shared_ptr;
#else
#	include<memory>
	using std::shared_ptr;
#endif

using namespace std;

class PersistentAloneSAPCollider
{
	private :
		struct AABBBound	// represent an extrmity of an Axis ALigned bounding box
		{
			AABBBound(int i, char l) : id(i),lower(l) {};
			int	id;		// AABB of the "id" shpere
			char	lower;	// is it the lower or upper bound of the AABB
			Real	value;		// value of the bound
		};

	struct AABBBoundComparator	// strucuture that compares 2 AABBBounds => used in the sort algorithm
	{
		bool operator() (shared_ptr<AABBBound> b1, shared_ptr<AABBBound> b2)
		{
			return b1->value<b2->value;
		}
	};

	protected :
		int nbPotentialInteractions;		// number of potential transientInteractions = number of interacting AABB
		unsigned int			nbObjects;
		vector<shared_ptr<AABBBound> >	xBounds;	// AABB extremity of the sphere number "id" projected onto the X axis
		vector<shared_ptr<AABBBound> >	yBounds;	// AABB extremity of the sphere number "id" projected onto the Y axis
		vector<shared_ptr<AABBBound> >	zBounds;	// AABB extremity of the sphere number "id" projected onto the Z axis
		vector<Real>			maximums;	// upper right corner of the AABB of the objects =>  for spheres = center[i]-radius
		vector<Real> minimums;	// lower left corner of the AABB of the objects =>  for spheres = center[i]+radius
		
		// Used the first time broadInteractionTest is called, to initialize and sort the xBounds, yBounds,
		// and zBounds arrays and to initialize the overlappingBB collection
		void updateIds(unsigned int nbElements, ContactVecSet& contacts);

		// Permutation sort the xBounds, yBounds, zBounds arrays according to the "value" field
		// Calls updateOverlapingBBSet every time a permutation between 2 AABB i and j occurs
		void sortBounds(vector<shared_ptr<AABBBound> >& bounds, int nbElements, ContactVecSet& contacts);

		// Tests if the AABBi and AABBj really overlap.
		// If yes, adds the pair (id1,id2) to the collection of overlapping AABBs
		// If no, removes the (id1,id2) to the collection of overlapping AABBs if necessary
		void updateOverlapingBBSet(int id1,int id2, ContactVecSet& contacts);

		// update the "value" field of the xBounds, yBounds, zBounds arrays
		void updateBounds(int nbElements);

		// Used the first time broadInteractionTest is called
		// It is necessary to initialise the overlapping AABB collection because this collection is only
		// incrementally udated each time step
		void findOverlappingBB(vector<shared_ptr<AABBBound> >& bounds, int nbElements, ContactVecSet& contacts);

	public :
		PersistentAloneSAPCollider();
		virtual ~PersistentAloneSAPCollider();

		// return a list "transientInteractions" of pairs of Body which Bounding volume are in potential interaction
		void action(const vector<SphericalDEM>& spheres, ContactVecSet& contacts);

};

#endif // PERSISTENTSAPCOLLIDER_HPP

