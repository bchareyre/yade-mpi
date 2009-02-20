/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "SpatialQuickSortCollider.hpp"
#include <yade/core/MetaBody.hpp>
#include <yade/core/BodyContainer.hpp>
#include <math.h>
#include <algorithm>


SpatialQuickSortCollider::SpatialQuickSortCollider() : BroadInteractor()
{
}

SpatialQuickSortCollider::~SpatialQuickSortCollider()
{

}

void SpatialQuickSortCollider::registerAttributes()
{
    BroadInteractor::registerAttributes();
}

void SpatialQuickSortCollider::action(MetaBody* ncb)
{
     shared_ptr<BodyContainer> bodies = ncb->bodies;

	size_t nbElements=bodies->size();
       if (nbElements!=rank.size())
       {
	   size_t n = rank.size();
	   rank.resize(nbElements);
	   for (; n<nbElements; ++n)
	       rank[n] = shared_ptr<AABBBound>(new AABBBound);
       }

	Vector3r min,max;
	shared_ptr<Body> b;
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(int i=0 ; bi!=biEnd ; ++bi,++i )
	{
	   b = *bi;
	   
	   if(b->boundingVolume) 
	   {
	      min = b->boundingVolume->min;
	      max = b->boundingVolume->max;
	   }
	   else
	      min=max=b->physicalParameters->se3.position;

	   rank[i]->id = b->getId();
	   rank[i]->min = min;
	   rank[i]->max = max;
	}

	shared_ptr< InteractionContainer > transientInteractions = ncb->transientInteractions;

	InteractionContainer::iterator ii    = transientInteractions->begin();
	InteractionContainer::iterator iiEnd = transientInteractions->end();
	for( ; ii!=iiEnd ; ++ii)
	   (*ii)->cycle = false;


	sort(rank.begin(), rank.end(), xBoundComparator()); // sotring along X

	int id,id2; size_t j;
	shared_ptr<Interaction> interaction;
	for(int i=0,e=nbElements-1; i<e; ++i)
	{
	    id  = rank[i]->id;
	    min = rank[i]->min; 
	    max = rank[i]->max; 
	    j=i;
	    while(++j<nbElements)
	    {
		if ( rank[j]->min[0] < max[0])
		{
		    if (   rank[j]->min[1] < max[1]
			&& rank[j]->max[1] > min[1]
			&& rank[j]->min[2] < max[2]
			&& rank[j]->max[2] > min[2])
		    {
			id2=rank[j]->id;
			if ( (interaction = transientInteractions->find(body_id_t(id),body_id_t(id2))) == 0)
			{
			    interaction = shared_ptr<Interaction>(new Interaction(id,id2) );
			    interaction->cycle=true;
			    transientInteractions->insert(interaction);
			}
			else
			    interaction->cycle = true;
		    }
		}
		else break;
	    }
	}

	ii    = transientInteractions->begin();
	iiEnd = transientInteractions->end();
	for( ; ii!=iiEnd ; ++ii)
	   if ( !(interaction = *ii)->cycle && !interaction->isReal ) transientInteractions->erase( interaction->getId1(), interaction->getId2());

}

