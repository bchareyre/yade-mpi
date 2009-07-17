/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Collider.hpp>
#include<yade/core/InteractionContainer.hpp>
#include <list>
#include <set>
#include <vector>
#include <algorithm>


/*! \brief Collision detection engine based on regular triangulation.
 
 	This engine is using CGAL library (see http://www.cgal.org/)
 	It is still experimental.
 	Uncoment lines in sconscript file to compile it and just change the name of the collider in an existing xml and it will (should) work.
 	Also needed : uncommenting lines in core/Interaction.cpp and core/Interaction.hpp (see NOTE:TriangulationCollider
 */



class TesselationWrapper;

using namespace std;

class PersistentTriangulationCollider : public Collider
{
	private :
	
		TesselationWrapper* Tes;	
		// represent an extrmity of an Axis ALigned bounding box
		struct AABBBound
		{
			AABBBound(int i, char l) : id(i),lower(l) {};
			int		id;		// AABB of the "id" shpere
			char		lower;		// is it the lower or upper bound of the AABB
			Real		value;		// value of the bound
		};
		// strucuture that compare 2 AABBBounds => used in the sort algorithm
		struct AABBBoundComparator
		{
			bool operator() (shared_ptr<AABBBound> b1, shared_ptr<AABBBound> b2)
			{
				return b1->value<b2->value;
			}
		};

	protected :
		/// number of potential transientInteractions = number of interacting AABB
		int nbPotentialInteractions;

		/// number of AABB
		unsigned int nbObjects;

		/// AABB extremity of the sphere number "id" projected onto the X axis
		vector<shared_ptr<AABBBound> > xBounds;

		/// AABB extremity of the sphere number "id" projected onto the Y axis
		vector<shared_ptr<AABBBound> > yBounds;

		/// AABB extremity of the sphere number "id" projected onto the Z axis
		vector<shared_ptr<AABBBound> > zBounds;

		// collection of AABB that are in interaction
		//protected : vector< set<unsigned int> > overlappingBB;
		shared_ptr<InteractionContainer> transientInteractions;
		/// upper right corner of the AABB of the objects =>  for spheres = center[i]-radius
		vector<Real> maxima;

		/// lower left corner of the AABB of the objects =>  for spheres = center[i]+radius
		vector<Real> minima;

		/// Used the first time broadInteractionTest is called, to initialize and sort the xBounds, yBounds,
		/// and zBounds arrays and to initialize the overlappingBB collection
		void updateIds(unsigned int nbElements);

		/// Permutation sort the xBounds, yBounds, zBounds arrays according to the "value" field
		/// Calls updateOverlapingBBSet every time a permutation between 2 AABB i and j occurs
		void sortBounds(vector<shared_ptr<AABBBound> >& bounds, int nbElements);

		/// Tests if the AABBi and AABBj really overlap.
		/// If yes, adds the pair (id1,id2) to the collection of overlapping AABBs
		/// If no, removes the (id1,id2) to the collection of overlapping AABBs if necessary
		void updateOverlapingBBSet(int id1,int id2);

		/// update the "value" field of the xBounds, yBounds, zBounds arrays
		void updateBounds(int nbElements);

		/// Used the first time broadInteractionTest is called
		/// It is necessary to initialise the overlapping AABB collection because this collection is only
		/// incrementally udated each time step
		void findOverlappingBB(vector<shared_ptr<AABBBound> >& bounds, int nbElements);

	public :
		PersistentTriangulationCollider();

		virtual ~PersistentTriangulationCollider();

		/// return a list "transientInteractions" of pairs of Body which Bounding volume are in potential interaction
		void action(MetaBody *);

		//! When creating transient interaction, look first if a persistent link between the pair in question exists; in that case, skip it.
		bool noTransientIfPersistentExists;
		//! Don't break transient interaction once bodies don't overlap anymore; material law will be responsible for breaking it.
		bool haveDistantTransient;
		//! this flag is used to check if the packing has been triangulated
		bool isTriangulated;

	REGISTER_ATTRIBUTES(Collider,(noTransientIfPersistentExists)(haveDistantTransient));
	REGISTER_CLASS_NAME(PersistentTriangulationCollider);
	REGISTER_BASE_CLASS_NAME(Collider);

};

REGISTER_SERIALIZABLE(PersistentTriangulationCollider);


