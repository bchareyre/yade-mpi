/*CWBoon 2016 */
/* Please cite: */
/* CW Boon, GT Houlsby, S Utili (2015).  Designing Tunnel Support in Jointed Rock Masses Via the DEM.  Rock Mechanics and Rock Engineering,  48 (2), 603-632. */
#ifdef YADE_POTENTIAL_BLOCKS
#pragma once
#include<pkg/dem/PotentialBlock.hpp>
#include<pkg/dem/PotentialBlock2AABB.hpp>

#include <vector>
#include <pkg/common/PeriodicEngines.hpp>

#include <stdio.h>





#include <vtkPolyData.h>
#include<vtkXMLUnstructuredGridWriter.h>
#include<vtkTriangle.h>
#include<vtkSmartPointer.h>
#include<vtkFloatArray.h>
#include<vtkCellArray.h>
#include<vtkCellData.h>
#include <vtkStructuredPoints.h>
#include<vtkStructuredPointsWriter.h>
#include<vtkWriter.h>
#include<vtkExtractVOI.h>
#include<vtkXMLImageDataWriter.h>
#include<vtkXMLStructuredGridWriter.h>
#include<vtkTransformPolyDataFilter.h>
#include<vtkTransform.h>
#include<vtkXMLUnstructuredGridWriter.h>
#include <vtkAppendPolyData.h>
#include <vtkCylinderSource.h>

#include <ClpSimplex.hpp>
#include <CoinHelperFunctions.hpp>
#include <CoinTime.hpp>
#include <CoinBuild.hpp>
#include <CoinModel.hpp>
//#include "/home/boon/coin-Clp/Clp/src/ClpSimplex.hpp"
//#include "/home/boon/coin-Clp/CoinUtils/src/CoinHelperFunctions.hpp"
//#include "/home/boon/coin-Clp/CoinUtils/src/CoinTime.hpp"
//#include "/home/boon/coin-Clp/CoinUtils/src/CoinBuild.hpp"
//#include "/home/boon/coin-Clp/CoinUtils/src/CoinModel.hpp"

class RockLiningGlobal: public PeriodicEngine{	
	protected:
		double stiffnessMatrix[36];
		//double * globalStiffnessMatrix;
		double globalStiffnessMatrix[3*3*200*200];
  	public:
		#if 0
		struct Bolts{
			Bolts(Vector3r pt1, Vector3r  pt2){startingPoint = pt1; endPoint=pt2; }
			Vector3r startingPoint;
			Vector3r endPoint;
			
			/* variables stored in sequence starting from the block closest to the opening */
			vector<int> blockIDs; /*blocks intersected */
			vector<Vector3r> localCoordinates; /*local coordinates inside blocks */
			vector<double> initialLength;
		};
		vector<Bolts> bolt;
		#endif

