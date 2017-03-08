/* CWBoon 2016 */
#ifdef YADE_POTENTIAL_BLOCKS

#include "Gl1_PotentialBlock.hpp"

//#include<lib-opengl/OpenGLWrapper.hpp>
#include <vtkFloatArray.h>
#include<vtkUnstructuredGrid.h>
#include<vtkXMLUnstructuredGridWriter.h>
#include<vtkTriangle.h>
#include<vtkSmartPointer.h>
#include<vtkFloatArray.h>
#include<vtkCellArray.h>
#include<vtkCellData.h>
#include <vtkSampleFunction.h>
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


#include <vtkRenderWindowInteractor.h> 
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkXMLDataSetWriter.h>

#include<core/Clump.hpp>
//#include<lib-opengl/OpenGLWrapper.hpp> 
#include<pkg/dem/KnKsPBLaw.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<vtkLine.h>
#include <vtkSphereSource.h>
#include <vtkDiskSource.h>
#include <vtkRegularPolygonSource.h>
#include <vtkProperty.h>

#include <vtkLabeledDataMapper.h>
#include <vtkActor2D.h>
#include <vtkVectorText.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkConeSource.h>
#include <vtkCamera.h>
//#include<pkg/dem/Clump.hpp>
#include<pkg/common/Aabb.hpp>
#include <vtkImplicitBoolean.h>
#include <vtkIntArray.h>

#if 0
void Gl1_PotentialBlock::calcMinMax(const shared_ptr<Shape>& cm)
{
	PotentialBlock* pp = static_cast<PotentialBlock*>(cm.get());		
	int planeNo = pp->d.size();
	Real maxD = pp->d[0];

	for (int i=0; i<planeNo; ++i){
		if (pp->d[i] > maxD) {
			maxD = pp->d[i];
		}
	}

	Real R = pp->R;
	Real r = pp->r;
	Real maxTip = R; //std::max(maxD + r, R);
	min =-1.1*pp->minAabb; // 1.5*Vector3r(-maxTip,-maxTip,-maxTip);
	max = 1.1*pp->maxAabb; //-1.0*min;
	
	float dx = (max[0]-min[0])/((float)(sizeX));
	float dy = (max[1]-min[1])/((float)(sizeY));
	float dz = (max[2]-min[2])/((float)(sizeZ));

	isoStep=Vector3r(dx,dy,dz);
}


void Gl1_PotentialBlock::generateScalarField(const shared_ptr<Shape>& cm)
{
	for(int i=0;i<sizeX;i++){
		for(int j=0;j<sizeY;j++){
			for(int k=0;k<sizeZ;k++)
			{
				scalarField[i][j][k] = 0.0;
			}
		}
	}

	for(int i=0;i<sizeX;i++){
		for(int j=0;j<sizeY;j++){
			for(int k=0;k<sizeZ;k++){
				scalarField[i][j][k] = evaluateF(cm,  min[0]+ double(i)*isoStep[0],  min[1]+ double(j)*isoStep[1],  min[2]+double(k)*isoStep[2]);//  
			}
		}
	}
}



vector<Gl1_PotentialBlock::scalarF> Gl1_PotentialBlock::SF;
int Gl1_PotentialBlock::sizeX, Gl1_PotentialBlock::sizeY, Gl1_PotentialBlock::sizeZ;
bool Gl1_PotentialBlock::store;
bool Gl1_PotentialBlock::initialized;

void Gl1_PotentialBlock::clearMemory(){
SF.clear();
}


void Gl1_PotentialBlock::go( const shared_ptr<Shape>& cm, const shared_ptr<State>& state ,bool wire2, const GLViewInfo&){


	PotentialBlock* pp = static_cast<PotentialBlock*>(cm.get());	
	int shapeId = pp->id;
	
if(store == false){
	if(SF.size()>0){SF.clear(); initialized = false; }
	return;
}

if(initialized == false ){
			FOREACH(const shared_ptr<Body>& b, *scene->bodies){
				if (!b) continue;
				const shared_ptr<Shape>&  cmbody=b->shape; 
				calcMinMax(cmbody);
				mc.init(sizeX,sizeY,sizeZ,min,max);
				mc.resizeScalarField(scalarField,sizeX,sizeY,sizeZ);
					SF.push_back(scalarF());
					generateScalarField(cmbody);
					mc.computeTriangulation(scalarField,0.0);
					SF[b->id].triangles = mc.getTriangles();
					SF[b->id].normals = mc.getNormals();
					SF[b->id].nbTriangles = mc.getNbTriangles();
					for(int i=0; i<scalarField.size(); i++){
						for(int j=0; j<scalarField[i].size(); j++) scalarField[i][j].clear();
							scalarField[i].clear();
					}
					scalarField.clear();
			}
			initialized = true;
}


//if(std::isnan(shapeId)==true){return;}

   calcMinMax(cm);
	mc.init(sizeX,sizeY,sizeZ,min,max);
	mc.resizeScalarField(scalarField,sizeX,sizeY,sizeZ);	
	
	

	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3r(cm->color[0],cm->color[1],cm->color[2]));
	glColor3v(cm->color);
	

	 vector<Vector3r>& triangles 	= SF[shapeId].triangles; //mc.getTriangles();
	int nbTriangles			= SF[shapeId].nbTriangles; // //mc.getNbTriangles();
	 vector<Vector3r>& normals 	= SF[shapeId].normals; //mc.getNormals();	

	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING); // 2D
	glEnable(GL_NORMALIZE);
	glBegin(GL_TRIANGLES);

	for(int i=0;i<3*nbTriangles;++i)
	{
		glNormal3v(normals[i]);
		glVertex3v(triangles[i]);
		glNormal3v(normals[++i]);
		glVertex3v(triangles[i]);
		glNormal3v(normals[++i]);
		glVertex3v(triangles[i]);
	}
	glEnd();
	

	return;

}



