/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SDECIMPORT_HPP
#define SDECIMPORT_HPP

#include <yade/yade-core/FileGenerator.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>

class ForceRecorder;
class AveragePositionRecorder;
class VelocityRecorder;
class ResultantForceEngine;
class TriaxialStressController;

class IsotropicCompressionTest : public FileGenerator
{
	private	:
		Vector3r	 gravity
				,lowerCorner
				,upperCorner;

		Vector3f	 spheresColor;

		Real		 thickness
				,sphereYoungModulus
				,spherePoissonRatio
				,sphereFrictionDeg
				,boxYoungModulus
				,boxPoissonRatio
				,boxFrictionDeg
				,density
				,dampingForce
				,dampingMomentum

				,bigBallRadius
				,bigBallDensity
				,bigBallDropTimeSeconds
				,bigBallPoissonRatio
				,bigBallYoungModulus
				,bigBallFrictDeg
				,bigBallDropHeight
				
				,sigma_iso;

		bool		 wall_top
				,wall_bottom
				,wall_1
				,wall_2
				,wall_3
				,wall_4
				,wall_top_wire
				,wall_bottom_wire
				,wall_1_wire
				,wall_2_wire
				,wall_3_wire
				,wall_4_wire
				,bigBall
				,rotationBlocked
				,spheresRandomColor
				,recordBottomForce
				,recordAveragePositions
				,boxWalls;

		int		 recordIntervalIter
				,timeStepUpdateInterval
				,numberOfGrains;
				/*,wall_top_id
				,wall_bottom_id
				,wall_left_id
				,all_right_id
				,wall_front_id
				,wall_back_id;*/
		
		string		 forceRecordFile
				,positionRecordFile
				,velocityRecordFile
				,importFilename;
	
		shared_ptr<ForceRecorder> forcerec;
		shared_ptr<VelocityRecorder> velocityRecorder;
		shared_ptr<AveragePositionRecorder> averagePositionRecorder;
		//shared_ptr<ResultantForceEngine> resultantforceEngine;
		shared_ptr<TriaxialStressController> triaxialstressController;
			
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
		void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius,bool big,bool dynamic);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);

		typedef pair<Vector3r, Real> BasicSphere;
//! make a list of spheres non-overlapping sphere
		string GenerateCloud(vector<BasicSphere>& sphere_list, Vector3r lowerCorner, Vector3r upperCorner, long number, Real rad_std_dev, Real porosity);
	
	public : 
		IsotropicCompressionTest ();
		~IsotropicCompressionTest ();
		string generate();
	
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(IsotropicCompressionTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(IsotropicCompressionTest,false);

#endif // SDECIMPORT_HPP

