/*************************************************************************
*  Copyright (C) 2004,2007 by
*  	Olivier Galizzi <olivier.galizzi@imag.fr>
*  	Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
*  	Václav Šmilauer <eudoxos@arcig.cz>
*
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"PersistentSAPCollider.hpp"
#include<yade/core/Body.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/BodyContainer.hpp>
#include<limits>

PersistentSAPCollider::PersistentSAPCollider() : BroadInteractor()
{
	noTransientIfPersistentExists=false;
	haveDistantTransient=false;

	nbObjects=0;
	xBounds.clear();
	yBounds.clear();
	zBounds.clear();
	minima.clear();
	maxima.clear();
}

PersistentSAPCollider::~PersistentSAPCollider()
{

}

void PersistentSAPCollider::action(Body* body)
{

	MetaBody *ncb=YADE_CAST<MetaBody*>(body);
	shared_ptr<BodyContainer> bodies=ncb->bodies;
	
	if (2*bodies->size()!=xBounds.size()){
		xBounds.resize(2*bodies->size());
		yBounds.resize(2*bodies->size());
		zBounds.resize(2*bodies->size());
		minima.resize(3*bodies->size());
		maxima.resize(3*bodies->size());
	}

	// Updates the minima and maxima arrays according to the new center and radius of the spheres
	int offset;
	Vector3r min,max;

	BodyContainer::iterator bi=bodies->begin();
	BodyContainer::iterator bi_end = bodies->end();
	for(unsigned int i=0 ; bi!=bi_end; ++bi,i++) {
		const shared_ptr<Body>& b=*bi;
		offset = 3*i;
		//FIXME: this is broken: bodies without boundingVolume are just skipped, which means that some garbage values are used later!
		if(b->boundingVolume){ // can't assume that everybody has BoundingVolume
			min=b->boundingVolume->min; max=b->boundingVolume->max;
			minima[offset+0]=min[0]; minima[offset+1]=min[1]; minima[offset+2]=min[2];
			maxima[offset+0]=max[0]; maxima[offset+1]=max[1]; maxima[offset+2]=max[2];
		}
		else {
			/* assign the center of gravity as zero-volume bounding box;
			 * it should not create spurious interactions and
			 * is a better solution that putting nan's into minima and maxima which crashes on _some_ machines */
			const Vector3r& pos=b->physicalParameters->se3.position;
			minima[offset+0]=pos[0]; minima[offset+1]=pos[1]; minima[offset+2]=pos[2];
			maxima[offset+0]=pos[0]; maxima[offset+1]=pos[1]; maxima[offset+2]=pos[2];
		}
	}

	transientInteractions = ncb->transientInteractions;	
	InteractionContainer::iterator I_end = transientInteractions->end();
	for(InteractionContainer::iterator I=transientInteractions->begin(); I!=I_end; ++I) {
		if ((*I)->isReal) (*I)->isNew=false; // FIXME : remove this isNew flag and test if interactionPhysic ?
		if(!haveDistantTransient) (*I)->isReal=false; // reset this flag, is used later... (??)
	}
	
	updateIds(bodies->size());
	nbObjects=bodies->size();

	// permutation sort of the AABBBounds along the 3 axis performed in a independant manner
	sortBounds(xBounds, nbObjects);
	sortBounds(yBounds, nbObjects);
	sortBounds(zBounds, nbObjects);
}


void PersistentSAPCollider::updateIds(unsigned int nbElements)
{

	// the first time broadInteractionTest is called nbObject=0
	if (nbElements!=nbObjects){
		int begin=0, end=nbElements;
		if (nbElements>nbObjects) begin=nbObjects;

		// initialization if the xBounds, yBounds, zBounds
		for(int i=begin;i<end;i++){
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
	else updateBounds(nbElements);
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
	const shared_ptr<Interaction>& interaction=transientInteractions->find(body_id_t(id1),body_id_t(id2));
	bool found=(interaction!=0);//Bruno's Hack
	// if there is persistent interaction, we will not create transient one!
	bool foundPersistent = noTransientIfPersistentExists ? (persistentInteractions->find(body_id_t(id1),body_id_t(id2))!=0) : false;
	
	// test if the AABBs of the spheres number "id1" and "id2" are overlapping
	int offset1 = 3*id1;
	int offset2 = 3*id2;
	// FIXME: this is perhaps an expensive operation?!
	const shared_ptr<Body>& b1(Body::byId(body_id_t(id1))), b2(Body::byId(body_id_t(id2)));
	bool overlap =
		(b1->isStandalone() || b2->isStandalone() || b1->clumpId!=b2->clumpId ) && // only collide if at least one particle is standalone or they belong to different clumps
		!b1->isClump() && !b2->isClump() && // do not collide clumps, since they are just containers, never interact

		!(maxima[offset1]<minima[offset2] || maxima[offset2]<minima[offset1] || 
		maxima[offset1+1]<minima[offset2+1] || maxima[offset2+1]<minima[offset1+1] || 
		maxima[offset1+2]<minima[offset2+2] || maxima[offset2+2]<minima[offset1+2]);

	// inserts the pair p=(id1,id2) if the two AABB overlaps and if p does not exists in the overlappingBB
	if(overlap && !found && !foundPersistent) transientInteractions->insert(body_id_t(id1),body_id_t(id2));
	// removes the pair p=(id1,id2) if the two AABB do not overlapp any more and if p already exists in the overlappingBB
	else if(!overlap && found && (haveDistantTransient ? !interaction->isReal : true) ) transientInteractions->erase(body_id_t(id1),body_id_t(id2));
}


void PersistentSAPCollider::updateBounds(int nbElements)
{
	for(int i=0; i < 2*nbElements; i++){
		if (xBounds[i]->lower) xBounds[i]->value = minima[3*xBounds[i]->id+0];
		else xBounds[i]->value = maxima[3*xBounds[i]->id+0];

		if (yBounds[i]->lower) yBounds[i]->value = minima[3*yBounds[i]->id+1];
		else yBounds[i]->value = maxima[3*yBounds[i]->id+1];

		if (zBounds[i]->lower) zBounds[i]->value = minima[3*zBounds[i]->id+2];
		else zBounds[i]->value = maxima[3*zBounds[i]->id+2];
	}
}




void PersistentSAPCollider::findOverlappingBB(std::vector<shared_ptr<AABBBound> >& bounds, int nbElements){
	int i=0,j=0;
	while (i<2*nbElements) {
		while (i<2*nbElements && !bounds[i]->lower) i++;
		j=i+1;
		while (j<2*nbElements && bounds[j]->id!=bounds[i]->id){
			if (bounds[j]->lower) updateOverlapingBBSet(bounds[i]->id,bounds[j]->id);
			j++;
		}
		i++;
	}
}

YADE_PLUGIN();
