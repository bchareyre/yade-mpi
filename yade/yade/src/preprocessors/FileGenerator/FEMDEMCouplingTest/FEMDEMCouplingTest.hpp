/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#ifndef FEM_DEM_COUPLING_TEST_HPP
#define FEM_DEM_COUPLING_TEST_HPP 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "FileGenerator.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class FEMDEMCouplingTest : public FileGenerator
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	private :
		std::string 	femTxtFile;
		int 		nodeGroupMask;
		int 		tetrahedronGroupMask;
		int 		demGroupMask;
		
		Vector3r 	regionMin1;
		Vector3r 	regionMax1;
		Real 		radiusFEMDEM1;
	
		Vector3r 	regionMin2;
		Vector3r 	regionMax2;
		Real 		radiusFEMDEM2;
		
		Vector3r 	gravity;
		Vector3r 	groundSize;
		Vector3r 	groundPosition;
		
		Real 		dampingForce;
		Real 		dampingMomentum;
	
		Vector3r 	spheresOrigin;
		Vector3r 	nbSpheres;
		Real 		radiusDEM;
		Real 		density;
		Real 		supportSize;
		bool 		support1;
		bool 		support2;
		int 		timeStepUpdateInterval;
		
		Real 		sphereYoungModulus
				, spherePoissonRatio
				, sphereFrictionDeg;
				
		bool 		momentRotationLaw;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public : FEMDEMCouplingTest();
	public : virtual ~FEMDEMCouplingTest();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public : string generate();
	private : void createActors(shared_ptr<ComplexBody>& rootBody);
	private : void positionRootBody(shared_ptr<ComplexBody>& rootBody);
	private : void insertGround(shared_ptr<ComplexBody>& rootBody);
	private : void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
	private : void createSphere(shared_ptr<Body>& body, int i, int j, int k);

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public : void registerAttributes();
	REGISTER_CLASS_NAME(FEMDEMCouplingTest);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(FEMDEMCouplingTest,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FEM_DEM_COUPLING_TEST_HPP 

