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

#ifndef __ROTATINGBOX_H__
#define __ROTATINGBOX_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/FileGenerator.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class RotatingBox : public FileGenerator
{
	private : Vector3r nbSpheres;
	private : Vector3r nbBoxes;
	private : Vector3r rotationAxis,gravity;
	private	: Real minSize;
	private	: Real maxSize;
	private	: Real disorder;
	private : Real dampingForce;
	private : Real dampingMomentum;
	private : Real densityBox;
	private : Real densitySphere;
	private : bool isRotating;
	private : Real rotationSpeed;
	private : bool middleWireFrame;
	
	// construction
	public : RotatingBox ();
	public : ~RotatingBox ();
	
	private : void createKinematicBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool);
	private : void createSphere(shared_ptr<Body>& body, int i, int j, int k);
	private : void createBox(shared_ptr<Body>& body, int i, int j, int k);
	private : void createActors(shared_ptr<MetaBody>& rootBody);
	private : void positionRootBody(shared_ptr<MetaBody>& rootBody);

	public : virtual void registerAttributes();
	public : virtual string generate();

	REGISTER_CLASS_NAME(RotatingBox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(RotatingBox,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ROTATINGBOX_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

