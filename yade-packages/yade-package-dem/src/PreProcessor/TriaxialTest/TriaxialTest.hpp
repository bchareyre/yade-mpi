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
#include <yade/yade-lib-wm3-math/Vector3.hpp>

class ForceRecorder;
class AveragePositionRecorder;
class VelocityRecorder;
class TriaxialStressController;
class TriaxialCompressionEngine;

/*! \brief Isotropic compression + uniaxial compression test

	detailed description...
 */

class TriaxialTest : public FileGenerator
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
				
				,sigma_iso
				,strainRate;

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
				,wallStiffnessUpdateInterval
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
		shared_ptr<TriaxialCompressionEngine> triaxialcompressionEngine;
		shared_ptr<TriaxialStressController> triaxialstressController;
			
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
		void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius,bool big,bool dynamic);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
	
	public : 
		TriaxialTest ();
		~TriaxialTest ();
		string generate();
	
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(TriaxialTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(TriaxialTest,false);

#endif // SDECIMPORT_HPP