double Gl1_PotentialBlock::evaluateF(const shared_ptr<Shape>& cm, double x, double y, double z){

		PotentialBlock* pp = static_cast<PotentialBlock*>(cm.get());	
		Real k = pp->k;
		Real r = pp->r;
		Real R = pp->R;	


		int planeNo = pp->a.size();
		
		vector<double>a; vector<double>b; vector<double>c; vector<double>d; vector<double>p; Real pSum3 = 0.0;
		for (int i=0; i<planeNo; i++){
			a.push_back(pp->a[i]);
			b.push_back(pp->b[i]);		
			c.push_back(pp->c[i]);
			d.push_back(pp->d[i]);
			Real plane = a[i]*x + b[i]*y + c[i]*z - d[i]; if (plane<pow(10,-15)){plane = 0.0;} 
			p.push_back(plane);
			pSum3 += pow(p[i],2);
		}
		
		
		Real sphere  = (  pow(x,2) + pow(y,2) + pow(z,2) ) ;
		Real f = (1.0-k)*(pSum3/pow(r,2) - 1.0)+k*(sphere/pow(R,2)-1.0);

		return f;
		
}
#endif

ImpFuncPB * ImpFuncPB::New()
{
    // Skip factory stuff - create class
    return new ImpFuncPB;
}


// Create the function
ImpFuncPB::ImpFuncPB()
{
	clump = false;
    // Initialize members here if you need
}

ImpFuncPB::~ImpFuncPB()
{
    // Initialize members here if you need
}

// Evaluate function
double ImpFuncPB::FunctionValue(double x[3])
{
   int planeNo = a.size();
   vector<double>p; double pSum2 = 0.0;
	if(clump==false){
		Eigen::Vector3d xori(x[0],x[1],x[2]);
		Eigen::Vector3d xlocal = rotationMatrix*xori;
		xlocal[0] = rotationMatrix(0,0)*x[0] + rotationMatrix(0,1)*x[1] + rotationMatrix(0,2)*x[2];
		xlocal[1] = rotationMatrix(1,0)*x[0] + rotationMatrix(1,1)*x[1] + rotationMatrix(1,2)*x[2];
		xlocal[2] = rotationMatrix(2,0)*x[0] + rotationMatrix(2,1)*x[1] + rotationMatrix(2,2)*x[2];
		//std::cout<<"rotationMatrix: "<<endl<<rotationMatrix<<endl;
		   //x[0]=xlocal[0]; x[1]=xlocal[1]; x[2]=xlocal[2];

		   for (int i=0; i<planeNo; i++){
			double plane = a[i]*xlocal[0] + b[i]*xlocal[1] + c[i]*xlocal[2] - d[i]; if (plane<pow(10,-15)){plane = 0.0;} 
			p.push_back(plane);
			pSum2 += pow(p[i],2);
		   }			
		   double sphere  = (  pow(xlocal[0],2) + pow(xlocal[1],2) + pow(xlocal[2],2) ) ;
		  // Real f = (1.0-k)*(pSum2/pow(r,2) - 1.0)+k*(sphere/pow(R,2)-1.0);
		   Real f = (pSum2 - 1.0*pow(r,2) );
  		 return f;
	}else if(clump==true){
		
		Eigen::Vector3d xori(x[0],x[1],x[2]);
		Eigen::Vector3d clumpMemberCentre(clumpMemberCentreX,clumpMemberCentreY,clumpMemberCentreZ);
		Eigen::Vector3d xlocal = xori-clumpMemberCentre;
		//xlocal[0] = rotationMatrix[0]*x[0] + rotationMatrix[3]*x[1] + rotationMatrix[6]*x[2];
		//xlocal[1] = rotationMatrix[1]*x[0] + rotationMatrix[4]*x[1] + rotationMatrix[7]*x[2];
		//xlocal[2] = rotationMatrix[2]*x[0] + rotationMatrix[5]*x[1] + rotationMatrix[8]*x[2];
		//std::cout<<"rotationMatrix: "<<endl<<rotationMatrix<<endl;
		   //x[0]=xlocal[0]; x[1]=xlocal[1]; x[2]=xlocal[2];
		
		   for (int i=0; i<planeNo; i++){
			double plane = a[i]*xlocal[0] + b[i]*xlocal[1] + c[i]*xlocal[2] - d[i]; if (plane<pow(10,-15)){plane = 0.0;} 
			p.push_back(plane);
			pSum2 += pow(p[i],2);
		   }			
		   //double sphere  = (  pow(xlocal[0],2) + pow(xlocal[1],2) + pow(xlocal[2],2) ) ;
		  // Real f = (1.0-k)*(pSum2/pow(r,2) - 1.0)+k*(sphere/pow(R,2)-1.0);
		   Real f = (pSum2 - 1.5*pow(r,2) );
		return f;
  		// return 0;
	}
   // the value of the function
}


