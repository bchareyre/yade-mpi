/*CWBoon 2016 */
/* Please cite: */
/* CW Boon, GT Houlsby, S Utili (2015).  Designing Tunnel Support in Jointed Rock Masses Via the DEM.  Rock Mechanics and Rock Engineering,  48 (2), 603-632. */

#ifdef YADE_POTENTIAL_BLOCKS

#include "RockBolt.hpp"
#include<pkg/dem/KnKsLaw.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<core/Omega.hpp>
#include <ctime>
#include <cstdlib>



#include<vtkUnstructuredGrid.h>
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
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include<vtkXMLPolyDataWriter.h>
#include <vtkAppendPolyData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkXMLDataSetWriter.h>


#include<vtkLine.h>
#include <vtkSphereSource.h>
#include <vtkDiskSource.h>
#include <vtkRegularPolygonSource.h>
#include <vtkProperty.h>
#include <vtkLabeledDataMapper.h>
#include <vtkVectorText.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkIntArray.h>

#include <vtkLineSource.h>



void RockBolt::action(){

	if (openingCreated == true && installed == false){
			vector<double> distanceFrOpening; 
			FOREACH(const shared_ptr<Body>& b, *scene->bodies){
				if (!b) continue;
				if (b->isClump() == true) continue;
				PotentialBlock* pb=static_cast<PotentialBlock*>(b->shape.get()); 
				if(!pb) continue;
				if(pb->isBoundary == true || pb->erase== true || pb->isLining==true){continue;}	
				State* state1 = b->state.get();				
				Vector3r intersectionPt(0,0,0);
				if ( installBolts(pb,  state1, startingPoint, boltDirection, boltLength, intersectionPt )){
					blockIDs.push_back(b->id);
					pb->isBolt = true;
					distanceFrOpening.push_back((intersectionPt-startingPoint).norm());
				}
			}
			
			/* sort blocks according to distance from the centre */
			int totalBlocks = blockIDs.size();
			for (int i=0; i<totalBlocks; i++){
				double distance = distanceFrOpening[i];
				int blockID = blockIDs[i];
				int ihole = i;
				while (ihole> 0 && distanceFrOpening[ihole-1] > distance){
					distanceFrOpening[ihole] = distanceFrOpening[ihole-1];
					blockIDs[ihole] = blockIDs[ihole-1];
					ihole = ihole-1;
				}			
				distanceFrOpening[ihole] = distance;
				blockIDs[ihole] = blockID;	
			}
			
		
			Vector3r jointIntersection (0,0,0);
			for (int j=0; j<totalBlocks; j++){
				State* state1 = Body::byId(blockIDs[j],scene)->state.get();
				Shape* shape1 = Body::byId(blockIDs[j],scene)->shape.get();
				PotentialBlock *pb=static_cast<PotentialBlock*>(shape1);
				int totalPlanes = pb->a.size();
				int intersectNo = 0;
				vector<Vector3r> tempCoord; vector<double> distance;
				for (int i=0; i<totalPlanes; i++){						
					Vector3r plane = state1->ori*Vector3r(pb->a[i], pb->b[i], pb->c[i]); double planeD = plane.dot(state1->pos) + pb->d[i] +pb->r;
					if ( intersectPlane(pb, state1,startingPoint,boltDirection, boltLength, jointIntersection, plane, planeD)){
						double sign = plane.dot(boltDirection);
						jointIntersection = jointIntersection - Mathr::Sign(sign)*halfActiveLength*boltDirection;
						distance.push_back(jointIntersection.norm() ); 
						jointIntersection = state1->ori.conjugate()*(jointIntersection-state1->pos); 
						intersectNo++;
						if(intersectNo >2){std::cout<<"intersectNo > 2: "<<intersectNo<<endl;}else{ tempCoord.push_back(jointIntersection);}
					}
				}
				if(distance.size()==1 ){
					//localCoordinates.push_back(tempCoord[0]);
					/*add last*/
					Vector3r endPoint = startingPoint + boltLength*boltDirection;
					State* state1  = Body::byId(blockIDs[blockIDs.size()-1],scene)->state.get();
					endPoint = state1 ->ori.conjugate()*(endPoint-state1->pos);
					if(useMidPoint == false){
						localCoordinates.push_back(tempCoord[0]);
					}else{
						localCoordinates.push_back(0.5*(endPoint+tempCoord[0]));
					}
					localCoordinates.push_back(endPoint);
				}else{
					if(useMidPoint == false){
						if(distance[0] < distance[1] ){
							localCoordinates.push_back(tempCoord[0]);localCoordinates.push_back(tempCoord[1]);
						}else{
							localCoordinates.push_back(tempCoord[1]);localCoordinates.push_back(tempCoord[0]);
						}
					}else{
						
						Vector3r midPoint = 0.5*(tempCoord[0] + tempCoord[1]);
						if(j!=0){
							localCoordinates.push_back(midPoint);
							localCoordinates.push_back(midPoint);
						}else{
							if(distance[0] < distance[1] ){
								localCoordinates.push_back(tempCoord[0]);localCoordinates.push_back(midPoint);
							}else{
								localCoordinates.push_back(tempCoord[1]);localCoordinates.push_back(midPoint);
							}
						}
					}
				}
				tempCoord.clear(); distance.clear();
				//std::cout<<"j: "<<j<<", intersectNo: "<<intersectNo<<endl;
				
			}
			#if 0
			/* add first */
			vector<Vector3r> tempCoord = localCoordinates; localCoordinates.clear();
			State* stateB = Body::byId(blockIDs[0],scene)->state.get();
			Vector3r startPt = stateB->ori.conjugate()*(startingPoint-stateB->pos);
			localCoordinates.push_back(startPt);
			for (int i=0; i<tempCoord.size(); i++){
				localCoordinates.push_back(tempCoord[i]);
			}
			tempCoord.clear();
			#endif
			#if 0
			/* add last */
			if(localCoordinates.size() < 2*totalBlocks){
				Vector3r endPoint = startingPoint + boltLength*boltDirection;
				State* stateA = Body::byId(blockIDs[blockIDs.size()-1],scene)->state.get();
				endPoint = stateA->ori.conjugate()*(endPoint-stateA->pos);
				localCoordinates.push_back(endPoint);
				
			}
			#endif
			installed = true;
			distanceFrOpening.clear();
				
	}
	if (installed == true && blockIDs.size()>=2){
			
			averageForce = 0.0; maxForce = 0.0;
			int blockNo = blockIDs.size();
			for (int j=1; j<blockNo;j++){
				State* state1 = Body::byId(blockIDs[j-1],scene)->state.get();
				State* state2 = Body::byId(blockIDs[j],scene)->state.get();
				Shape* shape1 = Body::byId(blockIDs[j-1],scene)->shape.get();
				Shape* shape2 = Body::byId(blockIDs[j],scene)->shape.get();
				PotentialBlock *s1=static_cast<PotentialBlock*>(shape1);
				PotentialBlock *s2=static_cast<PotentialBlock*>(shape2);
				Vector3r nodeDistance = getNodeDistance(s1,state1,s2,state2,localCoordinates[2*j-1],localCoordinates[2*j]); /* 2 minus 1, from 1 to 2 */
				
				if (initialLength.size() < blockNo-1 ){ /*not initialized */
					initialLength.push_back(nodeDistance.norm()*Mathr::Sign(nodeDistance.dot(boltDirection) )); /* negative if there is overlap */
					initialDirection.push_back(nodeDistance);
					forces.push_back(0.0); axialForces.push_back(0.0); shearForces.push_back(0.0);ruptured.push_back(false);
					nodeDistanceVec.push_back(nodeDistance);
					nodePosition.push_back(Vector3r(0,0,0)); distanceFrCentre.push_back(0.0);
				}else{
					if (resetLengthInit == true){
						initialLength[j-1] =nodeDistance.norm()*Mathr::Sign(nodeDistance.dot(boltDirection) );
						resetLengthInit = false;
					}
					Vector3r direction = nodeDistance; direction.normalize(); double dirSign = 1.0;
					nodeDistanceVec[j-1] =nodeDistance;
					//if (initialDirection[j-1].norm()>pow(10,-11) ){ 
					//	dirSign = direction.dot(initialDirection[j-1]);
					//}else{
						dirSign =  direction.dot(boltDirection); //FIXME assume special case does not happen, i.e., activeLength is long enough
					//}					
					Vector3r axialForce = (normalStiffness*(Mathr::Sign(dirSign)*nodeDistance.norm() - initialLength[j-1])+preTension)*(Mathr::Sign(dirSign)*direction);/* the last term makes sure tension is always pointing in the direction of boltdirection */
					//Vector3r axialForce = (axialStiffness/initialLength[j-1]*(Mathr::Sign(dirSign)*nodeDistance.norm() - initialLength[j-1])+preTension)*(Mathr::Sign(dirSign)*direction);/* the last term makes sure tension is always pointing in the direction of boltdirection */
					Vector3r shearDir = boltDirection.cross(Vector3r(0,1,0)); shearDir.normalize();
					Vector3r shearForce = shearStiffness*(nodeDistance.dot(shearDir))*shearDir;
	
					if(axialForce.norm()>axialMax || shearForce.norm()>shearMax || ruptured[j-1] == true){
						axialForce = Vector3r(0,0,0); shearForce = Vector3r(0,0,0); ruptured[j-1]= true;		
					}
					axialForces[j-1] = axialForce.norm();
					shearForces[j-1] = shearForce.norm();
					forces[j-1] = (axialForce +shearForce).norm(); //*Mathr::Sign(dirSign);
					averageForce += forces[j-1];
					maxForce = std::max(maxForce,forces[j-1]);
					 
					Vector3r totalForce = axialForce + shearForce; 
					Vector3r c1x = state1->ori*localCoordinates[2*j-1]+0.5*nodeDistance;
					nodePosition[j-1] = state1->pos + c1x;
					distanceFrCentre[j-1]=nodePosition[j-1].dot(boltDirection);
					if(j==1){
						displacements=(state1->pos+state1->ori*localCoordinates[0]).dot(boltDirection);
					}
					Vector3r c2x = state2->ori*localCoordinates[2*j]-0.5*nodeDistance;
					scene->forces.addTorque(blockIDs[j-1],c1x.cross(totalForce));
					scene->forces.addTorque(blockIDs[j],-c2x.cross(totalForce));
					scene->forces.addForce(blockIDs[j-1],totalForce );
					scene->forces.addForce(blockIDs[j],-totalForce );
					
				}
			}
		
			averageForce = averageForce/static_cast<double>(blockNo-1);
		
	}
	if ((scene->iter-vtkRefTimeStep)%vtkIteratorInterval == 0 && installed == true && blockIDs.size()>=2){
	 	vtkRefTimeStep = scene->iter;
	 	vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
		int blockNo = blockIDs.size();

		/// BOLT FORCE //
		vtkSmartPointer<vtkPoints> boltNodalPoints = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkCellArray> boltNodalPointsCells = vtkSmartPointer<vtkCellArray>::New();
		vtkSmartPointer<vtkPoints> boltNode = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkCellArray> boltNodeCells = vtkSmartPointer<vtkCellArray>::New();
		vtkSmartPointer<vtkFloatArray> boltNodalForce = vtkSmartPointer<vtkFloatArray>::New();
		boltNodalForce->SetNumberOfComponents(3);
		boltNodalForce->SetName("Bolt Force");		//Linear velocity in Vector3 form
		vtkSmartPointer<vtkFloatArray> boltAxialForce = vtkSmartPointer<vtkFloatArray>::New();
		boltAxialForce->SetNumberOfComponents(3);
		boltAxialForce->SetName("AxialForce");		//Linear velocity in Vector3 form
		vtkSmartPointer<vtkFloatArray> boltShearForce = vtkSmartPointer<vtkFloatArray>::New();
		boltShearForce->SetNumberOfComponents(3);
		boltShearForce->SetName("Shear Force");		//Linear velocity in Vector3 form
		//#if 0;
		for (int i=0; i <blockNo; i++){
			State* state1 = Body::byId(blockIDs[i],scene)->state.get();
		  	Vector3r globalPoint1 = state1->ori*localCoordinates[2*i]+state1->pos;
			Vector3r globalPoint2 = state1->ori*localCoordinates[2*i+1]+state1->pos;
			vtkSmartPointer<vtkLineSource> lineSource =  vtkSmartPointer<vtkLineSource>::New();
			double p0[3] = {globalPoint1[0], globalPoint1[1], globalPoint1[2]};
  			double p1[3] = {globalPoint2[0], globalPoint2[1], globalPoint2[2]};
			lineSource->SetPoint1(p0);
			lineSource->SetPoint2(p1);
			appendFilter->AddInputConnection(lineSource-> GetOutputPort());
	
			vtkIdType pid2[1];  
			pid2[0]= boltNodalPoints->InsertNextPoint(globalPoint1[0], globalPoint1[1], globalPoint1[2]);
			boltNodalPointsCells->InsertNextCell(1,pid2);
			pid2[0]= boltNodalPoints->InsertNextPoint(globalPoint2[0], globalPoint2[1], globalPoint2[2]);
			boltNodalPointsCells->InsertNextCell(1,pid2);

			if(i < blockNo-1){
				/* draw a line between joints*/
				State* state2 = Body::byId(blockIDs[i+1],scene)->state.get();
				Vector3r globalPoint3 = state2->ori*localCoordinates[2*i+2]+state2->pos;
				vtkSmartPointer<vtkLineSource> lineSourceJoint =  vtkSmartPointer<vtkLineSource>::New();
				double p2[3] = {globalPoint2[0], globalPoint2[1], globalPoint2[2]};
	  			double p3[3] = {globalPoint3[0], globalPoint3[1], globalPoint3[2]};
				lineSourceJoint->SetPoint1(p2);
				lineSourceJoint->SetPoint2(p3);
				appendFilter->AddInputConnection(lineSourceJoint-> GetOutputPort());

				
				/* try to draw forces */
				vtkIdType pid[1]; Vector3r midPoint = 0.5*(globalPoint2+globalPoint3);
				pid[0] = boltNode->InsertNextPoint( midPoint[0],  midPoint[1],  midPoint[2]);
				boltNodeCells->InsertNextCell(1,pid);
				Vector3r plotDirection = boltDirection.cross(Vector3r(0,1,0));
				if(plotDirection.dot(Vector3r(1,0,0)) <0.0){
					plotDirection = -plotDirection;
				}
				plotDirection.normalize();
				Vector3r nodalForce = forces[i]*plotDirection;
				float f[3]={nodalForce[0],nodalForce[1],nodalForce[2]};
				boltNodalForce->InsertNextTupleValue(f);

				Vector3r axialForce = axialForces[i]*plotDirection;
				float fa[3]={axialForce[0],axialForce[1],axialForce[2]};
				boltAxialForce->InsertNextTupleValue(fa);

				Vector3r shearForce = shearForces[i]*plotDirection;
				float fs[3]={shearForce[0],shearForce[1],shearForce[2]};
				boltShearForce->InsertNextTupleValue(fs);

			}
			
			//lineSource->Update();		
  		}
		//#endif

		vtkSmartPointer<vtkUnstructuredGrid> pbUgCP2 = vtkSmartPointer<vtkUnstructuredGrid>::New();
		pbUgCP2->SetPoints(boltNodalPoints);
		pbUgCP2->SetCells(VTK_VERTEX, boltNodalPointsCells);
		vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerC = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
		writerC->SetDataModeToAscii();
		string fileBoltC=fileName+"boltNodalPoints"+name+"."+std::to_string(scene->iter)+".vtu";
		writerC->SetFileName(fileBoltC.c_str());
		writerC->SetInputData(pbUgCP2);
		writerC->Write();

		vtkSmartPointer<vtkUnstructuredGrid> pbUgCP = vtkSmartPointer<vtkUnstructuredGrid>::New();
		pbUgCP->SetPoints(boltNode);
		pbUgCP->SetCells(VTK_VERTEX, boltNodeCells);
		pbUgCP->GetPointData()->AddArray(boltNodalForce);
		pbUgCP->GetPointData()->AddArray(boltAxialForce);
		pbUgCP->GetPointData()->AddArray(boltShearForce);
		vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerB = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
		writerB->SetDataModeToAscii();
		string fileBolt=fileName+"boltNodeForce"+name+"."+std::to_string(scene->iter)+".vtu";
		writerB->SetFileName(fileBolt.c_str());
		writerB->SetInputData(pbUgCP);
		writerB->Write();

		vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkXMLPolyDataWriter::New();
		writer->SetDataModeToAscii();
		string fn=fileName+"-bolt"+name+"."+std::to_string(scene->iter)+".vtp";
		writer->SetFileName(fn.c_str());
		writer->SetInputConnection(appendFilter->GetOutputPort());
		writer->Write();
	}
	
}


