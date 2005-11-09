/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PersistentAloneSAPCollider.hpp"

PersistentAloneSAPCollider::PersistentAloneSAPCollider()
{
//	cerr << "PersistentAloneSAPCollider\n";

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


PersistentAloneSAPCollider::~PersistentAloneSAPCollider()
{

}


void PersistentAloneSAPCollider::action(const vector<SphericalDEM>& spheres, ContactVecSet& contacts)
{
	if (2*spheres.size()!=xBounds.size())
	{
		xBounds.resize(2*spheres.size());
		yBounds.resize(2*spheres.size());
		zBounds.resize(2*spheres.size());

		minimums.resize(3*spheres.size());
		maximums.resize(3*spheres.size());
	}

	if (contacts.size()!=spheres.size())
		contacts.resize(spheres.size());

	// Updates the minimums and maximums arrays according to the new center and radius of the spheres
	int offset;
	Vector3r min,max;

	SphericalDEM s;
	vector<SphericalDEM>::const_iterator si    = spheres.begin();
	vector<SphericalDEM>::const_iterator siEnd = spheres.end();
	for(unsigned int i=0 ; si!=siEnd ; ++si,i++ )
	{
		Real r = (*si).radius;
		Vector3r p = (*si).position;

		offset = 3*i;

		minimums[offset+0] = p[0]-r;
		minimums[offset+1] = p[1]-r;
		minimums[offset+2] = p[2]-r;
		maximums[offset+0] = p[0]+r;
		maximums[offset+1] = p[1]+r;
		maximums[offset+2] = p[2]+r;
	}

	ContactVecSet::iterator sci    = contacts.begin();
	ContactVecSet::iterator sciEnd = contacts.end();
	for( ; sci!=sciEnd ; ++sci)
	{
		ContactSet::iterator ci    = sci->begin();
		ContactSet::iterator ciEnd = sci->end();
		for( ; ci!=ciEnd ; ++ci)
		{
			if ((*ci).isReal) // if a interaction was only potential then no geometry was created for it and so this time it is still a new one
			{
				Contact * c = const_cast<Contact *>(&(*ci));
				c->isNew = false;
				c->isReal = false;
			}
		}
	}
	
	updateIds(spheres.size(),contacts);

	nbObjects = spheres.size();

	// permutation sort of the AABBBounds along the 3 axis performed in a independant manner
	sortBounds(xBounds, nbObjects, contacts);
	sortBounds(yBounds, nbObjects, contacts);
	sortBounds(zBounds, nbObjects, contacts);

}


void PersistentAloneSAPCollider::updateIds(unsigned int nbElements, ContactVecSet& contacts)
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
		sort(xBounds.begin(),xBounds.begin()+2*nbElements,AABBBoundComparator());
		sort(yBounds.begin(),yBounds.begin()+2*nbElements,AABBBoundComparator());
		sort(zBounds.begin(),zBounds.begin()+2*nbElements,AABBBoundComparator());

		// initialize the overlappingBB collection
		//for(unsigned int j=0;j<nbElements;j++)
		//	overlappingBB[j].clear(); //attention memoire

		findOverlappingBB(xBounds, nbElements, contacts);
		findOverlappingBB(yBounds, nbElements, contacts);
		findOverlappingBB(zBounds, nbElements, contacts);

	}
	else
		updateBounds(nbElements);
}


void PersistentAloneSAPCollider::sortBounds(vector<shared_ptr<AABBBound> >& bounds, int nbElements, ContactVecSet& contacts)
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
			updateOverlapingBBSet(tmp->id,bounds[j-1]->id,contacts);
			j--;
		}
		bounds[j] = tmp;
	}
}


void PersistentAloneSAPCollider::updateOverlapingBBSet(int id1,int id2, ContactVecSet& contacts)
{
	if (id1>id2)
		swap(id1,id2);

	Contact c;
	c.isNew = true;
	c.isReal = false;
	c.id = id2;

// 	// look if the paiur (id1,id2) already exists in the overleppingBB collection
	ContactSet::iterator csi = contacts[id1].find(c);
	bool found = (csi!=contacts[id1].end());

	// test if the AABBs of the spheres number "id1" and "id2" are overlapping
	int offset1 = 3*id1;
	int offset2 = 3*id2;
	bool overlapp = !(maximums[offset1]<minimums[offset2] || maximums[offset2]<minimums[offset1] || 
			  maximums[offset1+1]<minimums[offset2+1] || maximums[offset2+1]<minimums[offset1+1] || 
			  maximums[offset1+2]<minimums[offset2+2] || maximums[offset2+2]<minimums[offset1+2]);

	// inserts the pair p=(id1,id2) if the two AABB overlapps and if p does not exists in the overlappingBB
	if (overlapp && !found)
		contacts[id1].insert(c);
	// removes the pair p=(id1,id2) if the two AABB do not overlapp any more and if p already exists in the overlappingBB
	else if (!overlapp && found)
		contacts[id1].erase(csi);

}


void PersistentAloneSAPCollider::updateBounds(int nbElements)
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




void PersistentAloneSAPCollider::findOverlappingBB(std::vector<shared_ptr<AABBBound> >& bounds, int nbElements, ContactVecSet& contacts)
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
				updateOverlapingBBSet(bounds[i]->id,bounds[j]->id,contacts);
			j++;
		}
		i++;
	}
}

