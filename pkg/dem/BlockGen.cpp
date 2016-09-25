/*CWBoon 2016 */
/* Please cite: */
/* CW Boon, GT Houlsby, S Utili (2015).  A new rock slicing method based on linear programming.  Computers and Geotechnics 65, 12-29. */
/* The numerical library is changed from CPLEX to CLP because subscription to the academic initiative is required to use CPLEX for free */
#ifdef YADE_POTENTIAL_BLOCKS



#include<core/Clump.hpp>
#include<pkg/dem/KnKsPBLaw.hpp>

#include<pkg/dem/GlobalStiffnessTimeStepper.hpp>
#include<pkg/common/ElastMat.hpp>
#include<pkg/dem/TriaxialStressController.hpp>
#include<pkg/dem/TriaxialCompressionEngine.hpp>
#include <pkg/dem/TriaxialStateRecorder.hpp>
#include<pkg/common/Aabb.hpp>
#include<core/Scene.hpp>
#include<pkg/common/InsertionSortCollider.hpp>
#include<core/Interaction.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/common/GravityEngines.hpp>
#include<pkg/dem/NewtonIntegrator.hpp>
#include<pkg/dem/PotentialBlock.hpp>
#include<pkg/common/Dispatching.hpp>
#include<core/Body.hpp>
#include<pkg/common/Box.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/common/Facet.hpp>
#include<pkg/common/Wall.hpp>
#include<pkg/common/ForceResetter.hpp>
#include<pkg/common/InteractionLoop.hpp>
#include<pkg/dem/Shop.hpp>
#include<pkg/dem/PotentialBlock.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>
// random
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include<pkg/dem/SpherePack.hpp>
//#include<pkg/dem/MicroMacroAnalyser.hpp>

#include "BlockGen.hpp"
#include<pkg/dem/Ig2_PB_PB_ScGeom.hpp>
#include<pkg/dem/PotentialBlock2AABB.hpp>
#include<lib/pyutil/gil.hpp>

 
 /* IpOpt */

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>


CREATE_LOGGER(BlockGen);
YADE_PLUGIN((BlockGen));

//using namespace boost;
//using namespace std;



