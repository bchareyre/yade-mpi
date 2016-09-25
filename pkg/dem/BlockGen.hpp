/*CWBoon 2016 */
/* Please cite: */
/* CW Boon, GT Houlsby, S Utili (2015).  A new rock slicing method based on linear programming.  Computers and Geotechnics 65, 12-29. */
/* The numerical library is changed from CPLEX to CLP because subscription to the academic initiative is required to use CPLEX for free */
#ifdef YADE_POTENTIAL_BLOCKS
#pragma once
#include<core/FileGenerator.hpp>

#include<Python.h>


#include<lib/base/Math.hpp>
#include<lib/serialization/Serializable.hpp>
#include<lib/multimethods/Indexable.hpp>

#include <ClpSimplexDual.hpp>
#include <ClpSimplex.hpp>
#include <CoinHelperFunctions.hpp>
#include <CoinTime.hpp>
#include <CoinBuild.hpp>
#include <CoinModel.hpp>
#include <iomanip>
#include <cassert>

class GlobalStiffnessTimeStepper;

class BlockGen : public FileGenerator
{
	private	:
				
		
		
		void createActors(shared_ptr<Scene>& scene);
		void positionRootBody(shared_ptr<Scene>& scene);
		
		shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper;
		

	protected:
		std::ofstream output2;
		std::string myfile;
		std::string Key;
		static std::ofstream output;
		
	public : 
		~BlockGen ();
		bool generate(string&);
		template<class T>
		double gen_normal_3(T &generator){ return generator();}
		

		struct Discontinuity{
			Vector3r centre;
			Discontinuity(Vector3r pos){centre = pos; persistence = false; phi_b = 30.0; phi_r= 30.0; JRC=15; JCS = pow(10,6); asperity = 5; sigmaC = JCS;isBoundary= false;sliceBoundaries=false; lambda0=0.0; heatCapacity=0.0;hwater=-1.0;intactRock=false; throughGoing = false; constructionJoints=false;jointType=0;}
			Real a;
			Real b;
			Real c;
			Real d;
			Real a_p;
			Real b_p;
			Real c_p;
			Real d_p;
			bool persistence;
			bool isBoundary;
			bool sliceBoundaries;
			bool constructionJoints;
			vector<double> persistence_a;
			vector<double> persistence_b;
			vector<double> persistence_c;
			vector<double> persistence_d;
			/* Joint properties */
			double phi_b;
			double phi_r;
			double JRC;
			double JCS;
			double asperity;
			double sigmaC;
			double cohesion;
			double tension;
			double lambda0;
			double heatCapacity;
			double hwater;
			bool intactRock;
			bool throughGoing;
			int jointType;
		};
		struct Planes{
			vector<int> vertexID;
		};
		struct Block{
			Vector3r tempCentre;
			Vector3r centre;
			Block(Vector3r pos, Real kPP, Real rPP, Real RPP){centre = pos; k=kPP; r=rPP; R=RPP; tooSmall=false;isBoundary=false;tempCentre=pos;}
			vector<double> a;
			vector<double> b;
			vector<double> c;
			vector<double> d;
			vector<bool> redundant;	
			vector<bool> isBoundaryPlane;
			bool isBoundary;	
			vector<struct Block> subMembers;
			vector<Vector3r> falseVertex;
			vector<Vector3r> node; Real gridVol;
			Real r;
			Real R;
			Real k;
			bool tooSmall;
			/* Joint properties */
			vector<double> phi_b;
			vector<double> phi_r;
			vector<double> JRC;
			vector<double> JCS;
			vector<double> asperity;
			vector<double> sigmaC;
			vector<double> cohesion;
			vector<double> tension;
			vector<double> lambda0;
			vector<double> heatCapacity;
			vector<double> hwater;
			vector<bool> intactRock;
			vector<int> jointType;
			vector<struct Planes> planes;
		};
		

	

		

	double getSignedArea(const Vector3r pt1,const Vector3r pt2, const Vector3r pt3);
	double getDet(const Eigen::MatrixXd A);
	double getCentroidTetrahedron(const Eigen::MatrixXd A);


