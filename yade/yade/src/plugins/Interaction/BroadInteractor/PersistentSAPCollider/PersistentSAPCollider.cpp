/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PersistentSAPCollider.hpp"
#include "Body.hpp"
#include "ComplexBody.hpp"
#include "BodyContainer.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

//	nbPotentialInteractions = 0;

	//overlappingBB.clear();
// 	overlappingBB.resize(maxObject);
// 
// 	for(unsigned int i=0;i<2*maxObject;i++)
// 	{
// 		xBounds[i] = new AABBBound();
// 		yBounds[i] = new AABBBound();
// 		zBounds[i] = new AABBBound();
// 	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

PersistentSAPCollider::~PersistentSAPCollider()
{
/*	for(unsigned int i=0;i<2*maxObject;i++)
	{
		delete xBounds[i];
		delete yBounds[i];
		delete zBounds[i];
	}

	delete[] minimums;
	delete[] maximums;*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void PersistentSAPCollider::postProcessAttributes(bool)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void PersistentSAPCollider::registerAttributes()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void PersistentSAPCollider::broadInteractionTest(Body* body)
{

	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
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
	unsigned int i=0;
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() , ++i )
	{
		b = bodies->getCurrent();
		
		offset = 3*i;
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
	
	volatileInteractions = ncb->volatileInteractions;
	for( volatileInteractions->gotoFirstPotential() ; volatileInteractions->notAtEndPotential() ; volatileInteractions->gotoNextPotential())
	{
		// FIXME : remove this isNew flag and test if interactionPhysic ?
		if (volatileInteractions->getCurrent()->isReal) // if a interaction was only potential then no geometry was created for it and so this time it is still a new one
			volatileInteractions->getCurrent()->isNew = false;
		volatileInteractions->getCurrent()->isReal = false;
	}
	
	updateIds(bodies->size());
	nbObjects = bodies->size();

	// permutation sort of the AABBBounds along the 3 axis performed in a independant manner
	sortBounds(xBounds, nbObjects);
	sortBounds(yBounds, nbObjects);
	sortBounds(zBounds, nbObjects);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void PersistentSAPCollider::updateOverlapingBBSet(int id1,int id2)
{

// 	// look if the paiur (id1,id2) already exists in the overleppingBB collection
	bool found = (volatileInteractions->find(id1,id2)!=0);
	
	// test if the AABBs of the spheres number "id1" and "id2" are overlapping
	int offset1 = 3*id1;
	int offset2 = 3*id2;
	bool overlapp = !(maximums[offset1]<minimums[offset2] || maximums[offset2]<minimums[offset1] || 
			  maximums[offset1+1]<minimums[offset2+1] || maximums[offset2+1]<minimums[offset1+1] || 
			  maximums[offset1+2]<minimums[offset2+2] || maximums[offset2+2]<minimums[offset1+2]);

	// inserts the pair p=(id1,id2) if the two AABB overlapps and if p does not exists in the overlappingBB
	if (overlapp && !found)
		volatileInteractions->insert(id1,id2);
	// removes the pair p=(id1,id2) if the two AABB do not overlapp any more and if p already exists in the overlappingBB
	else if (!overlapp && found)
		volatileInteractions->erase(id1,id2);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
