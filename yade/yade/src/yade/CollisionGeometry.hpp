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

#ifndef __COLLISIONMODEL_H__
#define __COLLISIONMODEL_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "GeometricalModel.hpp"
#include "Indexable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class CollisionGeometry : public GeometricalModel, public Indexable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	// construction
	public : CollisionGeometry ();

	public : virtual ~CollisionGeometry ();

	//public : virtual bool collideWith(CollisionGeometry* collisionModel) = NULL;
	public : void registerAttributes();
	public : void processAttributes();

	// FIXME : why to put again getClassIndex
	public : virtual int& getClassIndex() { throw;};
	public : virtual const int& getClassIndex() const { throw;};

	REGISTER_CLASS_NAME(CollisionGeometry);
	REGISTER_INDEX_COUNTER(CollisionGeometry);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(CollisionGeometry,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __COLLISIONMODEL_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
