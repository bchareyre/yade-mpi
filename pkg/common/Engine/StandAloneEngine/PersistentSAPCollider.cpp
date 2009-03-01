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

CREATE_LOGGER(PersistentSAPCollider);

PersistentSAPCollider::PersistentSAPCollider() : Collider()
{
	haveDistantTransient=false;

	nbObjects=0;
	xBounds.clear();
	yBounds.clear();
	zBounds.clear();
	minima.clear();
	maxima.clear();

	//timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
}

PersistentSAPCollider::~PersistentSAPCollider()
{

}

void PersistentSAPCollider::action(MetaBody* ncb)
{
	rootBody=ncb;
	shared_ptr<BodyContainer> bodies=ncb->bodies;
	transientInteractions=ncb->transientInteractions;

//	timingDeltas->start();
	
	if (2*bodies->size()!=xBounds.size()){
		xBounds.resize(2*bodies->size());
		yBounds.resize(2*bodies->size());
		zBounds.resize(2*bodies->size());
		minima.resize(3*bodies->size());
		maxima.resize(3*bodies->size());
	}

//	timingDeltas->checkpoint("resizeArrays");

	// Updates the minima and maxima arrays according to the new center and radius of the spheres
	int offset;
	Vector3r min,max;

	const long numBodies=(long)bodies->size();
	//#pragma omp parallel for
	for(int id=0; id<numBodies; id++){
		const shared_ptr<Body>& b=(*bodies)[id];
		offset=3*id;
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

//	timingDeltas->checkpoint("minMaxUpdate");

	typedef pair<body_id_t,body_id_t> bodyIdPair;
	list<bodyIdPair> toBeDeleted;
	FOREACH(const shared_ptr<Interaction>& I,*ncb->transientInteractions){
		// TODO: this logic will be unitedly in Collider::handleExistingInteraction
		//
		// remove interactions deleted by the constitutive law: thay are not new, but nor real either
		// to make sure, do that only with haveDistantTransient
		if(haveDistantTransient && !I->isNew && !I->isReal) { toBeDeleted.push_back(bodyIdPair(I->getId1(),I->getId2())); continue; }
		// Once the interaction has been fully created, it is not "new" anymore
		if (I->isReal) I->isNew=false;
		// OTOH if is is now real anymore, it falls back to the potential state
		if(!haveDistantTransient && !I->isReal) I->isNew=true;
		// for non-distant interactions, isReal depends on whether there is geometrical overlap; that is calculated later
		// for distant: if isReal&&!isNew means:
		// 	the interaction was marked (by the constitutive law) as not real anymore should be deleted
		if(!haveDistantTransient) I->isReal=false;
		//if(!I->isReal){LOG_DEBUG("Interaction #"<<I->getId1()<<"=#"<<I->getId2()<<" is not real.");}
	}
	FOREACH(const bodyIdPair& p, toBeDeleted){ transientInteractions->erase(p.first,p.second); }

//	timingDeltas->checkpoint("deleteInvalid");
	
	updateIds(bodies->size());

//	timingDeltas->checkpoint("updateIds");

	nbObjects=bodies->size();

	// permutation sort of the AABBBounds along the 3 axis performed in a independant manner
	//#pragma omp parallel sections
	{
	//#pragma omp section
		sortBounds(xBounds, nbObjects);
	//#pragma omp section
		sortBounds(yBounds, nbObjects);
	//#pragma omp section
		sortBounds(zBounds, nbObjects);
	}

//	timingDeltas->checkpoint("sortBounds");
}

bool PersistentSAPCollider::probeBoundingVolume(const BoundingVolume& bv)
{
	probedBodies.clear();
	for( vector<shared_ptr<AABBBound> >::iterator 
			it=xBounds.begin(),et=xBounds.end(); it < et; ++it)
	{
		if ((*it)->value > bv.max[0]) break;
		if (!(*it)->lower) continue;
		int offset = 3*(*it)->id;
		if (!(maxima[offset] < bv.min[0] ||
				minima[offset+1] > bv.max[1] ||
				maxima[offset+1] < bv.min[1] ||
				minima[offset+2] > bv.max[2] ||
				maxima[offset+2] < bv.min[2] )) 
		{
			probedBodies.push_back((*it)->id);
		}
	}
	return (bool)probedBodies.size();
}


void PersistentSAPCollider::updateIds(unsigned int nbElements)
{
	// the first time broadInteractionTest is called nbObject=0
	if (nbElements!=nbObjects){
		int begin=0, end=nbElements;
		if (nbElements>nbObjects) begin=nbObjects;

		//timingDeltas->start();

		// initialization if the xBounds, yBounds, zBounds
		for(int i=begin;i<end;i++){
			xBounds[2*i]	= shared_ptr<AABBBound>(new AABBBound(i,1));
			xBounds[2*i+1]	= shared_ptr<AABBBound>(new AABBBound(i,0));
			yBounds[2*i]	= shared_ptr<AABBBound>(new AABBBound(i,1));
			yBounds[2*i+1]	= shared_ptr<AABBBound>(new AABBBound(i,0));
			zBounds[2*i]	= shared_ptr<AABBBound>(new AABBBound(i,1));
			zBounds[2*i+1]	= shared_ptr<AABBBound>(new AABBBound(i,0));
		}

		//timingDeltas->checkpoint("init");

		// initialization if the field "value" of the xBounds, yBounds, zBounds arrays
		updateBounds(nbElements);

		// modified quick sort of the xBounds, yBounds, zBounds arrays
		// The first time these arrays are not sorted so it is faster to use such a sort instead
		// of the permutation we are going to use next
		std::sort(xBounds.begin(),xBounds.begin()+2*nbElements,AABBBoundComparator());
		//timingDeltas->checkpoint("sortX");
		std::sort(yBounds.begin(),yBounds.begin()+2*nbElements,AABBBoundComparator());
		//timingDeltas->checkpoint("sortY");
		std::sort(zBounds.begin(),zBounds.begin()+2*nbElements,AABBBoundComparator());
		//timingDeltas->checkpoint("sortZ");

		// initialize the overlappingBB collection
		//for(unsigned int j=0;j<nbElements;j++)
		//	overlappingBB[j].clear(); //attention memoire

		findOverlappingBB(xBounds, nbElements);
		//timingDeltas->checkpoint("findX");
		findOverlappingBB(yBounds, nbElements);
		//timingDeltas->checkpoint("findY");
		findOverlappingBB(zBounds, nbElements);
		//timingDeltas->checkpoint("findZ");
	}
	else updateBounds(nbElements);
}


void PersistentSAPCollider::sortBounds(vector<shared_ptr<AABBBound> >& bounds, int nbElements){
	int j;
	for (int i=1; i<2*nbElements; i++){
		shared_ptr<AABBBound> tmp(bounds[i]);
		j=i;
		while (j>0 && tmp->value<bounds[j-1]->value) {
			bounds[j]=bounds[j-1];
			updateOverlapingBBSet(tmp->id,bounds[j-1]->id);
			j--;
		}
		bounds[j]=tmp;
	}
}

/* Note that this function is called only for bodies that actually overlap along some axis */
void PersistentSAPCollider::updateOverlapingBBSet(int id1,int id2){
 	// look if the pair (id1,id2) already exists in the overlappingBB collection
	const shared_ptr<Interaction>& interaction=transientInteractions->find(body_id_t(id1),body_id_t(id2));
	bool found=(interaction!=0);//Bruno's Hack
	// if there is persistent interaction, we will not create transient one!
	
	// test if the AABBs of the spheres number "id1" and "id2" are overlapping
	int offset1=3*id1, offset2=3*id2;
	const shared_ptr<Body>& b1(Body::byId(body_id_t(id1),rootBody)), b2(Body::byId(body_id_t(id2),rootBody));
	bool overlap =
		Collider::mayCollide(b1.get(),b2.get());
		// AABB collisions: 
		!(
			maxima[offset1  ]<minima[offset2  ] || maxima[offset2  ]<minima[offset1  ] || 
			maxima[offset1+1]<minima[offset2+1] || maxima[offset2+1]<minima[offset1+1] || 
			maxima[offset1+2]<minima[offset2+2] || maxima[offset2+2]<minima[offset1+2]);
	// inserts the pair p=(id1,id2) if the two AABB overlaps and if p does not exists in the overlappingBB
	//if((id1==0 && id2==1) || (id1==1 && id2==0)) LOG_DEBUG("Processing #0 #1");
	//if(interaction&&!interaction->isReal){ LOG_DEBUG("Unreal interaction #"<<id1<<"=#"<<id2<<" (overlap="<<overlap<<", haveDistantTransient="<<haveDistantTransient<<")");}
	if(overlap && !found){
		//LOG_DEBUG("Creating interaction #"<<id1<<"=#"<<id2);
		transientInteractions->insert(body_id_t(id1),body_id_t(id2));
	}
	// removes the pair p=(id1,id2) if the two AABB do not overlapp any more and if p already exists in the overlappingBB
	else if(!overlap && found && (haveDistantTransient ? !interaction->isReal : true) ){
		//LOG_DEBUG("Erasing interaction #"<<id1<<"=#"<<id2<<" (isReal="<<interaction->isReal<<")");
		transientInteractions->erase(body_id_t(id1),body_id_t(id2));
	}
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
