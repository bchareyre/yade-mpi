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

#ifndef __SDECPERMANENTLINKPHYSICS_HPP__
#define __SDECPERMANENTLINKPHYSICS_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhysicsOfInteraction.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class SDECPermanentLinkPhysics : public PhysicsOfInteraction
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes										      ///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public : Real kn;				// normal elastic constant.
	public : Real ks;				// shear elastic constant.

	public : Real knMax;
	public : Real ksMax;

	public : Real initialKn;			// initial normal elastic constant.
	public : Real initialKs;			// initial shear elastic constant.
	public : Real equilibriumDistance;		// equilibrium distance
	public : Real initialEquilibriumDistance;	// initial equilibrium distance
	public : Vector3r prevNormal;			// unit normal of the contact plane.
	public : Vector3r normalForce;			// normal force applied on a DE
	public : Vector3r shearForce;			// shear force applied on a DE

/////////////////////////////////////////////////////// FIXME : this goes to another dynlib - MDEM
	//public : Vector3r initRotation1;
	//public : Vector3r initRotation2;

	public : Quaternionr prevRotation1;
	//public : Quaternionr currentRotation1;
	public : Quaternionr prevRotation2;
	//public : Quaternionr currentRotation2;
	
	public : Vector3r thetar;
	public : Real heta;
	public : Real averageRadius;
	public : Real kr;
////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor								      ///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor */
	public : SDECPermanentLinkPhysics();

	/*! Destructor */
	public : virtual ~SDECPermanentLinkPhysics();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods										      ///
///////////////////////////////////////////////////////////////////////////////////////////////////

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	REGISTER_CLASS_NAME(SDECPermanentLinkPhysics);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(SDECPermanentLinkPhysics,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SDECPERMANENTLINKPHYSICS_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

