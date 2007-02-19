/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PersistentSAPCollider.hpp"
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-core/BodyContainer.hpp>

PersistentSAPCollider::PersistentSAPCollider() : BroadInteractor()
{
//	cerr << "PersistentSAPCollider\n";

	//this->maxObject = 150000;
	nbObjects=0;

	//xBounds.resize(2*maxObject);
	//yBounds.resize(2*maxObject);
	//zBounds.resize(2*maxObject);

	//minimums = new Real[3*maxObject];
	//maximums = new Real[3*maxObject];
	
	xBounds.clear();
	yBounds.clear();
	zBounds.clear();

	minimums.clear();
	maximums.clear();
}

PersistentSAPCollider::~PersistentSAPCollider()
{

}

void PersistentSAPCollider::action(Body* body)
{

	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;
	
	if (2*bodies->size()!=xBounds.size())
	{
		xBounds.resize(2*bodies->size());
		yBounds.resize(2*bodies->size());
		zBounds.resize(2*bodies->size());

		minimums.resize(3*bodies->size());
		maximums.resize(3*bodies->size());
	}

	// Updates the minimums and maximums arrays according to the new center and radius of the spheres
	int offset;
	Vector3r min,max;
	shared_ptr<Body> b;

	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(unsigned int i=0 ; bi!=biEnd ; ++bi,i++ )
	{
		b = *bi;
		
		offset = 3*i;
		//FIXME: this is broken: bodies without boundingVolume are just skipped, which means that some garbage values are used later!
		if(b->boundingVolume) // can't assume that everybody has BoundingVolume
		{
			min = b->boundingVolume->min;
			max = b->boundingVolume->max;
			minimums[offset+0] = min[0];
			minimums[offset+1] = min[1];
			minimums[offset+2] = min[2];
			maximums[offset+0] = max[0];
			maximums[offset+1] = max[1];
			maximums[offset+2] = max[2];
		}
	}
	
	transientInteractions = ncb->transientInteractions;
	InteractionContainer::iterator ii    = transientInteractions->begin();
	InteractionContainer::iterator iiEnd = transientInteractions->end();
	for( ; ii!=iiEnd ; ++ii)
	{
		shared_ptr<Interaction> interaction = *ii;
		// FIXME : remove this isNew flag and test if interactionPhysic ?
		if (interaction->isReal) // if a interaction was only potential then no geometry was created for it and so this time it is still a new one
			interaction->isNew = false;
		interaction->isReal = false;
	}
	
	updateIds(bodies->size());
	nbObjects = bodies->size();

	// permutation sort of the AABBBounds along the 3 axis performed in a independant manner
	sortBounds(xBounds, nbObjects);
	sortBounds(yBounds, nbObjects);
	sortBounds(zBounds, nbObjects);

}


void PersistentSAPCollider::updateIds(unsigned int nbElements)
{

	// the first time broadInteractionTest is called nbObject=0
	if (nbElements!=nbObjects)
	{
		int begin,end;

		end = nbElements;
		begin = 0;

		if (nbElements>nbObjects)
			begin = nbObjects;

		// initialization if the xBounds, yBounds, zBounds
		for(int i=begin;i<end;i++)
		{
			xBounds[2*i]	= shared_ptr<AABBBound>(new AABBBound(i,1));
			xBounds[2*i+1]	= shared_ptr<AABBBound>(new AABBBound(i,0));
			
			yBounds[2*i]	= shared_ptr<AABBBound>(new AABBBound(i,1));
			yBounds[2*i+1]	= shared_ptr<AABBBound>(new AABBBound(i,0));
			
			zBounds[2*i]	= shared_ptr<AABBBound>(new AABBBound(i,1));
			zBounds[2*i+1]	= shared_ptr<AABBBound>(new AABBBound(i,0));
		}

		// initialization if the field "value" of the xBounds, yBounds, zBounds arrays
		updateBounds(nbElements);

		// modified quick sort of the xBounds, yBounds, zBounds arrays
		// The first time these arrays are not sorted so it is faster to use such a sort instead
		// of the permutation we are going to use next
		std::sort(xBounds.begin(),xBounds.begin()+2*nbElements,AABBBoundComparator());
		std::sort(yBounds.begin(),yBounds.begin()+2*nbElements,AABBBoundComparator());
		std::sort(zBounds.begin(),zBounds.begin()+2*nbElements,AABBBoundComparator());

		// initialize the overlappingBB collection
		//for(unsigned int j=0;j<nbElements;j++)
		//	overlappingBB[j].clear(); //attention memoire

		findOverlappingBB(xBounds, nbElements);
		findOverlappingBB(yBounds, nbElements);
		findOverlappingBB(zBounds, nbElements);

	}
	else
		updateBounds(nbElements);
}