Vector3r RockBolt::getNodeDistance(const PotentialBlock* cm1,const State* state1,const PotentialBlock* cm2,const State* state2, const Vector3r localPt1, const Vector3r localPt2){
	Vector3r nodeDist = Vector3r(0,0,0.0);
	Vector3r global1 = state1->ori*localPt1 + state1->pos;
	Vector3r global2 = state2->ori*localPt2 + state2->pos;
	
	return (global2-global1);
	
}


double RockBolt::evaluateFNoSphereVol(const PotentialBlock* s1,const State* state1, const Vector3r newTrial){
	
	Vector3r tempP1 = newTrial - state1->pos;  
	/* Direction cosines */
	//state1.ori.normalize();
	Vector3r localP1 = state1->ori.conjugate()*tempP1; 
	Real x = localP1.x();
	Real y = localP1.y();
	Real z = localP1.z();
	int planeNo = s1->a.size();

	Real r = s1->r;  int insideCount = 0;
	for (int i=0; i<planeNo; i++){
		Real plane = s1->a[i]*x + s1->b[i]*y + s1->c[i]*z - s1->d[i]-1.0002*r; //-pow(10,-10); 
		if (Mathr::Sign(plane)*1.0<0.0){
			insideCount++;
		}
	}
	
	/* Complete potential particle */
	Real f = 1.0;
	if (insideCount == planeNo){ f = -1.0;}

	return f;

}


