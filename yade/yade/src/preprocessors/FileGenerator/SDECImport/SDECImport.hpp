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
	private	: Real ks_Box;
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
	
	// construction
	public : SDECImport ();
	public : ~SDECImport ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(SDECImport);
};

REGISTER_SERIALIZABLE(SDECImport,false);

#endif // __SDECIMPORT_H__
