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

#include "SDECLinearContactModel.hpp"
#include "SDECContactGeometry.hpp"
#include "SDECContactPhysics.hpp"
#include "Omega.hpp"
#include "NonConnexBody.hpp"
#include "SDECDiscreteElement.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDECLinearContactModel::SDECLinearContactModel() : InteractionPhysicsDispatcher()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDECLinearContactModel::~SDECLinearContactModel()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECLinearContactModel::postProcessAttributes(bool)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
void SDECLinearContactModel::registerAttributes()
{
	InteractionPhysicsDispatcher::registerAttributes();
}
	
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//FIXME : not easy for a user to write
void SDECLinearContactModel::computeMechanicalParameters(Body*body)
{
	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;
	shared_ptr<Interaction> contact;
	
	for( ncb->interactions->gotoFirst() ; ncb->interactions->notAtEnd() ; ncb->interactions->gotoNext() )
	{
		contact = ncb->interactions->getCurrent();

		int id1 = contact->getId1();
		int id2 = contact->getId2();

		shared_ptr<SDECDiscreteElement> de1 	= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id1]);
		shared_ptr<SDECDiscreteElement> de2 	= dynamic_pointer_cast<SDECDiscreteElement>((*bodies)[id2]);
		shared_ptr<SDECContactGeometry> currentContactGeometry = dynamic_pointer_cast<SDECContactGeometry>(contact->interactionGeometry);
		shared_ptr<SDECContactPhysics> currentContactPhysics;
		
		if ( contact->isNew)
		{
			contact->interactionPhysics = shared_ptr<SDECContactPhysics>(new SDECContactPhysics());
			currentContactPhysics = dynamic_pointer_cast<SDECContactPhysics>(contact->interactionPhysics);
			
			currentContactPhysics->initialKn			= 2*(de1->kn*de2->kn)/(de1->kn+de2->kn);
			currentContactPhysics->initialKs			= 2*(de1->ks*de2->ks)/(de1->ks+de2->ks);
			currentContactPhysics->prevNormal 			= currentContactGeometry->normal;
			currentContactPhysics->initialEquilibriumDistance	= currentContactGeometry->radius1+currentContactGeometry->radius2;
		}
		else
			currentContactPhysics = dynamic_pointer_cast<SDECContactPhysics>(contact->interactionPhysics);
		
		currentContactPhysics->kn = currentContactPhysics->initialKn;
		currentContactPhysics->ks = currentContactPhysics->initialKs;
		currentContactPhysics->equilibriumDistance = currentContactPhysics->initialEquilibriumDistance;
	}
};