void PotentialBlockVTKRecorderTunnel::action(){
	if(fileName.size()==0) return;
	vtkSmartPointer<vtkPoints> pbPos = vtkSmartPointer<vtkPoints>::New();
 	vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkAppendPolyData> appendFilterID = vtkSmartPointer<vtkAppendPolyData>::New();
	//vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	//vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

	// interactions ###############################################
	vtkSmartPointer<vtkPoints> intrBodyPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> intrCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> intrForceN = vtkSmartPointer<vtkFloatArray>::New();
	intrForceN->SetNumberOfComponents(3);
	intrForceN->SetName("forceN");
	vtkSmartPointer<vtkFloatArray> intrAbsForceT = vtkSmartPointer<vtkFloatArray>::New();
	intrAbsForceT->SetNumberOfComponents(1);
	intrAbsForceT->SetName("absForceT");
	// interactions ###############################################

	// interaction contact point ###############################################
	vtkSmartPointer<vtkPoints> pbContactPoint = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> pbCellsContact = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> pbNormalForce = vtkSmartPointer<vtkFloatArray>::New();
	pbNormalForce->SetNumberOfComponents(3);
	pbNormalForce->SetName("normalForce");		//Linear velocity in Vector3 form
	vtkSmartPointer<vtkFloatArray> pbShearForce = vtkSmartPointer<vtkFloatArray>::New();
	pbShearForce->SetNumberOfComponents(3);
	pbShearForce->SetName("shearForce");		//Angular velocity in Vector3 form
	vtkSmartPointer<vtkFloatArray> pbTotalForce = vtkSmartPointer<vtkFloatArray>::New();
	pbTotalForce->SetNumberOfComponents(3);
	pbTotalForce->SetName("totalForce");	
	vtkSmartPointer<vtkFloatArray> pbTotalStress = vtkSmartPointer<vtkFloatArray>::New();
	pbTotalStress->SetNumberOfComponents(3);
	pbTotalStress->SetName("totalStress");	
	vtkSmartPointer<vtkFloatArray> pbMobilizedShear = vtkSmartPointer<vtkFloatArray>::New();
	pbMobilizedShear->SetNumberOfComponents(1);
	pbMobilizedShear->SetName("mobilizedShear");	
	// interactions contact point###############################################


	// velocity ###################################################
	vtkSmartPointer<vtkCellArray> pbCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> pbLinVelVec = vtkSmartPointer<vtkFloatArray>::New();
	pbLinVelVec->SetNumberOfComponents(3);
	pbLinVelVec->SetName("linVelVec");		//Linear velocity in Vector3 form
	
	vtkSmartPointer<vtkFloatArray> pbLinVelLen = vtkSmartPointer<vtkFloatArray>::New();
	pbLinVelLen->SetNumberOfComponents(1);
	pbLinVelLen->SetName("linVelLen");		//Length (magnitude) of linear velocity
	
	vtkSmartPointer<vtkFloatArray> pbAngVelVec = vtkSmartPointer<vtkFloatArray>::New();
	pbAngVelVec->SetNumberOfComponents(3);
	pbAngVelVec->SetName("angVelVec");		//Angular velocity in Vector3 form
	
	vtkSmartPointer<vtkFloatArray> pbAngVelLen = vtkSmartPointer<vtkFloatArray>::New();
	pbAngVelLen->SetNumberOfComponents(1);
	pbAngVelLen->SetName("angVelLen");		//Length (magnitude) of angular velocity

	
	vtkSmartPointer<vtkFloatArray> pbDisplacementVec = vtkSmartPointer<vtkFloatArray>::New();
	pbDisplacementVec->SetNumberOfComponents(3);
	pbDisplacementVec->SetName("Displacement");		//Linear velocity in Vector3 form

	// velocity ####################################################

	// bodyId ##############################################################
	//#if 0
	vtkSmartPointer<vtkPoints> pbPosID = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> pbIdCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkIntArray> blockId = vtkSmartPointer<vtkIntArray>::New();
	blockId->SetNumberOfComponents(1);
	blockId->SetName("id");
	// bodyId ##############################################################
	//#endif
	int countID = 0;
	vtkSmartPointer<vtkVectorText> textArray2[scene->bodies->size()];
	vtkSmartPointer<vtkPolyDataMapper> txtMapper[scene->bodies->size()];
	vtkSmartPointer<vtkLinearExtrusionFilter> extrude[scene->bodies->size()];
	vtkSmartPointer<vtkActor> textActor[scene->bodies->size()];


	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if (!b) continue;
		if (b->isClumpMember() == true ) continue;
		
		//const PotentialBlock* pb=dynamic_cast<PotentialBlock*>(b->shape.get()); 
		//if(!pb) continue;
		if(REC_ID==true){
			//#if 0
			blockId->InsertNextValue(b->getId()); 
			vtkIdType pid[1];
			Vector3r pos(b->state->pos);
			pid[0] = pbPosID->InsertNextPoint(pos[0], pos[1], pos[2]);
			pbIdCells->InsertNextCell(1,pid);
			//#endif
			/* ################# Display id VTK extrusion vector ############## */
			
		#if 0
			textArray2[countID]= vtkVectorText::New();
			//#if 0
			int bid = b->id;
			std::string testString = boost::lexical_cast<std::string>(bid);
			const char * testChar = testString.c_str();
			textArray2[countID]->SetText(testChar);
			extrude[countID] = vtkLinearExtrusionFilter::New();
		   	extrude[countID]->SetInputConnection( textArray2[countID]->GetOutputPort());
			extrude[countID]->SetExtrusionTypeToNormalExtrusion();
			extrude[countID]->SetVector(0, 0, 1.0 );
			extrude[countID]->SetScaleFactor (1.0);
			#if 0
			txtMapper[countID] = vtkPolyDataMapper::New();
			txtMapper[countID]->SetInputConnection( extrude[countID]->GetOutputPort());
			textActor[countID] = vtkActor::New();
			textActor[countID]->SetMapper(txtMapper[countID]);
			textActor[countID]->RotateX(-90);
			textActor[countID]->SetPosition(b->state->pos[0], b->state->pos[1], b->state->pos[2]);
			double contactPtSize = 5.0;
			textActor[countID]->SetScale(3.0*contactPtSize);
			textActor[countID]->GetProperty()->SetColor(0.0,0.0,0.0);
			#endif
			//#if 0
			//txtMapper[countID] = vtkPolyDataMapper::New();
			//txtMapper[countID]->SetInputConnection( extrude[count]->GetOutputPort());
			//appendFilterID->AddInputConnection(extrude[countID]-> GetOutputPort());
			//vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
			//polydata->DeepCopy(extrude[countID]-> GetOutput());
			Vector3r centre (b->state->pos[0], b->state->pos[1], b->state->pos[2]);
			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
			transformFilter->SetInput( extrude[countID]-> GetOutput() );
			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
			transformFilter->SetTransform( transform );
			transform->PostMultiply();
			transform->Translate (centre[0], centre[1],centre[2]);
			//#endif
			appendFilterID->AddInputConnection(transformFilter-> GetOutputPort());
			//polydata->DeleteCells();

		#endif
			countID++;
			
		}
		//vtkSmartPointer<ImpFunc> function = ImpFunc::New();
		vtkSmartPointer<vtkImplicitBoolean> boolFunction = vtkSmartPointer<vtkImplicitBoolean>::New();
		vtkSmartPointer<ImpFuncPB>* functionBool;
		int ImplicitBoolNo = 0;
		double xmin = 0.0; double xmax = 0.0; double ymin = 0.0; double ymax = 0.0; double zmin = 0.0; double zmax =0.0;
		Vector3r particleColour (0,0,0);
		if(b->isClump() == false && b->isClumpMember() == false){
			const PotentialBlock* pb=dynamic_cast<PotentialBlock*>(b->shape.get()); if (pb->isLining==true){continue;}
			function->a = pb->a;  function->b = pb->b; function->c = pb->c; function->d = pb->d; function->R = pb->R; function->r = pb->r; function->k = pb->k; 			Eigen::Matrix3d directionCos = b->state->ori.conjugate().toRotationMatrix(); int count = 0;
			function->clump = false;
			for (int i=0; i<3; i++){
				for (int j=0; j<3; j++){
					//function->rotationMatrix[count] = directionCos(j,i);
					function->rotationMatrix(i,j) = directionCos(j,i);					
					count++;
				}
			}
			xmin = -pb->halfSize.x();  
			xmax = pb->halfSize.x();
			ymin = -pb->halfSize.y();
			ymax = pb->halfSize.y();
			zmin = -pb->halfSize.z();
			zmax = pb->halfSize.z();
			particleColour = pb->color;
		}else if(b->isClump() == true){
			//const Clump* clump = dynamic_cast<Clump*>(b->shape.get()); 
			const shared_ptr<Clump> clump(YADE_PTR_CAST<Clump>(b->shape)); 
			bool firstBound = true;

			//vtkSmartPointer<ImpFunc> functionBool [clump->ids.size()];
			functionBool = new vtkSmartPointer<ImpFuncPB> [clump->ids.size()];
			ImplicitBoolNo = clump->ids.size();
			
			for (unsigned i=0; i<clump->ids.size();i++){
				const shared_ptr<Body>  clumpMember = Body::byId(clump->ids[i],scene);
				const PotentialBlock* pbShape =dynamic_cast<PotentialBlock*>(clumpMember->shape.get()); 
				functionBool[i] = vtkSmartPointer<ImpFuncPB>::New();
				functionBool[i]->R = pbShape->R; 
				functionBool[i]->r = pbShape->r; 
				functionBool[i]->k = pbShape->k; 			
				functionBool[i]->clump=true;
				functionBool[i]->clumpMemberCentreX = clumpMember->state->pos.x()-b->state->pos.x();
				functionBool[i]->clumpMemberCentreY = clumpMember->state->pos.y()-b->state->pos.y();
				functionBool[i]->clumpMemberCentreZ = clumpMember->state->pos.z()-b->state->pos.z();

				Eigen::Matrix3d directionCos = clumpMember->state->ori.conjugate().toRotationMatrix(); 
				for (int j=0; j<pbShape->a.size();j++){
									
					Vector3r plane = directionCos.transpose()*Vector3r(pbShape->a[j], pbShape->b[j], pbShape->c[j]);
					double d = pbShape->d[j]; //-1.0*(plane.x()*(b->state->pos.x()-clumpMember->state->pos.x() ) + plane.y()*(b->state->pos.y()-clumpMember->state->pos.y() ) + plane.z()*(b->state->pos.z()-clumpMember->state->pos.z() ) - pbShape->d[j]);
					functionBool[i]->a.push_back(plane.x() );
					functionBool[i]->b.push_back(plane.y() );
					functionBool[i]->c.push_back(plane.z() );
					functionBool[i]->d.push_back(d );
				}
	
				const Aabb* aabb = static_cast<Aabb*>(clumpMember->bound.get());
				if (firstBound == true){
					xmin = aabb->min.x();
					xmax = aabb->max.x();
					ymin = aabb->min.y();
					ymax = aabb->max.y();
					zmin = aabb->min.z();
					zmax = aabb->max.z();
					firstBound = false;
					particleColour = pbShape->color;
				}else{
					xmin=std::min(xmin,aabb->min.x() );
					xmax=std::max(xmax,aabb->max.x() );
					ymin=std::min(ymin,aabb->min.y() );
					ymax=std::max(ymax,aabb->max.y() );
					zmin=std::min(zmin,aabb->min.z() );
					zmax=std::max(zmax,aabb->max.z() );
				}
				boolFunction->AddFunction(functionBool[i]);
			}
			xmin = xmin - b->state->pos.x();
			xmax = xmax - b->state->pos.x();
			ymin = ymin - b->state->pos.y();
			ymax = ymax - b->state->pos.y();
			zmin = zmin - b->state->pos.z();
			zmax = zmax - b->state->pos.z();
			boolFunction->SetOperationTypeToUnion();
			
		}
		vtkSmartPointer<vtkSampleFunction> sample = vtkSampleFunction::New();
		if(b->isClump() == false && b->isClumpMember() == false){
			sample->SetImplicitFunction(function);
		}else if(b->isClump()==true){
			sample->SetImplicitFunction(boolFunction);
			boolFunction->SetOperationTypeToUnion();
		}
		
		//double xmin = -value; double xmax = value; double ymin = -value; double ymax=value; double zmin=-value; double zmax=value;
		//double xmin = -std::max(pb->minAabb.x(),pb->maxAabb.x()); double xmax = -xmin; double ymin = -std::max(pb->minAabb.y(),pb->maxAabb.y()); double ymax=-ymin; double zmin=-std::max(pb->minAabb.z(),pb->maxAabb.z()); double zmax=-zmin;
		if (twoDimension == true){
			if(sampleY < 2){
				ymin = 0.0;
				ymax = 0.0;
			}else if(sampleZ < 2){
				zmin = 0.0;
				zmax = 0.0;
			}
		}
 		sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);
		//sample->SetModelBounds(pb->minAabb.x(), pb->maxAabb.x(), pb->minAabb.y(), pb->maxAabb.y(), pb->minAabb.z(), pb->maxAabb.z());
		int sampleXno = sampleX; int sampleYno = sampleY; int sampleZno = sampleZ;
		if(fabs(xmax-xmin)/static_cast<double>(sampleX) > maxDimension) { sampleXno = static_cast<int>(fabs(xmax-xmin)/maxDimension); }
		if(fabs(ymax-ymin)/static_cast<double>(sampleY) > maxDimension) { sampleYno = static_cast<int>(fabs(ymax-ymin)/maxDimension); }
		if(fabs(zmax-zmin)/static_cast<double>(sampleZ) > maxDimension) { sampleZno = static_cast<int>(fabs(zmax-zmin)/maxDimension); }
		if (twoDimension == true){
			if(sampleY < 2){
				sampleYno = 1;
			}else if(sampleZ < 2){
				sampleZno = 1;
			}
		}		
		sample->SetSampleDimensions(sampleXno,sampleYno,sampleZno);
		sample->ComputeNormalsOff();
		//sample->Update();
		vtkSmartPointer<vtkContourFilter> contours = vtkContourFilter::New();
		contours->SetInputConnection(sample->GetOutputPort());
		contours->SetNumberOfContours(1);
		contours->SetValue(0,0.0);		 
		vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
		contours->Update();
		polydata->DeepCopy(contours->GetOutput());
		//polydata->Update();
		
		vtkSmartPointer<vtkUnsignedCharArray> pbColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
		pbColors->SetName("pbColors");
		pbColors->SetNumberOfComponents(3);
		Vector3r color = particleColour; //Vector3r(0,100,0);
		if (b->isDynamic() == false){ color = Vector3r(157,157,157); } 
		unsigned char c[3]; //c = {color[0],color[1],color[2]};
		c[0]=color[0];
		c[1]=color[1];
		c[2]=color[2];
		int nbCells=polydata->GetNumberOfPoints(); 
		for (int i=0;i<nbCells;i++){
			pbColors->InsertNextTupleValue(c);	
		}
		polydata->GetPointData()->SetScalars(pbColors);
		//polydata->Update();

		
		Vector3r centre (b->state->pos[0], b->state->pos[1], b->state->pos[2]);
		Quaternionr orientation= b->state->ori; orientation.normalize();
		AngleAxisr aa(orientation); Vector3r axis = aa.axis(); /* axis.normalize(); */ double angle = aa.angle()/3.14159*180.0;	double xAxis = axis[0]; double yAxis = axis[1]; double zAxis = axis[2];	
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetInputData( polydata );
		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

		transformFilter->SetTransform( transform );
		transform->PostMultiply();
		
		transform->Translate (centre[0], centre[1],centre[2]);
		//transform->RotateWXYZ(angle,xAxis, yAxis, zAxis);
		//transformFilter->Update();
		appendFilter->AddInputConnection(transformFilter-> GetOutputPort());


		// ################## velocity ####################
		if(REC_VELOCITY == true){
			vtkIdType pid[1];
			Vector3r pos(b->state->pos);
			pid[0] = pbPos->InsertNextPoint(pos[0], pos[1], pos[2]);
			pbCells->InsertNextCell(1,pid);
			const Vector3r& vel = b->state->vel;
			float v[3]; //v = { vel[0],vel[1],vel[2] };
			v[0]=vel[0];
			v[1]=vel[1];
			v[2]=vel[2];
			pbLinVelVec->InsertNextTupleValue(v);
			pbLinVelLen->InsertNextValue(vel.norm());
			const Vector3r& angVel = b->state->angVel;
			float av[3]; //av = { angVel[0],angVel[1],angVel[2] };
			av[0]=angVel[0];
			av[1]=angVel[1];
			av[2]=angVel[2];	
			pbAngVelVec->InsertNextTupleValue(av);
			pbAngVelLen->InsertNextValue(angVel.norm());
			//if(b->state->refPos.squaredNorm()>0.001){ //if initialized
				Vector3r displacement = pos- b->state->refPos;
				float disp[3];
				disp[0]=displacement[0];
				disp[1]=displacement[1];
				disp[2]=displacement[2];				
				pbDisplacementVec->InsertNextTupleValue(disp);

			//}
		}
		// ################ velocity ###########################
		polydata->DeleteCells();
		sample->Delete();
		contours->Delete();
		//function->Delete();
		//#if 0
		if(b->isClump()==true){
			//boolFunction->Delete();
			for(int i=0; i < ImplicitBoolNo; i++){
				functionBool[i]->Delete();
			}
			//boolFunction = NULL;
		}
		//#endif
		sample = NULL;
		contours = NULL;
	}

	if(REC_VELOCITY == true){
			vtkSmartPointer<vtkUnstructuredGrid> pbUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
			pbUg->SetPoints(pbPos);
			pbUg->SetCells(VTK_VERTEX, pbCells);
			pbUg->GetPointData()->AddArray(pbLinVelVec);
			pbUg->GetPointData()->AddArray(pbAngVelVec);
			pbUg->GetPointData()->AddArray(pbLinVelLen);
			pbUg->GetPointData()->AddArray(pbAngVelLen);
			pbUg->GetPointData()->AddArray(pbDisplacementVec);
			vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerA = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
			writerA->SetDataModeToAscii();
			string fv=fileName+"vel."+std::to_string(scene->iter)+".vtu";
			writerA->SetFileName(fv.c_str());
			writerA->SetInputData(pbUg);
			writerA->Write();
			//writerA->Delete();
			//pbUg->Delete();
	}


	//###################### bodyId ###############################
	if(REC_ID == true){
			#if 0
			vtkSmartPointer<vtkXMLPolyDataWriter> writerA = vtkXMLPolyDataWriter::New();
			writerA->SetDataModeToAscii();
			string fn=fileName+"-Id."+std::to_string(scene->iter)+".vtp";
			writerA->SetFileName(fn.c_str());
			writerA->SetInputConnection(appendFilterID->GetOutputPort());//(extrude->GetOutputPort());
			writerA->Write();
	
			writerA->Delete();	
			#endif
			//#if 0
			vtkSmartPointer<vtkUnstructuredGrid> pbUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
			pbUg->SetPoints(pbPosID);
			pbUg->SetCells(VTK_VERTEX, pbIdCells);
			pbUg->GetPointData()->AddArray(blockId);
			vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerA = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
			writerA->SetDataModeToAscii();
			string fv=fileName+"Id."+std::to_string(scene->iter)+".vtu";
			writerA->SetFileName(fv.c_str());
			writerA->SetInputData(pbUg);
			writerA->Write();
			//writerA->Delete();
			//pbUg->Delete();
			//#endif
	}

