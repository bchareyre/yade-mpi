/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  Copyright (C) 2007 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef COHESIVE_TRIAXIAL_TEST_HPP
#define COHESIVE_TRIAXIAL_TEST_HPP

#include<yade/core/FileGenerator.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class ForceRecorder;
class AveragePositionRecorder;
class VelocityRecorder;
class TriaxialStressController;
class TriaxialCompressionEngine;
class TriaxialStateRecorder;

/*! \brief Isotropic compression + uniaxial compression test

	detailed description...
 */

class CohesiveTriaxialTest : public FileGenerator
{
	private	:
		Vector3r	 gravity
				,lowerCorner
				,upperCorner;

		Vector3r	 spheresColor;

		Real		 thickness
				,sphereYoungModulus
				,spherePoissonRatio
				,sphereFrictionDeg
				,normalCohesion
				,shearCohesion
				,boxYoungModulus
				,boxPoissonRatio
				,boxFrictionDeg
				,density
				,dampingForce
				,dampingMomentum
				,defaultDt
				,radiusDeviation

// 				,bigBallRadius
// 				,bigBallDensity
// 				,bigBallDropTimeSeconds
// 				,bigBallPoissonRatio
// 				,bigBallYoungModulus
// 				,bigBallFrictDeg
// 				,bigBallDropHeight
				
				,sigma_iso
				,strainRate
				,StabilityCriterion
				,maxMultiplier ///max multiplier of diameters during internal compaction
				,finalMaxMultiplier;
		 
		bool		setCohesionOnNewContacts
				,wall_top
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
				,autoCompressionActivation
				,bigBall
				,rotationBlocked
				,spheresRandomColor
				,recordBottomForce
				,recordAveragePositions
				,boxWalls
				,internalCompaction
				,saveAnimationSnapshots;

		int		 recordIntervalIter
				,timeStepUpdateInterval
				,timeStepOutputInterval
				,wallStiffnessUpdateInterval
				,radiusControlInterval
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
				,importFilename
				,AnimationSnapshotsBaseName
				,WallStressRecordFile;
	
		shared_ptr<ForceRecorder> forcerec;
		shared_ptr<VelocityRecorder> velocityRecorder;
		shared_ptr<AveragePositionRecorder> averagePositionRecorder;
		shared_ptr<TriaxialCompressionEngine> triaxialcompressionEngine;
		shared_ptr<TriaxialStressController> triaxialstressController;
		shared_ptr<TriaxialStateRecorder> triaxialStateRecorder;
			
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
		void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius,bool dynamic);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
	
	public : 
		CohesiveTriaxialTest ();
		~CohesiveTriaxialTest ();
		bool generate();
	
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(CohesiveTriaxialTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(CohesiveTriaxialTest,false);

#endif // COHESIVE_TRIAXIAL_TEST_HPP

