#ifndef __SDECIMPORT_H__
#define __SDECIMPORT_H__

#include "FileGenerator.hpp"
#include "Vector3.hpp"

class ForceRecorder;
class AveragePositionRecorder;
class VelocityRecorder;

class SDECImpactTest : public FileGenerator
{
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
	private : void createSphere(shared_ptr<Body>& body, Vector3r translation, Real radius,bool big,bool dynamic);
	private : void createActors(shared_ptr<ComplexBody>& rootBody);
	private : void positionRootBody(shared_ptr<ComplexBody>& rootBody);

	public : void registerAttributes();
	public : string generate();

	REGISTER_CLASS_NAME(SDECImpactTest);
};

REGISTER_SERIALIZABLE(SDECImpactTest,false);

#endif // __SDECIMPORT_H__