#if 0
	if(REC_ID== true){
		int counter =0;
		FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			if (!b) {continue;}
			if (b->isClumpMember()==true) {continue;}
			textArray2[counter]->Delete();
			//txtMapper[count]->Delete();
			extrude[counter]->Delete();
			counter++;
		}
	}

#endif
	// ################## contact point ####################
	if(REC_INTERACTION == true){	
			int count = 0;	
			FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
				if(!I->isReal()) {
					continue;
				}
				const KnKsPBPhys* phys = YADE_CAST<KnKsPBPhys*>(I->phys.get());
				const ScGeom* geom = YADE_CAST<ScGeom*>(I->geom.get());
				vtkIdType pid[1];
				Vector3r pos(geom->contactPoint);
				pid[0] = pbContactPoint->InsertNextPoint(pos[0], pos[1], pos[2]);
				pbCellsContact->InsertNextCell(1,pid);
				//intrBodyPos->InsertNextPoint(geom->contactPoint[0],geom->contactPoint[1],geom->contactPoint[2]); 
				// gives _signed_ scalar of normal force, following the convention used in the respective constitutive law
				float fn[3]={phys->normalForce[0],phys->normalForce[1], phys->normalForce[2]};
				float fs[3]={phys->shearForce[0], phys->shearForce[1],  phys->shearForce[2]};
				float totalForce[3] = {fn[0]+fs[0], fn[1]+fs[1], fn[2]+fs[2]};
				float totalStress[3] = {0.0,0.0,0.0}; //{totalForce[0]/phys->contactArea, totalForce[1]/phys->contactArea, totalForce[2]/phys->contactArea}; 
				float mobilizedShear = phys->mobilizedShear;
				pbTotalForce->InsertNextTupleValue(totalForce);
				pbMobilizedShear->InsertNextValue(mobilizedShear);
				pbNormalForce->InsertNextTupleValue(fn);
				pbShearForce->InsertNextTupleValue(fs);
				pbTotalStress->InsertNextTupleValue(totalStress);
				count++;
			}
			if(count>0){
				vtkSmartPointer<vtkUnstructuredGrid> pbUgCP = vtkSmartPointer<vtkUnstructuredGrid>::New();
				pbUgCP->SetPoints(pbContactPoint);
				pbUgCP->SetCells(VTK_VERTEX, pbCellsContact);
				pbUgCP->GetPointData()->AddArray(pbNormalForce);
				pbUgCP->GetPointData()->AddArray(pbShearForce);
				pbUgCP->GetPointData()->AddArray(pbTotalForce);
				pbUgCP->GetPointData()->AddArray(pbMobilizedShear);
				pbUgCP->GetPointData()->AddArray(pbTotalStress);
				vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerB = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
				writerB->SetDataModeToAscii();
				string fcontact=fileName+"contactPoint."+std::to_string(scene->iter)+".vtu";
				writerB->SetFileName(fcontact.c_str());
				writerB->SetInputData(pbUgCP);
				writerB->Write();
				//writerB->Delete();
				//pbUgCP->Delete();
			}
	}
			

			
	// ################ contact point ###########################
	  	
	 

	
	vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkXMLPolyDataWriter::New();
	writer->SetDataModeToAscii();
	string fn=fileName+"-pb."+std::to_string(scene->iter)+".vtp";
	writer->SetFileName(fn.c_str());
	writer->SetInputConnection(appendFilter->GetOutputPort());
	writer->Write();
	
	writer->Delete();	
	
	//intrBodyPos->Delete(); 
	//intrForceN->Delete();
	//intrAbsForceT->Delete();
	//pbContactPoint->Delete(); 
	//pbCellsContact->Delete(); 
	//pbNormalForce->Delete(); 
	//pbShearForce->Delete();
	//pbCells->Delete();
	//pbLinVelVec->Delete();
	//pbLinVelLen->Delete();
	//pbAngVelVec->Delete();
	//pbAngVelLen->Delete();

}


