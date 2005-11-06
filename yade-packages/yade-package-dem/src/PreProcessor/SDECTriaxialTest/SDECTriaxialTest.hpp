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

#ifndef __SDECTRIAXIALTEST_HPP__
#define __SDECTRIAXIALTEST_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/FileGenerator.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class SDECTriaxialTest : public FileGenerator
{
	private	: Vector3r lowerCorner;
	private	: Vector3r upperCorner;
	
	private : Real spacing, disorder, minRadius, maxRadius;
	private : Vector3r nbSpheres;
	private : Real boxYoungModulus,boxPoissonRatio,boxFrictionDeg;
	private : Real sphereYoungModulus,spherePoissonRatio,sphereFrictionDeg,density;
	
	private : Real doesNothingYet_TM;


	public : SDECTriaxialTest();
	public : virtual ~SDECTriaxialTest();
	
	public : string generate();

	private : void createSphere(shared_ptr<Body>& body, int i, int j, int k);
	private : void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
	private : void createActors(shared_ptr<MetaBody>& rootBody);
	private : void positionRootBody(shared_ptr<MetaBody>& rootBody);
	
	public : void registerAttributes();
	REGISTER_CLASS_NAME(SDECTriaxialTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);


};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(SDECTriaxialTest,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SDECTRIAXIALTEST_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

