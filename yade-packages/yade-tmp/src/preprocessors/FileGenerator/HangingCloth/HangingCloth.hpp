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

#ifndef __HANGINGCLOTH_H__
#define __HANGINGCLOTH_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/FileGenerator.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class HangingCloth : public FileGenerator
{
	private : int width;
	private : int height;
	private : Real springStiffness ;
	private : Real springDamping ;	
	private : Real particleDamping ;
	private : Real clothMass;
	private : int cellSize;
	private : shared_ptr<Interaction>  spring;
	private : bool fixPoint1;
	private : bool fixPoint2;
	private : bool fixPoint3;
	private : bool fixPoint4;
	private : bool ground;
	private : Real dampingForce;
	private : Real sphereYoungModulus,spherePoissonRatio,sphereFrictionDeg;
// spheres
	private : Vector3r nbSpheres,gravity;
	private : Real density;
	private : Real minRadius;
	private : Real maxRadius;
	private : Real disorder;
	private : Real spacing;
	private : Real dampingMomentum;
	private : bool linkSpheres;

	// construction
	public : HangingCloth ();
	public : ~HangingCloth ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	
	private : shared_ptr<Interaction>& createSpring(const shared_ptr<MetaBody>& rootBody,int i,int j);
	private : void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
	private : void createSphere(shared_ptr<Body>& body, int i, int j, int k);
	
	public : string generate();

	REGISTER_CLASS_NAME(HangingCloth);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(HangingCloth,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __HANGINGCLOTH_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
