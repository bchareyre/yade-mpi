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

#ifndef __LINKEDSPHERES_H__
#define __LINKEDSPHERES_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/FileGenerator.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-core/Body.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class SDECLinkedSpheres : public FileGenerator
{
	private	: Vector3r nbSpheres; 

	private	: Real minRadius,density;
	private	: Real maxRadius;
	private	: Real disorder;
	private : Vector3r gravity;
	private	: Real spacing;
	private	: Real supportSize;
	private	: bool support1;
	private	: bool support2;
	private : Real dampingForce;
	private : Real dampingMomentum;
	private : int timeStepUpdateInterval;
	private : Real sphereYoungModulus,spherePoissonRatio,sphereFrictionDeg;
	private : bool momentRotationLaw;
	
	private : void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
	private : void createSphere(shared_ptr<Body>& body, int i, int j, int k);
	private : void createActors(shared_ptr<MetaBody>& rootBody);
	private : void positionRootBody(shared_ptr<MetaBody>& rootBody);
	
	// construction
	public : SDECLinkedSpheres ();
	public : ~SDECLinkedSpheres ();

	
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(SDECLinkedSpheres);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(SDECLinkedSpheres,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __LINKEDSPHERES_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
