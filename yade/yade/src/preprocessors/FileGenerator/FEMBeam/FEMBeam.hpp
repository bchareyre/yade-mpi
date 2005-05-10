#ifndef FEM_BEAM_HPP
#define FEM_BEAM_HPP 

#include <yade/FileGenerator.hpp>

#include <string>

class FEMBeam : public FileGenerator
{
	private :
		Vector3r gravity;
		
		std::string femTxtFile;
		int nodeGroupMask;
		int tetrahedronGroupMask;
		
		Vector3r regionMin1, regionMax1;
		Vector3r translationAxis1;
		Real     velocity1;
		Vector3r regionMin2, regionMax2;
		Vector3r translationAxis2;
		Real     velocity2;
//	private : Real dampingForce;
//	private : Real dampingMomentum;

// construction
	public : FEMBeam ();
	public : ~FEMBeam ();

	private : void createActors(shared_ptr<MetaBody>& rootBody);
	private : void positionRootBody(shared_ptr<MetaBody>& rootBody);
	private : void imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real velocity);

	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(FEMBeam);
};

REGISTER_SERIALIZABLE(FEMBeam,false);

#endif // SDEC_SPHERES_PLANE_HPP 