void PersistentSAPCollider::sortBounds(vector<shared_ptr<AABBBound> >& bounds, int nbElements)
{
	int i,j;
	shared_ptr<AABBBound> tmp;

	for (i=1; i<2*nbElements; i++)
	{
		tmp = bounds[i];
		j = i;
		while (j>0 && tmp->value<bounds[j-1]->value)
		{
			bounds[j] = bounds[j-1];
			updateOverlapingBBSet(tmp->id,bounds[j-1]->id);
			j--;
		}
		bounds[j] = tmp;
	}
}


void PersistentSAPCollider::updateOverlapingBBSet(int id1,int id2)
{

// 	// look if the pair (id1,id2) already exists in the overleppingBB collection
	bool found = (transientInteractions->find(id1,id2)!=0);
	
	// test if the AABBs of the spheres number "id1" and "id2" are overlapping
	int offset1 = 3*id1;
	int offset2 = 3*id2;
	#ifdef HIGHLEVEL_CLUMPS
		Body::id_t clumpId1=Body::byId(id1)->clumpId, clumpId2=Body::byId(id2)->clumpId;
	#endif
	bool overlap =
	#ifdef HIGHLEVEL_CLUMPS
		(clumpId1==Body::ID_NONE || clumpId2==Body::ID_NONE || clumpId1!=clumpId2) && // only collide if at least one particle is standalone or belongs to different clumps
		clumpId1!=(Body::id_t)id1 && clumpId2!=(Body::id_t)id2 && // do not collide clumps, since they are just containers, they never interact
	#endif
		!(maximums[offset1]<minimums[offset2] || maximums[offset2]<minimums[offset1] || 
		maximums[offset1+1]<minimums[offset2+1] || maximums[offset2+1]<minimums[offset1+1] || 
		maximums[offset1+2]<minimums[offset2+2] || maximums[offset2+2]<minimums[offset1+2]);

	// inserts the pair p=(id1,id2) if the two AABB overlaps and if p does not exists in the overlappingBB
	if (overlap && !found)
		transientInteractions->insert(id1,id2);
	// removes the pair p=(id1,id2) if the two AABB do not overlapp any more and if p already exists in the overlappingBB
	else if (!overlap && found)
		transientInteractions->erase(id1,id2);

}


void PersistentSAPCollider::updateBounds(int nbElements)
{

	for(int i=0; i < 2*nbElements; i++)
	{
		if (xBounds[i]->lower)
			xBounds[i]->value = minimums[3*xBounds[i]->id+0];
		else
			xBounds[i]->value = maximums[3*xBounds[i]->id+0];

		if (yBounds[i]->lower)
			yBounds[i]->value = minimums[3*yBounds[i]->id+1];
		else
			yBounds[i]->value = maximums[3*yBounds[i]->id+1];

		if (zBounds[i]->lower)
			zBounds[i]->value = minimums[3*zBounds[i]->id+2];
		else
			zBounds[i]->value = maximums[3*zBounds[i]->id+2];
	}
}




void PersistentSAPCollider::findOverlappingBB(std::vector<shared_ptr<AABBBound> >& bounds, int nbElements)
{

	int i,j;

	i = j = 0;
	while (i<2*nbElements)
	{
		while (i<2*nbElements && !bounds[i]->lower)
			i++;
		j=i+1;
		while (j<2*nbElements && bounds[j]->id!=bounds[i]->id)
		{
			if (bounds[j]->lower)
				updateOverlapingBBSet(bounds[i]->id,bounds[j]->id);
			j++;
		}
		i++;
	}
}