void PotentialBlockVTKRecorder::action(){
	if(fileName.size()==0) return;
	vtkSmartPointer<vtkPoints> pbPos = vtkSmartPointer<vtkPoints>::New();
 	vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkAppendPolyData> appendFilterID = vtkSmartPointer<vtkAppendPolyData>::New();
	//vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	//vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

	// interactions ###############################################
	vtkSmartPointer<vtkPoints> intrBodyPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> intrCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> intrForceN = vtkSmartPointer<vtkFloatArray>::New();
	intrForceN->SetNumberOfComponents(3);
	intrForceN->SetName("forceN");
	vtkSmartPointer<vtkFloatArray> intrAbsForceT = vtkSmartPointer<vtkFloatArray>::New();
	intrAbsForceT->SetNumberOfComponents(1);
	intrAbsForceT->SetName("absForceT");
	// interactions ###############################################

	// interaction contact point ###############################################
	vtkSmartPointer<vtkPoints> pbContactPoint = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> pbCellsContact = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> pbNormalForce = vtkSmartPointer<vtkFloatArray>::New();
	pbNormalForce->SetNumberOfComponents(3);
	pbNormalForce->SetName("normalForce");		//Linear velocity in Vector3 form
	vtkSmartPointer<vtkFloatArray> pbShearForce = vtkSmartPointer<vtkFloatArray>::New();
	pbShearForce->SetNumberOfComponents(3);
	pbShearForce->SetName("shearForce");		//Angular velocity in Vector3 form
	// interactions contact point###############################################


	// velocity ###################################################
	vtkSmartPointer<vtkCellArray> pbCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> pbLinVelVec = vtkSmartPointer<vtkFloatArray>::New();
	pbLinVelVec->SetNumberOfComponents(3);
	pbLinVelVec->SetName("linVelVec");		//Linear velocity in Vector3 form
	
	vtkSmartPointer<vtkFloatArray> pbLinVelLen = vtkSmartPointer<vtkFloatArray>::New();
	pbLinVelLen->SetNumberOfComponents(1);
	pbLinVelLen->SetName("linVelLen");		//Length (magnitude) of linear velocity
	
	vtkSmartPointer<vtkFloatArray> pbAngVelVec = vtkSmartPointer<vtkFloatArray>::New();
	pbAngVelVec->SetNumberOfComponents(3);
	pbAngVelVec->SetName("angVelVec");		//Angular velocity in Vector3 form
	
	vtkSmartPointer<vtkFloatArray> pbAngVelLen = vtkSmartPointer<vtkFloatArray>::New();
	pbAngVelLen->SetNumberOfComponents(1);
	pbAngVelLen->SetName("angVelLen");		//Length (magnitude) of angular velocity
	// velocity ####################################################

	// bodyId ##############################################################
	//#if 0
	vtkSmartPointer<vtkPoints> pbPosID = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> pbIdCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkIntArray> blockId = vtkSmartPointer<vtkIntArray>::New();
	blockId->SetNumberOfComponents(1);
	blockId->SetName("id");
	// bodyId ##############################################################
	//#endif
	int countID = 0;
	vtkSmartPointer<vtkVectorText> textArray2[scene->bodies->size()];
	vtkSmartPointer<vtkPolyDataMapper> txtMapper[scene->bodies->size()];
	vtkSmartPointer<vtkLinearExtrusionFilter> extrude[scene->bodies->size()];
	vtkSmartPointer<vtkActor> textActor[scene->bodies->size()];


	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if (!b) continue;
		if (b->isClump() == true) continue;
		const PotentialBlock* pb=dynamic_cast<PotentialBlock*>(b->shape.get()); 
		if(!pb) continue;
	
		if(REC_ID==true){
			//#if 0
			blockId->InsertNextValue(b->getId()); 
			vtkIdType pid[1];
			Vector3r pos(b->state->pos);
			pid[0] = pbPosID->InsertNextPoint(pos[0], pos[1], pos[2]);
			pbIdCells->InsertNextCell(1,pid);
			//#endif
	
			countID++;	
		}
		//vtkSmartPointer<ImpFuncPB> function = ImpFuncPB::New();
		function->a = pb->a;  function->b = pb->b; function->c = pb->c; function->d = pb->d; function->R = pb->R; function->r = pb->r; function->k = pb->k; 			Eigen::Matrix3d directionCos = b->state->ori.conjugate().toRotationMatrix(); int count = 0;
		for (int i=0; i<3; i++){
			for (int j=0; j<3; j++){
				//function->rotationMatrix[count] = directionCos(j,i);
				function->rotationMatrix(i,j) = directionCos(j,i);
				count++;
			}
		}


		vtkSmartPointer<vtkSampleFunction> sample = vtkSampleFunction::New();
		sample->SetImplicitFunction(function);
		double value = 1.05*pb->R; 

		
		double xmin = -pb->halfSize.x(); double xmax = pb->halfSize.x(); double ymin = -pb->halfSize.y(); double ymax=pb->halfSize.y(); double zmin=-pb->halfSize.z(); double zmax=pb->halfSize.z();
		//double xmin = -value; double xmax = value; double ymin = -value; double ymax=value; double zmin=-value; double zmax=value;
		//double xmin = -std::max(pb->minAabb.x(),pb->maxAabb.x()); double xmax = -xmin; double ymin = -std::max(pb->minAabb.y(),pb->maxAabb.y()); double ymax=-ymin; double zmin=-std::max(pb->minAabb.z(),pb->maxAabb.z()); double zmax=-zmin;
		if(twoDimension==true){
			ymax = 0.0;
			ymin = 0.0;
		}
 		sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);
		//sample->SetModelBounds(pb->minAabb.x(), pb->maxAabb.x(), pb->minAabb.y(), pb->maxAabb.y(), pb->minAabb.z(), pb->maxAabb.z());
		int sampleXno = sampleX; int sampleYno = sampleY; int sampleZno = sampleZ;
		if(fabs(xmax-xmin)/static_cast<double>(sampleX) > maxDimension) { sampleXno = static_cast<int>(fabs(xmax-xmin)/maxDimension); }
		if(fabs(ymax-ymin)/static_cast<double>(sampleY) > maxDimension) { sampleYno = static_cast<int>(fabs(ymax-ymin)/maxDimension); }
		if(fabs(zmax-zmin)/static_cast<double>(sampleZ) > maxDimension) { sampleZno = static_cast<int>(fabs(zmax-zmin)/maxDimension); }
		if(twoDimension==true){sampleYno=1;}
		sample->SetSampleDimensions(sampleXno,sampleYno,sampleZno);
		sample->ComputeNormalsOff();
		//sample->Update();
		vtkSmartPointer<vtkContourFilter> contours = vtkContourFilter::New();
		contours->SetInputConnection(sample->GetOutputPort());
		contours->SetNumberOfContours(1);
		contours->SetValue(0,0.0);		 
		vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
		contours->Update();
		polydata->DeepCopy(contours->GetOutput());
		//polydata->Update();
		
		vtkSmartPointer<vtkUnsignedCharArray> pbColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
		pbColors->SetName("pbColors");
		pbColors->SetNumberOfComponents(3);
		Vector3r color = pb->color; //Vector3r(0,100,0);
		//if (b->isDynamic() == false){ color = Vector3r(157,157,157); } 
		color = Vector3r(157,157,157);
		unsigned char c[3]; //c = {color[0],color[1],color[2]};
		c[0]=color[0];
		c[1]=color[1];
		c[2]=color[2];
		int nbCells=polydata->GetNumberOfPoints(); 
		for (int i=0;i<nbCells;i++){
			pbColors->InsertNextTupleValue(c);	
		}	
		polydata->GetPointData()->SetScalars(pbColors);
		//polydata->Update();

		
		

		Vector3r centre (b->state->pos[0], b->state->pos[1], b->state->pos[2]);
		Quaternionr orientation= b->state->ori; orientation.normalize();
		AngleAxisr aa(orientation); Vector3r axis = aa.axis(); /* axis.normalize(); */ double angle = aa.angle()/3.14159*180.0;	double xAxis = axis[0]; double yAxis = axis[1]; double zAxis = axis[2];	
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetInputData( polydata );
		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

		transformFilter->SetTransform( transform );
		transform->PostMultiply();
		
		transform->Translate (centre[0], centre[1],centre[2]);
		//transform->RotateWXYZ(angle,xAxis, yAxis, zAxis);
		//transformFilter->Update();
		appendFilter->AddInputConnection(transformFilter-> GetOutputPort());


		// ################## velocity ####################
		if(REC_VELOCITY == true){
			vtkIdType pid[1];
			Vector3r pos(b->state->pos);
			pid[0] = pbPos->InsertNextPoint(pos[0], pos[1], pos[2]);
			pbCells->InsertNextCell(1,pid);
			const Vector3r& vel = b->state->vel;
			float v[3]; //v = { vel[0],vel[1],vel[2] };
			v[0]=vel[0];
			v[1]=vel[1];
			v[2]=vel[2];
			pbLinVelVec->InsertNextTupleValue(v);
			pbLinVelLen->InsertNextValue(vel.norm());
			const Vector3r& angVel = b->state->angVel;
			float av[3]; //av = { angVel[0],angVel[1],angVel[2] };
			av[0]=angVel[0];
			av[1]=angVel[1];
			av[2]=angVel[2];	
			pbAngVelVec->InsertNextTupleValue(av);
			pbAngVelLen->InsertNextValue(angVel.norm());
		}
		// ################ velocity ###########################
		polydata->DeleteCells();
		sample->Delete();
		contours->Delete();
		//function->Delete();
		sample = NULL;
		contours = NULL;
	}

	if(REC_VELOCITY == true){
			vtkSmartPointer<vtkUnstructuredGrid> pbUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
			pbUg->SetPoints(pbPos);
			pbUg->SetCells(VTK_VERTEX, pbCells);
			pbUg->GetPointData()->AddArray(pbLinVelVec);
			pbUg->GetPointData()->AddArray(pbAngVelVec);
			pbUg->GetPointData()->AddArray(pbLinVelLen);
			pbUg->GetPointData()->AddArray(pbAngVelLen);
			vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerA = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
			writerA->SetDataModeToAscii();
			string fv=fileName+"vel."+std::to_string(scene->iter)+".vtu";
			writerA->SetFileName(fv.c_str());
			writerA->SetInputData(pbUg);
			writerA->Write();
			//writerA->Delete();
			//pbUg->Delete();
	}

	//###################### bodyId ###############################
	if(REC_ID == true){
			#if 0
			vtkSmartPointer<vtkXMLPolyDataWriter> writerA = vtkXMLPolyDataWriter::New();
			writerA->SetDataModeToAscii();
			string fn=fileName+"-Id."+std::to_string(scene->iter)+".vtp";
			writerA->SetFileName(fn.c_str());
			writerA->SetInputConnection(appendFilterID->GetOutputPort());//(extrude->GetOutputPort());
			writerA->Write();
	
			writerA->Delete();	
			#endif
			//#if 0
			vtkSmartPointer<vtkUnstructuredGrid> pbUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
			pbUg->SetPoints(pbPosID);
			pbUg->SetCells(VTK_VERTEX, pbIdCells);
			pbUg->GetPointData()->AddArray(blockId);
			vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerA = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
			writerA->SetDataModeToAscii();
			string fv=fileName+"Id."+std::to_string(scene->iter)+".vtu";
			writerA->SetFileName(fv.c_str());
			writerA->SetInputData(pbUg);
			writerA->Write();
			//writerA->Delete();
			//pbUg->Delete();
			//#endif
	}


