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

#ifndef __SDECIMPORT_H__
#define __SDECIMPORT_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/FileGenerator.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class ForceRecorder;
class AveragePositionRecorder;
class VelocityRecorder;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class SDECImpactTest : public FileGenerator
{
	private	: Vector3r gravity;
	private	: Vector3r lowerCorner;
	private	: Vector3r upperCorner;
	private	: Real thickness;
	private	: string importFilename;
	private : Real sphereYoungModulus,spherePoissonRatio,sphereFrictionDeg;
	private : Real boxYoungModulus,boxPoissonRatio,boxFrictionDeg;
	private	: Real density;
	private	: bool wall_top;
	private	: bool wall_bottom;
	private	: bool wall_1;
	private	: bool wall_2;
	private	: bool wall_3;
	private	: bool wall_4;
	private	: bool wall_top_wire;
	private	: bool wall_bottom_wire;
	private	: bool wall_1_wire;
	private	: bool wall_2_wire;
	private	: bool wall_3_wire;
	private	: bool wall_4_wire;
	private	: bool bigBall;
	private	: bool rotationBlocked;
	private	: Vector3f spheresColor;
	private	: bool spheresRandomColor;
	private : bool recordBottomForce;
	private : string forceRecordFile,positionRecordFile,velocityRecordFile;
	private : bool recordAveragePositions;
	private : int recordIntervalIter;
	private : Real dampingForce;
	private : Real dampingMomentum;
	
	private : bool boxWalls;
	
	private	: Real bigBallRadius;
	private	: Real bigBallDensity;
	private	: Real bigBallDropTimeSeconds;
	private : Real bigBallPoissonRatio;
	private : Real bigBallYoungModulus;
	private : Real bigBallFrictDeg;
	private : Real bigBallDropHeight;
	private : int timeStepUpdateInterval;

	private : shared_ptr<ForceRecorder> forcerec;
	private : shared_ptr<VelocityRecorder> velocityRecorder;
	private : shared_ptr<AveragePositionRecorder> averagePositionRecorder;
	
	// construction
	public : SDECImpactTest ();
	public : ~SDECImpactTest ();
	
	private : void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
	private : void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius,bool big,bool dynamic);
	private : void createActors(shared_ptr<MetaBody>& rootBody);
	private : void positionRootBody(shared_ptr<MetaBody>& rootBody);

	public : void registerAttributes();
	public : string generate();

	REGISTER_CLASS_NAME(SDECImpactTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(SDECImpactTest,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SDECIMPORT_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
