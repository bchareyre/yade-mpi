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
	else
		bvFactoriesManager.buildBoundingVolume(b->cm,b->se3,b->bv);
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
	else
		bvFactoriesManager.buildBoundingVolume(b->cm,b->se3,b->bv);
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
		for(unsigned int i=0;i<bvFactories.size();i++)
			bvFactoriesManager.addPair(bvFactories[i][0],bvFactories[i][1],bvFactories[i][2]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void BoundingVolumeUpdator::registerAttributes()
{
	REGISTER_ATTRIBUTE(bvFactories);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void BoundingVolumeUpdator::addBVFactories(const string& str1,const string& str2,const string& str3)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	v.push_back(str3);
	bvFactories.push_back(v);

}
