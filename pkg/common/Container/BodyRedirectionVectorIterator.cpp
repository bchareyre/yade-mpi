/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BodyRedirectionVectorIterator.hpp"


BodyRedirectionVectorIterator::BodyRedirectionVectorIterator() : BodyContainerIterator()
{

}


BodyRedirectionVectorIterator::~BodyRedirectionVectorIterator()
{

}


bool BodyRedirectionVectorIterator::isDifferent(const BodyContainerIterator& i)
{
	return (vii != static_cast<const BodyRedirectionVectorIterator&>(i).vii );
}


void BodyRedirectionVectorIterator::increment()
{
	++vii;
}


void BodyRedirectionVectorIterator::affect(const BodyContainerIterator& i)
{
	vii = static_cast<const BodyRedirectionVectorIterator&>(i).vii;
}


shared_ptr<Body> BodyRedirectionVectorIterator::getValue()
{
	return (*vii);
}


shared_ptr<BodyContainerIterator> BodyRedirectionVectorIterator::createPtr()
{
	return shared_ptr<BodyContainerIterator>(new BodyRedirectionVectorIterator());
}


