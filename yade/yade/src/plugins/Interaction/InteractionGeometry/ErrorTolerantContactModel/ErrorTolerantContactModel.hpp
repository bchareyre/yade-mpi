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

#ifndef __ERRORTOLERANTCONTACTMODEL_H__
#define __ERRORTOLERANTCONTACTMODEL_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "GeometryOfInteraction.hpp"
#include "Vector3.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class ErrorTolerantContactModel : public GeometryOfInteraction
{
	public : vector<pair<Vector3r,Vector3r> > closestPoints;
	public : Vector3r o1p1;
	public : Vector3r o2p2;
	public : Vector3r normal;
	//public : Vector3r t;	
	//public : Vector3r s;
	//public : Vector3r l;
	//public : Real nu;
	//public : std::pair<t_Vertex,t_Vertex> idVertex;
	//public : t_ConnexionType type;
	
	// construction
	public : ErrorTolerantContactModel ();
	public : ~ErrorTolerantContactModel ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	REGISTER_CLASS_NAME(ErrorTolerantContactModel);
	//REGISTER_CLASS_INDEX(ErrorTolerantContactModel);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(ErrorTolerantContactModel,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ERRORTOLERANTCONTACTMODEL_H__
