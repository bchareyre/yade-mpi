/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<core/FileGenerator.hpp>


typedef pair<Vector3r, Real> BasicSphere;
//! make a list of spheres non-overlapping sphere

class SimpleShear : public FileGenerator
{
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
		void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius);
		void createActors(shared_ptr<Scene>& scene);
		//method to create a list (containing the positions of centers and radii) of n non interpenetrating spheres, occupying a rectangle with a given (rather high) porosity (issued from TriaxialTest) :
		string GenerateCloud(vector<BasicSphere>& sphere_list,Vector3r lowerCorner,Vector3r upperCorner,long number,Real rad_std_dev, Real porosity);
// 		to create the same list but by reading a text file containing the information :
		std::pair<string,bool> ImportCloud(vector<BasicSphere>& sphere_list,string importFilename);


	public :
		~SimpleShear ();
		bool generate(std::string& message);


	YADE_CLASS_BASE_DOC_ATTRS(SimpleShear,FileGenerator,"Preprocessor for a simple shear box model. The packing initially conforms a gas-like, very loose, state (see utils.makeCloud function), but importing some existing packing from a text file can be also performed after little change in the source code. In its current state, the preprocessor carries out an oedometric compression, until a value of normal stress equal to 2 MPa (and a stable mechanical state). Others Engines such as :yref:`KinemCNDEngine`, :yref:`KinemCNSEngine` and :yref:`KinemCNLEngine`, could be used to apply resp. constant normal displacement, constant normal rigidity and constant normal stress paths using such a simple shear box.",
/*				  ((string,filename,"../porosite0_44.txt",,"file with the list of spheres centers and radii"))*/
				  ((Real,thickness,0.001,,"thickness of the boxes constituting the shear box [$m$]"))
				  ((Real,length,0.1,,"initial length (along x-axis) of the shear box [$m$]"))
				  ((Real,height,0.02,,"initial height (along y-axis) of the shear box [$m$]"))
				  ((Real,width,0.04,,"initial width (along z-axis) of the shear box [$m$]"))
				  ((Real,density,2600,,"density of the spheres [$kg/m^3$]"))
				  ((Real,matYoungModulus,4.0e9,,"value of :yref:`FrictMat.young` for the bodies [$Pa$]"))
				  ((Real,matPoissonRatio,0.04,,"value of :yref:`FrictMat.poisson` for the bodies [-]"))
				  ((Real,matFrictionDeg,37,,"value of :yref:`FrictMat.frictionAngle` within the packing and for the two horizontal boundaries (friction is zero along other boundaries) [$^\\circ$] (the necessary conversion in [$rad$] is done automatically)"))
				  ((bool,gravApplied,false,,"depending on this, :yref:`GravityEngine` is added or not to the scene to take into account the weight of particles"))
				  ((Vector3r,gravity,Vector3r(0,-9.81,0),,"vector corresponding to used gravity (if :yref:`gravApplied<SimpleShear.gravApplied>`) [$m/s^2$]"))
				  ((int,timeStepUpdateInterval,50,,"value of :yref:`TimeStepper::timeStepUpdateInterval` for the :yref:`TimeStepper` used here"))
				  );
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(SimpleShear);



