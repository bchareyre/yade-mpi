/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
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

class SDECImpactTest : public FileGenerator
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
				,bigBallDropHeight;

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
				,timeStepUpdateInterval;
		
		string		 forceRecordFile
				,positionRecordFile
				,velocityRecordFile
				,importFilename;
	
		shared_ptr<ForceRecorder> forcerec;
		shared_ptr<VelocityRecorder> velocityRecorder;
		shared_ptr<AveragePositionRecorder> averagePositionRecorder;
	
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
		void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius,bool big,bool dynamic);
		void createActors(shared_ptr<World>& rootBody);
		void positionRootBody(shared_ptr<World>& rootBody);
	
	public : 
		SDECImpactTest ();
		~SDECImpactTest ();
		bool generate();
	
	protected :
	REGISTER_ATTRIBUTES(FileGenerator,/*(lowerCorner)(upperCorner)(thickness)*/(importFilename)(boxWalls)(sphereYoungModulus)(spherePoissonRatio)(sphereFrictionDeg)(boxYoungModulus)(boxPoissonRatio)(boxFrictionDeg)(density)(dampingForce)(dampingMomentum)(rotationBlocked)(timeStepUpdateInterval)/*(wall_top)(wall_bottom)(wall_1)(wall_2)(wall_3)(wall_4)(wall_top_wire)(wall_bottom_wire)(wall_1_wire)(wall_2_wire)(wall_3_wire)(wall_4_wire)(spheresColor)(spheresRandomColor)(recordBottomForce)(recordAveragePositions)(gravity)*/(forceRecordFile)(positionRecordFile)(velocityRecordFile)(recordIntervalIter)(bigBall)(bigBallRadius)(bigBallDensity)(bigBallDropTimeSeconds)(bigBallFrictDeg)(bigBallYoungModulus)(bigBallPoissonRatio)(bigBallDropHeight));
	REGISTER_CLASS_NAME(SDECImpactTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(SDECImpactTest);


