/*CWBoon 2015 */
#ifdef YADE_POTENTIAL_BLOCKS
#pragma once

#include<vector>
#include<core/Shape.hpp>
#include<Eigen/Core>
#include <Eigen/LU> 
#include <Eigen/QR>
#include<lib/base/openmp-accu.hpp>
namespace yade{
class PotentialBlock : public Shape
{

	public:
		
		
		struct Planes{
			vector<int> vertexID;
		};
		struct Vertices{
			vector<int> edgeID;
			vector<int> planeID;
		};
		struct Edges{
			vector<int> vertexID;
		};
		
		void addPlaneStruct();
		void addVertexStruct();
		void addEdgeStruct();
		
		vector<Planes> planeStruct;	
		vector<Vertices> vertexStruct;	
		vector<Edges> edgeStruct;
	
		Eigen::MatrixXd Amatrix;
		Eigen::MatrixXd Dmatrix;
		virtual ~PotentialBlock ();
		void postLoad(PotentialBlock&);
	
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(PotentialBlock,Shape,"Geometry of PotentialBlock.",
		((bool, isLining, false,, "false"))
		((double, liningStiffness, pow(10.0,8),, "lining stiffness"))
		((double, liningFriction, 20.0,, "lining stiffness"))
		((double, liningLength, 0.0,, "lining stiffness"))
		((double, liningTensionGap, 0.0,, "lining stiffness"))
		((Vector3r, liningNormalPressure, Vector3r(0,0,0),, "lining stiffness"))
		((Vector3r, liningTotalPressure, Vector3r(0,0,0),, "lining stiffness"))
		((bool, isBoundary, false,, "boundary"))
		((bool, isEastBoundary, false,, "boundary"))
		((bool, isBolt, false,, "boundary"))
		((bool, fixedNormal, false,, "boundary"))
		((Vector3r, boundaryNormal, Vector3r::Zero(),,"normal direction of boundary"))
		((bool, AabbMinMax, false,, "aabb"))
		((Vector3r, minAabb, Vector3r::Zero(),,"min from box centre"))
		((Vector3r, maxAabb, Vector3r::Zero(),,"max frin box centre"))
		((Vector3r, minAabbRotated, Vector3r::Zero(),,"min from box centre"))
		((Vector3r, maxAabbRotated, Vector3r::Zero(),,"max frin box centre"))
		((Vector3r, halfSize, Vector3r::Zero(),,"max frin box centre"))
		((vector<Vector3r> , node, ,, "nodes "))
		((Real, gridVol, ,, "nodes "))
		((Quaternionr , oriAabb, Quaternionr::Identity(),, "r "))
		((Real , r, 0.1,, "r "))
		((Real , R, 1.0,, "R "))
		((Real , k, 0.1,, "k "))
		((Real , volume, 0.1,, "k "))
		((int, id, -1,, " for graphics"))
		((bool, erase, false,, " for graphics"))
		((vector<bool>, intactRock, false,, " for graphics"))
		((vector<bool>, isBoundaryPlane, ,, "whether it is a boundary"))
		((vector<double>, hwater, ,, "height of pore water"))
		((vector<double>, JRC, ,, "rock joint"))
		((vector<double>, JCS, ,, "rock joint"))
		((vector<double>, asperity, ,, "rock joint"))
		((vector<double>, sigmaC, ,,"rock joint"))
		((vector<double>, phi_b, ,, "rock joint"))
		((vector<double>, phi_r, ,, "rock joint"))
		((vector<double>, cohesion, ,, "rock joint"))
		((vector<double>, tension, ,, "rock joint"))
		((vector<double>, lambda0, ,, "rock joint"))
		((vector<double>, heatCapacity, ,, "rock joint"))
		((vector<double>, rFactor, ,, "individual factors for r"))
		((vector<Vector3r>, vertices,,,"vertices"))
		//((Eigen::MatrixXd , Amatrix, ,, "a "))
		//((Eigen::MatrixXd , Dmatrix, ,, "b "))
		((double, waterVolume, ,, "water"))
		((vector<Vector3r> ,verticesCD, ,, "vertices "))
		((vector<double> , a, ,, "a "))
		((vector<double> , b, ,, "b "))
		((vector<double> , c, ,, "c "))
		((vector<double> , d, ,, "d "))
		((vector<int> , jointType, ,, "jointType"))
		,
		createIndex(); /*ctor*/
		#if 0
		for (int i=0; i<a.size(); i++){
			Amatrix(i,0) = a[i]; Amatrix(i,1)=b[i]; Amatrix(i,2)=c[i];
			Dmatrix(i,0) = d[i] + r;
	 	}
		#endif
	
	);
	//#endif

	REGISTER_CLASS_INDEX(PotentialBlock,Shape);
	
};
}
using namespace yade; 

REGISTER_SERIALIZABLE(PotentialBlock);

#ifdef __cplusplus
extern "C" {
#endif
	 void dgesv_(const int *N, const int *nrhs, double *Hessian, const int *lda, int *ipiv, double *gradient, const int *ldb, int *info);
	void dsyev_(const char *jobz, const char *uplo, const int *N, double *A, const int *lda, double *W, double *work, int *lwork, int *info);
#ifdef __cplusplus
};
#endif
#endif // YADE_POTENTIAL_BLOCKS
