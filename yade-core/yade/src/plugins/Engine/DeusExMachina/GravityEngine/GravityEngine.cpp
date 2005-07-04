/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#include "GravityEngine.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-common/ParticleParameters.hpp>
#include <yade/yade-common/Force.hpp>
#include <yade/yade-core/MetaBody.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

GravityEngine::GravityEngine() : gravity(Vector3r::ZERO) , actionParameterForce(new Force)
{
}

GravityEngine::~GravityEngine()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GravityEngine::registerAttributes()
{
	REGISTER_ATTRIBUTE(gravity);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GravityEngine::applyCondition(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
	{
		shared_ptr<Body>& b = bodies->getCurrent();
		ParticleParameters* p = dynamic_cast<ParticleParameters*>(b->physicalParameters.get());
		if(p)
			static_cast<Force*>( ncb->actionParameters->find( b->getId() , actionParameterForce->getClassIndex() ).get() )
				->force += gravity * p->mass;
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