		Vector3r getNodeDistance(const PotentialBlock* cm1,const State* state1,const PotentialBlock* cm2,const State* state2, const Vector3r localPt1, const Vector3r localPt2);
		bool installLining(const PotentialBlock* cm1,const State* state1,const Vector3r startingPt,const Vector3r direction, const double length, Vector3r& intersectionPt);
		int insertNode(Vector3r pos, double mass, double intervalLength);
		double evaluateFNoSphereVol(const PotentialBlock* s1,const State* state1, const Vector3r newTrial);
		bool intersectPlane(const PotentialBlock* s1,const State* state1,const Vector3r startingPt,const Vector3r direction, const double length, Vector3r& intersectionPt, const Vector3r plane, const double planeD);
  		virtual void action(void);
  	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(RockLiningGlobal,PeriodicEngine,"Engine recording potential blocks as surfaces into files with given periodicity.",
		((bool,assembledKglobal,false ,,"global stiffness matrix"))	
		((double,density,0.0 ,,"density"))	
		((double,lumpedMass,0.0 ,,"lumpedMass"))
		((double,EA,0.0 ,,"EA"))	
		((double,EI,0.0 ,,"EI"))
		((double,initOverlap,pow(10,-5),,"initialOverlap"))	
		((double,expansionFactor,pow(10,-5),,"alpha deltaT"))
		((double,contactLength,1.0 ,,"contactLength"))	
		((vector<double>, sigmaMax, ,,"sigma max"))
		((vector<double>, sigmaMin, ,,"sigma min"))
		((double,ElasticModulus,0.0 ,,"E"))
		((double,liningThickness,0.1 ,,"liningThickness"))
		((double,Inertia,0.0 ,,"I"))
		((vector<double>,lengthNode, ,,"L"))
		((vector<int>,stickIDs, ,,"L"))
		((double,Area,0.02 ,,"A"))
		((double,interfaceStiffness,pow(10,8) ,,"L"))
		((double,interfaceFriction,30.0 ,,"L"))
		((double,interfaceCohesion,0.5*pow(10,6) ,,"L"))
		((double,interfaceTension,0.8*pow(10,6) ,,"L"))
		((int,totalNodes,0 ,,"L"))
		((Vector3r,startingPoint,Vector3r(0,0,0) ,,"startingPt"))
		((vector<int>,blockIDs, ,,"ids"))
		((vector<Vector3r>,localCoordinates, ,,"local coordinates of intersection"))
		((vector<Vector3r>,refPos, ,,"initial u"))	
		((vector<Vector3r>,refDir, ,,"initial v"))	
		((vector<Quaternionr>,refOri, ,,"initial theta"))
		((vector<double>,refAngle, ,,"initial theta"))
		((vector<double>,moment, ,,"moment"))
		((vector<double>,axialForces, ,,"force"))
		((vector<double>,shearForces, ,,"force"))
		((vector<double>,displacement, ,,"force"))
		((vector<double>,radialDisplacement, ,,"force"))
		((double,openingRad,5.0 ,,"estimated opening radius"))
		((bool,installed,false ,,"installed?"))
		((bool,openingCreated,false ,,"opening created?"))
		((vector<bool>,ruptured, ,,"ruptured"))
		((Real,axialMax,1000000000 ,,"maximum axial force"))
		((Real,shearMax,1000000000 ,,"maximum shear force"))
		((int,vtkIteratorInterval,10000 ,,"how often to print vtk"))	
		((int,vtkRefTimeStep,1 ,,"first timestep to print vtk"))	
		((string,fileName,,,"File prefix to save to"))	
		((string,name,,,"File prefix to save to"))
		,
			//globalStiffnessMatrix = new double[totalNodes*3*totalNodes*3];
			
		,
  	);


};
REGISTER_SERIALIZABLE(RockLiningGlobal);

#ifdef __cplusplus
extern "C" {
#endif

/* LAPACK LU */	
	//int dgesv(int varNo, int varNo2, double *H, int varNo3, int *pivot, double* g, int varNo4, int info){
	 extern void dgesv_(const int *N, const int *nrhs, double *Hessian, const int *lda, int *ipiv, double *gradient, const int *ldb, int *info);
	// int ans;
	// dgesv_(&varNo, &varNo2, H, &varNo3, pivot,g, &varNo4, &ans);
	// return ans;
	//}	

/* LAPACK Cholesky */
	extern void dpbsv_(const char *uplo, const int *n, const int *kd, const int *nrhs, double *AB, const int *ldab, double *B, const int *ldb, int *info);

/* LAPACK QR */
	extern void dgels_(const char *Trans, const int *m, const int *n, const int *nrhs, double *A, const int *lda, double *B, const int *ldb, const double *work, const int *lwork, int *info);

	
/*BLAS */
	extern void dgemm_(const char *transA, const char *transB, const int *m, const int *n, const int *k, const double *alpha, double *A, const int *lda, double *B, const int *ldb, const double *beta, double *C, const int *ldc);

	extern void dgemv_(const char *trans, const int *m, const int *n, const double *alpha, double *A, const int *lda, double *x, const int *incx, const double *beta, double *y, const int *incy);
		
	extern void dcopy_(const int *N, double *x, const int *incx, double *y, const int *incy);

	extern double ddot_(const int *N, double *x, const int *incx, double *y, const int *incy);

	extern void daxpy_(const int *N, const double *da, double *dx, const int *incx, double *dy, const int *incy);

	extern void dscal_(const int *N, const double *alpha, double *x, const int *incx);


	void dsyev_(const char *jobz, const char *uplo, const int *N, double *A, const int *lda, double *W, double *work, int *lwork, int *info);


#ifdef __cplusplus
};
#endif
#endif // YADE_POTENTIAL_BLOCKS
