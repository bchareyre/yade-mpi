#ifndef __SAPCOLLIDER_H__
#define __SAPCOLLIDER_H__

#include <list>
#include <set>
#include <vector>
#include <algorithm>

#include "BroadCollider.hpp"

class SAPCollider : public BroadCollider
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// represent an extrmity of an Axis ALigned bounding box
	struct AABBBound
	{
		AABBBound() {};
		char lower; // is it the lower or upper bound of the AABB
		int id;		// AABB of the "id" shpere
		float value;// value of the bound
	};

	// strucuture that compare 2 AABBBounds => used in the sort algorithm
	struct AABBBoundComparator
	{
		bool operator() (AABBBound* b1, AABBBound* b2)
		{
			return b1->value<b2->value;
		}
	};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// number of potential collision = number of colliding AABB
	protected : int nbPotentialCollisions;

	// maximum number of object that that collider can handle
	protected : unsigned int maxObject;

	// number of AABB
	protected : unsigned int nbObjects;

	// AABB extremity of the sphere number "id" projected onto the X axis
	protected : std::vector<AABBBound*> xBounds;

	// AABB extremity of the sphere number "id" projected onto the Y axis
	protected : std::vector<AABBBound*> yBounds;

	// AABB extremity of the sphere number "id" projected onto the Z axis
	protected : std::vector<AABBBound*> zBounds;

	// collection of AABB that are in collision
	protected : std::vector< std::set<unsigned int> > overlappingBB;

	// upper right corner of the AABB of the objects =>  for spheres = center[i]-radius
	protected : float * maximums;

	// lower left corner of the AABB of the objects =>  for spheres = center[i]+radius
	protected : float * minimums;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// construction
	public : SAPCollider ();
	public : ~SAPCollider ();

	public : void afterDeserialization();
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// return a list "interactions" of pairs of Body which Bounding volume are in collisions
	public : void broadCollisionTest(Body * body);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// Used the first time broadCollisionTest is called, to initialize and sort the xBounds, yBounds,
	// and zBounds arrays and to initialize the overlappingBB collection
	protected : void updateIds(unsigned int nbElements);

	// Permutation sort the xBounds, yBounds, zBounds arrays according to the "value" field
	// Calls updateOverlapingBBSet every time a permutation between 2 AABB i and j occurs
	protected : void sortBounds(std::vector<AABBBound*>& bounds, int nbElements);

	// Tests if the AABBi and AABBj really overlap.
	// If yes, adds the pair (id1,id2) to the collection of overlapping AABBs
	// If no, removes the (id1,id2) to the collection of overlapping AABBs if necessary
	protected : void updateOverlapingBBSet(int id1,int id2);

	// update the "value" field of the xBounds, yBounds, zBounds arrays
	protected : void updateBounds(int nbElements);

	// Used the first time broadCollisionTest is called
	// It is necessary to initialise the overlapping AABB collection because this collection is only
	// incrementally udated each time step
	protected : void findOverlappingBB(std::vector<AABBBound*>& bounds, int nbElements);

	REGISTER_CLASS_NAME(SAPCollider);

};


REGISTER_SERIALIZABLE(SAPCollider,false);


#endif // __SAPCOLLIDER_H__
