#ifndef FEM_BEAM_HPP
#define FEM_BEAM_HPP 

#include "FileGenerator.hpp"

#include <string>

class FEMBeam : public FileGenerator
{
	private :
//		Vector3r gravity;
		std::string femTxtFile;
		int nodeGroupMask;
		int tetrahedronGroupMask;
//	private : Real dampingForce;
//	private : Real dampingMomentum;

// construction
	public : FEMBeam ();
	public : ~FEMBeam ();

	private : void createActors(shared_ptr<ComplexBody>& rootBody);
	private : void positionRootBody(shared_ptr<ComplexBody>& rootBody);

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(FEMBeam);
};

REGISTER_SERIALIZABLE(FEMBeam,false);

#endif // SDEC_SPHERES_PLANE_HPP 

