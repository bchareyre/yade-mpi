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

#ifndef __TETRAHEDRONSTEST_HPP__
#define __TETRAHEDRONSTEST_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/FileGenerator.hpp>
#include <yade/yade-common/Tetrahedron.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class TetrahedronsTest : public FileGenerator
{
	private : Vector3r nbTetrahedrons;
	private : Real minRadius,density;
	private : Vector3r groundSize,gravity;
	private : Real maxRadius;
	private : Real dampingForce;
	private	: Real disorder;
	private : Real dampingMomentum;
	private : int timeStepUpdateInterval;
	private : bool rotationBlocked;
	private : Real sphereYoungModulus,spherePoissonRatio,sphereFrictionDeg;
	// construction
	public : TetrahedronsTest ();
	public : ~TetrahedronsTest ();


	private : void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
	private : void createTetrahedron(shared_ptr<Body>& body, int i, int j, int k);
	private : void createActors(shared_ptr<MetaBody>& rootBody);
	private : void positionRootBody(shared_ptr<MetaBody>& rootBody);
	
	private : void loadTRI(shared_ptr<Tetrahedron>& tet, const string& fileName);

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(TetrahedronsTest);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(TetrahedronsTest,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __TETRAHEDRONSTEST_HPP__ 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
