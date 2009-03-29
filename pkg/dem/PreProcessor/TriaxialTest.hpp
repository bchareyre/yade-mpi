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
class GlobalStiffnessTimeStepper;

/*! \brief Isotropic compression + triaxial compression test

	This preprocessor is designed to :
	1/ generate random loose packings and compress them under isotropic confining stress, either squeezing the packing between moving rigid boxes or expanding the particles while boxes are fixed (depending on flag "InternalCompaction").
	2/ simulate all sorts triaxial loading path (there is however a default loading path corresponding to constant lateral stress in 2 directions and constant strain rate on the third direction - this loading path is used when the flag AutoCompressionActivation = true, otherwise the simulation stops after isotropic compression)
	
	
	Essential engines :
	
	1/ The TrixialCompressionEngine is used for controlling the state of the sample and simulating loading paths. TrixialCompressionEngine inherits from TriaxialStressController, which can compute stress- strain-like quantities in the packing and maintain a constant level of stress at each boundary. TriaxialCompressionEngine has few more members in order to impose constant strain rate and control the transition between isotropic compression and triaxial test.
		
	2/ The class TriaxialStateRecorder is used to write to a file the history of stresses and strains.
	
	3/ TriaxialTest is currently using a group of classes including GlobalStiffness (data), GlobalStiffnessCounter (updater) and GlobalStiffnessTimeStepper to compute an appropriate dt for the numerical scheme. The TriaxialTest is the only preprocessor using these classes in Yade because they have been developped AFTER most of preprocessor examples, BUT they can be used in principle in any situation and they have nothing specifically related to the triaxial test.
	
	@note TriaxialStressController::ComputeUnbalancedForce(...) returns a value that can be usefull for evaluating the stability of the packing. It is defined as (mean force on particles)/(mean contact force), so that it tends to 0 in a stable packing. This parameter is checked by TriaxialCompressionEngine to switch from one stage of the simulation to the next one (e.g. stop isotropic confinment and start axial loading)
	
 */

class TriaxialTest : public FileGenerator
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
				,boxYoungModulus
				,boxPoissonRatio
				,boxFrictionDeg
				,density
				,dampingForce
				,dampingMomentum
				,defaultDt
				
				,sigmaIsoCompaction,
				sigmaLateralConfinement,
				strainRate,
				maxWallVelocity,
				StabilityCriterion,
				maxMultiplier, ///max multiplier of diameters during internal compaction
				finalMaxMultiplier,
				wallOversizeFactor, // make walls bigger (/smaller) than necessary by this factor
				radiusStdDev,
				radiusMean,
				fixedPorosity;
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
				//! see docs for TriaxialCompressionEngine and TriaxialCompressionEngine::autoUnload
				,autoUnload
				//! stop the simulation or run it forever (i.e. until the user stops it)
				,autoStopSimulation
			
				,rotationBlocked
				,spheresRandomColor
				,boxWalls
				//! flag for choosing between moving boundaries or increasing particles sizes during the compaction stage.
				,internalCompaction
				,saveAnimationSnapshots
				,biaxial2dTest
				//!flag to choose an isotropic compaction until a fixed porosity choosing a same translation speed for the six walls
				,isotropicCompaction;

		#ifdef BEX_CONTAINER
			//! Generate parallel simulation, if it is supported
			bool parallel;
		#endif

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
				,Key //A code that is added to output filenames
				//! string that contains some subset (max. 2) of {'x','y','z'} ; containes axes will have box dimension hardcoded, even if box is scaled as mean_radius is prescribed: scaling will be applied on the rest.
				,fixedBoxDims;
	
		shared_ptr<TriaxialCompressionEngine> triaxialcompressionEngine;
		shared_ptr<TriaxialStressController> triaxialstressController;
		shared_ptr<TriaxialStateRecorder> triaxialStateRecorder;
		shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper;
			
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
		void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius,bool big,bool dynamic);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);

		typedef pair<Vector3r, Real> BasicSphere;
		//! generate a list of non-overlapping spheres
		string GenerateCloud(vector<BasicSphere>& sphere_list, Vector3r lowerCorner, Vector3r upperCorner, long number, Real rad_std_dev, Real mean_radius, Real porosity);

	
	public : 
		TriaxialTest ();
		~TriaxialTest ();
		bool generate();
	
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(TriaxialTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(TriaxialTest);