bool RockBolt::installBolts(const PotentialBlock* s1,const State* state1,const Vector3r startingPt,const Vector3r direction, const double length, Vector3r& intersectionPt){

  Vector3r endPt = startingPt + length*direction;
  
 // PotentialBlock *s1=static_cast<PotentialBlock*>(cm1.get());
  int planeNoA = s1->a.size();


/* line equality */
// x = x0 + t*dirX
// y = y0 + t*dirY
// z = z0 + t*dirZ

/* linear inequality for blocks */
// Ax - d < 0

/* Variables to keep things neat */
  int NUMCON = 3 /* equality */ + planeNoA /*block inequality */; 
  int NUMVAR = 3/*3D */ + 1 /*t */+ 1 /* s */; 
  double s = 0.0;
  bool converge = true;

  Matrix3r Q1 = (state1->ori.conjugate()).toRotationMatrix(); 
  Eigen::MatrixXd A1 = Eigen::MatrixXd::Zero(planeNoA,3);
  for (int i=0; i < planeNoA; i++){
	A1(i,0) = s1->a[i]; A1(i,1) = s1->b[i]; A1(i,2) = s1->c[i];
  }
  Eigen::MatrixXd AQ1 = A1*Q1;
  Eigen::MatrixXd pos1(3,1); 
  pos1(0,0) = state1->pos.x(); 
  pos1(1,0) = state1->pos.y(); 
  pos1(2,0) = state1->pos.z();
  Eigen::MatrixXd Q1pos1 = AQ1*pos1;


ClpSimplex  model2;

model2.setOptimizationDirection(1);
 // Create space for 3 columns and 10000 rows
               int numberRows = NUMCON;
               int numberColumns = NUMVAR;
               // This is fully dense - but would not normally be so
            
               // Arrays will be set to default values
              model2.resize(0, numberColumns);
	model2.setObjectiveCoefficient(0,0.0);
	model2.setObjectiveCoefficient(1,0.0);
	model2.setObjectiveCoefficient(2,0.0);
	model2.setObjectiveCoefficient(3,0.0);
	model2.setObjectiveCoefficient(4,1.0);
              
		for (int k = 0; k < 3; k++){
		    model2.setColumnLower(k,-COIN_DBL_MAX);
		    model2.setColumnUpper(k,COIN_DBL_MAX);                   
		}
                model2.setColumnLower(3,openingRad);
		   model2.setColumnUpper(3,length); 
		 model2.setColumnLower(4,-COIN_DBL_MAX);
		    model2.setColumnUpper(4,COIN_DBL_MAX);  
               // Rows
	double rowLower[numberRows];
	double rowUpper[numberRows];

  


  



rowLower[0] = startingPt.x();
rowLower[1] = startingPt.y();
rowLower[2] = startingPt.z();

rowUpper[0] = startingPt.x();
rowUpper[1] = startingPt.y();
rowUpper[2] = startingPt.z();

for (int k = 0; k < planeNoA; k++){
		    rowLower[3+k]= -COIN_DBL_MAX;                    
		    rowUpper[3+k] = s1->d[k] + s1->r + Q1pos1(k,0); 
}

int row1Index[] = {0,3};
double row1Value[] = {1.0,  -1.0*direction.x()};
model2.addRow(2,row1Index,row1Value,rowLower[0],rowUpper[0]);

int row2Index[] = {1,3};
double row2Value[] = {1.0,  -1.0*direction.y()};
model2.addRow(2,row2Index,row2Value,rowLower[1],rowUpper[1]);

int row3Index[] = {2,3};
double row3Value[] = {1.0,  -1.0*direction.z()};
model2.addRow(2,row3Index,row3Value,rowLower[2],rowUpper[2]);

for (int i=0; i<planeNoA; i++){

	int rowIndex[] = {0,1,2,4};
	double rowValue[] = {AQ1(i,0),AQ1(i,1),AQ1(i,2),-1.0};
	model2.addRow(4,rowIndex,rowValue,rowLower[3+i],rowUpper[3+i]);
}

model2.scaling(0);
model2.setLogLevel(0);
model2.primal();

          double * columnPrimal = model2.primalColumnSolution();


	 Vector3r temp = Vector3r(columnPrimal[0],columnPrimal[1],columnPrimal[2]);
	 intersectionPt = temp; //state1->ori.conjugate()*(temp-state1->pos); 
	 s = columnPrimal[4];

 int convergeSuccess = model2.status();
   if(s>-pow(10,-8) || convergeSuccess !=0){
	 return false;
   }else{
	 return true;
   }

}


