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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
 
#include "BoundingVolumeUpdator.hpp"
#include "NonConnexBody.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

//FIXME : should this class be abstract interface
BoundingVolumeUpdator::BoundingVolumeUpdator () : Actor()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

BoundingVolumeUpdator::~BoundingVolumeUpdator ()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void BoundingVolumeUpdator::updateBoundingVolume(shared_ptr<Body> b)
{
	if (b->containSubBodies)
	{
		shared_ptr<NonConnexBody>  ncb = dynamic_pointer_cast<NonConnexBody>(b);
		shared_ptr<BodyContainer> bodies = ncb->bodies;
		for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext())
		{
			updateBoundingVolume(bodies->getCurrent());
		}
	}
	
	//b->updateBoundingVolume(Se3());
		
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void BoundingVolumeUpdator::updateBoundingVolume(Body* b)
{
	if (b->containSubBodies)
	{
		NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(b);
		shared_ptr<BodyContainer> bodies = ncb->bodies;
		for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext())
		{
			updateBoundingVolume(bodies->getCurrent());
		}
	}
		
	//shared_ptr<BoundingVolumeFactoryFunctor> bvf = bvUpdatorManager.update(b->cm,b->bv);
	//callBacks[b->cm->getClassIndex()][b->bv->getClassIndex()];
		
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void BoundingVolumeUpdator::action(Body* b)
{
	updateBoundingVolume(b);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void BoundingVolumeUpdator::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		//for(unsigned int i=0;i<bvUpdatorFunctors.size();i++)
		//	bvUpdatorManager.addPair(bvUpdatorFunctors[i][0],bvUpdatorFunctors[i][1],bvUpdatorFunctors[i][2]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void BoundingVolumeUpdator::registerAttributes()
{
	//REGISTER_ATTRIBUTE(bvUpdatorFunctors);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
