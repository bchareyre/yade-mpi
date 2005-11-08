/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BodyContainer.hpp"
#include "Body.hpp"


BodyContainer::BodyContainer()
{ 
	body.clear();
}


BodyContainer::~BodyContainer()
{
}


void BodyContainer::registerAttributes()
{
	REGISTER_ATTRIBUTE(body);
}


void BodyContainer::preProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		body.clear();
	}
	else
	{
		body.clear();
		BodyContainer::iterator i    = this->begin();
		BodyContainer::iterator iEnd = this->end();
		for( ; i!=iEnd ; ++i )
			body.push_back(*i);
	}
}


void BodyContainer::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		this->clear();
		vector<shared_ptr<Body> >::iterator it    = body.begin();
		vector<shared_ptr<Body> >::iterator itEnd = body.end();
		for( ; it != itEnd ; ++it)
			this->insert(*it);
		body.clear();
	}
	else
	{
		body.clear();
	}
}


void BodyContainer::setId(shared_ptr<Body>& b, unsigned int newId)
{
	b->id = newId;
}

