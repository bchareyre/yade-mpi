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

#ifndef __BOUNDINGVOLUME_H__
#define __BOUNDINGVOLUME_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <GL/glut.h>
#include <GL/gl.h>
#include "Se3.hpp"
#include "Serializable.hpp"
#include "Indexable.hpp"
#include "GeometricalModel.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all bounding volumes.

	All the bounding volumes (BoundingSphere, AABB ...) must derived from this class. A bounding volume is used to speed up the collision detection. Instead of computing if 2 complex polyhedron collide each other, it is much faster to first test if their bounding volume (for example a AABB) are in collision.
*/
class BoundingVolume : public GeometricalModel, public Indexable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Color of the bounding volume. Used only for opengl drawing purpose */
	public : Vector3r color;

	/*! Minimum of the bounding volume */
	public : Vector3r min;

	/*! Maximum of the bounding volume */
	public : Vector3r max;


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////


	/*!  Constructor */
	public : BoundingVolume ();

	/*! Destructor */
	public : virtual ~BoundingVolume ();


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////


	/*! Abstract function to overload. It is called to draw the bounding volume in an opengl context */
	public : virtual void glDraw() {};

	//public : virtual void move(Se3r& ) {};
	/*! Abstract function to overload. It is called to update the current bounding volume by an incremental 3D transformation */
	public : virtual void update(Se3r& ) {};

	public : void registerAttributes();

	REGISTER_CLASS_NAME(BoundingVolume);	
	
	public : virtual int& getClassIndex() { throw;};
	public : virtual const int& getClassIndex() const { throw;};

	REGISTER_INDEX_COUNTER(BoundingVolume);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(BoundingVolume,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BOUNDINGVOLUME_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
