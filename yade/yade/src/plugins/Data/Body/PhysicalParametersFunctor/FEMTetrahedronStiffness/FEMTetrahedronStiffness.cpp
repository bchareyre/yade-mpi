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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "FEMSetParameters.hpp"
#include "FEMTetrahedronStiffness.hpp"
#include "FEMTetrahedronData.hpp"

// FIXME - uh, libnames FEMTetrahedronData and FEMTetrahedronStiffness are maybe wrong! it will be fixed, when whole tetrahedron-as-interaction mess will be cleaned up

void FEMTetrahedronStiffness::go(	  const shared_ptr<PhysicalParameters>& par
					, Body* body)

{
	MetaBody* rootBody = dynamic_cast<MetaBody*>(body);
	shared_ptr<FEMSetParameters> physics = dynamic_pointer_cast<FEMSetParameters>(par);
	nodeGroupMask = physics->nodeGroupMask;
	tetrahedronGroupMask = physics->tetrahedronGroupMask;
	
	for( rootBody->bodies->gotoFirst() ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )  
	// FIXME - this loop should be somewhere in InteractionPhysicsDispatcher
	{
		if(rootBody->bodies->getCurrent()->getGroupMask() & tetrahedronGroupMask)
			dynamic_cast<FEMTetrahedronData*>( rootBody->bodies->getCurrent()->physicalParameters.get() )->calcKeMatrix(rootBody);
		// FIXME - that should be done inside InteractionPhysicsFunctor
	}
	
}
