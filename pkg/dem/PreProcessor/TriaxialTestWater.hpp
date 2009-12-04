/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr                                            *
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/FileGenerator.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class ForceRecorder;
class AveragePositionRecorder;
class VelocityRecorder;
class TriaxialStressController;
class TriaxialCompressionEngine;
class TriaxialStateRecorder;
class CapillaryStressRecorder;
class ContactStressRecorder;

/*! \brief Isotropic compression + triaxial compression test

	This preprocessor is designed to :
	1/ generate random loose packings and compress them under isotropic confining stress, either squeezing the packing between moving rigid boxes or expanding the particles while boxes are fixed (depending on flag "InternalCompaction").
	2/ simulate all sorts triaxial loading path (there is however a default loading path corresponding to constant lateral stress in 2 directions and constant strain rate on the third direction - this loading path is used when the flag AutoCompressionActivation = true, otherwise the simulation stops after isotropic compression)
	
	
	Essential engines :
	
	1/ The TrixialCompressionEngine is used for controlling the state of the sample and simulating loading paths. TrixialCompressionEngine inherits from TriaxialStressController, which can compute stress- strain-like quantities in the packing and maintain a constant level of stress at each boundary. TriaxialCompressionEngine has few more members in order to impose constant strain rate and control the transition between isotropic compression and triaxial test.
		
	2/ The class TriaxialStateRecorder is used to write to a file the history of stresses and strains.
	
	3/ TriaxialTestWater is currently using GlobalStiffnessTimeStepper to compute an appropriate dt for the numerical scheme. The TriaxialTestWater is the only preprocessor using these classes in Yade because they have been developped AFTER most of preprocessor examples, BUT they can be used in principle in any situation and they have nothing specifically related to the triaxial test.
	
	@note TriaxialStressController::ComputeUnbalancedForce(...) returns a value that can be usefull for evaluating the stability of the packing. It is defined as (mean force on particles)/(mean contact force), so that it tends to 0 in a stable packing. This parameter is checked by TriaxialCompressionEngine to switch from one stage of the simulation to the next one (e.g. stop isotropic confinment and start axial loading)
	
 */

class TriaxialTestWater : public FileGenerator
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
				//! If a different value of friction is to be used during the compaction phase
				,compactionFrictionDeg
				,Rdispersion
				,boxYoungModulus
				,boxPoissonRatio
				,boxFrictionDeg
				,density
				,dampingForce
				,dampingMomentum
				,defaultDt
				
				,sigmaIsoCompaction
				,CapillaryPressure
				,sigmaLateralConfinement
				,strainRate
				,maxWallVelocity
				,StabilityCriterion
				,maxMultiplier ///max multiplier of diameters during internal compaction
				,finalMaxMultiplier
				,wallOversizeFactor; // make walls bigger (/smaller) than necessary by this factor

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
				//! do we just want to generate a stable packing under isotropic pressure (false) or do we want the triaxial loading to start automatically right after compaction stage (true)?
				,autoCompressionActivation
			
				,rotationBlocked
				,spheresRandomColor
				,boxWalls
				//! flag for choosing between moving boundaries or increasing particles sizes during the compaction stage.
				,internalCompaction
				,water
				,fusionDetection
				,binaryFusion
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
		
		string		importFilename
				,AnimationSnapshotsBaseName
				,WallStressRecordFile
				,capillaryStressRecordFile
				,contactStressRecordFile
				,Key;//A code that is added to output filenames
	
		shared_ptr<TriaxialCompressionEngine> triaxialcompressionEngine;
		shared_ptr<TriaxialStressController> triaxialstressController;
		shared_ptr<TriaxialStateRecorder> triaxialStateRecorder;
		shared_ptr<CapillaryStressRecorder> capillaryStressRecorder;
		shared_ptr<ContactStressRecorder> contactStressRecorder;
			
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
		void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius,bool big,bool dynamic);
		void createActors(shared_ptr<Scene>& rootBody);
		void positionRootBody(shared_ptr<Scene>& rootBody);
	
	public : 
		TriaxialTestWater ();
		~TriaxialTestWater ();
		bool generate();
	
	REGISTER_ATTRIBUTES(FileGenerator,
		(lowerCorner)
		(upperCorner)
		(thickness)
		(importFilename)
		(outputFileName)
		//(nlayers)
		//(boxWalls)
		(internalCompaction)
		(maxMultiplier)
		(finalMaxMultiplier)

		(sphereYoungModulus)
		(spherePoissonRatio)
		(sphereFrictionDeg)
		(compactionFrictionDeg)
		(boxYoungModulus)
		(boxPoissonRatio)
		(boxFrictionDeg)

		(density)
		(defaultDt)
		(dampingForce)
		(dampingMomentum)
		(rotationBlocked)
		(timeStepUpdateInterval)
		(timeStepOutputInterval)
		(wallStiffnessUpdateInterval)
		(radiusControlInterval)
		(numberOfGrains)
		(Rdispersion)
		(strainRate)
		(maxWallVelocity)
		(StabilityCriterion)
		(autoCompressionActivation)
	//	(wall_top)
	//	(wall_bottom)
	//	(wall_1)
	//	(wall_2)
	//	(wall_3)
	//	(wall_4)
	//	(wall_top_wire)
	//	(wall_bottom_wire)
	//	(wall_1_wire)
	//	(wall_2_wire)
	//	(wall_3_wire)
	//	(wall_4_wire)
	//	(spheresColor)
	//	(spheresRandomColor)
		(recordIntervalIter)
		(saveAnimationSnapshots)
		(AnimationSnapshotsBaseName)
		(WallStressRecordFile)
	// 	(capillaryStressRecordFile)
		(contactStressRecordFile)

		(wallOversizeFactor)

	//	(gravity)
		
		//(bigBall)
		//(bigBallRadius)
		//(bigBallDensity)
		//(bigBallDropTimeSeconds)
		//(bigBallFrictDeg)
		//(bigBallYoungModulus)
		//(bigBallPoissonRatio)
		//(bigBallDropHeight)
		//(sigma_iso)
		(sigmaIsoCompaction)
		(sigmaLateralConfinement)
		(Key)
		
		(water)
		(CapillaryPressure)
		(fusionDetection)
		(binaryFusion)
	);
	REGISTER_CLASS_NAME(TriaxialTestWater);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(TriaxialTestWater);


