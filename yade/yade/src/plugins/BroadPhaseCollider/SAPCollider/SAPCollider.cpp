#include "SAPCollider.hpp"
#include "Body.hpp"
#include "Contact.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SAPCollider::SAPCollider () : BroadPhaseCollider()
{
	this->maxObject = 150000;
	nbObjects=0;
	
	xBounds.resize(2*maxObject);
	yBounds.resize(2*maxObject);
	zBounds.resize(2*maxObject);

	minimums = new float[3*maxObject];
	maximums = new float[3*maxObject];

	nbPotentialCollisions = 0;

	overlappingBB.clear();
	overlappingBB.resize(maxObject);

	for(unsigned int i=0;i<2*maxObject;i++)
	{
		xBounds[i] = new AABBBound();
		yBounds[i] = new AABBBound();
		zBounds[i] = new AABBBound();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SAPCollider::~SAPCollider ()
{
	for(unsigned int i=0;i<2*maxObject;i++)
	{
		delete xBounds[i];		
		delete yBounds[i];		
		delete zBounds[i];
	}	

	delete minimums;
	delete maximums;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SAPCollider::processAttributes()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SAPCollider::registerAttributes()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int SAPCollider::broadPhaseCollisionTest(const std::vector<shared_ptr<Body> >& bodies,std::list<shared_ptr<Interaction> >& interactions)
{
	unsigned int i;
	
	// Updates the minimums and maximums arrays according to the new center and radius of the spheres
	int offset;
	Vector3 min,max;
	for(i=0; i < bodies.size(); i++)
	{
		offset = 3*i;
		min = bodies[i]->bv->min;
		max = bodies[i]->bv->max;
		minimums[offset+0] = min[0];
		minimums[offset+1] = min[1];
		minimums[offset+2] = min[2];
		maximums[offset+0] = max[0];
		maximums[offset+1] = max[1];
		maximums[offset+2] = max[2];
	}

	updateIds(bodies.size());
	nbObjects = bodies.size();

	// permutation sort of the AABBBounds along the 3 axis performed in a independant manner
	sortBounds(xBounds, nbObjects);
	sortBounds(yBounds, nbObjects);
	sortBounds(zBounds, nbObjects);	
					
	// just copy the structure containing overlapping AABB into a more easy to use linked list of pair of overlapping AABB
	std::set<unsigned int>::iterator it;
	std::set<unsigned int>::iterator itEnd;	
	
	/*std::list<Interaction*>::iterator ci = interactions.begin();
	std::list<Interaction*>::iterator ciEnd = interactions.end();
	for(;ci!=ciEnd;++ci)
	{
		delete (*ci)->interactionModel;
		delete (*ci);
	}*/
	interactions.clear();
	
	nbPotentialCollisions = 0;
	for(i=0;i<nbObjects;i++)
	{
		it = overlappingBB[i].begin();
		itEnd = overlappingBB[i].end();		
		for(;it!=itEnd;++it)
		{
			if (!(bodies[i]->isDynamic==false && bodies[*it]->isDynamic==false))
			{
				nbPotentialCollisions++;
				interactions.push_back(shared_ptr<Interaction>(new Contact(i,*it)));
			}
		}
	} 
	  	
	return nbPotentialCollisions;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SAPCollider::updateIds(unsigned int nbElements)
{

	// the first time broadPhaseCollisionTest is called nbObject=0
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
			xBounds[2*i]->id	= i;
			xBounds[2*i]->lower	= 1;
			
			xBounds[2*i+1]->id	= i;
			xBounds[2*i+1]->lower	= 0;
		
			yBounds[2*i]->id	= i;
			yBounds[2*i]->lower	= 1;
		
			yBounds[2*i+1]->id	= i;
			yBounds[2*i+1]->lower	= 0;

			zBounds[2*i]->id	= i;
			zBounds[2*i]->lower	= 1;
		
			zBounds[2*i+1]->id	= i;
			zBounds[2*i+1]->lower	= 0;
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
		for(unsigned int j=0;j<nbElements;j++)
			overlappingBB[j].clear(); //attention memoire
		
		findOverlappingBB(xBounds, nbElements);
		findOverlappingBB(yBounds, nbElements); 
		findOverlappingBB(zBounds, nbElements); 
	
	}
	else
		updateBounds(nbElements);	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SAPCollider::sortBounds(std::vector<AABBBound*>& bounds, int nbElements)
{
	int i,j;
	AABBBound * tmp;

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

void SAPCollider::updateOverlapingBBSet(int id1,int id2)
{
	
	if (id1>id2) // beacause (i,j)==(j,i)
	{
		int tmp=id2;
		id2 = id1;
		id1=tmp;
	}
	
	// look if the paiur (id1,id2) already exists in the overleppingBB collection
	std::set< unsigned int>::iterator it = overlappingBB[id1].find(id2);
	
	// test if the AABBs of the spheres number "id1" and "id2" are overlapping
	float * min1 = minimums + 3*id1;
	float * max1 = maximums + 3*id1;
	float * min2 = minimums + 3*id2;
	float * max2 = maximums + 3*id2;

	bool overlapp = !(max1[0]<min2[0] || max2[0]<min1[0] || max1[1]<min2[1] || max2[1]<min1[1] || max1[2]<min2[2] || max2[2]<min1[2]);
	
	// inserts the pair p=(id1,id2) if the two AABB overlapps and if p does not exists in the overlappingBB
	if (overlapp && it == overlappingBB[id1].end())
		overlappingBB[id1].insert(id2);
	// removes the pair p=(id1,id2) if the two AABB do not overlapp any more and if p already exists in the overlappingBB
	else if (!overlapp && it != overlappingBB[id1].end()) 
		overlappingBB[id1].erase(it);
		
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SAPCollider::updateBounds(int nbElements)
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

void SAPCollider::findOverlappingBB(std::vector<AABBBound*>& bounds, int nbElements)
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
