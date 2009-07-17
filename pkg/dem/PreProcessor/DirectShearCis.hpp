/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

/*! \brief A shear test		DEPRECATED, replaced by SimpleShear

This preprocessor allows to simulate a shear test of a sample contained in a deformable parallelogram box
The movement of the box is controlled by the Engine "CinemCisEngine" (in pkg/common/Engine/DeusExMachina/)
The sample could be generated via the same method used in TriaxialTest Preprocesor (=> see GenerateCloud) or by reading a text file containing positions and radii of a sample (=> see ImportCloud). This last one is the one by default used by this PreProcessor as it is written here => you need to have such a file.
 */


#include<yade/core/FileGenerator.hpp>


typedef pair<Vector3r, Real> BasicSphere;
//! make a list of spheres non-overlapping sphere

using namespace std;


class DirectShearCis : public FileGenerator
{
	private :
		Vector3r	 gravity;

// 		long		nBilles;
		Real		/*porosite,*/ 
				thickness
				,width
				,height
				,profondeur;
		Real		 density
				,sphereYoungModulus
				,spherePoissonRatio
				,sphereFrictionDeg
				,boxYoungModulus
				,boxPoissonRatio
				;
		Real		 displacement
				,shearSpeed;
		Real		 dampingForce
				,dampingMomentum;

		int		 recordIntervalIter
				,timeStepUpdateInterval;

		string		filename;

		bool		gravApplied
				,shearApplied;
		
					
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
		void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
		//method to create a list (containing the positions of centers and radii) of n non interpenetrating spheres, occupying a rectangle with a given (rather high) porosity (issued from TriaxialTest) :
		string GenerateCloud(vector<BasicSphere>& sphere_list,Vector3r lowerCorner,Vector3r upperCorner,long number,Real rad_std_dev, Real porosity);
// 		to create the same list but by reading a text file containing the informations :
		std::pair<string,bool> ImportCloud(vector<BasicSphere>& sphere_list,string importFilename);


	public :
		DirectShearCis ();
		~DirectShearCis ();
		bool generate();

	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(FileGenerator,(filename)(gravity)/*(nBilles)(porosite)*/(thickness)(width)(height)(profondeur)(density)(sphereYoungModulus)(spherePoissonRatio)(sphereFrictionDeg)(boxYoungModulus)(boxPoissonRatio)(shearSpeed)(gravApplied)(shearApplied)(dampingForce)(dampingMomentum)(timeStepUpdateInterval));
	REGISTER_CLASS_NAME(DirectShearCis);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(DirectShearCis);



