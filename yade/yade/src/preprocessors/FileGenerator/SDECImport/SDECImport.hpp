#ifndef __SDECIMPORT_H__
#define __SDECIMPORT_H__

#include "FileGenerator.hpp"
#include "Vector3.hpp"

class SDECImport : public FileGenerator
{
	private	: Vector3r lowerCorner;
	private	: Vector3r upperCorner;
	private	: Real thickness;
	private	: string importFilename;
	private	: Real kn_Spheres;
	private	: Real ks_Spheres;
	private	: Real kn_Box;
	private	: Real ks_Box,density;
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
	private	: Vector3f spheresColor;
	private	: bool spheresRandomColor;
	private : bool recordBottomForce;
	private : string forceRecordFile,positionRecordFile;
	private : bool recordAveragePositions;
	private : int recordIntervalIter;
	private : Real dampingForce;
	private : Real dampingMomentum;
	
	private	: Real bigBallRadius;
	private	: Real bigBallDensity;
	private	: Real bigBallDropTimeSeconds;
	
	// construction
	public : SDECImport ();
	public : ~SDECImport ();
	
	private : void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
	private : void createSphere(shared_ptr<Body>& body, Vector3r translation, Real radius);
	private : void createActors(shared_ptr<ComplexBody>& rootBody);
	private : void positionRootBody(shared_ptr<ComplexBody>& rootBody);

	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(SDECImport);
};

REGISTER_SERIALIZABLE(SDECImport,false);

#endif // __SDECIMPORT_H__
