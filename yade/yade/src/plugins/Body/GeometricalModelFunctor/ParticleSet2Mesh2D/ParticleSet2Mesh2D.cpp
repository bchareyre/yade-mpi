/***************************************************************************
 *   Copyright (C) 2005 by Janek Kozicki                                   *
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

#include "ParticleSet2Mesh2D.hpp"
#include "ParticleSetParameters.hpp"
#include "Mesh2D.hpp"
#include "ComplexBody.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSet2Mesh2D::go(	  const shared_ptr<BodyPhysicalParameters>&
				, shared_ptr<GeometricalModel>& gm
				, const Body* body)
{
	Mesh2D* mesh2d = static_cast<Mesh2D*>(gm.get());
	
	const ComplexBody * ncb = dynamic_cast<const ComplexBody*>(body);
	const shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
 	unsigned int i=0;
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() , ++i )
 		mesh2d->vertices[i]=bodies->getCurrent()->physicalParameters->se3.translation;
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
