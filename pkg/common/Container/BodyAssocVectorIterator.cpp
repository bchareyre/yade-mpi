/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BodyAssocVectorIterator.hpp"


BodyAssocVectorIterator::BodyAssocVectorIterator() : BodyContainerIterator()
{

}


BodyAssocVectorIterator::~BodyAssocVectorIterator()
{

}


bool BodyAssocVectorIterator::isDifferent(const BodyContainerIterator& i)
{
	return (bii != static_cast<const BodyAssocVectorIterator&>(i).bii );
}


void BodyAssocVectorIterator::increment()
{
	++bii;
}


void BodyAssocVectorIterator::affect(const BodyContainerIterator& i)
{
	bii = static_cast<const BodyAssocVectorIterator&>(i).bii;
}


shared_ptr<Body> BodyAssocVectorIterator::getValue()
{
	return (*bii).second;
}


shared_ptr<BodyContainerIterator> BodyAssocVectorIterator::createPtr()
{
	return shared_ptr<BodyContainerIterator>(new BodyAssocVectorIterator());
}