BlockGen::~BlockGen () {}
std::ofstream BlockGen::output("BlockGenFindExtreme.txt", fstream::trunc); 
bool BlockGen::generate(string& message)
{

	
	scene = shared_ptr<Scene>(new Scene);
	shared_ptr<Body> body;
	positionRootBody(scene);
	createActors(scene);

	/* Create domain: start with one big block */
	vector<Block> blk;
	Vector3r firstBlockCentre(0,0,0);
	firstBlockCentre.x() = 0.0; //0.5*(boundarySizeXmax + boundarySizeXmin);
	firstBlockCentre.y() = 0.0; //0.5*(boundarySizeYmax + boundarySizeYmin);
	firstBlockCentre.z() = 0.0; //0.5*(boundarySizeZmax + boundarySizeZmin);
	blk.push_back(Block(firstBlockCentre,kForPP, rForPP, RForPP));
	
	Real xmin = fabs(firstBlockCentre.x() - boundarySizeXmin);
	Real xmax = fabs(-firstBlockCentre.x() + boundarySizeXmax);
	Real ymin = fabs(firstBlockCentre.y() - boundarySizeYmin);
	Real ymax = fabs(-firstBlockCentre.y() + boundarySizeYmax);
	Real zmin = fabs(firstBlockCentre.z() - boundarySizeZmin);
	Real zmax = fabs(-firstBlockCentre.z() + boundarySizeZmax);
	blk[0].a.push_back(1.0); blk[0].a.push_back(-1.0); blk[0].a.push_back(0.0); blk[0].a.push_back(0.0); blk[0].a.push_back(0.0); blk[0].a.push_back(0.0); 
	blk[0].b.push_back(0.0); blk[0].b.push_back(0.0);  blk[0].b.push_back(1.0); blk[0].b.push_back(-1.0); blk[0].b.push_back(0.0); blk[0].b.push_back(0.0);
	blk[0].c.push_back(0.0); blk[0].c.push_back(0.0);  blk[0].c.push_back(0.0); blk[0].c.push_back(0.0); blk[0].c.push_back(1.0); blk[0].c.push_back(-1.0);
	blk[0].d.push_back(xmax); blk[0].d.push_back(xmin);  blk[0].d.push_back(ymax); blk[0].d.push_back(ymin); blk[0].d.push_back(zmax); blk[0].d.push_back(zmin); 
	blk[0].redundant.push_back(false); blk[0].redundant.push_back(false); blk[0].redundant.push_back(false); blk[0].redundant.push_back(false); blk[0].redundant.push_back(false); blk[0].redundant.push_back(false);
	blk[0].JRC.push_back(15.0);blk[0].JRC.push_back(15.0);blk[0].JRC.push_back(15.0);blk[0].JRC.push_back(15.0);blk[0].JRC.push_back(15.0);blk[0].JRC.push_back(15.0);
	blk[0].JCS.push_back(pow(10,6));blk[0].JCS.push_back(pow(10,6));blk[0].JCS.push_back(pow(10,6));blk[0].JCS.push_back(pow(10,6));blk[0].JCS.push_back(pow(10,6));blk[0].JCS.push_back(pow(10,6));
	blk[0].sigmaC.push_back(pow(10,6));blk[0].sigmaC.push_back(pow(10,6));blk[0].sigmaC.push_back(pow(10,6));blk[0].sigmaC.push_back(pow(10,6));blk[0].sigmaC.push_back(pow(10,6));blk[0].sigmaC.push_back(pow(10,6));
	blk[0].phi_r.push_back(40.0);blk[0].phi_r.push_back(40.0);blk[0].phi_r.push_back(40.0);blk[0].phi_r.push_back(40.0);blk[0].phi_r.push_back(40.0);blk[0].phi_r.push_back(40.0);
	blk[0].phi_b.push_back(40.0);blk[0].phi_b.push_back(40.0);blk[0].phi_b.push_back(40.0);blk[0].phi_b.push_back(40.0);blk[0].phi_b.push_back(40.0);blk[0].phi_b.push_back(40.0);
	blk[0].asperity.push_back(5.0);blk[0].asperity.push_back(5.0);blk[0].asperity.push_back(5.0);blk[0].asperity.push_back(5.0);blk[0].asperity.push_back(5.0);blk[0].asperity.push_back(5.0);
	blk[0].cohesion.push_back(0.0);blk[0].cohesion.push_back(0.0);blk[0].cohesion.push_back(0.0);blk[0].cohesion.push_back(0.0);blk[0].cohesion.push_back(0.0);blk[0].cohesion.push_back(0.0);
	blk[0].tension.push_back(0.0);blk[0].tension.push_back(0.0);blk[0].tension.push_back(0.0);blk[0].tension.push_back(0.0);blk[0].tension.push_back(0.0);blk[0].tension.push_back(0.0);
	blk[0].isBoundaryPlane.push_back(false);blk[0].isBoundaryPlane.push_back(false);blk[0].isBoundaryPlane.push_back(false);blk[0].isBoundaryPlane.push_back(false);blk[0].isBoundaryPlane.push_back(false);blk[0].isBoundaryPlane.push_back(false);
	blk[0].lambda0.push_back(0.0);blk[0].lambda0.push_back(0.0);blk[0].lambda0.push_back(0.0);blk[0].lambda0.push_back(0.0);blk[0].lambda0.push_back(0.0);blk[0].lambda0.push_back(0.0);
	blk[0].heatCapacity.push_back(0.0);blk[0].heatCapacity.push_back(0.0);blk[0].heatCapacity.push_back(0.0);blk[0].heatCapacity.push_back(0.0);blk[0].heatCapacity.push_back(0.0);blk[0].heatCapacity.push_back(0.0);
	blk[0].hwater.push_back(-1.0);blk[0].hwater.push_back(-1.0);blk[0].hwater.push_back(-1.0);blk[0].hwater.push_back(-1.0);blk[0].hwater.push_back(-1.0);blk[0].hwater.push_back(-1.0);
	blk[0].intactRock.push_back(false);blk[0].intactRock.push_back(false);blk[0].intactRock.push_back(false);blk[0].intactRock.push_back(false);blk[0].intactRock.push_back(false);blk[0].intactRock.push_back(false);
	blk[0].jointType.push_back(0); blk[0].jointType.push_back(0); blk[0].jointType.push_back(0); blk[0].jointType.push_back(0); blk[0].jointType.push_back(0); blk[0].jointType.push_back(0);

	/* List of Discontinuities */
	vector<Discontinuity> joint;

	/* Read boundary size */
	Real boundarySize = max(max(fabs(boundarySizeXmax-boundarySizeXmin),fabs(boundarySizeYmax-boundarySizeYmin)),fabs(boundarySizeZmax-boundarySizeZmin));
	Real dip = 0.0;Real dipdir = 0.0;

	
	/*Python input for discontinuities */
	for(unsigned int i=0; i<joint_a.size(); i++){
		joint.push_back(Discontinuity(globalOrigin));
		joint[i].a = joint_a[i]; joint[i].b=joint_b[i]; joint[i].c=joint_c[i]; joint[i].d=joint_d[i];
		//std::cout<<"joint i: "<<i<<", a: "<<joint_a[i]<<", b: "<<joint_b[i]<<", c: "<<joint_c[i]<<", d: "<<joint_d[i]<<endl;	
	}	

	

	if(boundaries){
		/* Read csv file for info on discontinuities */
		const char *filenameChar = filenameBoundaries.c_str();
		ifstream file ( filenameChar ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		string value;
		/* skip first line */
		getline ( file, value); 
		int count = 0;
		Vector3r jointCentre(0,0,0);
		const double PI = std::atan(1.0)*4;
		int abdCount=0;  Vector3r planeNormal(0,0,0);  int jointNo = 0; double persistenceA=0; double persistenceB=0; double spacing = 0;
		while ( file.good() )
		{
		    	count ++;
		    	getline ( file, value, ';' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
			//std::cout<<"count: "<<count<<", value: "<<value<<endl;
		    	const char *valueChar = value.c_str();
		    	double valueDouble = atof ( valueChar);
			if(count == 1) dip = valueDouble;
			if(count == 2) {
				dipdir = valueDouble;
				double dipdirN = 0.0;
				double dipN = 90.0-dip;

				

				if(dipdir > 180.0){
					dipdirN = dipdir - 180.0;
				}else{
					dipdirN = dipdir + 180.0;
				}
				Real dipRad = dipN/180.0*PI;
				Real dipdirRad = dipdirN/180.0*PI;
				Real a = cos(dipdirRad)*cos(dipRad);
				Real b = sin(dipdirRad)*cos(dipRad);
				Real c = sin(dipRad);
				Real l = sqrt(a*a + b*b +c*c);
				joint.push_back(Discontinuity(globalOrigin));
				int i = joint.size()-1; jointNo=i;
				joint[i].a = a/l;   joint[i].b = b/l;   joint[i].c = c/l;  joint[i].d = 0.0;
				planeNormal = Vector3r(a/l,b/l,c/l); 
				//std::cout<<"planeNormal: "<<planeNormal<<endl;
			}
			if(count == 3) jointCentre.x() = valueDouble;
			if(count == 4) jointCentre.y() = valueDouble;
			if(count == 5) {jointCentre.z() = valueDouble; joint[jointNo].centre = jointCentre; joint[jointNo].isBoundary = true;}
			if(count == 6) {joint[jointNo].phi_b = valueDouble;}
			if(count == 7) {joint[jointNo].phi_r = valueDouble;}
			if(count == 8) {joint[jointNo].JRC = valueDouble;}
			if(count == 9) {joint[jointNo].JCS = valueDouble;}
			if(count == 10){joint[jointNo].asperity = valueDouble;}
			if(count == 11){joint[jointNo].sigmaC = valueDouble;}
			if(count == 12){joint[jointNo].cohesion = valueDouble;}
			if(count == 13){joint[jointNo].tension = valueDouble;}
			if(count == 14){joint[jointNo].lambda0 = valueDouble;}
			if(count == 15){joint[jointNo].heatCapacity = valueDouble;}
			if(count == 16){joint[jointNo].hwater = valueDouble;joint[jointNo].throughGoing = true;count = 0;}
		}
	}



	if(sliceBoundaries){
		/* Read csv file for info on discontinuities */
		const char *filenameChar = filenameSliceBoundaries.c_str();
		ifstream file ( filenameChar ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		string value;
		/* skip first line */
		getline ( file, value); 
		int count = 0;
		Vector3r jointCentre(0,0,0);
		const double PI = std::atan(1.0)*4;
		Vector3r planeNormal(0,0,0);  int jointNo = 0;
		while ( file.good() )
		{
		    	count ++;
		    	getline ( file, value, ';' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
			//std::cout<<"count: "<<count<<", value: "<<value<<endl;
		    	const char *valueChar = value.c_str();
		    	double valueDouble = atof ( valueChar);
			if(count == 1) dip = valueDouble;
			if(count == 2) {
				dipdir = valueDouble;
				double dipdirN = 0.0;
				double dipN = 90.0-dip;
				if(dipdir > 180.0){
					dipdirN = dipdir - 180.0;
				}else{
					dipdirN = dipdir + 180.0;
				}
				Real dipRad = dipN/180.0*PI;
				Real dipdirRad = dipdirN/180.0*PI;
				Real a = cos(dipdirRad)*cos(dipRad);
				Real b = sin(dipdirRad)*cos(dipRad);
				Real c = sin(dipRad);
				Real l = sqrt(a*a + b*b +c*c);
				joint.push_back(Discontinuity(globalOrigin));
				int i = joint.size()-1; jointNo=i;
				joint[i].a = a/l;   joint[i].b = b/l;   joint[i].c = c/l;  joint[i].d = 0.0;
				planeNormal = Vector3r(a/l,b/l,c/l); 
				//std::cout<<"planeNormal: "<<planeNormal<<endl;
			}
			if(count == 3) jointCentre.x() = valueDouble;
			if(count == 4) jointCentre.y() = valueDouble;
			if(count == 5) {jointCentre.z() = valueDouble; joint[jointNo].centre = jointCentre; joint[jointNo].sliceBoundaries = true;}
			if(count == 6){joint[jointNo].phi_b = valueDouble;}
			if(count == 7){joint[jointNo].phi_r = valueDouble;}
			if(count == 8){joint[jointNo].JRC = valueDouble;}
			if(count == 9){joint[jointNo].JCS = valueDouble;}
			if(count == 10){joint[jointNo].asperity = valueDouble;}
			if(count == 11){joint[jointNo].sigmaC = valueDouble;}
			if(count == 12){joint[jointNo].cohesion = valueDouble;}
			if(count == 13){joint[jointNo].tension = valueDouble;joint[jointNo].throughGoing = true;count = 0;}
		}
	}


	if(persistentPlanes){
		/* Read csv file for info on discontinuities */
		const char *filenameChar = filenamePersistentPlanes.c_str();
		ifstream file ( filenameChar ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		string value;
		/* skip first line */
		getline ( file, value); 
		int count = 0; 
		const double PI = std::atan(1.0)*4;
		int boundaryNo = 0; int boundaryCount = 0; int abdCount=0;  Vector3r planeNormal(0,0,0);  int jointNo = 0; double persistenceA=0; double persistenceB=0; double spacing = 0;double a,b,c,d,l = 0.0;
		while ( file.good() )
		{
		    	count ++;
		    	getline ( file, value, ';' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
			//std::cout<<"count: "<<count<<", value: "<<value<<endl;
		    	const char *valueChar = value.c_str();
		    	double valueDouble = atof ( valueChar);
			
			double phi_b,phi_r,JRC,JCS,asperity, sigmaC,cohesion, tension = 0.0;
			if(count == 1) dip = valueDouble;
			if(count == 2) dipdir = valueDouble;
			if(count ==3) {
					spacing = valueDouble;
					//std::cout<<"dip: "<<dip<<", dipdir: "<<dipdir<<", spacing: "<<spacing<<endl;
					double dipdirN = 0.0;
					double dipN = 90.0-dip;
					if(dipdir > 180.0){
						dipdirN = dipdir - 180.0;
					}else{
						dipdirN = dipdir + 180.0;
					}
					Real dipRad = dipN/180.0*PI;
					Real dipdirRad = dipdirN/180.0*PI;
					 a = cos(dipdirRad)*cos(dipRad);
					 b = sin(dipdirRad)*cos(dipRad);
					 c = sin(dipRad);
					 l = sqrt(a*a + b*b +c*c);
					planeNormal = Vector3r(a/l,b/l,c/l); //save for later
					//std::cout<<"planeNormal1: "<<planeNormal<<endl;
			}
			if(count == 4){phi_b = valueDouble;}
			if(count == 5){phi_r = valueDouble;}
			if(count == 6){JRC = valueDouble;}
			if(count == 7){JCS = valueDouble;}
			if(count == 8){asperity = valueDouble;}
			if(count == 9){sigmaC = valueDouble;}
			if(count == 10){cohesion = valueDouble;}
			if(count == 11){
				tension = valueDouble;
				joint.push_back(Discontinuity(firstBlockCentre));
				int i = joint.size()-1;
				joint[i].a = a/l;   joint[i].b = b/l;   joint[i].c = c/l;  joint[i].d = 0.0;
				joint[i].phi_b = phi_b; joint[i].phi_r = phi_r; joint[i].JRC = JRC; joint[i].JCS = JCS; joint[i].asperity = asperity; joint[i].sigmaC = sigmaC; joint[i].cohesion = cohesion; joint[i].tension = tension;
				planeNormal = Vector3r(a/l,b/l,c/l); //save for later
				//std::cout<<"planeNormal: "<<planeNormal<<endl;
				int number = boundarySize/spacing;
				for(int j=1; j<=number/2.0 /* -1 */; j++){
					joint.push_back(Discontinuity(firstBlockCentre));
					int i = joint.size()-1;
					joint[i].a = a/l;   joint[i].b = b/l;   joint[i].c = c/l;  joint[i].d = spacing*static_cast<double>(j); 
					joint[i].phi_b = phi_b; joint[i].phi_r = phi_r; joint[i].JRC = JRC; joint[i].JCS = JCS; joint[i].asperity = asperity; joint[i].sigmaC = sigmaC; joint[i].cohesion = cohesion; joint[i].tension = tension;
					joint.push_back(Discontinuity(firstBlockCentre));
					i = joint.size()-1;
					joint[i].a = -a/l;   joint[i].b = -b/l;   joint[i].c = -c/l;  joint[i].d =spacing*static_cast<double>(j); 
					joint[i].phi_b = phi_b; joint[i].phi_r = phi_r; joint[i].JRC = JRC; joint[i].JCS = JCS; joint[i].asperity = asperity; joint[i].sigmaC = sigmaC; joint[i].cohesion = cohesion; joint[i].tension = tension;
				}
				count = 0;
			}
		
		}
	}


	if(jointProbabilistic){
		/* Read csv file for info on discontinuities */
		const char *filenameChar = filenameProbabilistic.c_str();
		ifstream file ( filenameChar ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		string value;
		/* skip first line */
		getline ( file, value); 
		int count = 0; int linecount = 0; 
		Real dip = 0.0;Real dipdir = 0.0; Vector3r jointCentre(0,0,0);
		const double PI = std::atan(1.0)*4;
		int boundaryNo = 0; int boundaryCount = 0; int abdCount=0;  Vector3r planeNormal(0,0,0);  int jointNo = 0; double persistenceA=0; double persistenceB=0; double spacing = 0;
		boost::normal_distribution<> nd(0.0, 1.0);
		boost::variate_generator<boost::mt19937,boost::normal_distribution<> > generator(boost::mt19937(time(0)),nd);
		while ( file.good() )
		{
			//std::cout<<"reading file, count"<<count<<endl;
		    	count ++; 
		    	getline ( file, value, ';' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
			//std::cout<<"count: "<<count<<", value: "<<value<<endl;
		    	const char *valueChar = value.c_str(); 
		    	double valueDouble = atof ( valueChar);  
			if(count == 1) { dip = valueDouble;  }
			if(count == 2) {
				dipdir = valueDouble; 
				double dipdirN = 0.0;
				double dipN = 90.0-dip;
				if(dipdir > 180.0){
					dipdirN = dipdir - 180.0;
				}else{
					dipdirN = dipdir + 180.0;
				}
				if(probabilisticOrientation == true && dip < 120.0 && dip > 60.0 ){
					double perturb = gen_normal_3(generator);
					dipN = dipN + perturb;
					std::cout<<"perturb: "<<perturb<<endl;
				}
				Real dipRad = dipN/180.0*PI;
				Real dipdirRad = dipdirN/180.0*PI;
				Real a = cos(dipdirRad)*cos(dipRad);
				Real b = sin(dipdirRad)*cos(dipRad);
				Real c = sin(dipRad);
				Real l = sqrt(a*a + b*b +c*c); 
				joint.push_back(Discontinuity(globalOrigin));
				int i = joint.size()-1; jointNo=i;
				joint[i].a = a/l;   joint[i].b = b/l;   joint[i].c = c/l;  joint[i].d = 0.0;
				planeNormal = Vector3r(a/l,b/l,c/l);  
			}
			if(count == 3) {jointCentre.x() = valueDouble;}
			if(count == 4) {jointCentre.y() = valueDouble;}
			if(count == 5) {jointCentre.z() = valueDouble; joint[jointNo].centre = jointCentre; }
			if(count == 6) {
				
				double strike = dipdir - 90.0;
				double strikeRad = strike/180.0*PI;
				Vector3r Nstrike = Vector3r(cos(strikeRad), sin(strikeRad), 0.0);
				Vector3r Ndip = planeNormal.cross(Nstrike); Ndip.normalize();
				Matrix3r Qp=Eigen::Matrix3d::Zero();
				Qp (0,0) = Nstrike.x(); Qp(0,1) = Ndip.x(); Qp(0,2)=planeNormal.x();
				Qp (1,0) = Nstrike.y();	Qp(1,1) = Ndip.y(); Qp(1,2) = planeNormal.y();
				Qp (2,0) = Nstrike.z(); Qp(2,1) = Ndip.z(); Qp(2,2) = planeNormal.z();

				
				Vector3r rotatedPersistence(1.0,0.0,0);
				#if 0
				Vector3r oriNormal(0,0,1); //normal vector of x-y plane
				Vector3r crossProd = oriNormal.cross(planeNormal);
				Quaternionr Qp;
				Qp.w() = sqrt(oriNormal.squaredNorm() * planeNormal.squaredNorm()) + oriNormal.dot(planeNormal);
				Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
				if(crossProd.norm() < pow(10,-7)){Qp = Quaternionr::Identity();}
				Qp.normalize();
				#endif
				rotatedPersistence = Qp*rotatedPersistence;
				
				//std::cout<<"planeNormal : "<<planeNormal<<", rotatedPersistence: "<<rotatedPersistence<<endl;
				joint[jointNo].persistence_a.push_back(rotatedPersistence.x()); joint[jointNo].persistence_a.push_back(-rotatedPersistence.x()); 
				joint[jointNo].persistence_b.push_back(rotatedPersistence.y()); joint[jointNo].persistence_b.push_back(-rotatedPersistence.y());
				joint[jointNo].persistence_c.push_back(rotatedPersistence.z()); joint[jointNo].persistence_c.push_back(-rotatedPersistence.z());
				joint[jointNo].persistence_d.push_back(valueDouble);   joint[jointNo].persistence_d.push_back(valueDouble); 
			}
			if(count == 7){
				double strike = dipdir - 90.0;
				double strikeRad = strike/180.0*PI;
				Vector3r Nstrike = Vector3r(cos(strikeRad), sin(strikeRad), 0.0); std::cout<<"Nstrike: "<<Nstrike<<endl;
				Vector3r Ndip = planeNormal.cross(Nstrike); Ndip.normalize(); std::cout<<"Ndip: "<<Ndip<<endl;
				Matrix3r Qp=Eigen::Matrix3d::Zero();
				Qp (0,0) = Nstrike.x(); Qp(0,1) = Ndip.x(); Qp(0,2)=planeNormal.x();
				Qp (1,0) = Nstrike.y();	Qp(1,1) = Ndip.y(); Qp(1,2) = planeNormal.y();
				Qp (2,0) = Nstrike.z(); Qp(2,1) = Ndip.z(); Qp(2,2) = planeNormal.z();
				

				Vector3r rotatedPersistence(0.0,1.0,0.0);
				#if 0
				Vector3r oriNormal(0,0,1); //normal vector of x-y plane
				Vector3r crossProd = oriNormal.cross(planeNormal);
				Quaternionr Qp;
				Qp.w() = sqrt(oriNormal.squaredNorm() * planeNormal.squaredNorm()) + oriNormal.dot(planeNormal);
				Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
				if(crossProd.norm() < pow(10,-7)){Qp = Quaternionr::Identity();}
				Qp.normalize();
				#endif
				rotatedPersistence = Qp*rotatedPersistence;
				//std::cout<<"planeNormal : "<<planeNormal<<", rotatedPersistence: "<<rotatedPersistence<<endl;
				joint[jointNo].persistence_a.push_back(rotatedPersistence.x()); joint[jointNo].persistence_a.push_back(-rotatedPersistence.x()); 
				joint[jointNo].persistence_b.push_back(rotatedPersistence.y()); joint[jointNo].persistence_b.push_back(-rotatedPersistence.y());
				joint[jointNo].persistence_c.push_back(rotatedPersistence.z()); joint[jointNo].persistence_c.push_back(-rotatedPersistence.z());
				joint[jointNo].persistence_d.push_back(valueDouble);   joint[jointNo].persistence_d.push_back(valueDouble); 
			}
			if(count == 8){joint[jointNo].phi_b = valueDouble;}
			if(count == 9){joint[jointNo].phi_r = valueDouble;}
			if(count == 10){joint[jointNo].JRC = valueDouble;}
			if(count == 11){joint[jointNo].JCS = valueDouble;}
			if(count == 12){joint[jointNo].asperity = valueDouble;}
			if(count == 13){joint[jointNo].sigmaC = valueDouble;}
			if(count == 14){joint[jointNo].cohesion = valueDouble;}
			if(count == 15){joint[jointNo].tension = valueDouble;}
			if(count == 16){
				joint[jointNo].jointType = static_cast<int>(valueDouble);
			}
			if(count == 17){count = 0; /* to include comments */ 
				if(intactRockDegradation==true){
					joint[jointNo].intactRock = true;
				}
			}
			
			
		}
		
	}


	if(slopeFace){
		/* Read csv file for info on discontinuities */
		const char *filenameChar = filenameSlopeFace.c_str();
		ifstream file ( filenameChar ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		string value;
		/* skip first line */
		getline ( file, value); 
		int count = 0;
		Vector3r jointCentre(0,0,0);
		const double PI = std::atan(1.0)*4;
		Vector3r planeNormal(0,0,0);  int jointNo = 0;
		while ( file.good() )
		{
		    	count ++;
		    	getline ( file, value, ';' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
			//std::cout<<"count: "<<count<<", value: "<<value<<endl;
		    	const char *valueChar = value.c_str();
		    	double valueDouble = atof ( valueChar);
			if(count == 1) dip = valueDouble;
			if(count == 2) {
				dipdir = valueDouble;
				double dipdirN = 0.0;
				double dipN = 90.0-dip;
				if(dipdir > 180.0){
					dipdirN = dipdir - 180.0;
				}else{
					dipdirN = dipdir + 180.0;
				}
				Real dipRad = dipN/180.0*PI;
				Real dipdirRad = dipdirN/180.0*PI;
				Real a = cos(dipdirRad)*cos(dipRad);
				Real b = sin(dipdirRad)*cos(dipRad);
				Real c = sin(dipRad);
				Real l = sqrt(a*a + b*b +c*c);
				joint.push_back(Discontinuity(globalOrigin));
				int i = joint.size()-1; jointNo=i;
				joint[i].a = a/l;   joint[i].b = b/l;   joint[i].c = c/l;  joint[i].d = 0.0;
				planeNormal = Vector3r(a/l,b/l,c/l); 
				//std::cout<<"planeNormal: "<<planeNormal<<endl;
			}
			if(count == 3) jointCentre.x() = valueDouble;
			if(count == 4) jointCentre.y() = valueDouble;
			if(count == 5) {jointCentre.z() = valueDouble; joint[jointNo].centre = jointCentre;}
			if(count == 6) {
				Vector3r rotatedPersistence(1.0,0.0,0);
				Vector3r oriNormal(0,0,1); //normal vector of x-y plane
				Vector3r crossProd = oriNormal.cross(planeNormal);
				Quaternionr Qp;
				Qp.w() = sqrt(oriNormal.squaredNorm() * planeNormal.squaredNorm()) + oriNormal.dot(planeNormal);
				Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
				if(crossProd.norm() < pow(10,-7)){Qp = Quaternionr::Identity();}
				Qp.normalize();
				rotatedPersistence = Qp*rotatedPersistence;
				//std::cout<<"planeNormal : "<<planeNormal<<", rotatedPersistence: "<<rotatedPersistence<<endl;
				joint[jointNo].persistence_a.push_back(rotatedPersistence.x()); joint[jointNo].persistence_a.push_back(-rotatedPersistence.x()); 
				joint[jointNo].persistence_b.push_back(rotatedPersistence.y()); joint[jointNo].persistence_b.push_back(-rotatedPersistence.y());
				joint[jointNo].persistence_c.push_back(rotatedPersistence.z()); joint[jointNo].persistence_c.push_back(-rotatedPersistence.z());
				joint[jointNo].persistence_d.push_back(valueDouble);   joint[jointNo].persistence_d.push_back(valueDouble); 
			}
			if(count == 7){
				Vector3r rotatedPersistence(0.0,1.0,0.0);
				Vector3r oriNormal(0,0,1); //normal vector of x-y plane
				Vector3r crossProd = oriNormal.cross(planeNormal);
				Quaternionr Qp;
				Qp.w() = sqrt(oriNormal.squaredNorm() * planeNormal.squaredNorm()) + oriNormal.dot(planeNormal);
				Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
				if(crossProd.norm() < pow(10,-7)){Qp = Quaternionr::Identity();}
				Qp.normalize();
				rotatedPersistence = Qp*rotatedPersistence;
				//std::cout<<"planeNormal : "<<planeNormal<<", rotatedPersistence: "<<rotatedPersistence<<endl;
				joint[jointNo].persistence_a.push_back(rotatedPersistence.x()); joint[jointNo].persistence_a.push_back(-rotatedPersistence.x()); 
				joint[jointNo].persistence_b.push_back(rotatedPersistence.y()); joint[jointNo].persistence_b.push_back(-rotatedPersistence.y());
				joint[jointNo].persistence_c.push_back(rotatedPersistence.z()); joint[jointNo].persistence_c.push_back(-rotatedPersistence.z());
				joint[jointNo].persistence_d.push_back(valueDouble);   joint[jointNo].persistence_d.push_back(valueDouble); 
			}
			if(count == 8){joint[jointNo].phi_b = valueDouble;}
			if(count == 9){joint[jointNo].phi_r = valueDouble;}
			if(count == 10){joint[jointNo].JRC = valueDouble;}
			if(count == 11){joint[jointNo].JCS = valueDouble;}
			if(count == 12){joint[jointNo].asperity = valueDouble;}
			if(count == 13){joint[jointNo].sigmaC = valueDouble;}
			if(count == 14){joint[jointNo].cohesion = valueDouble;}
			if(count == 15){joint[jointNo].tension = valueDouble;joint[jointNo].throughGoing=true;joint[jointNo].constructionJoints=true; count = 0;}
			//if(intactRockDegradation==true){
			//	joint[jointNo].intactRock = true; 
			//}
		}
	}


	if(opening){
		/* Read csv file for info on discontinuities */
		const char *filenameChar = filenameOpening.c_str();
		ifstream file ( filenameChar ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		string value;
		/* skip first line */
		getline ( file, value); 
		int count = 0; int linecount = 0; 
		Vector3r jointCentre(0,0,0);
		const double PI = std::atan(1.0)*4;
		int boundaryNo = 0; int boundaryCount = 0; int abdCount=0;  Vector3r planeNormal(0,0,0);  int jointNo = 0; double persistenceA=0; double persistenceB=0; double spacing = 0;
		while ( file.good() )
		{
		    	count ++;
		    	getline ( file, value, ';' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
			//std::cout<<"count: "<<count<<", value: "<<value<<endl;
		    	const char *valueChar = value.c_str();
		    	double valueDouble = atof ( valueChar);
			if(count == 1) { planeNormal.x() = valueDouble;}
			if(count == 2) { planeNormal.y() = valueDouble;}	
			if(count == 3) { planeNormal.z() = valueDouble; planeNormal.normalize();}
			if(count == 4) {
				joint.push_back(Discontinuity(jointCentre)); 
				int i = joint.size()-1; jointNo=i; //std::cout<<"planeNormal: "<<planeNormal<<endl;
				joint[i].a = planeNormal.x();   joint[i].b = planeNormal.y();   joint[i].c = planeNormal.z();  joint[i].d = valueDouble;
			}
			if(count == 5) {
				Vector3r rotatedPersistence(1.0,0.0,0);
				Vector3r oriNormal(0,0,1); //normal vector of x-y plane
				Vector3r crossProd = oriNormal.cross(planeNormal);
				Quaternionr Qp;
				Qp.w() = sqrt(oriNormal.squaredNorm() * planeNormal.squaredNorm()) + oriNormal.dot(planeNormal);
				Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
				if(crossProd.norm() < pow(10,-7)){Qp = Quaternionr::Identity();}
				Qp.normalize();
				rotatedPersistence = Qp*rotatedPersistence;
				rotatedPersistence.normalize();
				//std::cout<<"planeNormal : "<<planeNormal<<", rotatedPersistence: "<<rotatedPersistence<<", check orthogonal: "<<planeNormal.dot(rotatedPersistence)<<endl;
				joint[jointNo].persistence_a.push_back(rotatedPersistence.x()); joint[jointNo].persistence_a.push_back(-rotatedPersistence.x()); 
				joint[jointNo].persistence_b.push_back(rotatedPersistence.y()); joint[jointNo].persistence_b.push_back(-rotatedPersistence.y());
				joint[jointNo].persistence_c.push_back(rotatedPersistence.z()); joint[jointNo].persistence_c.push_back(-rotatedPersistence.z());
				joint[jointNo].persistence_d.push_back(valueDouble);   joint[jointNo].persistence_d.push_back(valueDouble); 
			}
			if(count == 6){
				Vector3r rotatedPersistence(0.0,1.0,0.0);
				Vector3r oriNormal(0,0,1); //normal vector of x-y plane
				Vector3r crossProd = oriNormal.cross(planeNormal);
				Quaternionr Qp;
				Qp.w() = sqrt(oriNormal.squaredNorm() * planeNormal.squaredNorm()) + oriNormal.dot(planeNormal);
				Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
				if(crossProd.norm() < pow(10,-7)){Qp = Quaternionr::Identity();}
				Qp.normalize();
				rotatedPersistence = Qp*rotatedPersistence;
				rotatedPersistence.normalize();
				//std::cout<<"planeNormal : "<<planeNormal<<", rotatedPersistence: "<<rotatedPersistence<<", check orthogonal: "<<planeNormal.dot(rotatedPersistence)<<endl;
				joint[jointNo].persistence_a.push_back(rotatedPersistence.x()); joint[jointNo].persistence_a.push_back(-rotatedPersistence.x()); 
				joint[jointNo].persistence_b.push_back(rotatedPersistence.y()); joint[jointNo].persistence_b.push_back(-rotatedPersistence.y());
				joint[jointNo].persistence_c.push_back(rotatedPersistence.z()); joint[jointNo].persistence_c.push_back(-rotatedPersistence.z());
				joint[jointNo].persistence_d.push_back(valueDouble);   joint[jointNo].persistence_d.push_back(valueDouble); 
			}
			if(count == 7){joint[jointNo].phi_b = valueDouble;}
			if(count == 8){joint[jointNo].phi_r = valueDouble;}
			if(count == 9){joint[jointNo].JRC = valueDouble;}
			if(count == 10){joint[jointNo].JCS = valueDouble;}
			if(count == 11){joint[jointNo].asperity = valueDouble;}
			if(count == 12){joint[jointNo].sigmaC = valueDouble;}
			if(count == 13){joint[jointNo].cohesion = valueDouble;}
			if(count == 14){joint[jointNo].tension = valueDouble;joint[jointNo].throughGoing=true;joint[jointNo].constructionJoints=true; count = 0;}
		}
	}

	
std::cout<<"joint_a.size(): "<<joint_a.size()<<", joint size: "<<joint.size()<<endl;
	


	/* Perform contact detection between discontinuity and blocks.  For any blocks that touch the plane, split it into half */
	for (unsigned int j=0; j<joint.size(); j++){
		std::cout<<"Slicing progress.... joint no: "<<j+1<<"/"<<joint.size()<<endl;
		int blkOriCount = blk.size();
		for (int i=0; i<blkOriCount; i++){
			int subMemberSize = blk[i].subMembers.size();
			int subMemberIter = 0;
			Block presentBlock = Block(blk[i].centre,kForPP,rForPP,RForPP );
			 do{
				
				if (subMemberSize == 0){
					presentBlock = blk[i];
				}else{
					presentBlock=blk[i].subMembers[subMemberIter];
				}
				
				/* Fast contact detection */
				Vector3r vertexFrJoint(0,0,0);
				if(joint[j].throughGoing == false){
					double twoRadiiDist = 1.2*(presentBlock.R+ std::max(fabs(joint[j].persistence_d[0]), fabs(joint[j].persistence_d[2])));
					double centroidDist = (joint[j].centre-presentBlock.tempCentre).norm();
					if(centroidDist > twoRadiiDist ){subMemberIter++; continue;}
					/* std::cout<<"centroidDist: "<<centroidDist<<", twoRadiiDist: "<<twoRadiiDist<<", presentBlock.R: "<<presentBlock.R<<", presentBlock.tempCentre: "<<presentBlock.tempCentre<<", joint.centre: "<<joint[j].centre<<endl; */
				}
				
				
				if (contactDetectionLPCLPglobal(joint[j], presentBlock,vertexFrJoint) && presentBlock.tooSmall==false ){
					std::cout<<"joint["<<j<<"] sliced successful"<<endl;

					if(presentBlock.isBoundary == true && joint[j].sliceBoundaries == false){ continue;}
					if(presentBlock.isBoundary == false && joint[j].sliceBoundaries == true){ continue;}
					
					Real fns = evaluateFNoSphere(presentBlock, vertexFrJoint);
					//std::cout<<"fns: "<<fns<<endl;		
					/* Split the block into two */ //shrink d

					/* Make a copy of parent block */
					int blkNo = blk.size(); /*size = currentCount+1 */
					Block blkA = Block(presentBlock.centre,kForPP,rForPP,RForPP );
					Block blkB = Block(presentBlock.centre,kForPP,rForPP,RForPP );
					blkA = presentBlock;
					blkB = presentBlock;
					int planeNo = presentBlock.a.size();
					/* Add plane from joint to new block */
					blkB.a.push_back(-1.0*joint[j].a);
					blkB.b.push_back(-1.0*joint[j].b);
					blkB.c.push_back(-1.0*joint[j].c);
					Real newNegD= (1.0*joint[j].a*(blkB.centre.x()-joint[j].centre.x()) + 1.0*joint[j].b*(blkB.centre.y()-joint[j].centre.y()) +1.0* joint[j].c*(blkB.centre.z()-joint[j].centre.z())  - joint[j].d)  ;		
					blkB.d.push_back(newNegD  - shrinkFactor* blkB.r ); /*shrink */
					blkB.redundant.push_back(false);
					blkB.JRC.push_back(joint[j].JRC);
					blkB.JCS.push_back(joint[j].JCS);
					blkB.sigmaC.push_back(joint[j].sigmaC);
					blkB.phi_r.push_back(joint[j].phi_r);
					blkB.phi_b.push_back(joint[j].phi_b);
					blkB.asperity.push_back(joint[j].asperity);
					blkB.cohesion.push_back(joint[j].cohesion);
					blkB.tension.push_back(joint[j].tension);
					blkB.isBoundaryPlane.push_back(joint[j].isBoundary);
					blkB.lambda0.push_back(joint[j].lambda0);
					blkB.heatCapacity.push_back(joint[j].heatCapacity);
					blkB.hwater.push_back(joint[j].hwater);
					blkB.intactRock.push_back(joint[j].intactRock);
					blkB.jointType.push_back(joint[j].jointType);
					if(joint[j].isBoundary == true){
						for(int k=0; k<planeNo; k++){ /*planeNo is previous size before adding the new plane */
							blkB.isBoundaryPlane[k] = false;
						} 
					}
				
					/* Add plane from joint to parent block */
					blkA.a.push_back(joint[j].a);
					blkA.b.push_back(joint[j].b);
					blkA.c.push_back(joint[j].c);
					Real newPosD= -1.0*(joint[j].a*(blkA.centre.x()-joint[j].centre.x()) + joint[j].b*(blkA.centre.y()-joint[j].centre.y()) + joint[j].c*(blkA.centre.z()-joint[j].centre.z())  - joint[j].d);
					blkA.d.push_back(newPosD   -  shrinkFactor*blkA.r );  /*shrink */	
					blkA.redundant.push_back(false);
					blkA.JRC.push_back(joint[j].JRC);
					blkA.JCS.push_back(joint[j].JCS);
					blkA.sigmaC.push_back(joint[j].sigmaC);
					blkA.phi_r.push_back(joint[j].phi_r);
					blkA.phi_b.push_back(joint[j].phi_b);
					blkA.asperity.push_back(joint[j].asperity);
					blkA.cohesion.push_back(joint[j].cohesion);
					blkA.tension.push_back(joint[j].tension);
					blkA.isBoundaryPlane.push_back(joint[j].isBoundary);
					blkA.lambda0.push_back(joint[j].lambda0);
					blkA.heatCapacity.push_back(joint[j].heatCapacity);
					blkA.hwater.push_back(joint[j].hwater);
					blkA.intactRock.push_back(joint[j].intactRock);
					blkA.jointType.push_back(joint[j].jointType);
					if(joint[j].isBoundary == true){
						for(int k=0; k<planeNo; k++){ /*planeNo is previous size before adding the new plane */
							blkA.isBoundaryPlane[k] = false;
						} 
					}
					//std::cout<<"detected"<<endl;
#if 0
					/* Adjust block centroid, after every discontinuity is introduced */
					Vector3r startingPt = Vector3r(0,0,0); //centroid
					bool converge = startingPointFeasibility(blkA, startingPt);
					Vector3r centroid = blkA.centre+startingPt;
					Vector3r prevCentre = blkA.centre; 
					blkA.centre = centroid;
					Real maxd=0.0;
					for(unsigned int h=0; h<blkA.a.size(); h++){
						blkA.d[h]= -1.0*(blkA.a[h]*(centroid.x()-prevCentre.x()) + blkA.b[h]*(centroid.y()-prevCentre.y()) + blkA.c[h]*(centroid.z()-prevCentre.z())  - blkA.d[h]);	
						if(blkA.d[h] < 0.0){
							std::cout<<"blkA.d[h]: "<<blkA.d[h]<<endl;
						}
						if(fabs(blkA.d[h] )> maxd){maxd= fabs(blkA.d[h]);}
					}
					if(converge== false){ 
						blkA.tooSmall = true;
						bool inside = checkCentroid(blkA,blkA.centre);
						std::cout<<"blki inside: "<<inside<<endl;					
					}
					blkA.R = 1.2*maxd;
			
					/* Adjust block centroid, after every discontinuity is introduced */
					startingPt = Vector3r(0,0,0); maxd = 0.0;
					converge = startingPointFeasibility(blkB, startingPt);
					prevCentre = blkB.centre; 
					centroid = blkB.centre+startingPt;
					blkB.centre = centroid;
					for(unsigned int h=0; h<blkB.a.size(); h++){
						blkB.d[h]= -1.0*(blkB.a[h]*(centroid.x()-prevCentre.x()) + blkB.b[h]*(centroid.y()-prevCentre.y()) + blkB.c[h]*(centroid.z()-prevCentre.z())  - blkB.d[h]);	
						if(blkB.d[h] < 0.0){
							std::cout<<"blkB.d[h]: "<<blkB.d[h]<<endl;
						}
						if(fabs(blkB.d[h] )> maxd){maxd=fabs( blkB.d[h]);}
					}
					if(converge== false){ 
						blkB.tooSmall = true;
						bool inside = checkCentroid(blkB,blkB.centre);
						std::cout<<"blkNo inside: "<<inside<<endl;					
					}
					blkB.R = 1.2*maxd;

			
					/* Identify redundant planes */
					for(int k=0; k<planeNo; k++){//The last plane is the new discontinuity.  It is definitely part of the new blocks.
						Discontinuity plane=Discontinuity(blkA.centre);
						plane.a=blkA.a[k];
						plane.b=blkA.b[k];
						plane.c=blkA.c[k];
						plane.d=blkA.d[k];
						Vector3r falseVertex (0.0,0.0,0.0);
					
						if (!checkRedundancyLP(plane, blkA,falseVertex) ){
							blkA.redundant[k] = true;
						}
						Discontinuity plane2=Discontinuity(blkB.centre);
						plane2.a=blkB.a[k];
						plane2.b=blkB.b[k];
						plane2.c=blkB.c[k];
						plane2.d=blkB.d[k];
						if (!checkRedundancyLP(plane2, blkB,falseVertex) ){
							blkB.redundant[k] = true;
						}
					}
		
					/* Remove redundant planes */
					unsigned int no = 0;
					while(no<blkA.a.size()){ //The last plane is the new discontinuity.  It is definitely part of the new blocks.
						//std::cout<<"no: "<<no<<", a[no]: "<<blkA.a[no]<<" redundant: "<<blkA.redundant[no]<<endl;
						if(blkA.redundant[no] == true){
							blkA.a.erase(blkA.a.begin()+no);
							blkA.b.erase(blkA.b.begin()+no);
							blkA.c.erase(blkA.c.begin()+no);
							blkA.d.erase(blkA.d.begin()+no);
							blkA.redundant.erase(blkA.redundant.begin()+no);
							blkA.JRC.erase(blkA.JRC.begin()+no);
							blkA.JCS.erase(blkA.JCS.begin()+no);
							blkA.sigmaC.erase(blkA.sigmaC.begin()+no);
							blkA.phi_r.erase(blkA.phi_r.begin()+no);
							blkA.phi_b.erase(blkA.phi_b.begin()+no);
							blkA.asperity.erase(blkA.asperity.begin()+no);
							blkA.cohesion.erase(blkA.cohesion.begin()+no);
							blkA.tension.erase(blkA.tension.begin()+no);
							blkA.isBoundaryPlane.erase(blkA.isBoundaryPlane.begin()+no);
							blkA.lambda0.erase(blkA.lambda0.begin()+no);
							blkA.heatCapacity.erase(blkA.heatCapacity.begin()+no);
							blkA.hwater.erase(blkA.hwater.begin()+no);
							blkA.intactRock.erase(blkA.intactRock.begin()+no);
							blkA.jointType.erase(blkA.jointType.begin()+no);
							no = 0;
						}else{
							no = no+1;
						}
					
						
					}
			
					no = 0;
					while(no<blkB.a.size()){ //The 1st plane is the new discontinuity.  It is definitely part of the new blocks.
						//std::cout<<"no: "<<no<<", a[no]: "<<blkB.a[no]<<" redundant: "<<blkB.redundant[no]<<endl;
						if(blkB.redundant[no] == true){
							blkB.a.erase(blkB.a.begin()+no);
							blkB.b.erase(blkB.b.begin()+no);
							blkB.c.erase(blkB.c.begin()+no);
							blkB.d.erase(blkB.d.begin()+no);
							blkB.redundant.erase(blkB.redundant.begin()+no);
							blkB.JRC.erase(blkB.JRC.begin()+no);
							blkB.JCS.erase(blkB.JCS.begin()+no);
							blkB.sigmaC.erase(blkB.sigmaC.begin()+no);
							blkB.phi_r.erase(blkB.phi_r.begin()+no);
							blkB.phi_b.erase(blkB.phi_b.begin()+no);
							blkB.asperity.erase(blkB.asperity.begin()+no);
							blkB.cohesion.erase(blkB.cohesion.begin()+no);
							blkB.tension.erase(blkB.tension.begin()+no);
							blkB.isBoundaryPlane.erase(blkB.isBoundaryPlane.begin()+no);
							blkB.lambda0.erase(blkB.lambda0.begin()+no);
							blkB.heatCapacity.erase(blkB.heatCapacity.begin()+no);
							blkB.hwater.erase(blkB.hwater.begin()+no);
							blkB.intactRock.erase(blkB.intactRock.begin()+no);
							blkB.jointType.erase(blkB.jointType.begin()+no);
							no = 0;
						}else{
							no = no+1;
						}
			
					}
	#endif		

					/* Identify boundary blocks */
					if(joint[j].isBoundary == true){
						Vector3r startingPt = Vector3r(0,0,0); //centroid
						//bool converge=startingPointFeasibility(blkA, startingPt);
						double radius  = inscribedSphereCLP(blkA, startingPt, twoDimension);						
						Vector3r centroidA = blkA.centre+startingPt;
						double fA = joint[j].a*(centroidA.x()-joint[j].centre.x()) + joint[j].b*(centroidA.y()-joint[j].centre.y()) + joint[j].c*(centroidA.z()-joint[j].centre.z()) - joint[j].d;
						if (fA>0.0){
							blkA.isBoundary = true;
						}else{
							blkB.isBoundary = true;
						}
					}

					
					double RblkA = 0.0;  Vector3r tempCentreA(0,0,0);
					double RblkB = 0.0;  Vector3r tempCentreB(0,0,0);
					/* Prune blocks that are too small or too elongated */
					bool tooSmall = false; 
					if(joint[j].throughGoing == false){
						double conditioningFactor = 1.0;
						Real minX = 0.0; Real minY = 0.0; Real minZ = 0.0;
						Real maxX = 0.0; Real maxY = 0.0; Real maxZ = 0.0;
					
						Discontinuity plane=Discontinuity(Vector3r(0,0,0));
						plane.a=directionA.x(); //1.0;
						plane.b=directionA.y(); //0.0;
						plane.c=directionA.z(); //0.0;
						plane.d=1.2*boundarySizeXmax;
						Vector3r falseVertex (0.0,0.0,0.0);
						if (contactBoundaryLPCLP(plane, blkA,falseVertex) ){	
							minX = directionA.dot(falseVertex); //falseVertex.x();
						}else{tooSmall=true;}
						tempCentreA = tempCentreA + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=directionB.x(); //0.0;
						plane.b=directionB.y(); //1.0;
						plane.c=directionB.z(); //0.0;
						plane.d=1.2*boundarySizeYmax;
						if (contactBoundaryLPCLP(plane, blkA,falseVertex) ){	
							minY = directionB.dot(falseVertex); //falseVertex.y();
						}else{tooSmall=true;}
						tempCentreA = tempCentreA + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=directionC.x(); //0.0;
						plane.b=directionC.y(); //0.0;
						plane.c=directionC.z(); //1.0;
						plane.d=1.2*boundarySizeZmax;
						if (contactBoundaryLPCLP(plane, blkA,falseVertex) ){	
							minZ = directionC.dot(falseVertex); //falseVertex.z();
						}else{tooSmall=true;}
						tempCentreA = tempCentreA + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=-directionA.x(); //-1.0;
						plane.b=-directionA.y(); //0.0;
						plane.c=-directionA.z(); //0.0;
						plane.d=1.2*boundarySizeXmin;
						if (contactBoundaryLPCLP(plane, blkA,falseVertex) ){	
							maxX = directionA.dot(falseVertex); //falseVertex.x();
						}else{tooSmall=true;}
						tempCentreA = tempCentreA + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=-directionB.x(); //0.0;
						plane.b=-directionB.y(); //-1.0;
						plane.c=-directionB.z(); //0.0;
						plane.d=1.2*boundarySizeYmin;
						if (contactBoundaryLPCLP(plane, blkA,falseVertex) ){	
							maxY = directionB.dot(falseVertex); //falseVertex.y();
						}else{tooSmall=true;}
						tempCentreA = tempCentreA + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=-directionC.x(); // 0.0;
						plane.b=-directionC.y(); //0.0;
						plane.c=-directionC.z(); //-1.0;
						plane.d=1.2*boundarySizeZmin;
						if (contactBoundaryLPCLP(plane,blkA,falseVertex) ){	
							maxZ = directionC.dot(falseVertex); //falseVertex.z();
						}else{tooSmall=true;}
						tempCentreA = tempCentreA + falseVertex;
					
						
						Real maxXoverall = fabs(maxX-minX);
						Real maxYoverall = fabs(maxY-minY);
						Real maxZoverall = fabs(maxZ-minZ);
						tempCentreA = 1.0/6.0*tempCentreA; Vector3r tempA(0,0,0);
						double chebyshevRa = inscribedSphereCLP(blkA, tempA, twoDimension); tempCentreA = tempA; //std::cout<<"chebyshevRa: "<<chebyshevRa<<endl;
						RblkA = sqrt(maxXoverall*maxXoverall + maxYoverall*maxYoverall + maxZoverall*maxZoverall);
						std::cout<<"blockA, minX: "<<minX<<", maxX: "<<maxX<<", minY: "<<minY<<", maxY: "<<maxY<<", minZ: "<<minZ<<", maxZ: "<<maxZ<<", RblkA: "<<RblkA<<", tempCentreA: "<<tempCentreA<<endl;
						if (2.0*chebyshevRa < minSize){ //(maxXoverall < minSize || maxZoverall < minSize){
							tooSmall = true; 
						}
					
						if(maxXoverall/(2.0*chebyshevRa) > maxRatio || maxYoverall/(2.0*chebyshevRa) > maxRatio || maxZoverall/(2.0*chebyshevRa) > maxRatio){
							tooSmall = true;
						}
					
						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=directionA.x(); //1.0;
						plane.b=directionA.y(); //0.0;
						plane.c=directionA.z(); //0.0;
						plane.d=1.2*boundarySizeXmax;
						if (contactBoundaryLPCLP(plane, blkB,falseVertex) ){	
							minX = directionA.dot(falseVertex); //falseVertex.x();
						}else{tooSmall=true;}
						tempCentreB = tempCentreB + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=directionB.x(); //0.0;
						plane.b=directionB.y(); //1.0;
						plane.c=directionB.z(); //0.0;
						plane.d=1.2*boundarySizeYmax;
						if (contactBoundaryLPCLP(plane, blkB,falseVertex) ){	
							minY = directionB.dot(falseVertex); //falseVertex.y();
						}else{tooSmall=true;}
						tempCentreB = tempCentreB + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=directionC.x(); //0.0;
						plane.b=directionC.y(); //0.0;
						plane.c=directionC.z(); //1.0;
						plane.d=1.2*boundarySizeZmax;
						if (contactBoundaryLPCLP(plane, blkB,falseVertex) ){	
							minZ = directionC.dot(falseVertex); //falseVertex.z();
						}else{tooSmall=true;}
						tempCentreB = tempCentreB + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=-directionA.x(); //-1.0;
						plane.b=-directionA.y(); //0.0;
						plane.c=-directionA.z(); //0.0;
						plane.d=1.2*boundarySizeXmin;
						if (contactBoundaryLPCLP(plane, blkB,falseVertex) ){	
							maxX = directionA.dot(falseVertex); //falseVertex.x();
						}else{tooSmall=true;}
						tempCentreB = tempCentreB + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=-directionB.x(); //0.0;
						plane.b=-directionB.y(); //-1.0;
						plane.c=-directionB.z(); //0.0;
						plane.d=1.2*boundarySizeYmin;
						if (contactBoundaryLPCLP(plane, blkB,falseVertex) ){	
							maxY = directionB.dot(falseVertex); //falseVertex.y();
						}else{tooSmall=true;}
						tempCentreB = tempCentreB + falseVertex;

						plane=Discontinuity(Vector3r(0,0,0));
						plane.a=-directionC.x(); //0.0;
						plane.b=-directionC.y(); //0.0;
						plane.c=-directionC.z(); //-1.0;
						plane.d=1.2*boundarySizeZmin;
						if (contactBoundaryLPCLP(plane,blkB,falseVertex) ){	
							maxZ = directionC.dot(falseVertex); //falseVertex.z();
						}else{tooSmall=true;}
						tempCentreB = tempCentreB + falseVertex;
					
						
						maxXoverall = fabs(maxX-minX);
						maxYoverall = fabs(maxY-minY);
						maxZoverall = fabs(maxZ-minZ);
						tempCentreB = 1.0/6.0*tempCentreB; Vector3r tempB(0,0,0);
						double chebyshevRb = inscribedSphereCLP(blkB, tempB, twoDimension); tempCentreB = tempB; //std::cout<<"chebyshevRb: "<<chebyshevRb<<endl;
						RblkB = sqrt(maxXoverall*maxXoverall + maxYoverall*maxYoverall + maxZoverall*maxZoverall);
						std::cout<<"blockB, minX: "<<minX<<", maxX: "<<maxX<<", minY: "<<minY<<", maxY: "<<maxY<<", minZ: "<<minZ<<", maxZ: "<<maxZ<<", RblkB: "<<RblkB<<", tempCentreB: "<<tempCentreB<<endl;

						if(chebyshevRa < 0.0 || chebyshevRb < 0.0){
							std::cout<<"1 chebyshevRa: "<<chebyshevRa<<", chebyshevRb: "<<chebyshevRb<<endl;
							tooSmall = true; 
						}
						if (2.0*chebyshevRb < minSize){//(maxXoverall < minSize || maxZoverall < minSize){
							std::cout<<"2 chebyshevRa: "<<chebyshevRa<<", chebyshevRb: "<<chebyshevRb<<endl;
							tooSmall = true; 
						}
					
						if(maxXoverall/(2.0*chebyshevRb) > maxRatio || maxYoverall/(2.0*chebyshevRb) > maxRatio || maxZoverall/(2.0*chebyshevRb) > maxRatio){
							tooSmall = true;
						}
						
					}
					
					
					if(tooSmall== false){
					#if 0
						/* Identify redundant planes */
						for(int k=0; k<planeNo; k++){//The last plane is the new discontinuity.  It is definitely part of the new blocks.
							Discontinuity plane=Discontinuity(blkA.centre);
							plane.a=blkA.a[k];
							plane.b=blkA.b[k];
							plane.c=blkA.c[k];
							plane.d=blkA.d[k];
							Vector3r falseVertex (0.0,0.0,0.0);
					
							if (!checkRedundancyLP(plane, blkA,falseVertex) ){
								blkA.redundant[k] = true;
							}
							Discontinuity plane2=Discontinuity(blkB.centre);
							plane2.a=blkB.a[k];
							plane2.b=blkB.b[k];
							plane2.c=blkB.c[k];
							plane2.d=blkB.d[k];
							if (!checkRedundancyLP(plane2, blkB,falseVertex) ){
								blkB.redundant[k] = true;
							}
						}
		
						/* Remove redundant planes */
						unsigned int no = 0;
						while(no<blkA.a.size()){ //The last plane is the new discontinuity.  It is definitely part of the new blocks.
							//std::cout<<"no: "<<no<<", a[no]: "<<blkA.a[no]<<" redundant: "<<blkA.redundant[no]<<endl;
							if(blkA.redundant[no] == true){
								blkA.a.erase(blkA.a.begin()+no);
								blkA.b.erase(blkA.b.begin()+no);
								blkA.c.erase(blkA.c.begin()+no);
								blkA.d.erase(blkA.d.begin()+no);
								blkA.redundant.erase(blkA.redundant.begin()+no);
								blkA.JRC.erase(blkA.JRC.begin()+no);
								blkA.JCS.erase(blkA.JCS.begin()+no);
								blkA.sigmaC.erase(blkA.sigmaC.begin()+no);
								blkA.phi_r.erase(blkA.phi_r.begin()+no);
								blkA.phi_b.erase(blkA.phi_b.begin()+no);
								blkA.asperity.erase(blkA.asperity.begin()+no);
								blkA.cohesion.erase(blkA.cohesion.begin()+no);
								blkA.tension.erase(blkA.tension.begin()+no);
								blkA.isBoundaryPlane.erase(blkA.isBoundaryPlane.begin()+no);
								blkA.lambda0.erase(blkA.lambda0.begin()+no);
								blkA.heatCapacity.erase(blkA.heatCapacity.begin()+no);
								blkA.hwater.erase(blkA.hwater.begin()+no);
								blkA.intactRock.erase(blkA.intactRock.begin()+no);
								blkA.jointType.erase(blkA.jointType.begin()+no);
								no = 0;
							}else{
								no = no+1;
							}
					
						
						}
			
						no = 0;
						while(no<blkB.a.size()){ //The 1st plane is the new discontinuity.  It is definitely part of the new blocks.
							//std::cout<<"no: "<<no<<", a[no]: "<<blkB.a[no]<<" redundant: "<<blkB.redundant[no]<<endl;
							if(blkB.redundant[no] == true){
								blkB.a.erase(blkB.a.begin()+no);
								blkB.b.erase(blkB.b.begin()+no);
								blkB.c.erase(blkB.c.begin()+no);
								blkB.d.erase(blkB.d.begin()+no);
								blkB.redundant.erase(blkB.redundant.begin()+no);
								blkB.JRC.erase(blkB.JRC.begin()+no);
								blkB.JCS.erase(blkB.JCS.begin()+no);
								blkB.sigmaC.erase(blkB.sigmaC.begin()+no);
								blkB.phi_r.erase(blkB.phi_r.begin()+no);
								blkB.phi_b.erase(blkB.phi_b.begin()+no);
								blkB.asperity.erase(blkB.asperity.begin()+no);
								blkB.cohesion.erase(blkB.cohesion.begin()+no);
								blkB.tension.erase(blkB.tension.begin()+no);
								blkB.isBoundaryPlane.erase(blkB.isBoundaryPlane.begin()+no);
								blkB.lambda0.erase(blkB.lambda0.begin()+no);
								blkB.heatCapacity.erase(blkB.heatCapacity.begin()+no);
								blkB.hwater.erase(blkB.hwater.begin()+no);
								blkB.intactRock.erase(blkB.intactRock.begin()+no);
								blkB.jointType.erase(blkB.jointType.begin()+no);
								no = 0;
							}else{
								no = no+1;
							}
			
						}
					#endif
						if(joint[j].constructionJoints==true){
							if(subMemberSize>0){
								blk[i].subMembers.push_back(Block(blkB.centre,kForPP,rForPP,RForPP ));
								int subMemberCount = blk[i].subMembers.size()-1;
								blk[i].subMembers[subMemberIter] = blkA;
								blk[i].subMembers[subMemberCount] = blkB;
								blk[i].subMembers[subMemberIter].R = RblkA;
								blk[i].subMembers[subMemberCount].R = RblkB;
								blk[i].subMembers[subMemberIter].tempCentre = tempCentreA;
								blk[i].subMembers[subMemberCount].tempCentre = tempCentreB;
							}else{
								blk[i].subMembers.push_back(Block(blkA.centre,kForPP,rForPP,RForPP ));
								blk[i].subMembers.push_back(Block(blkB.centre,kForPP,rForPP,RForPP ));
								int subMemberCount = blk[i].subMembers.size()-2;
								blk[i].subMembers[subMemberCount] = blkA;
								blk[i].subMembers[subMemberCount+1] = blkB;
								blk[i].subMembers[subMemberCount].R = RblkA;
								blk[i].subMembers[subMemberCount+1].R = RblkB;
								blk[i].subMembers[subMemberCount].tempCentre = tempCentreA;
								blk[i].subMembers[subMemberCount+1].tempCentre = tempCentreB;
							}
						}else{
							blk.push_back(Block(blkB.centre,kForPP,rForPP,RForPP ));
							blk[blkNo] = blkB;
							blk[i] = blkA;
							blk[blkNo].R = RblkB;
							blk[i].R=RblkA;
							blk[blkNo].tempCentre = tempCentreB;
							blk[i].tempCentre = tempCentreA;
						}	
					
					}
				}/* outer if-braces for detected */
				
				subMemberIter++;

			}while(subMemberIter<subMemberSize);
		}/* outer loop for block */

		

	}/* outer loop for joint */
	


	/* NEW Find temp centre and remove redundant planes */
	for (int i=0; i<blk.size(); i++){
		std::cout<<"Redundancy progress.... block no: "<<i+1<<"/"<<blk.size()<<endl;
		if(blk[i].subMembers.size()>0){
			for(int j=0; j<blk[i].subMembers.size();j++){
				/* Adjust block centroid, after every discontinuity is introduced */
					Vector3r startingPt = blk[i].subMembers[j].centre; //centroid
					
					//bool converge = startingPointFeasibility(blk[i].subMembers[j], startingPt);
					double radius = inscribedSphereCLP(blk[i].subMembers[j], startingPt, twoDimension);
					bool converge = true;
					if (radius < 0.0){converge = false;}					
					Vector3r centroid = blk[i].subMembers[j].centre+startingPt;
					Vector3r prevCentre = blk[i].subMembers[j].centre; 
					blk[i].subMembers[j].centre = centroid;
					Real maxd=0.0;
					for(unsigned int h=0; h<blk[i].subMembers[j].a.size(); h++){
						blk[i].subMembers[j].d[h]= -1.0*(blk[i].subMembers[j].a[h]*(centroid.x()-prevCentre.x()) + blk[i].subMembers[j].b[h]*(centroid.y()-prevCentre.y()) + blk[i].subMembers[j].c[h]*(centroid.z()-prevCentre.z())  - blk[i].subMembers[j].d[h]);	
						if(blk[i].subMembers[j].d[h] < 0.0){
							std::cout<<"blk.d[h]: "<<blk[i].subMembers[j].d[h]<<endl;
						}
						if(fabs(blk[i].subMembers[j].d[h] )> maxd){maxd= fabs(blk[i].subMembers[j].d[h]);}
					}
					if(converge== false){ 
						blk[i].subMembers[j].tooSmall = true;
						bool inside = checkCentroid(blk[i].subMembers[j],blk[i].subMembers[j].centre);
						std::cout<<"blki inside: "<<inside<<endl;					
					}
					blk[i].subMembers[j].R = 1.2*maxd;

			
					/* Identify redundant planes */
					for(int k=0; k<blk[i].subMembers[j].a.size(); k++){//The last plane is the new discontinuity.  It is definitely part of the new blocks.
						Discontinuity plane=Discontinuity(blk[i].subMembers[j].centre);
						plane.a=blk[i].subMembers[j].a[k];
						plane.b=blk[i].subMembers[j].b[k];
						plane.c=blk[i].subMembers[j].c[k];
						plane.d=blk[i].subMembers[j].d[k];
						Vector3r falseVertex (0.0,0.0,0.0);
					
						if (!checkRedundancyLPCLP(plane, blk[i].subMembers[j],falseVertex) ){
							blk[i].subMembers[j].redundant[k] = true;
						}
						
					}
		
					/* Remove redundant planes */
					unsigned int no = 0;
					while(no<blk[i].subMembers[j].a.size()){ //The last plane is the new discontinuity.  It is definitely part of the new blocks.
						//std::cout<<"no: "<<no<<", a[no]: "<<blk.a[no]<<" redundant: "<<blk.redundant[no]<<endl;
						if(blk[i].subMembers[j].redundant[no] == true){
							blk[i].subMembers[j].a.erase(blk[i].subMembers[j].a.begin()+no);
							blk[i].subMembers[j].b.erase(blk[i].subMembers[j].b.begin()+no);
							blk[i].subMembers[j].c.erase(blk[i].subMembers[j].c.begin()+no);
							blk[i].subMembers[j].d.erase(blk[i].subMembers[j].d.begin()+no);
							blk[i].subMembers[j].redundant.erase(blk[i].subMembers[j].redundant.begin()+no);
							blk[i].subMembers[j].JRC.erase(blk[i].subMembers[j].JRC.begin()+no);
							blk[i].subMembers[j].JCS.erase(blk[i].subMembers[j].JCS.begin()+no);
							blk[i].subMembers[j].sigmaC.erase(blk[i].subMembers[j].sigmaC.begin()+no);
							blk[i].subMembers[j].phi_r.erase(blk[i].subMembers[j].phi_r.begin()+no);
							blk[i].subMembers[j].phi_b.erase(blk[i].subMembers[j].phi_b.begin()+no);
							blk[i].subMembers[j].asperity.erase(blk[i].subMembers[j].asperity.begin()+no);
							blk[i].subMembers[j].cohesion.erase(blk[i].subMembers[j].cohesion.begin()+no);
							blk[i].subMembers[j].tension.erase(blk[i].subMembers[j].tension.begin()+no);
							blk[i].subMembers[j].isBoundaryPlane.erase(blk[i].subMembers[j].isBoundaryPlane.begin()+no);
							blk[i].subMembers[j].lambda0.erase(blk[i].subMembers[j].lambda0.begin()+no);
							blk[i].subMembers[j].heatCapacity.erase(blk[i].subMembers[j].heatCapacity.begin()+no);
							blk[i].subMembers[j].hwater.erase(blk[i].subMembers[j].hwater.begin()+no);
							blk[i].subMembers[j].intactRock.erase(blk[i].subMembers[j].intactRock.begin()+no);
							blk[i].subMembers[j].jointType.erase(blk[i].subMembers[j].jointType.begin()+no);
							no = 0;
						}else{
							no = no+1;
						}
					
						
					}

			}
		}else{
			/* Adjust block centroid, after every discontinuity is introduced */
					Vector3r startingPt = blk[i].centre; //centroid
					
					//bool converge = startingPointFeasibility(blk[i], startingPt);
					double radius = inscribedSphereCLP(blk[i], startingPt, twoDimension);					
					bool converge = true;
					if (radius < 0.0){converge = false;}
					Vector3r centroid = blk[i].centre+startingPt;
					Vector3r prevCentre = blk[i].centre; 
					blk[i].centre = centroid;
					Real maxd=0.0;
					for(unsigned int h=0; h<blk[i].a.size(); h++){
						blk[i].d[h]= -1.0*(blk[i].a[h]*(centroid.x()-prevCentre.x()) + blk[i].b[h]*(centroid.y()-prevCentre.y()) + blk[i].c[h]*(centroid.z()-prevCentre.z())  - blk[i].d[h]);	
						if(blk[i].d[h] < 0.0){
							std::cout<<"blk.d[h]: "<<blk[i].d[h]<<endl;
						}
						if(fabs(blk[i].d[h] )> maxd){maxd= fabs(blk[i].d[h]);}
					}
					if(converge== false){ 
						blk[i].tooSmall = true;
						bool inside = checkCentroid(blk[i],blk[i].centre);
						std::cout<<"blki inside: "<<inside<<endl;					
					}
					blk[i].R = 1.2*maxd;

			
					/* Identify redundant planes */
					for(int k=0; k<blk[i].a.size(); k++){//The last plane is the new discontinuity.  It is definitely part of the new blocks.
						Discontinuity plane=Discontinuity(blk[i].centre);
						plane.a=blk[i].a[k];
						plane.b=blk[i].b[k];
						plane.c=blk[i].c[k];
						plane.d=blk[i].d[k];
						Vector3r falseVertex (0.0,0.0,0.0);
					
						if (!checkRedundancyLPCLP(plane, blk[i],falseVertex) ){
							blk[i].redundant[k] = true;
						}
						
					}
		
					/* Remove redundant planes */
					unsigned int no = 0;
					while(no<blk[i].a.size()){ //The last plane is the new discontinuity.  It is definitely part of the new blocks.
						//std::cout<<"no: "<<no<<", a[no]: "<<blk.a[no]<<" redundant: "<<blk.redundant[no]<<endl;
						if(blk[i].redundant[no] == true){
							blk[i].a.erase(blk[i].a.begin()+no);
							blk[i].b.erase(blk[i].b.begin()+no);
							blk[i].c.erase(blk[i].c.begin()+no);
							blk[i].d.erase(blk[i].d.begin()+no);
							blk[i].redundant.erase(blk[i].redundant.begin()+no);
							blk[i].JRC.erase(blk[i].JRC.begin()+no);
							blk[i].JCS.erase(blk[i].JCS.begin()+no);
							blk[i].sigmaC.erase(blk[i].sigmaC.begin()+no);
							blk[i].phi_r.erase(blk[i].phi_r.begin()+no);
							blk[i].phi_b.erase(blk[i].phi_b.begin()+no);
							blk[i].asperity.erase(blk[i].asperity.begin()+no);
							blk[i].cohesion.erase(blk[i].cohesion.begin()+no);
							blk[i].tension.erase(blk[i].tension.begin()+no);
							blk[i].isBoundaryPlane.erase(blk[i].isBoundaryPlane.begin()+no);
							blk[i].lambda0.erase(blk[i].lambda0.begin()+no);
							blk[i].heatCapacity.erase(blk[i].heatCapacity.begin()+no);
							blk[i].hwater.erase(blk[i].hwater.begin()+no);
							blk[i].intactRock.erase(blk[i].intactRock.begin()+no);
							blk[i].jointType.erase(blk[i].jointType.begin()+no);
							no = 0;
						}else{
							no = no+1;
						}
					
						
					}
			

		}
	}
	/* Create blocks */
	for (unsigned int i=0; i<blk.size(); i++){
		std::cout<<"Generating progress.... block no: "<<i+1<<"/"<<blk.size()<<endl;
		if(blk[i].subMembers.size()>0){
			//#if 0
			shared_ptr<Body> clumpBody=shared_ptr<Body>(new Body());
			shared_ptr<Clump> clump=shared_ptr<Clump>(new Clump());
			clumpBody->shape=clump;
			clumpBody->setDynamic(true);
			clumpBody->setBounded(false);
			 // Body::id_t clumpId=scene->bodies->insert(clumpBody);
			// std::cout<<"clumpId: "<<clumpId<<endl;
			
		
			//#endif

			vector<int> memberId;
			int clumpMemberCount = 0;
		       	for(int j=0; j<blk[i].subMembers.size();j++){
				if(createBlock(body,blk[i].subMembers[j],j) ){
						//scene->bodies->insert(body);
						 Body::id_t lastId=(Body::id_t)scene->bodies->insert(body);
						 /* new */ memberId.push_back(lastId);
						 //Clump::add(clumpBody, /* body*/  Body::byId(lastId,scene) );
						//clump->ids.push_back(lastId);
						
						 clumpMemberCount++;
				}
				//std::cout<<"Generating progress.... sub-block no: "<<j+1<<"/"<<blk[i].subMembers.size()<<endl;
			}

			if(clumpMemberCount > 1){
				Body::id_t clumpId=scene->bodies->insert(clumpBody); //std::cout<<"ok 1"<<endl;
				for(int j=0; j<memberId.size(); j++){
				 	Clump::addNonSpherical(clumpBody, /* body*/  Body::byId(memberId[j],scene) ); //std::cout<<"ok 2"<<endl;
					clump->ids.push_back(memberId[j]);  //std::cout<<"ok 3"<<endl;
					
				}
				Clump::updatePropertiesNonSpherical(clumpBody,/*intersecting*/ false,scene); 
				//std::cout<<"ok 4"<<endl;
			}

			
		        
		}else{
			if(createBlock(body, blk[i],i)){ 
				scene->bodies->insert(body);
			
			}
		}	
	}
	scene->dt=defaultDt;
	//globalStiffnessTimeStepper->defaultDt=0.000001; //defaultDt;
	//scene->updateBound();
	//scene->bound->min = Vector3r(5,5,5);
	//scene->bound->max = -1.0*Vector3r(5,5,5);
	std::cout<<"complete "<<endl;


	return true;
}


//#if 0
bool BlockGen::createBlock(shared_ptr<Body>& body,  struct Block block, int number ){
	

//std::cout<<"createBlockBegin"<<endl;
	if(block.tooSmall==true){
		return false;
	}

	block.r = block.r + initialOverlap;

//std::cout<<"afterTooSmall, block.a.size()"<<block.a.size()<<endl;
	
	/** FIND BOUNDING VOLUME **/
	Real minX = 0.0; Real minY = 0.0; Real minZ = 0.0;
	Real maxX = 0.0; Real maxY = 0.0; Real maxZ = 0.0;
	Discontinuity plane=Discontinuity(Vector3r(0,0,0));
	plane.a=1.0;
	plane.b=0.0;
	plane.c=0.0;
	plane.d=1.2*boundarySizeXmax;
	Vector3r falseVertex (0.0,0.0,0.0);
	if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
		minX = falseVertex.x();
	}else{return false;}

	plane=Discontinuity(Vector3r(0,0,0));
	plane.a=0.0;
	plane.b=1.0;
	plane.c=0.0;
	plane.d=1.2*boundarySizeYmax;
	if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
		minY = falseVertex.y();
	}else{return false;}

	plane=Discontinuity(Vector3r(0,0,0));
	plane.a=0.0;
	plane.b=0.0;
	plane.c=1.0;
	plane.d=1.2*boundarySizeZmax;
	if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
		minZ = falseVertex.z();
	}else{return false;}

	plane=Discontinuity(Vector3r(0,0,0));
	plane.a=-1.0;
	plane.b=0.0;
	plane.c=0.0;
	plane.d=1.2*boundarySizeXmin;
	if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
		maxX = falseVertex.x();
	}else{return false;}

	plane=Discontinuity(Vector3r(0,0,0));
	plane.a=0.0;
	plane.b=-1.0;
	plane.c=0.0;
	plane.d=1.2*boundarySizeYmin;
	if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
		maxY = falseVertex.y();
	}else{return false;}

	plane=Discontinuity(Vector3r(0,0,0));
	plane.a=0.0;
	plane.b=0.0;
	plane.c=-1.0;
	plane.d=1.2*boundarySizeZmin;
	if (contactBoundaryLPCLP(plane,block,falseVertex) ){	
		maxZ = falseVertex.z();
	}else{return false;}
	
//std::cout<<"afterContactBoundary"<<endl;	

	Real maxXoverall = 1.01*std::max(fabs(maxX),fabs(minX));
	Real maxYoverall = 1.01*std::max(fabs(maxY),fabs(minY));
	Real maxZoverall = 1.01*std::max(fabs(maxZ),fabs(minZ));
//if(number == 482 ){
//	   maxXoverall=10.0; maxYoverall=10.0; maxZoverall=10.0;
//	    std::cout<<"maxXoverall: "<<maxXoverall<<", maxYoverall: "<<maxYoverall<<", maxZoverall: "<<maxZoverall<<endl;
	
//  }	
	
	bool converge = true; 
        Vector3r startingPt = Vector3r(0,0,0); //0.5*Vector3r(minX+maxX,minY+maxY,minZ+maxZ);

	Vector3r centroid = block.centre;
	double blockVol = 0.0;
	Vector3r localCentre = calCentroid(block, blockVol);  
	centroid += localCentre; 	
	Vector3r prevCentre = block.centre;
	block.centre = centroid;
				
	for(unsigned int k=0; k<block.a.size(); k++){
		block.d[k]= -1.0*(block.a[k]*(centroid.x()-prevCentre.x()) + block.b[k]*(centroid.y()-prevCentre.y()) + block.c[k]*(centroid.z()-prevCentre.z())  - block.d[k]);	
	}



	minX = -1.0*(centroid.x()-prevCentre.x()-minX);
	minY = -1.0*(centroid.y()-prevCentre.y()-minY);
	minZ = -1.0*(centroid.z()-prevCentre.z()-minZ);
	maxX = -1.0*(centroid.x()-prevCentre.x()-maxX);
	maxY = -1.0*(centroid.y()-prevCentre.y()-maxY);
	maxZ = -1.0*(centroid.z()-prevCentre.z()-maxZ);
	Vector3r max(maxX,maxY,maxZ);
	Vector3r  min(-minX,-minY,-minZ);
	

	maxXoverall = 1.05*std::max(fabs(maxX),fabs(minX));
	maxYoverall = 1.05*std::max(fabs(maxY),fabs(minY));
	maxZoverall = 1.05*std::max(fabs(maxZ),fabs(minZ));
	Vector3r tempMax(maxXoverall,maxYoverall,maxZoverall);
	//std::cout<<"max: "<<tempMax<<endl;


	body = shared_ptr<Body>(new Body);
	shared_ptr<Aabb> aabb(new Aabb);
	shared_ptr<PotentialBlock> pBlock(new PotentialBlock);
	pBlock->minAabbRotated = 1.02*min;
	pBlock->maxAabbRotated = 1.02*max;

	body->setDynamic(true);
	Real volume = 0.0;
	Real Ixx = 0.0; Real Iyy = 0.0; Real Izz = 0.0; Real Ixy = 0.0; Real Ixz = 0.0; Real Iyz = 0.0;
	Vector3r boundingDist = Vector3r(fabs(max[0]+min[0]),fabs(max[1]+min[1]),fabs(max[2]+min[2]));

//std::cout<<"beforeInertia"<<endl;
	calculateInertia(block,Ixx,Iyy,Izz, Ixy, Ixz, Iyz);
	//std::cout<<"Ixx: "<<Ixx<<", Iyy: "<<Iyy<<", Izz: "<<Izz<<", Ixy: "<<Ixy<<", Ixz: "<<Ixz<<", Iyz: "<<Iyz<<endl;
	if(fabs(Ixx) <pow(10,-15) ){Ixx = 0.0;}
	if(fabs(Iyy) <pow(10,-15) ){Iyy = 0.0;}
	if(fabs(Izz) <pow(10,-15) ){Izz = 0.0;}
	if(fabs(Ixy) <pow(10,-15) ){Ixy = 0.0;}
	if(fabs(Iyz) <pow(10,-15) ){Iyz = 0.0;}
	if(fabs(Ixz) <pow(10,-15) ){Ixz = 0.0;}
	//std::cout<<"Ixx: "<<Ixx<<", Iyy: "<<Iyy<<", Izz: "<<Izz<<", Ixy: "<<Ixy<<", Ixz: "<<Ixz<<", Iyz: "<<Iyz<<endl;
	body->state->mass = blockVol*density;  //blockVol
	pBlock->volume = blockVol;
	char jobz = 'V'; char uplo = 'L'; int N=3; double A[9]; int lda=3; double eigenValues[3]; double work[15]; int lwork = 15; int info = 0; 
	A[0] = Ixx; A[1]=Ixy; A[2]=Ixz; A[3]=Ixy; A[4]= Iyy; A[5]=Iyz; A[6]=Ixz; A[7]=Iyz; A[8]=Izz; 
	dsyev_(&jobz, &uplo, &N, &A[0], &lda, &eigenValues[0], &work[0], &lwork, &info);

	

	Vector3r eigenVec1 (A[0],A[1],A[2]); eigenVec1.normalize();
	Vector3r eigenVec2 (A[3],A[4],A[5]); eigenVec2.normalize();
	Vector3r eigenVec3 (A[6],A[7],A[8]); eigenVec3.normalize();
	Eigen::Matrix3d lapackEigenVec;
	lapackEigenVec(0,0) = eigenVec1[0]; lapackEigenVec(0,1) = eigenVec2[0]; lapackEigenVec(0,2)=eigenVec3[0]; 
	lapackEigenVec(1,0) = eigenVec1[1]; lapackEigenVec(1,1) = eigenVec2[1]; lapackEigenVec(1,2)=eigenVec3[1];
	lapackEigenVec(2,0) = eigenVec1[2]; lapackEigenVec(2,1) = eigenVec2[2]; lapackEigenVec(2,2)=eigenVec3[2];
	/* make sure diagonals are positive */	
	if(lapackEigenVec(0,0)<0.0){ 
		lapackEigenVec(0,0) = -lapackEigenVec(0,0); lapackEigenVec(1,0) = -lapackEigenVec(1,0); lapackEigenVec(2,0) = -lapackEigenVec(2,0);
	}
	if(lapackEigenVec(1,1)<0.0){ 
		lapackEigenVec(0,1) = -lapackEigenVec(0,1); lapackEigenVec(1,1) = -lapackEigenVec(1,1); lapackEigenVec(2,1) = -lapackEigenVec(2,1);
	}
	if(lapackEigenVec(2,2)<0.0){ 
		lapackEigenVec(0,2) = -lapackEigenVec(0,2); lapackEigenVec(1,2) = -lapackEigenVec(1,2); lapackEigenVec(2,2) = -lapackEigenVec(2,2);
	}
	Eigen::Matrix3d lapackEigenVal = Eigen::Matrix3d::Zero();
	lapackEigenVal(0,0) = eigenValues[0]; lapackEigenVal(1,1) = eigenValues[1]; lapackEigenVal(2,2) = eigenValues[2];
	//std::cout<<"Ixx: "<<eigenValues[0]<<", Iyy: "<<eigenValues[1]<<", Izz: "<<eigenValues[2]<<endl;
	Quaternionr q (lapackEigenVec); //q.normalize();
	double q0 =0.5*sqrt(lapackEigenVec(0,0) + lapackEigenVec(1,1) + lapackEigenVec(2,2) + 1.0);
	double q1 = (lapackEigenVec(1,2) - lapackEigenVec(2,1) )/(4*q0);
	double q2 = (lapackEigenVec(2,0) - lapackEigenVec(0,2) )/(4*q0);
	double q3 = (lapackEigenVec(0,1) - lapackEigenVec(1,0) )/(4*q0);
	q.w() = q0; q.x()=q1; q.y()=q2; q.z()=q3; q.normalize(); 
	q=Quaternionr::Identity();

	if(exactRotation == true){
		body->state->inertia= inertiaFactor*Vector3r(lapackEigenVal(0,0)*density,lapackEigenVal(1,1)*density,lapackEigenVal(2,2)*density);
	}else{
		double maxInertia = std::max(std::max(std::max(lapackEigenVal(0,0),lapackEigenVal(1,1)),lapackEigenVal(2,2)),2.0/5.0*body->state->mass/density*minSize*minSize);
		body->state->inertia= inertiaFactor*Vector3r(maxInertia*density,maxInertia*density,maxInertia*density);
	}
	body->state->pos=block.centre;
	body->state->ori =  q.conjugate(); 

	shared_ptr<FrictMat> mat(new FrictMat);
	mat->frictionAngle		= frictionDeg * Mathr::PI/180.0;
	aabb->color		= Vector3r(0,1,0);

//std::cout<<"afterInertia"<<endl;
	int planeNo = block.a.size();
	
	Eigen::MatrixXd tempA(planeNo,3);
	Eigen::MatrixXd tempD(planeNo,1);
	for (int i=0; i<planeNo; i++){
		Vector3r plane(block.a[i],block.b[i],block.c[i]);
		plane = q*plane; 
		pBlock->a.push_back(plane.x());  block.a[i] = plane.x();
		pBlock->b.push_back(plane.y());  block.b[i] = plane.y();
		pBlock->c.push_back(plane.z());  block.c[i] = plane.z();
		pBlock->d.push_back( block.d[i] /* newd */); 
		pBlock->addPlaneStruct();
		//#if 0
		tempA(i,0) = block.a[i];
		tempA(i,1) = block.b[i];
		tempA(i,2) = block.c[i];
		tempD(i,0) = block.d[i] + block.r;
		//#endif
		pBlock->JRC.push_back(block.JRC[i]);
		pBlock->JCS.push_back(block.JCS[i]);
		pBlock->sigmaC.push_back(block.sigmaC[i]);
		pBlock->phi_r.push_back(block.phi_r[i]);
		pBlock->phi_b.push_back(block.phi_b[i]);
		pBlock->asperity.push_back(block.asperity[i]);
		pBlock->cohesion.push_back(block.cohesion[i]);
		pBlock->tension.push_back(block.tension[i]);
		pBlock->isBoundaryPlane.push_back(block.isBoundaryPlane[i]);
		pBlock->lambda0.push_back(block.lambda0[i]);
		pBlock->heatCapacity.push_back(block.heatCapacity[i]);
		pBlock->hwater.push_back(block.hwater[i]);
		pBlock->intactRock.push_back(block.intactRock[i]);
		pBlock->jointType.push_back(block.jointType[i]);
		if(i>10000){std::cout<<"i: "<<i<<endl;	}
	}
	pBlock->Amatrix =tempA;
	pBlock->Dmatrix = tempD;
	//std::cout<<"blockgen, A: "<<pBlock->Amatrix<<", D: "<<pBlock->Dmatrix<<endl;
		////////////////////////////////////////////////////////
		plane=Discontinuity(Vector3r(0,0,0));
		plane.a=1.0;
		plane.b=0.0;
		plane.c=0.0;
		plane.d=1.2*boundarySizeXmax;
		if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
			minX = falseVertex.x();
		}else{return false;}

		plane=Discontinuity(Vector3r(0,0,0));
		plane.a=0.0;
		plane.b=1.0;
		plane.c=0.0;
		plane.d=1.2*boundarySizeYmax;
		if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
			minY = falseVertex.y();
		}else{return false;}

		plane=Discontinuity(Vector3r(0,0,0));
		plane.a=0.0;
		plane.b=0.0;
		plane.c=1.0;
		plane.d=1.2*boundarySizeZmax;
		if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
			minZ = falseVertex.z();
		}else{return false;}

		plane=Discontinuity(Vector3r(0,0,0));
		plane.a=-1.0;
		plane.b=0.0;
		plane.c=0.0;
		plane.d=1.2*boundarySizeXmin;
		if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
			maxX = falseVertex.x();
		}else{return false;}

		plane=Discontinuity(Vector3r(0,0,0));
		plane.a=0.0;
		plane.b=-1.0;
		plane.c=0.0;
		plane.d=1.2*boundarySizeYmin;
		if (contactBoundaryLPCLP(plane, block,falseVertex) ){	
			maxY = falseVertex.y();
		}else{return false;}

		plane=Discontinuity(Vector3r(0,0,0));
		plane.a=0.0;
		plane.b=0.0;
		plane.c=-1.0;
		plane.d=1.2*boundarySizeZmin;
		if (contactBoundaryLPCLP(plane,block,falseVertex) ){	
			maxZ = falseVertex.z();
		}else{return false;}
		Vector3r oldmax = max;
		min = Vector3r(-minX,-minY,-minZ);
		max = Vector3r(maxX,maxY,maxZ);
		//std::cout<<"number: "<<number<<", min : "<<min<<", max: "<<max<<endl;
		//////////////////////////////////////////////

	
	pBlock->oriAabb = body->state->ori;
	pBlock->r = block.r;// + initialOverlap; 
	pBlock->k = block.k;
	pBlock->AabbMinMax=true;
	pBlock->minAabb = 1.02*min;
	pBlock->maxAabb = 1.02*max;
	pBlock->R =  std::max(max.norm(),min.norm()); //findExtreme(block); //block.R; //tempMax.norm(); //
	//if(min.norm()/max.norm() <0.125 || min.norm()/max.norm() > 8.0){return false;}
	pBlock->id =number;
	pBlock->color           = Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()); //std::max(std::max(maxXoverall, maxYoverall),maxZoverall) ; //
	if(block.isBoundary == true){
		pBlock->color  = Vector3r(0,100,0);
		body->setDynamic(false);
	}
	pBlock->isBoundary = block.isBoundary;
	
	body->bound	= aabb;
	body->material	= mat;
	//if (exactRotation == false){
	//	double sphereInertia = 2.0/5.0*body->state->mass*pBlock->R*pBlock->R;
	//	body->state->inertia= inertiaFactor*Vector3r(sphereInertia,sphereInertia,sphereInertia);
	//}

//#if 0

//std::cout<<"beforeVertices"<<endl;

	 /* find vertices */
	  double D[3]; double Ax[9]; Eigen::Matrix3d Aplanes;
	  for (int i=0; i<planeNo; i++ ){
		for (int j=i+1; j<planeNo; j++){
			for(int k=j+1; k<planeNo; k++){
				
				Vector3r plane1 = Vector3r(pBlock->a[i],pBlock->b[i], pBlock->c[i]);	
				Vector3r plane2 = Vector3r(pBlock->a[j],pBlock->b[j], pBlock->c[j]);	
				Vector3r plane3 = Vector3r(pBlock->a[k],pBlock->b[k], pBlock->c[k]);	
				
				Vector3r centre = body->state->pos;
				double d1 = pBlock->d[i]+pBlock->r;	
				double d2 = pBlock->d[j]+pBlock->r;
				double d3 = pBlock->d[k]+pBlock->r;
	
				D[0]=d1;
				D[1]=d2;
				D[2]=d3;
				Ax[0]=plane1.x(); Ax[3]=plane1.y(); Ax[6]=plane1.z();  Aplanes(0,0)= Ax[0]; Aplanes(0,1) = Ax[3]; Aplanes(0,2) = Ax[6];
				Ax[1]=plane2.x(); Ax[4]=plane2.y(); Ax[7]=plane2.z();  Aplanes(1,0) = Ax[1]; Aplanes(1,1) = Ax[4];Aplanes(1,2) = Ax[7];
				Ax[2]=plane3.x(); Ax[5]=plane3.y(); Ax[8]=plane3.z();  Aplanes(2,0) = Ax[2]; Aplanes(2,1) = Ax[5];Aplanes(2,2) = Ax[8];
				bool parallel = false;
				if (fabs(plane1.dot(plane2))<1.0002 && fabs(plane1.dot(plane2))>0.9998){parallel = true;}
				if (fabs(plane1.dot(plane3))<1.0002 && fabs(plane1.dot(plane3))>0.9998){parallel = true;}
				if (fabs(plane2.dot(plane3))<1.0002 && fabs(plane2.dot(plane3))>0.9998){parallel = true;}
				double det = Aplanes.determinant();
				
				if(fabs(det)>pow(10,-15) ){
				
					int ipiv[3];  int bColNo=1; int info=0; /* LU */ int three =3;
					dgesv_( &three, &bColNo, Ax, &three, ipiv, D, &three, &info);
					if (info!=0){
						//std::cout<<"linear algebra error"<<endl;
					}else{
						bool inside = true; Vector3r vertex(D[0],D[1],D[2]);
					
						for (int i=0; i<planeNo; i++){
							Real plane =  pBlock->a[i]*vertex.x() + pBlock->b[i]*vertex.y() + pBlock->c[i]*vertex.z()  - pBlock->d[i]- pBlock->r; if (plane>pow(10,-3)){inside = false;} 	
						}
					
						
						if (inside == true){
							//std::cout<<"vertex: "<<vertex<<", planeV1: "<<planeV1<<", planeV2: "<<planeV2<<", planeV3: "<<planeV3<<", plane1: "<<plane1<<", plane2: "<<plane2<<", plane3: "<<plane3<<", det: "<<det<<endl;

							/*Vertices */
							pBlock->verticesCD.push_back(vertex);
							int vertexID = pBlock->verticesCD.size()-1;
							pBlock->addVertexStruct();
							pBlock->vertexStruct[vertexID].planeID.push_back(i);	/*Note that the planeIDs are arranged from small to large! */
							pBlock->vertexStruct[vertexID].planeID.push_back(j);    /* planeIDs are arranged in the same sequence as [a,b,c] and d */
							pBlock->vertexStruct[vertexID].planeID.push_back(k);    /* vertices store information on planeIDs */
							
							/*Planes */
							pBlock->planeStruct[i].vertexID.push_back(vertexID);	/* planes store information on vertexIDs */
							pBlock->planeStruct[j].vertexID.push_back(vertexID);	
							pBlock->planeStruct[k].vertexID.push_back(vertexID);	
							

						}
					}
				}
			}		
		}
	  }
//#endif	 

#if 0
	int vertexNo = pBlock->verticesCD.size(); int edgeCount=0;
	for (int i=0; i<vertexNo; i++ ){
		for (int j=0; j<vertexNo; j++){
			if(i==j){continue;}
			int v1a = pBlock->vertexStruct[i].planeID[0];
			int v2a = pBlock->vertexStruct[i].planeID[1];
			int v3a = pBlock->vertexStruct[i].planeID[2];
			int v1b = pBlock->vertexStruct[j].planeID[0];
			int v2b = pBlock->vertexStruct[j].planeID[1];
			int v3b = pBlock->vertexStruct[j].planeID[2];
			
			if(  (v1a != v1b && v2a == v2b && v3a == v3b) || (v1a == v1b && v2a != v2b && v3a == v3b) || (v1a == v1b && v2a == v2b && v3a != v3b)  ){
				double length = ( pBlock->verticesCD[i] - pBlock->verticesCD[j] ).norm();
				if(length<pow(10,-3) ){ continue; } 
				pBlock->addEdgeStruct();
				pBlock->edgeStruct[edgeCount].vertexID.push_back(i); /* edges store information on vertexIDs */
				pBlock->edgeStruct[edgeCount].vertexID.push_back(j);
				pBlock->vertexStruct[i].edgeID.push_back(edgeCount); /* vertices store information on edgeIDs */
				pBlock->vertexStruct[j].edgeID.push_back(edgeCount);
				edgeCount++;
			}
		}
	}
#endif
	body->setAspherical(true);
	body->shape	= pBlock;

	//std::cout<<"BLOCKGEN pBlock->verticesCD.size() "<<pBlock->verticesCD.size()<<", pBlock->vertexStruct.size(): "<<pBlock->vertexStruct.size()<<endl;
	

	return true;
}
//#endif



void BlockGen::createActors(shared_ptr<Scene>& scene){
	shared_ptr<IGeomDispatcher> interactionGeometryDispatcher(new IGeomDispatcher);
	shared_ptr<Ig2_PB_PB_ScGeom> cd(new Ig2_PB_PB_ScGeom);
	
	cd-> stepAngle=calAreaStep;
	cd->twoDimension = twoDimension;
	interactionGeometryDispatcher->add(cd);
	
	shared_ptr<IPhysDispatcher> interactionPhysicsDispatcher(new IPhysDispatcher);
	shared_ptr<Ip2_FrictMat_FrictMat_KnKsPBPhys> ss(new Ip2_FrictMat_FrictMat_KnKsPBPhys);
	ss->Knormal = Kn;
	ss->Kshear = Ks;
	ss->kn_i = Kn;
	ss->ks_i = Ks;
	ss->viscousDamping = viscousDamping;
	ss->useOverlapVol = useOverlapVol;
	ss->useFaceProperties = useFaceProperties;
	ss->unitWidth2D = unitWidth2D;
	ss->calJointLength = calJointLength;
	ss->twoDimension = twoDimension;
	ss->brittleLength = brittleLength;
	ss->u_peak = peakDisplacement;
	ss->maxClosure = maxClosure;
	interactionPhysicsDispatcher->add(ss);

	//shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	//gravityCondition->gravity = gravity;
	
	if (useGlobalStiffnessTimeStepper){
		globalStiffnessTimeStepper=shared_ptr<GlobalStiffnessTimeStepper>(new GlobalStiffnessTimeStepper);
		globalStiffnessTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
		globalStiffnessTimeStepper->defaultDt = defaultDt;
	}

	scene->engines.clear();
	scene->engines.push_back(shared_ptr<Engine>(new ForceResetter));
	shared_ptr<InsertionSortCollider> collider(new InsertionSortCollider);
	collider->verletDist = 0.1*minSize;
	collider->boundDispatcher->add(new PotentialBlock2AABB);

	scene->engines.push_back(collider);
	shared_ptr<InteractionLoop> ids(new InteractionLoop);
	ids->geomDispatcher=interactionGeometryDispatcher;
	ids->physDispatcher=interactionPhysicsDispatcher;
	ids->lawDispatcher=shared_ptr<LawDispatcher>(new LawDispatcher);
	shared_ptr<Law2_SCG_KnKsPBPhys_KnKsPBLaw> see(new Law2_SCG_KnKsPBPhys_KnKsPBLaw);
	see->traceEnergy = traceEnergy;
	see->Talesnick = Talesnick;
	see->neverErase = neverErase;
	ids->lawDispatcher->add(see);
	scene->engines.push_back(ids);
	//scene->engines.push_back(globalStiffnessTimeStepper);
	//scene->engines.push_back(gravityCondition);
	shared_ptr<NewtonIntegrator> newton(new NewtonIntegrator);
	newton->damping=dampingMomentum;
	newton->gravity= gravity; //Vector3r(0,0,9.81);
	// FIXME:
	// newton->damping3DEC=damp3DEC;
	newton->exactAsphericalRot=exactRotation;
	scene->engines.push_back(newton);
	//scene->initializers.clear();	
}




Real BlockGen::evaluateFNoSphere(struct Block block, Vector3r presentTrial){
	Real x = presentTrial[0]-block.centre[0];
	Real y = presentTrial[1]-block.centre[1];
	Real z = presentTrial[2]-block.centre[2];
	int planeNo = block.a.size();
	vector<double>a; vector<double>b; vector<double>c; vector<double>d; vector<double>p; Real pSum2 = 0.0;
	for (int i=0; i<planeNo; i++){
		a.push_back(block.a[i]);
		b.push_back(block.b[i]);		
		c.push_back(block.c[i]);
		d.push_back(block.d[i]);
		Real plane = a[i]*x + b[i]*y + c[i]*z - d[i]; if (plane<pow(10,-15)){plane = 0.0;} 
		p.push_back(plane);
		pSum2 += pow(p[i],2);
	}
	Real r = block.r; 
	Real R = block.R; 
	Real k = block.k; 
	/* Additional sphere */
	
	/* Complete potential particle */
	Real f = pSum2 - r*r;

	return f;
}


bool BlockGen::checkCentroid(struct Block block, Vector3r presentTrial){
	Real x = presentTrial[0]-block.centre[0];
	Real y = presentTrial[1]-block.centre[1];
	Real z = presentTrial[2]-block.centre[2];
	int planeNo = block.a.size(); bool allNegative = true;
	for (int i=0; i<planeNo; i++){
		Real plane = block.a[i]*x + block.b[i]*y + block.c[i]*z - block.d[i]; if (plane<pow(10,-15)){plane = 0.0;} else{ allNegative = false;}
		
	}

	return allNegative;
}






double BlockGen::getSignedArea(const Vector3r pt1, const Vector3r pt2, const Vector3r pt3){ 
	/* if positive, counter clockwise, 2nd point makes a larger angle */
	/* if negative, clockwise, 3rd point makes a larger angle */ 
	Eigen::MatrixXd triangle(4,2);
	triangle(0,0) = pt1.x();  triangle(0,1) = pt1.y(); // triangle(0,2) = pt1.z(); 
	triangle(1,0) = pt2.x();  triangle(1,1) = pt2.y(); // triangle(1,2) = pt2.z(); 
	triangle(2,0) = pt3.x();  triangle(2,1) = pt3.y(); // triangle(2,2) = pt3.z(); 
	triangle(3,0) = pt1.x();  triangle(3,1) = pt1.y(); // triangle(3,2) = pt1.z(); 
	double determinant = getDet(triangle);
	return determinant; //triangle.determinant();
}


double BlockGen::getDet(const Eigen::MatrixXd A){ 
	/* if positive, counter clockwise, 2nd point makes a larger angle */
	/* if negative, clockwise, 3rd point makes a larger angle */ 
	int rowNo = A.rows();  double firstTerm = 0.0; double secondTerm = 0.0;
	for(int i=0; i<rowNo-1; i++){
		firstTerm += A(i,0)*A(i+1,1);
		secondTerm += A(i,1)*A(i+1,0);
	}
	
	return firstTerm-secondTerm;
}


double BlockGen::getCentroidTetrahedron(const Eigen::MatrixXd A){ 
	/* if positive, counter clockwise, 2nd point makes a larger angle */
	/* if negative, clockwise, 3rd point makes a larger angle */ 
	int rowNo = A.rows();  double firstTerm = 0.0; double secondTerm = 0.0;
	for(int i=0; i<rowNo-1; i++){
		firstTerm += A(i,0)*A(i+1,1);
		secondTerm += A(i,1)*A(i+1,0);
	}
	
	return firstTerm-secondTerm;
}


void BlockGen::calculateInertia(struct Block block, Real& Ixx, Real& Iyy, Real& Izz,Real& Ixy, Real& Ixz, Real& Iyz){
/* find vertices */
	  Ixx = 0.0; Iyy = 0.0; Izz= 0.0; Ixy = 0.0; Ixz= 0.0; Iyz=0.0;
	  vector<Vector3r> vertices;
	  Vector3r pointInside = Vector3r(0,0,0);
	  double totalVolume=0;
	  int planeNo = block.a.size();
	  double D[3]; double Ax[9]; Eigen::Matrix3d Aplanes; Vector3r centroid(0,0,0);
	  for (int i=0; i<planeNo; i++ ){
		for (int j=i+1; j<planeNo; j++){
			for(int k=j+1; k<planeNo; k++){
				
				Vector3r plane1 = Vector3r(block.a[i],block.b[i], block.c[i]);	
				Vector3r plane2 = Vector3r(block.a[j],block.b[j], block.c[j]);	
				Vector3r plane3 = Vector3r(block.a[k],block.b[k], block.c[k]);	
				double d1 = block.d[i] + block.r;	
				double d2 = block.d[j] + block.r;
				double d3 = block.d[k] + block.r;
	
				D[0]=d1;
				D[1]=d2;
				D[2]=d3;
				Ax[0]=plane1.x(); Ax[3]=plane1.y(); Ax[6]=plane1.z();  Aplanes(0,0)= Ax[0];  Aplanes(0,1) = Ax[3]; Aplanes(0,2) = Ax[6];
				Ax[1]=plane2.x(); Ax[4]=plane2.y(); Ax[7]=plane2.z();  Aplanes(1,0)= Ax[1];  Aplanes(1,1) = Ax[4]; Aplanes(1,2) = Ax[7];
				Ax[2]=plane3.x(); Ax[5]=plane3.y(); Ax[8]=plane3.z();  Aplanes(2,0)= Ax[2];  Aplanes(2,1) = Ax[5]; Aplanes(2,2) = Ax[8];
				bool parallel = false;
				if (fabs(plane1.dot(plane2))<1.0002 && fabs(plane1.dot(plane2))>0.9998){parallel = true;}
				if (fabs(plane1.dot(plane3))<1.0002 && fabs(plane1.dot(plane3))>0.9998){parallel = true;}
				if (fabs(plane2.dot(plane3))<1.0002 && fabs(plane2.dot(plane3))>0.9998){parallel = true;}
				double det = Aplanes.determinant();
				
				if(fabs(det)>pow(10,-15) ){
				//if (parallel == false){
					int ipiv[3];  int bColNo=1; int info=0; /* LU */ int three =3;
					dgesv_( &three, &bColNo, Ax, &three, ipiv, D, &three, &info);
					if (info!=0){
						//std::cout<<"linear algebra error"<<endl;
					}else{
						bool inside = true; Vector3r vertex(D[0],D[1],D[2]);
					
						for (int i=0; i<planeNo; i++){
							Real plane =  block.a[i]*vertex.x() + block.b[i]*vertex.y() + block.c[i]*vertex.z()  - block.d[i]- block.r; if (plane>pow(10,-3)){inside = false;} 	
						}
					
						
						if (inside == true){
							vertices.push_back(vertex);
						}
					}
				}
			}		
		}
	  }

	  vector<Vector3r> verticesOnPlane; vector<Vector3r> oriVerticesOnPlane;
	  for (int j=0; j<block.a.size(); j++){
		if(verticesOnPlane.size()>0){
			verticesOnPlane.clear(); oriVerticesOnPlane.clear();
		}
		for (int i=0; i<vertices.size();i++){		
			Vector3r vertex =vertices[i]; /*local coordinates*/
			double plane = block.a[j]*vertex.x() + block.b[j]*vertex.y() + block.c[j]*vertex.z() - block.d[j] - block.r; 
			if( fabs(plane) < pow(10,-3) ){
				Vector3r planeNormal = Vector3r(block.a[j],block.b[j],block.c[j]);
				Vector3r oriNormal(0,0,1); //normal vector of x-y plane
				Vector3r crossProd = oriNormal.cross(planeNormal);
				Quaternionr Qp;
				Qp.w() = 1.0 + oriNormal.dot(planeNormal);
				Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
				Qp.normalize();
				if(crossProd.norm() < pow(10,-7)){
					Qp = Quaternionr::Identity();
				}
				Vector3r rotatedCoord = Qp.conjugate()*vertex;
				//std::cout<<"rotatedCoord: "<<rotatedCoord<<", oriCoord: "<<vertex<<endl;
				verticesOnPlane.push_back(rotatedCoord);
				oriVerticesOnPlane.push_back(vertex);	
			}
		}
		if(verticesOnPlane.size() == 0 ){continue;}
		/* REORDER VERTICES counterclockwise positive*/
		vector<Vector3r> orderedVerticesOnPlane; vector<Vector3r> oriOrderedVerticesOnPlane;
		int h = 0; int k = 1; int m =2;
		Vector3r pt1 = verticesOnPlane[h];
		Vector3r pt2 = verticesOnPlane[k];
		Vector3r pt3 = verticesOnPlane[m];
		orderedVerticesOnPlane.push_back(pt1);	 oriOrderedVerticesOnPlane.push_back(oriVerticesOnPlane[0]);
		int counter = 1;
		while(counter<verticesOnPlane.size()){		
				
				while (m<verticesOnPlane.size()){	
					
					pt1 = verticesOnPlane[h];
				 	pt2 = verticesOnPlane[k];
					pt3 = verticesOnPlane[m];
					if (getSignedArea(pt1,pt2,pt3) < 0.0){
						/* clockwise means 3rd point is better than 2nd */ 
						k=m; /*3rd point becomes 2nd point */
					 	pt2 = verticesOnPlane[k];
						
					}/* else counterclockwise is good.  We need to find and see whether there is a point(3rd point) better than the 2nd point */
					/* advance m */
					m=m+1;
					while(m==h || m==k){
						m=m+1;
					}
				
				}
				//std::cout<<"h: "<<h<<", k :"<<k<<", m: "<<m<<endl;
				orderedVerticesOnPlane.push_back(pt2);	
				oriOrderedVerticesOnPlane.push_back(oriVerticesOnPlane[k]);
				h=k;
				/* advance k */
				k=0;
				while(k==h ){
					k=k+1;
				}
				/* advance m */
				m=0;
				while(m==h || m==k){
					m=m+1;
				}
				counter++;
		}
		

		Eigen::MatrixXd vertexOnPlane(orderedVerticesOnPlane.size()+1,2);
		Vector3r baseOnPolygon (0,0,0); Vector3r oriBaseOnPolygon (0,0,0);
		for(int i=0; i< orderedVerticesOnPlane.size(); i++){
			vertexOnPlane(i,0)=orderedVerticesOnPlane[i].x(); vertexOnPlane(i,1)=orderedVerticesOnPlane[i].y(); //vertexOnPlane(i,2)=orderedVerticesOnPlane[i].z();		
			//std::cout<<"vertexOnPlane0: "<<vertexOnPlane(i,0)<<", vertexOnPlane1: "<<vertexOnPlane(i,1)<<endl;
			baseOnPolygon += orderedVerticesOnPlane[i];
			oriBaseOnPolygon += oriOrderedVerticesOnPlane[i];
		}
		baseOnPolygon = baseOnPolygon/static_cast<double>(orderedVerticesOnPlane.size());
		oriBaseOnPolygon = oriBaseOnPolygon/static_cast<double>(oriOrderedVerticesOnPlane.size());
		int lastEntry = orderedVerticesOnPlane.size();
		vertexOnPlane(lastEntry,0)=orderedVerticesOnPlane[0].x(); vertexOnPlane(lastEntry,1)=orderedVerticesOnPlane[0].y(); //vertexOnPlane(lastEntry,2)=orderedVerticesOnPlane[0].z();
		//std::cout<<"vertexOnPlane0: "<<vertexOnPlane(lastEntry,0)<<", vertexOnPlane1: "<<vertexOnPlane(lastEntry,1)<<endl;

		double det = getDet(vertexOnPlane);
		double area = 0.5*det; //(vertexOnPlane.determinant());
		double height = -1.0*( block.a[j]*pointInside.x() + block.b[j]*pointInside.y() + block.c[j]*pointInside.z() - block.d[j]-block.r ); 
		double volume = 1.0/3.0*area*height;
		totalVolume += volume;
		//std::cout<<"orderedVerticesOnPlane.size(): "<<orderedVerticesOnPlane.size()<<", volume: "<<volume<<", area: "<<area<<", height: "<<height<<endl;


		double areaPyramid = 0.0;
		Vector3r centroidPyramid(0,0,0);
		double volumePyramid = 0.0;
		double heightTetra = 0.0;
		for(int i=0; i<vertexOnPlane.rows()-1; i++){
			
			Eigen::MatrixXd B(4,2);
			B(0,0)=vertexOnPlane(i,0);   B(0,1)=vertexOnPlane(i,1);
			B(1,0)=vertexOnPlane(i+1,0); B(1,1)=vertexOnPlane(i+1,1);
			B(2,0)=baseOnPolygon.x(); B(2,1)=baseOnPolygon.y();
			B(3,0)=vertexOnPlane(i,0);   B(3,1)=vertexOnPlane(i,1);
			double tempArea = getDet(B);
			double areaTri = 0.5*tempArea;
			areaPyramid += areaTri;
			heightTetra = fabs(orderedVerticesOnPlane[i].z());
			double tetraVol = 1.0/3.0*areaTri*heightTetra;
			volumePyramid += tetraVol;
			Vector3r centroidTetra (0,0,0);
			if( i==vertexOnPlane.rows()-2 ){
				centroidTetra = 0.25*(oriOrderedVerticesOnPlane[i]+oriOrderedVerticesOnPlane[0] + oriBaseOnPolygon + pointInside);
			}else{
				centroidTetra = 0.25*(oriOrderedVerticesOnPlane[i]+oriOrderedVerticesOnPlane[i+1] + oriBaseOnPolygon + pointInside);
			}
			centroidPyramid += (tetraVol*centroidTetra);
			Ixx += tetraVol*((pow((centroidTetra.y()  - pointInside[1]),2) + pow((centroidTetra.z()  - pointInside[2]),2) ));
			Iyy += tetraVol*((pow((centroidTetra.x() - pointInside[0]),2) + pow((centroidTetra.z()  - pointInside[2]),2) ));
			Izz += tetraVol*((pow((centroidTetra.x()  - pointInside[0]),2) + pow((centroidTetra.y()  - pointInside[1]),2) ));
			Ixy -= tetraVol*(((centroidTetra.x()  - pointInside[0]) *(centroidTetra.y()  - pointInside[1])));
			Ixz -= tetraVol*(((centroidTetra.x()  - pointInside[0]) *(centroidTetra.z()  - pointInside[2])));
			Iyz -= tetraVol*(((centroidTetra.y() - pointInside[1]) *(centroidTetra.z()  - pointInside[2])));
		}
		centroid += centroidPyramid;
		
		//std::cout<<"oriOrderedVerticesOnPlane.size(): "<<oriOrderedVerticesOnPlane.size()<<", volumePyramid: "<<volumePyramid<<", areaPyramid: "<<areaPyramid<<", height: "<<heightTetra<<endl;
		orderedVerticesOnPlane.clear(); oriOrderedVerticesOnPlane.clear();
	}

	centroid = centroid/totalVolume;
	//std::cout<<"centroid: "<<centroid<<endl;
	//std::cout<<"totalVolume: "<<totalVolume<<endl;
	vertices.clear();
	verticesOnPlane.clear();
	oriVerticesOnPlane.clear();
	
}

Vector3r BlockGen::calCentroid(struct Block block, double & blockVol){
	/* find vertices */
	  vector<Vector3r> vertices;
	  Vector3r pointInside = Vector3r(0,0,0);
	  double totalVolume=0;
	  int planeNo = block.a.size();
	  double D[3]; double Ax[9]; Eigen::Matrix3d Aplanes; Vector3r centroid(0,0,0);
	  for (int i=0; i<planeNo; i++ ){
		for (int j=i+1; j<planeNo; j++){
			for(int k=j+1; k<planeNo; k++){
				
				Vector3r plane1 = Vector3r(block.a[i],block.b[i], block.c[i]);	
				Vector3r plane2 = Vector3r(block.a[j],block.b[j], block.c[j]);	
				Vector3r plane3 = Vector3r(block.a[k],block.b[k], block.c[k]);	
				double d1 = block.d[i] + block.r;	
				double d2 = block.d[j] + block.r;
				double d3 = block.d[k] + block.r;
	
				D[0]=d1;
				D[1]=d2;
				D[2]=d3;
				Ax[0]=plane1.x(); Ax[3]=plane1.y(); Ax[6]=plane1.z();  Aplanes(0,0)= Ax[0];  Aplanes(0,1) = Ax[3]; Aplanes(0,2) = Ax[6];
				Ax[1]=plane2.x(); Ax[4]=plane2.y(); Ax[7]=plane2.z();  Aplanes(1,0)= Ax[1];  Aplanes(1,1) = Ax[4]; Aplanes(1,2) = Ax[7];
				Ax[2]=plane3.x(); Ax[5]=plane3.y(); Ax[8]=plane3.z();  Aplanes(2,0)= Ax[2];  Aplanes(2,1) = Ax[5]; Aplanes(2,2) = Ax[8];
				bool parallel = false;
				if (fabs(plane1.dot(plane2))<1.0002 && fabs(plane1.dot(plane2))>0.9998){parallel = true;}
				if (fabs(plane1.dot(plane3))<1.0002 && fabs(plane1.dot(plane3))>0.9998){parallel = true;}
				if (fabs(plane2.dot(plane3))<1.0002 && fabs(plane2.dot(plane3))>0.9998){parallel = true;}
				double det = Aplanes.determinant();
				
				if(fabs(det)>pow(10,-15) ){
				//if (parallel == false){
					int ipiv[3];  int bColNo=1; int info=0; /* LU */ int three =3;
					dgesv_( &three, &bColNo, Ax, &three, ipiv, D, &three, &info);
					if (info!=0){
						//std::cout<<"linear algebra error"<<endl;
					}else{
						bool inside = true; Vector3r vertex(D[0],D[1],D[2]);
					
						for (int i=0; i<planeNo; i++){
							Real plane =  block.a[i]*vertex.x() + block.b[i]*vertex.y() + block.c[i]*vertex.z()  - block.d[i]- block.r; if (plane>pow(10,-3)){inside = false;} 	
						}
					
						
						if (inside == true){
							vertices.push_back(vertex);
							
						}
					}
				}
			}		
		}
	  }
	  	

	  vector<Vector3r> verticesOnPlane; vector<Vector3r> oriVerticesOnPlane;
	  for (int j=0; j<block.a.size(); j++){
		if(verticesOnPlane.size()>0){
			verticesOnPlane.clear(); oriVerticesOnPlane.clear();
		}
		for (int i=0; i<vertices.size();i++){		
			Vector3r vertex =vertices[i]; /*local coordinates*/
			double plane = block.a[j]*vertex.x() + block.b[j]*vertex.y() + block.c[j]*vertex.z() - block.d[j] - block.r; 
			if( fabs(plane) < pow(10,-3) ){
				Vector3r planeNormal = Vector3r(block.a[j],block.b[j],block.c[j]);
				Vector3r oriNormal(0,0,1); //normal vector of x-y plane
				Vector3r crossProd = oriNormal.cross(planeNormal);
				Quaternionr Qp;
				Qp.w() = 1.0 + oriNormal.dot(planeNormal);
				Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
				Qp.normalize();
				if(crossProd.norm() < pow(10,-7)){
					Qp = Quaternionr::Identity();
				}
				Vector3r rotatedCoord = Qp.conjugate()*vertex;
				//std::cout<<"rotatedCoord: "<<rotatedCoord<<", oriCoord: "<<vertex<<endl;
				verticesOnPlane.push_back(rotatedCoord);
				oriVerticesOnPlane.push_back(vertex);	
			}
		}
		if(verticesOnPlane.size() == 0 ){continue;}
		/* REORDER VERTICES counterclockwise positive*/
		vector<Vector3r> orderedVerticesOnPlane; vector<Vector3r> oriOrderedVerticesOnPlane;
		int h = 0; int k = 1; int m =2;
		Vector3r pt1 = verticesOnPlane[h];
		Vector3r pt2 = verticesOnPlane[k];
		Vector3r pt3 = verticesOnPlane[m];
		orderedVerticesOnPlane.push_back(pt1);	 oriOrderedVerticesOnPlane.push_back(oriVerticesOnPlane[0]);
		int counter = 1;
		while(counter<verticesOnPlane.size()){		
				
				while (m<verticesOnPlane.size()){	
					
					pt1 = verticesOnPlane[h];
				 	pt2 = verticesOnPlane[k];
					pt3 = verticesOnPlane[m];
					if (getSignedArea(pt1,pt2,pt3) < 0.0){
						/* clockwise means 3rd point is better than 2nd */ 
						k=m; /*3rd point becomes 2nd point */
					 	pt2 = verticesOnPlane[k];
						
					}/* else counterclockwise is good.  We need to find and see whether there is a point(3rd point) better than the 2nd point */
					/* advance m */
					m=m+1;
					while(m==h || m==k){
						m=m+1;
					}
				
				}
				//std::cout<<"h: "<<h<<", k :"<<k<<", m: "<<m<<endl;
				orderedVerticesOnPlane.push_back(pt2);	
				oriOrderedVerticesOnPlane.push_back(oriVerticesOnPlane[k]);
				h=k;
				/* advance k */
				k=0;
				while(k==h ){
					k=k+1;
				}
				/* advance m */
				m=0;
				while(m==h || m==k){
					m=m+1;
				}
				counter++;
		}
		

		Eigen::MatrixXd vertexOnPlane(orderedVerticesOnPlane.size()+1,2);
		Vector3r baseOnPolygon (0,0,0); Vector3r oriBaseOnPolygon (0,0,0);
		for(int i=0; i< orderedVerticesOnPlane.size(); i++){
			vertexOnPlane(i,0)=orderedVerticesOnPlane[i].x(); vertexOnPlane(i,1)=orderedVerticesOnPlane[i].y(); //vertexOnPlane(i,2)=orderedVerticesOnPlane[i].z();		
			//std::cout<<"vertexOnPlane0: "<<vertexOnPlane(i,0)<<", vertexOnPlane1: "<<vertexOnPlane(i,1)<<endl;
			baseOnPolygon += orderedVerticesOnPlane[i];
			oriBaseOnPolygon += oriOrderedVerticesOnPlane[i];
		}
		baseOnPolygon = baseOnPolygon/static_cast<double>(orderedVerticesOnPlane.size());
		oriBaseOnPolygon = oriBaseOnPolygon/static_cast<double>(oriOrderedVerticesOnPlane.size());
		int lastEntry = orderedVerticesOnPlane.size();
		vertexOnPlane(lastEntry,0)=orderedVerticesOnPlane[0].x(); vertexOnPlane(lastEntry,1)=orderedVerticesOnPlane[0].y(); //vertexOnPlane(lastEntry,2)=orderedVerticesOnPlane[0].z();
		//std::cout<<"vertexOnPlane0: "<<vertexOnPlane(lastEntry,0)<<", vertexOnPlane1: "<<vertexOnPlane(lastEntry,1)<<endl;

		double det = getDet(vertexOnPlane);
		double area = 0.5*det; //(vertexOnPlane.determinant());
		double height = -1.0*( block.a[j]*pointInside.x() + block.b[j]*pointInside.y() + block.c[j]*pointInside.z() - block.d[j]-block.r ); 
		double volume = 1.0/3.0*area*height;
		totalVolume += volume;
		//std::cout<<"orderedVerticesOnPlane.size(): "<<orderedVerticesOnPlane.size()<<", volume: "<<volume<<", area: "<<area<<", height: "<<height<<endl;


		double areaPyramid = 0.0;
		Vector3r centroidPyramid(0,0,0);
		double volumePyramid = 0.0;
		double heightTetra = 0.0;
		for(int i=0; i<vertexOnPlane.rows()-1; i++){
			
			Eigen::MatrixXd B(4,2);
			B(0,0)=vertexOnPlane(i,0);   B(0,1)=vertexOnPlane(i,1);
			B(1,0)=vertexOnPlane(i+1,0); B(1,1)=vertexOnPlane(i+1,1);
			B(2,0)=baseOnPolygon.x(); B(2,1)=baseOnPolygon.y();
			B(3,0)=vertexOnPlane(i,0);   B(3,1)=vertexOnPlane(i,1);
			double tempArea = getDet(B);
			double areaTri = 0.5*tempArea;
			areaPyramid += areaTri;
			heightTetra = fabs(orderedVerticesOnPlane[i].z());
			double tetraVol = 1.0/3.0*areaTri*heightTetra;
			volumePyramid += tetraVol;
			Vector3r centroidTetra (0,0,0);
			if( i==vertexOnPlane.rows()-2 ){
				centroidTetra = 0.25*(oriOrderedVerticesOnPlane[i]+oriOrderedVerticesOnPlane[0] + oriBaseOnPolygon + pointInside);
			}else{
				centroidTetra = 0.25*(oriOrderedVerticesOnPlane[i]+oriOrderedVerticesOnPlane[i+1] + oriBaseOnPolygon + pointInside);
			}
			centroidPyramid += (tetraVol*centroidTetra);
			
		}
		centroid += centroidPyramid;
		
		//std::cout<<"oriOrderedVerticesOnPlane.size(): "<<oriOrderedVerticesOnPlane.size()<<", volumePyramid: "<<volumePyramid<<", areaPyramid: "<<areaPyramid<<", height: "<<heightTetra<<endl;
		orderedVerticesOnPlane.clear(); oriOrderedVerticesOnPlane.clear();
	}

	centroid = centroid/totalVolume;
	//std::cout<<"centroid: "<<centroid<<", totalVolume: "<<totalVolume<<endl;
	blockVol = totalVolume;
	vertices.clear();
	verticesOnPlane.clear();
	oriVerticesOnPlane.clear();
	return centroid;
}
bool BlockGen::contactDetectionLPCLPglobal(struct Discontinuity joint, struct Block block, Vector3r& touchingPt){

  if(block.tooSmall == true){return false;}
/* Parameters for particles A and B */
  int planeNoA = block.a.size();
  int persistenceNoA = joint.persistence_a.size();
/* Variables to keep things neat */
  int NUMCON = 1 /* equality */ + planeNoA /*block inequality */ + persistenceNoA /*persistence inequalities */; 
  int NUMVAR = 3/*3D */ + 1 /* s */; 
  double s = 0.0;
  bool converge = true;

  double xlocalA=0; double ylocalA = 0; double zlocalA = 0;
  double xlocalB=0; double ylocalB = 0; double zlocalB = 0;
  Vector3r localA (0,0,0);
  Vector3r xGlobalA (0,0,0);
  Vector3r localB (0,0,0);
  Vector3r xGlobalB (0,0,0);

/* LINEAR CONSTRAINTS */
ClpSimplex  model2;
  
model2.setOptimizationDirection(1);
 // Create space for 3 columns and 10000 rows
int numberRows = NUMCON;
int numberColumns = NUMVAR;
             
// Arrays will be set to default values
model2.resize(0, numberColumns);
            
       
		// Columns - objective was packed
model2.setObjectiveCoefficient(0, 0.0);
model2.setObjectiveCoefficient(1, 0.0);
model2.setObjectiveCoefficient(2, 0.0);
model2.setObjectiveCoefficient(3, 1.0);
model2.setColumnLower(0, -COIN_DBL_MAX);
model2.setColumnLower(1, -COIN_DBL_MAX);
model2.setColumnLower(2, -COIN_DBL_MAX);
model2.setColumnLower(3, -COIN_DBL_MAX);
model2.setColumnUpper(0, COIN_DBL_MAX);
model2.setColumnUpper(1, COIN_DBL_MAX);
model2.setColumnUpper(2, COIN_DBL_MAX);
model2.setColumnUpper(3, COIN_DBL_MAX);
 	  
double rowLower[numberRows];
double rowUpper[numberRows];
	       
               // Rows
  rowLower[0] = joint.a*joint.centre.x() + joint.b*joint.centre.y() + joint.c*joint.centre.z() + joint.d; //3 plane = 0
  for(int i=0; i<planeNoA; i++  ){
    rowLower[1+i] = -COIN_DBL_MAX; 
  };
  for(int i=0; i<persistenceNoA; i++ ){
    rowLower[1+planeNoA+i] =-COIN_DBL_MAX; 
  };

  rowUpper[0] = joint.a*joint.centre.x() + joint.b*joint.centre.y() + joint.c*joint.centre.z() + joint.d; //3 plane = 0
  for(int i=0; i<planeNoA; i++  ){
    rowUpper[1+i] = block.d[i] + block.r; 
  };
  for(int i=0; i<persistenceNoA; i++ ){
    rowUpper[1+planeNoA+i] = joint.persistence_d[i] +( joint.persistence_a[i]*joint.centre.x() + joint.persistence_b[i]*joint.centre.y() + joint.persistence_c[i]*joint.centre.z()); //joint.persistence_d[i] ;
  };


int row1Index[] = {0, 1, 2};
double row1Value[] = {joint.a, joint.b, joint.c};
model2.addRow(3, row1Index, row1Value,rowLower[0], rowUpper[0]);

for (int i = 0; i < planeNoA;i++){
	int rowIndex[] = {0, 1, 2, 3};
	double rowValue[] = {block.a[i], block.b[i], block.c[i], -1.0};
	model2.addRow(4, rowIndex, rowValue,rowLower[1+i], rowUpper[1+i]);
}        

for (int i = 0; i < persistenceNoA;i++){
	int rowIndex[] = {0, 1, 2, 3};
	double rowValue[] = {joint.persistence_a[i], joint.persistence_b[i], joint.persistence_c[i], -1.0};
	model2.addRow(4, rowIndex, rowValue,rowLower[1+planeNoA+i], rowUpper[1+planeNoA+i]);
} 

  model2.scaling(0);
model2.setLogLevel(0);
   model2.primal();
   //model2.writeMps("a_clp.mps");
          // Print column solution


          // Alternatively getColSolution()
          double * columnPrimal = model2.primalColumnSolution();

 	 xlocalA = columnPrimal[0];
	 ylocalA = columnPrimal[1];
	 zlocalA = columnPrimal[2];
	 localA = Vector3r(xlocalA,ylocalA,zlocalA);
	 xGlobalA = localA + block.centre;
	 localB = Vector3r(xlocalB,ylocalB,zlocalB);
	 xGlobalB = localB + joint.centre;
	 touchingPt = localA; //xGlobalA; 
	 s = columnPrimal[3];
  // std::cout<<"xlocalA: "<<xlocalA<<", ylocalA: "<<ylocalA<<", zlocalA: "<<zlocalA<<", s: "<<s<<endl;
 int convergeSuccess = model2.status();
   if(s>-pow(10,-12) || convergeSuccess !=0 ){
	// delete & model2;
	 return false;
   }else{
	//delete & model2;
	 return true;
   }

}


bool BlockGen::contactBoundaryLPCLP(struct Discontinuity joint, struct Block block, Vector3r& touchingPt){

if(block.tooSmall == true){return false;}

  Vector3r solution(0,0,0);
/* Parameters for particles A and B */
  int planeNoA = block.a.size();
/* Variables to keep things neat */
  int NUMCON = planeNoA /*block inequality */; 
  int NUMVAR = 3/*3D */; 
  double s = 0.0;
  double xlocalA=0; double ylocalA = 0; double zlocalA = 0;
  Vector3r localA (0,0,0);
  Vector3r xGlobalA (0,0,0);

/* LINEAR CONSTRAINTS */
ClpSimplex  model2;
  
model2.setOptimizationDirection(1);
 // Create space for 3 columns and 10000 rows
int numberRows = NUMCON;
int numberColumns = NUMVAR;
model2.resize(0, numberColumns);
            
       
// Columns - objective was packed
model2.setObjectiveCoefficient(0, joint.a);
model2.setObjectiveCoefficient(1, joint.b);
model2.setObjectiveCoefficient(2, joint.c);
model2.setColumnLower(0, -COIN_DBL_MAX);
model2.setColumnLower(1, -COIN_DBL_MAX);
model2.setColumnLower(2, -COIN_DBL_MAX);
model2.setColumnUpper(0, COIN_DBL_MAX);
model2.setColumnUpper(1, COIN_DBL_MAX);
model2.setColumnUpper(2, COIN_DBL_MAX);

double rowLower[numberRows];
double rowUpper[numberRows];

// Rows
for(int i=0; i<planeNoA; i++  ){
	rowUpper[i] = block.d[i]+block.r; 
        rowLower[i] = -COIN_DBL_MAX; 
}

for (int i = 0; i < planeNoA;i++){
	int rowIndex[] = {0, 1, 2};
	double rowValue[] = {block.a[i], block.b[i], block.c[i]};
	model2.addRow(3, rowIndex, rowValue,rowLower[i], rowUpper[i]);
}    
model2.scaling(0);               
//model2.setSparseFactorization(0); 
model2.setLogLevel(0);
 //model2.setDualTolerance(10000.0) ;   
   //model2.dual();
model2.primal();   
//model2.writeMps("contactBoundary.mps");

         

          // Alternatively getColSolution()
          double * columnPrimal = model2.primalColumnSolution();

    xGlobalA = Vector3r(columnPrimal[0],columnPrimal[1],columnPrimal[2]);
    touchingPt = xGlobalA; 
  
   
 int convergeSuccess = model2.status();
   if(convergeSuccess==3 ||convergeSuccess==4 ){
	//delete & model2;
	 return false;
  }else{
	//delete & model2;
	 return true;
   }

}

bool BlockGen::contactBoundaryLPCLPslack(struct Discontinuity joint, struct Block block, Vector3r& touchingPt){

if(block.tooSmall == true){return false;}

  Vector3r solution(0,0,0);
/* Parameters for particles A and B */
  int planeNoA = block.a.size();
/* Variables to keep things neat */
  int NUMCON = planeNoA /*block inequality */ + 1; 
  int NUMVAR = 3 +1/*3D */; 
  double s = 0.0;
  double xlocalA=0; double ylocalA = 0; double zlocalA = 0;
  Vector3r localA (0,0,0);
  Vector3r xGlobalA (0,0,0);

ClpSimplex  model2;
          {
		model2.setOptimizationDirection(1);
 // Create space for 3 columns and 10000 rows
               int numberRows = NUMCON;
               int numberColumns = NUMVAR;
               // This is fully dense - but would not normally be so
               int numberElements = numberRows * numberColumns;
               // Arrays will be set to default values
              model2.resize(numberRows, numberColumns);
              double * elements = new double [numberElements];
              CoinBigIndex * starts = new CoinBigIndex [numberColumns+1];
              int * rows = new int [numberElements];;
              int * lengths = new int[numberColumns];
               // Now fill in - totally unsafe but ....
               double * columnLower = model2.columnLower();
               double * columnUpper = model2.columnUpper();
               double * objective = model2.objective();
               double * rowLower = model2.rowLower();
               double * rowUpper = model2.rowUpper();
		// Columns - objective was packed
		  objective[0] = 0.0; //joint.a;
		  objective[1] = 0.0; //joint.b;
		  objective[2] = 0.0; //joint.c;
		  objective[3] = 1.0;
                for (int k = 0; k < numberColumns; k++){
		    columnLower[k]= -COIN_DBL_MAX;                    
		    columnUpper[k] = COIN_DBL_MAX; 
		}
               // Rows
		for(int i=0; i<planeNoA; i++  ){
		    rowUpper[i] = block.d[i]+block.r; 
                    rowLower[i] = -COIN_DBL_MAX; 
                }
		rowUpper[planeNoA] = COIN_DBL_MAX;  
                rowLower[planeNoA] = 0.0; 
               // assign to matrix
             
 // Now elements

  Matrix3r Q1 = Matrix3r::Identity(); // toRotationMatrix() conjugates 
  Matrix3r Q2 = Matrix3r::Identity();



  /* column 0 xA*/
  starts[0] = 0;
  CoinBigIndex put = 0;
  for(int i=0; i < planeNoA; i++){
		elements[put] = (block.a[i]);  rows[put] = (i);
		put++;
  }
elements[put] = (joint.a);  rows[put] = planeNoA;
put++;
  lengths[0] = planeNoA+1;


  /* column 1 yA*/
  starts[1] = put;
  for(int i=0; i < planeNoA; i++){
		elements[put] = (block.b[i]);  rows[put] = (i);
		put++;
  }
elements[put] = (joint.b);  rows[put] = planeNoA;
put++;
  lengths[1] = planeNoA+1;



   /* column 2 zA*/
  starts[2] = put;
  for(int i=0; i < planeNoA; i++){
		elements[put] = (block.c[i]);  rows[put] = (i);
		put++;
  }
elements[put] = (joint.c);  rows[put] = planeNoA;
put++;
  lengths[2] = planeNoA+1;


   /* column 3 s*/
  starts[3] = put;
  for(int i=0; i < planeNoA; i++){
		elements[put] = 0.0;  rows[put] = (i);
		put++;
  }
elements[put] = -1.0;  rows[put] = planeNoA;
put++;
  lengths[3] = planeNoA+1;
  starts[numberColumns] = put;


   CoinPackedMatrix * matrix = new CoinPackedMatrix(true, 0.0, 0.0);
   //matrix->assignMatrix(true, numberRows, numberColumns, numberElements,&aval[0], &asub[0], &aptrb[0], &columnCount[0]);
model2.setLogLevel(0);
   matrix->assignMatrix(true, numberRows, numberColumns, numberElements,elements, rows, starts, lengths);
               ClpPackedMatrix * clpMatrix = new ClpPackedMatrix(matrix);
               model2.replaceMatrix(clpMatrix, true);

}

   model2.primal();
  // model2.writeMps("contactBoundary.mps");
          // Print column solution
          int numberColumns = model2.numberColumns();

          // Alternatively getColSolution()
          double * columnPrimal = model2.primalColumnSolution();

    xGlobalA = Vector3r(columnPrimal[0],columnPrimal[1],columnPrimal[2]);
    touchingPt = xGlobalA; 
  
   
 int convergeSuccess = model2.status();

   if(convergeSuccess==3 || convergeSuccess==4 ){
	//delete & model2;
	 return false;
   }else{
	//delete & model2;
	 return true;
   }

return true;

}


bool BlockGen::checkRedundancyLPCLP(struct Discontinuity joint, struct Block block, Vector3r& touchingPt){

if(block.tooSmall == true){return false;}

  Vector3r solution(0,0,0);
/* Parameters for particles A and B */
  int planeNoA = block.a.size();
/* Variables to keep things neat */
  int NUMCON = planeNoA /*block inequality */; 
  int NUMVAR = 3/*3D */; 
  double s = 0.0;
  double xlocalA=0; double ylocalA = 0; double zlocalA = 0;
  Vector3r localA (0,0,0);
  Vector3r xGlobalA (0,0,0);

ClpSimplex  model2;
          {
		model2.setOptimizationDirection(1);
 // Create space for 3 columns and 10000 rows
               int numberRows = NUMCON;
               int numberColumns = NUMVAR;
               // This is fully dense - but would not normally be so
               int numberElements = numberRows * numberColumns;
               // Arrays will be set to default values
              model2.resize(numberRows, numberColumns);
              double * elements = new double [numberElements];
              CoinBigIndex * starts = new CoinBigIndex [numberColumns+1];
              int * rows = new int [numberElements];;
              int * lengths = new int[numberColumns];
               // Now fill in - totally unsafe but ....
               double * columnLower = model2.columnLower();
               double * columnUpper = model2.columnUpper();
               double * objective = model2.objective();
               double * rowLower = model2.rowLower();
               double * rowUpper = model2.rowUpper();
		// Columns - objective was packed
		  objective[0] = -joint.a;
		  objective[1] = -joint.b;
		  objective[2] = -joint.c;
                for (int k = 0; k < numberColumns; k++){
		    columnLower[k]= -COIN_DBL_MAX;                    
		    columnUpper[k] = COIN_DBL_MAX; 
		}
               // Rows
		for(int i=0; i<planeNoA; i++  ){
			rowLower[i]= -COIN_DBL_MAX; 
		    rowUpper[i] = block.d[i]+block.r;   
		}

               // assign to matrix


  Matrix3r Q1 = Matrix3r::Identity(); // toRotationMatrix() conjugates 
  Matrix3r Q2 = Matrix3r::Identity();
  /* column 0 xA*/
  starts[0] = 0;
  CoinBigIndex put = 0;
  for(int i=0; i < planeNoA; i++){
	elements[put] = (block.a[i]);  rows[put] = (i);
	put++;
  }
  lengths[0] = planeNoA;


  /* column 1 yA*/
  starts[1] = put;
  for(int i=0; i < planeNoA; i++){
	elements[put] =(block.b[i]);  rows[put] = (i);
	put++;
  }
  lengths[1] = planeNoA;


   /* column 2 zA*/
  starts[2] = put;
  for(int i=0; i < planeNoA; i++){
	elements[put] =(block.c[i]);  rows[put] = (i);
	put++;
  }
  lengths[2] = planeNoA;
  starts[numberColumns] = put;

  CoinPackedMatrix * matrix = new CoinPackedMatrix(true, 0.0, 0.0);
   //matrix->assignMatrix(true, numberRows, numberColumns, numberElements,&aval[0], &asub[0], &aptrb[0], &columnCount[0]);
model2.setLogLevel(0);
   matrix->assignMatrix(true, numberRows, numberColumns, numberElements,elements, rows, starts, lengths);
               ClpPackedMatrix * clpMatrix = new ClpPackedMatrix(matrix);
               model2.replaceMatrix(clpMatrix, true);

}
   model2.scaling(0);
   model2.dual();
 //  model2.writeMps("redundancy.mps");
          // Print column solution
          int numberColumns = model2.numberColumns();

          // Alternatively getColSolution()
          double * columnPrimal = model2.primalColumnSolution();

    xGlobalA = Vector3r(columnPrimal[0],columnPrimal[1],columnPrimal[2]);
     touchingPt = xGlobalA; 
Real f = touchingPt.x()*joint.a + touchingPt.y()*joint.b + touchingPt.z()*joint.c - joint.d-block.r;
   

  if(fabs(f)>pow(10,-3) ){
	//delete & model2;
	 return false;
   }else{
	//delete & model2;
	 return true;
   }

}


double BlockGen::inscribedSphereCLP(struct Block block, Vector3r& initialPoint, bool twoDimension){

/* minimise s */
/* s.t. Ax - s <= d*/
  bool converge = true;
/* Parameters for particles A and B */
  double s = 0.0; /* get value of x[3] after optimization */
  int planeNoA = block.a.size(); 
vector<int> plane2Dno;
if (twoDimension == true){
	for (int i=0; i<planeNoA; i++){
		Vector3r plane = Vector3r(block.a[i],block.b[i],block.c[i]);
		if( fabs( plane.dot(Vector3r(0,1,0)) ) >0.99 ){
			plane2Dno.push_back(i);
			//std::cout<<"2d: "<<i<<endl;
		}
	}
	planeNoA = planeNoA-2;
}

  int NUMCON = planeNoA; 
  int NUMVAR = 3/*3D*/ +1;  double xlocalA=0.0; double ylocalA = 0.0; double zlocalA =0.0; Vector3r localA(0,0,0);

ClpSimplex  model2;
  
model2.setOptimizationDirection(1);
 // Create space for 3 columns and 10000 rows
int numberRows = NUMCON;
int numberColumns = NUMVAR;
model2.resize(0, numberColumns);
            
       
// Columns - objective was packed
model2.setObjectiveCoefficient(0, 0.0);
model2.setObjectiveCoefficient(1, 0.0);
model2.setObjectiveCoefficient(2, 0.0);
model2.setObjectiveCoefficient(3,-1.0);
model2.setColumnLower(0, -COIN_DBL_MAX);
model2.setColumnLower(1, -COIN_DBL_MAX);
model2.setColumnLower(2, -COIN_DBL_MAX);
model2.setColumnLower(3, -COIN_DBL_MAX);
model2.setColumnUpper(0, COIN_DBL_MAX);
model2.setColumnUpper(1, COIN_DBL_MAX);
model2.setColumnUpper(2, COIN_DBL_MAX);
model2.setColumnUpper(3, COIN_DBL_MAX);

double rowLower[numberRows];
double rowUpper[numberRows];
if(twoDimension == true){ model2.setColumnLower(1, 0.0);model2.setColumnUpper(1, 0.0);}

int planeIndex[planeNoA];     
		// Rows
		int counter = 0;
		for(int i=0; i<block.a.size(); i++  ){
		    if (twoDimension == true){
				if (i==plane2Dno[0] || i==plane2Dno[1] ){
					continue;
				}
			}
		    rowUpper[counter] = block.d[i]+block.r;
		    planeIndex[counter] = i;
		    counter++;
		}

                for (int k = 0; k < planeNoA; k++) {
                    rowLower[k] = -COIN_DBL_MAX; 
                }


for (int i = 0; i < planeNoA;i++){
	int rowIndex[] = {0, 1, 2, 3};
	double rowValue[] = {block.a[planeIndex[i]], block.b[planeIndex[i]], block.c[planeIndex[i]], 1.0};
	model2.addRow(4, rowIndex, rowValue,rowLower[i], rowUpper[i]);
}        


model2.setLogLevel(0);

model2.scaling(0);
   

   model2.dual();
   //model2.writeMps("inscribedSphere.mps");
          // Print column solution
 

          // Alternatively getColSolution()
          double * columnPrimal = model2.primalColumnSolution();

 	xlocalA = columnPrimal[0];
	 ylocalA = columnPrimal[1];
	 zlocalA = columnPrimal[2];
	 localA = Vector3r(xlocalA,ylocalA,zlocalA);
	 initialPoint = localA; 
	 s = columnPrimal[3];
   
 int convergeSuccess = model2.status();

  if( convergeSuccess!=0){
	//delete & model2;
	return -1; //false;
  }
  //delete & model2;
  return s;
}
void BlockGen::positionRootBody(shared_ptr<Scene>& scene)
{
}
#endif // YADE_POTENTIAL_BLOCKS