		bool createBlock(shared_ptr<Body>& body, struct BlockGen::Block block, int no);
		bool contactDetection(struct BlockGen::Discontinuity joint, struct BlockGen::Block block,  Vector3r& touchingPt);
		
		bool contactDetectionLPCLP(struct BlockGen::Discontinuity joint, struct BlockGen::Block block,  Vector3r& touchingPt);
		bool contactDetectionLPCLPglobal(struct BlockGen::Discontinuity joint, struct BlockGen::Block block,  Vector3r& touchingPt);
	
		bool checkRedundancyLPCLP(struct BlockGen::Discontinuity joint, struct BlockGen::Block block,  Vector3r& touchingPt);
		bool startingPointFeasibility(struct BlockGen::Block block,  Vector3r& initialPoint);
	
		double inscribedSphereCLP(struct BlockGen::Block block,  Vector3r& initialPoint, bool twoDimension);
		
		
		bool contactBoundaryLPCLP(struct BlockGen::Discontinuity joint, struct BlockGen::Block block,  Vector3r& touchingPt);
		bool contactBoundaryLPCLPslack(struct BlockGen::Discontinuity joint, struct BlockGen::Block block,  Vector3r& touchingPt);
		
		

	
		
		
		
		void calculateInertia(struct Block block, Real& Ixx, Real& Iyy, Real& Izz,Real& Ixy, Real& Ixz, Real& Iyz);
		
		Vector3r calCentroid(struct Block block, double & blockVol);
		
