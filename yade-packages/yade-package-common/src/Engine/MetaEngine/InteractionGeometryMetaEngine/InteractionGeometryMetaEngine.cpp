/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#include "InteractionGeometryMetaEngine.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/MetaBody.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// void InteractionGeometryMetaEngine::postProcessAttributes(bool deserializing)
// {
// 	postProcessDispatcher2D(deserializing);
// }
// 
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// void InteractionGeometryMetaEngine::registerAttributes()
// {
// 	REGISTER_ATTRIBUTE(functorNames);
// 	REGISTER_ATTRIBUTE(functorArguments);
// }

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionGeometryMetaEngine::action(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	shared_ptr<InteractionContainer>& persistentInteractions = ncb->persistentInteractions;
	InteractionContainer::iterator ii    = persistentInteractions->begin();
	InteractionContainer::iterator iiEnd = persistentInteractions->end(); 
	for( ; ii!=iiEnd ; ++ii)
	{
		const shared_ptr<Interaction> interaction = *ii;
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		interaction->isReal = true;
		operator()( b1->interactionGeometry , b2->interactionGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	}
	
	shared_ptr<InteractionContainer>& volatileInteractions = ncb->volatileInteractions;
	ii    = volatileInteractions->begin();
	iiEnd = volatileInteractions->end(); 
	for(  ; ii!=iiEnd ; ++ii)
	{
		const shared_ptr<Interaction> interaction = *ii;
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		
		interaction->isReal =

		// FIXME put this inside VolatileInteractionCriterion dynlib
			( persistentInteractions->find(interaction->getId1(),interaction->getId2()) == 0 )
		 	&&
			operator()( b1->interactionGeometry , b2->interactionGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
			
	}
}