//#if 0
	// ################## contact point ####################
	if(REC_INTERACTION == true){	
			int count = 0;	
			FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
				if(!I->isReal()) {
					continue;
				}
				const KnKsPBPhys* phys = YADE_CAST<KnKsPBPhys*>(I->phys.get());
				const ScGeom* geom = YADE_CAST<ScGeom*>(I->geom.get());
				vtkIdType pid[1];
				Vector3r pos(geom->contactPoint);
				pid[0] = pbContactPoint->InsertNextPoint(pos[0], pos[1], pos[2]);
				pbCellsContact->InsertNextCell(1,pid);
				//intrBodyPos->InsertNextPoint(geom->contactPoint[0],geom->contactPoint[1],geom->contactPoint[2]); 
				// gives _signed_ scalar of normal force, following the convention used in the respective constitutive law
				float fn[3]={phys->normalForce[0],phys->normalForce[1], phys->normalForce[2]};
				float fs[3]={phys->shearForce[0], phys->shearForce[1],  phys->shearForce[2]};
				pbNormalForce->InsertNextTupleValue(fn);
				pbShearForce->InsertNextTupleValue(fs);
				count++;
			}
			if(count>0){
				vtkSmartPointer<vtkUnstructuredGrid> pbUgCP = vtkSmartPointer<vtkUnstructuredGrid>::New();
				pbUgCP->SetPoints(pbContactPoint);
				pbUgCP->SetCells(VTK_VERTEX, pbCellsContact);
				pbUgCP->GetPointData()->AddArray(pbNormalForce);
				pbUgCP->GetPointData()->AddArray(pbShearForce);
				vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerB = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
				writerB->SetDataModeToAscii();
				string fcontact=fileName+"contactPoint."+std::to_string(scene->iter)+".vtu";
				writerB->SetFileName(fcontact.c_str());
				writerB->SetInputData(pbUgCP);
				writerB->Write();
				//writerB->Delete();
				//pbUgCP->Delete();
			}
	}
//#endif		

			
	// ################ contact point ###########################
	  	
	 

	
	vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkXMLPolyDataWriter::New();
	writer->SetDataModeToAscii();
	string fn=fileName+"-pb."+std::to_string(scene->iter)+".vtp";
	writer->SetFileName(fn.c_str());
	writer->SetInputConnection(appendFilter->GetOutputPort());
	writer->Write();
	
	writer->Delete();	
	
	//intrBodyPos->Delete(); 
	//intrForceN->Delete();
	//intrAbsForceT->Delete();
	//pbContactPoint->Delete(); 
	//pbCellsContact->Delete(); 
	//pbNormalForce->Delete(); 
	//pbShearForce->Delete();
	//pbCells->Delete();
	//pbLinVelVec->Delete();
	//pbLinVelLen->Delete();
	//pbAngVelVec->Delete();
	//pbAngVelLen->Delete();

}

YADE_PLUGIN(/*(Gl1_PotentialBlock)*/(PotentialBlockVTKRecorder)(PotentialBlockVTKRecorderTunnel));


//YADE_REQUIRE_FEATURE(OPENGL)

#endif // YADE_POTENTIAL_BLOCKS