bool RockBolt::intersectPlane(const PotentialBlock* s1,const State* state1,const Vector3r startingPt,const Vector3r direction, const double length, Vector3r& intersectionPt, const Vector3r plane, const double planeD){
  bool feasible = true;
  Vector3r endPt = startingPt + length*direction;
  
 // PotentialBlock *s1=static_cast<PotentialBlock*>(cm1.get());
  int planeNoA = s1->a.size();

/* Variables to keep things neat */
  int NUMCON = 3 /* equality */ + 1 /*planeEquality */; 
  int NUMVAR = 3/*3D */ + 1 /*t */; 
  double t = 0.0;
  bool converge = true;

/* line equality */
// x = x0 + t*dirX
// y = y0 + t*dirY
// z = z0 + t*dirZ

/* linear equality for blocks */
// Ax - d = 0
/* LINEAR CONSTRAINTS */
  
  
ClpSimplex  model2;
          
model2.setOptimizationDirection(1);
 // Create space for 3 columns and 10000 rows
               int numberRows = NUMCON;
               int numberColumns = NUMVAR;
               // This is fully dense - but would not normally be so
            
               // Arrays will be set to default values
              model2.resize(0, numberColumns);
	model2.setObjectiveCoefficient(0,0.0);
	model2.setObjectiveCoefficient(1,0.0);
	model2.setObjectiveCoefficient(2,0.0);
	model2.setObjectiveCoefficient(3,1.0);

              
		for (int k = 0; k < 4; k++){
		    model2.setColumnLower(k,-COIN_DBL_MAX);
		    model2.setColumnUpper(k,COIN_DBL_MAX);                   
		}
                
               // Rows
	double rowLower[numberRows];
	double rowUpper[numberRows];

  

  



rowLower[0] = startingPt.x();
rowLower[1] = startingPt.y();
rowLower[2] = startingPt.z();
rowLower[3] = planeD;
rowUpper[0] = startingPt.x();
rowUpper[1] = startingPt.y();
rowUpper[2] = startingPt.z();
rowUpper[3] = planeD;

int row1Index[] = {0,3};
double row1Value[] = {1.0,  -1.0*direction.x()};
model2.addRow(2,row1Index,row1Value,rowLower[0],rowUpper[0]);

int row2Index[] = {1,3};
double row2Value[] = {1.0,  -1.0*direction.y()};
model2.addRow(2,row2Index,row2Value,rowLower[1],rowUpper[1]);

int row3Index[] = {2,3};
double row3Value[] = {1.0,  -1.0*direction.z()};
model2.addRow(2,row3Index,row3Value,rowLower[2],rowUpper[2]);

int row4Index[] = {0,1,2};
double row4Value[] = {plane.x(),plane.y(),plane.z()};
model2.addRow(3,row4Index,row4Value,rowLower[3],rowUpper[3]);

model2.scaling(0);
model2.setLogLevel(0);
model2.primal();
          double * columnPrimal = model2.primalColumnSolution();


	 Vector3r temp = Vector3r(columnPrimal[0],columnPrimal[1],columnPrimal[2]);
	 intersectionPt = temp; //state1->ori.conjugate()*(temp-state1->pos); 
	 t = columnPrimal[3];

 

	
double f = evaluateFNoSphereVol(s1,state1,intersectionPt);
//std::cout<<"t: "<<t<<", f: "<<f<<", status: "<<status<<endl;
 int convergeSuccess = model2.status();
   if( t>1.001*length || t< 0.0 || f> 0.0|| convergeSuccess !=0){
	 return false;
   }else{
	 return true;
   }

}

YADE_PLUGIN((RockBolt));
#endif // YADE_POTENTIAL_BLOCKS