		bool checkCentroid(struct Block block, Vector3r presentTrial);
		Real evaluateFNoSphere(struct Block block, Vector3r presentTrial);
		
	

		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(
		BlockGen,FileGenerator,"Prepare a scene for triaxial tests. See full documentation at http://yade-dem.org/wiki/TriaxialTest."
		,
   		/* public */
		((Real,dampingMomentum,0.2,,"Coefficient of Cundal-Non-Viscous damping (applied on on the 3 components of torques)"))
		((Real, maxClosure, 0.0002, ,"vmi"))
		((Real, peakDisplacement, 0.02, ,"vmi"))
		((double, brittleLength, 2.0, ,"brittle rock"))
		((Real,damp3DEC,0.8,,"Coefficient of Cundal-Non-Viscous damping (applied on on the 3 components of torques)"))
		((Real,unitWidth2D,1.0,,"out of plane distance"))
		((Real,density,2600,,"density of blocks"))
		((Real,Kn,pow(10,-5),,"density of blocks"))
		((Real,Ks,pow(10,-5),,"density of blocks"))
		((Real,frictionDeg,18.0,,"friction angle [°]"))
		((Vector3r,globalOrigin,Vector3r(0.0,0.0,0.0),,"friction angle [°]"))
		((Real,inertiaFactor,1.0,,"to avoid wobbling"))
		((Real,rForPP,0.1,,"r"))
		((Real,kForPP,0.01,,"k"))
		((Real,RForPP,3.5,,"R"))
		((int,numberOfGrids,1,,"R"))
		((bool,probabilisticOrientation,false,,"use random number generator"))
		((bool,Talesnick,false,,"R"))
		((bool,neverErase,false,,"erase non interacting contacts"))
		((bool,calJointLength,false,,"whether to calculate jointLength"))
		((bool,twoDimension,false,,"whether it is 2D"))
                ((Real,shrinkFactor,1.0,,"ratio to shrink r"))
		((Real,viscousDamping,0.8,,"viscous damping"))
		((bool, intactRockDegradation, false, ,"brittle rock"))
		((Real,initialOverlap,0.0,,"initial overlap between blocks"))
		((Vector3r,gravity,Vector3r(0.0,-9.81,0.0),,"gravity"))
		((Real,defaultDt,-1,,"Max time-step. Used as initial value if defined. Latter adjusted by the time stepper."))
		((int,timeStepUpdateInterval,50,,"interval for :yref:`GlobalStiffnessTimeStepper`"))
		((bool,traceEnergy,true,,"friction angle [°]"))
		((bool,exactRotation,true,,"friction angle [°]"))
		((Real,minSize,50.0,,"length X of domain"))
		((Real,maxRatio,3.0,,"length X of domain"))
		((Real,boundarySizeXmax,1.0,,"length X of domain"))
		((Real,boundarySizeYmax,1.0,,"length Y of domain"))
		((Real,boundarySizeZmax,1.0,,"length Z of domain"))
		((Real,boundarySizeXmin,1.0,,"length X of domain"))
		((Real,boundarySizeYmin,1.0,,"length Y of domain"))
		((Real,boundarySizeZmin,1.0,,"length Z of domain"))
		((Vector3r,directionA,Vector3r(1,0,0),,"length Z of domain"))
		((Vector3r,directionB,Vector3r(0,1,0),,"length Z of domain"))
		((Vector3r,directionC,Vector3r(0,0,1),,"length Z of domain"))
		((Real,calAreaStep,10.0,,"length Z of domain"))
		((Real,extremeDist,0.5,,"boundary to base calculation of octree algorithms"))  //FIXME remove when not needed anymore
		((Real,subdivisionRatio,0.1,,"smallest size/boundary of octree algorithms"))
		((vector<double>,joint_a,,,"a"))
		((vector<double>,joint_b,,,"b"))
		((vector<double>,joint_c,,,"c"))
		((vector<double>,joint_d,,,"d"))
		/* check for different joint types */
		((bool, persistentPlanes,false,,"check persistence"))
		((bool, jointProbabilistic,false,,"check for filename jointProbabilistic"))	
		((bool, opening,false,,"check for filename opening"))	
		((bool, boundaries,false,,"check for filename boundaries"))
		((bool, slopeFace,false,,"check for filename boundaries"))
		((bool, sliceBoundaries,false,,"check for filename boundaries"))
		((bool, useGlobalStiffnessTimeStepper,false,,"check for filename boundaries"))
		/* which contact law to use */
		((bool, useBartonBandis,false,,"whether to use Barton Bandis contact law"))
		((bool, useFaceProperties, false, , "use face properties"))
		((bool, useOverlapVol,false,,"check for filename opening"))
		((std::string,filenamePersistentPlanes,"/home/booncw/yade/BranchA/scripts/Tunnel/jointPersistent.csv",,"filename to look for joint properties"))
		((std::string,filenameProbabilistic,"/home/booncw/yade/BranchA/scripts/boon/Tunnel/jointProbabilistic.csv",,"filename to look for joint with probabilistic models"))
		((std::string,filenameBoundaries,"/home/booncw/yade/BranchA/scripts/boon/Tunnel/boundaries.csv",,"filename to look for joint with probabilistic models"))
		((std::string,filenameOpening,"/home/booncw/yade/BranchA/scripts/boon/Tunnel/opening.csv",,"filename to look for joint outline of joints"))
		((std::string,filenameSlopeFace,"/home/booncw/yade/BranchA/scripts/boon/Tunnel/opening.csv",,"filename to look for joint outline of joints"))
		((std::string,filenameSliceBoundaries,"/home/booncw/yade/BranchA/scripts/boon/Tunnel/sliceBoundaries.csv",,"filename to look for joint outline of joints"))
		//((double*, array_a,,,"a"))	
		,
		/* init */
		,
		/* constructor for private */
		Key			="";
		myfile = "./BlkGen"+Key;
		output2.open(myfile.c_str(), fstream::app);

		,
		//.def("setContactProperties",&TriaxialCompressionEngine::setContactProperties,"Assign a new friction angle (degrees) to dynamic bodies and relative interactions")
		 );
	DECLARE_LOGGER;
};

#ifdef __cplusplus
extern "C" {
#endif
	 void dgesv_(const int *N, const int *nrhs, double *Hessian, const int *lda, int *ipiv, double *gradient, const int *ldb, int *info);
	void dsyev_(const char *jobz, const char *uplo, const int *N, double *A, const int *lda, double *W, double *work, int *lwork, int *info);
#ifdef __cplusplus
};
#endif

REGISTER_SERIALIZABLE(BlockGen);
#endif // YADE_POTENTIAL_BLOCKS
