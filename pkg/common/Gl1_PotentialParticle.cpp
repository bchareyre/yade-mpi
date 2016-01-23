/*CWBoon 2015 */
#ifdef YADE_POTENTIAL_PARTICLES
#include "Gl1_PotentialParticle.hpp"

//#include<lib-opengl/OpenGLWrapper.hpp>
#include <vtkFloatArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkTriangle.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkSampleFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkWriter.h>
#include <vtkExtractVOI.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>

#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkAppendPolyData.h>


#include <vtkRenderWindowInteractor.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkXMLDataSetWriter.h>


#include <lib/opengl/OpenGLWrapper.hpp>
#include <pkg/dem/KnKsLaw.hpp>
#include <pkg/dem/ScGeom.hpp>
#include <vtkLine.h>
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
#include <pkg/common/Aabb.hpp>
#include <vtkImplicitBoolean.h>
#include <vtkIntArray.h>

#ifdef YADE_OPENGL
void Gl1_PotentialParticle::calcMinMax(const PotentialParticle& pp) {
	int planeNo = pp.d.size();
	Real maxD = pp.d[0];

	for (int i=0; i<planeNo; ++i) {
		if (pp.d[i] > maxD) {
			maxD = pp.d[i];
		}
	}

	min = -aabbEnlargeFactor*pp.minAabb;
	max =  aabbEnlargeFactor*pp.maxAabb;

	Real dx = (max[0]-min[0])/((Real)(sizeX-1));
	Real dy = (max[1]-min[1])/((Real)(sizeY-1));
	Real dz = (max[2]-min[2])/((Real)(sizeZ-1));

	isoStep=Vector3r(dx,dy,dz);
}


void Gl1_PotentialParticle::generateScalarField(const PotentialParticle& pp) {
	for(int i=0; i<sizeX; i++) {
		for(int j=0; j<sizeY; j++) {
			for(int k=0; k<sizeZ; k++) {
				scalarField[i][j][k] = evaluateF(pp,  min[0]+ Real(i)*isoStep[0],  min[1]+ Real(j)*isoStep[1],  min[2]+Real(k)*isoStep[2]);//
			}
		}
	}
}



vector<Gl1_PotentialParticle::scalarF> Gl1_PotentialParticle::SF;
int Gl1_PotentialParticle::sizeX, Gl1_PotentialParticle::sizeY, Gl1_PotentialParticle::sizeZ;
Real Gl1_PotentialParticle::aabbEnlargeFactor;
bool Gl1_PotentialParticle::store;
bool Gl1_PotentialParticle::initialized;


void Gl1_PotentialParticle::go( const shared_ptr<Shape>& cm, const shared_ptr<State>& state ,bool wire2, const GLViewInfo&) {

	PotentialParticle* pp = static_cast<PotentialParticle*>(cm.get());
	int shapeId = pp->id;

	if(store == false) {
		if(SF.size()>0) {
			SF.clear();
			initialized = false;
		}
	}


	if(initialized == false ) {
		FOREACH(const shared_ptr<Body>& b, *scene->bodies) {
			if (!b) continue;
			PotentialParticle* cmbody = dynamic_cast<PotentialParticle*>(b->shape.get());
			if (!cmbody) continue;
			calcMinMax(*cmbody);
			mc.init(sizeX,sizeY,sizeZ,min,max);
			mc.resizeScalarField(scalarField,sizeX,sizeY,sizeZ);
			SF.push_back(scalarF());
			generateScalarField(*cmbody);
			mc.computeTriangulation(scalarField,0.0);
			SF[b->id].triangles = mc.getTriangles();
			SF[b->id].normals = mc.getNormals();
			SF[b->id].nbTriangles = mc.getNbTriangles();
			for(unsigned int i=0; i<scalarField.size(); i++) {
				for(unsigned int j=0; j<scalarField[i].size(); j++) scalarField[i][j].clear();
				scalarField[i].clear();
			}
			scalarField.clear();
		}
		initialized = true;
	}

	// FIXME : check that : one of those 2 lines are useless
	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3r(cm->color[0],cm->color[1],cm->color[2]));
	glColor3v(cm->color);


	const vector<Vector3r>& triangles = SF[shapeId].triangles; //mc.getTriangles();
	int nbTriangles = SF[shapeId].nbTriangles; // //mc.getNbTriangles();
	const vector<Vector3r>& normals = SF[shapeId].normals; //mc.getNormals();

	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING); // 2D
	glEnable(GL_NORMALIZE);
	glBegin(GL_TRIANGLES);

	for(int i=0; i<3*nbTriangles; ++i) {
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



Real Gl1_PotentialParticle::evaluateF(const PotentialParticle& pp, Real x, Real y, Real z) {
	Real k = pp.k;
	Real r = pp.r;
	Real R = pp.R;


	int planeNo = pp.a.size();

	vector<Real>a;
	vector<Real>b;
	vector<Real>c;
	vector<Real>d;
	vector<Real>p;
	Real pSum3 = 0.0;
	for (int i=0; i<planeNo; i++) {
		a.push_back(pp.a[i]);
		b.push_back(pp.b[i]);
		c.push_back(pp.c[i]);
		d.push_back(pp.d[i]);
		Real plane = a[i]*x + b[i]*y + c[i]*z - d[i];
		if (plane<pow(10,-15)) {
			plane = 0.0;
		}
		p.push_back(plane);
		pSum3 += pow(p[i],2);
	}

	Real sphere  = (  pow(x,2) + pow(y,2) + pow(z,2) ) ;
	Real f = (1.0-k)*(pSum3/pow(r,2) - 1.0)+k*(sphere/pow(R,2)-1.0);

	return f;
}
#endif // YADE_OPENGL


ImpFunc * ImpFunc::New() {
	// Skip factory stuff - create class
	return new ImpFunc;
}


// Create the function
ImpFunc::ImpFunc() {
	clump = false;
	// Initialize members here if you need
}

ImpFunc::~ImpFunc() {
	// Initialize members here if you need
}

// Evaluate function
Real ImpFunc::FunctionValue(Real x[3]) {
	int planeNo = a.size();
	vector<Real>p;
	Real pSum2 = 0.0;
	if (!clump) {
		Eigen::Vector3d xori(x[0],x[1],x[2]);
		Eigen::Vector3d xlocal = rotationMatrix*xori;
		xlocal[0] = rotationMatrix(0,0)*x[0] + rotationMatrix(0,1)*x[1] + rotationMatrix(0,2)*x[2];
		xlocal[1] = rotationMatrix(1,0)*x[0] + rotationMatrix(1,1)*x[1] + rotationMatrix(1,2)*x[2];
		xlocal[2] = rotationMatrix(2,0)*x[0] + rotationMatrix(2,1)*x[1] + rotationMatrix(2,2)*x[2];
		//std::cout<<"rotationMatrix: "<<endl<<rotationMatrix<<endl;
		//x[0]=xlocal[0]; x[1]=xlocal[1]; x[2]=xlocal[2];

		for (int i=0; i<planeNo; i++) {
			Real plane = a[i]*xlocal[0] + b[i]*xlocal[1] + c[i]*xlocal[2] - d[i];
			if (plane<pow(10,-15)) {
				plane = 0.0;
			}
			p.push_back(plane);
			pSum2 += pow(p[i],2);
		}
		Real sphere  = (  pow(xlocal[0],2) + pow(xlocal[1],2) + pow(xlocal[2],2) ) ;
		Real f = (1.0-k)*(pSum2/pow(r,2) - 1.0)+k*(sphere/pow(R,2)-1.0);
		return f;
	} else {

		Eigen::Vector3d xori(x[0],x[1],x[2]);
		Eigen::Vector3d clumpMemberCentre(clumpMemberCentreX,clumpMemberCentreY,clumpMemberCentreZ);
		Eigen::Vector3d xlocal = xori-clumpMemberCentre;
		//xlocal[0] = rotationMatrix[0]*x[0] + rotationMatrix[3]*x[1] + rotationMatrix[6]*x[2];
		//xlocal[1] = rotationMatrix[1]*x[0] + rotationMatrix[4]*x[1] + rotationMatrix[7]*x[2];
		//xlocal[2] = rotationMatrix[2]*x[0] + rotationMatrix[5]*x[1] + rotationMatrix[8]*x[2];
		//std::cout<<"rotationMatrix: "<<endl<<rotationMatrix<<endl;
		//x[0]=xlocal[0]; x[1]=xlocal[1]; x[2]=xlocal[2];

		for (int i=0; i<planeNo; i++) {
			Real plane = a[i]*xlocal[0] + b[i]*xlocal[1] + c[i]*xlocal[2] - d[i];
			if (plane<pow(10,-15)) {
				plane = 0.0;
			}
			p.push_back(plane);
			pSum2 += pow(p[i],2);
		}
		Real sphere  = (  pow(xlocal[0],2) + pow(xlocal[1],2) + pow(xlocal[2],2) ) ;
		Real f = (1.0-k)*(pSum2/pow(r,2) - 1.0)+k*(sphere/pow(R,2)-1.0);
		return f;
		// return 0;
	}
	// the value of the function
}





void PotentialParticleVTKRecorder::action() {
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
	vtkSmartPointer<vtkPoints> pbPosID = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> pbIdCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkIntArray> blockId = vtkSmartPointer<vtkIntArray>::New();
	blockId->SetNumberOfComponents(1);
	blockId->SetName("id");
	// bodyId ##############################################################
	int countID = 0;
	vtkSmartPointer<vtkVectorText> textArray2[scene->bodies->size()];
	vtkSmartPointer<vtkPolyDataMapper> txtMapper[scene->bodies->size()];
	vtkSmartPointer<vtkLinearExtrusionFilter> extrude[scene->bodies->size()];
	vtkSmartPointer<vtkActor> textActor[scene->bodies->size()];


	FOREACH(const shared_ptr<Body>& b, *scene->bodies) {
		if (!b) continue;
		if (b->isClump() == true) continue;
		const PotentialParticle* pb=dynamic_cast<PotentialParticle*>(b->shape.get());
		if(!pb) continue;

		if(REC_ID==true) {
			blockId->InsertNextValue(b->getId());
			vtkIdType pid[1];
			Vector3r pos(b->state->pos);
			pid[0] = pbPosID->InsertNextPoint(pos[0], pos[1], pos[2]);
			pbIdCells->InsertNextCell(1,pid);

			countID++;
		}
		//vtkSmartPointer<ImpFunc> function = ImpFunc::New();
		function->a = pb->a;
		function->b = pb->b;
		function->c = pb->c;
		function->d = pb->d;
		function->R = pb->R;
		function->r = pb->r;
		function->k = pb->k;
		Eigen::Matrix3d directionCos = b->state->ori.conjugate().toRotationMatrix();
		int count = 0;
		for (int i=0; i<3; i++) {
			for (int j=0; j<3; j++) {
				//function->rotationMatrix[count] = directionCos(j,i);
				function->rotationMatrix(i,j) = directionCos(j,i);
				count++;
			}
		}


		vtkSmartPointer<vtkSampleFunction> sample = vtkSampleFunction::New();
		sample->SetImplicitFunction(function);

		Real xmin = -std::max(pb->minAabb.x(),pb->maxAabb.x());
		Real xmax = -xmin;
		Real ymin = -std::max(pb->minAabb.y(),pb->maxAabb.y());
		Real ymax=-ymin;
		Real zmin=-std::max(pb->minAabb.z(),pb->maxAabb.z());
		Real zmax=-zmin;
		if(twoDimension==true) {
			ymax = 0.0;
			ymin = 0.0;
		}
		sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);
		//sample->SetModelBounds(pb->minAabb.x(), pb->maxAabb.x(), pb->minAabb.y(), pb->maxAabb.y(), pb->minAabb.z(), pb->maxAabb.z());
		int sampleXno = sampleX;
		int sampleYno = sampleY;
		int sampleZno = sampleZ;
		if(fabs(xmax-xmin)/static_cast<Real>(sampleX) > maxDimension) {
			sampleXno = static_cast<int>(fabs(xmax-xmin)/maxDimension);
		}
		if(fabs(ymax-ymin)/static_cast<Real>(sampleY) > maxDimension) {
			sampleYno = static_cast<int>(fabs(ymax-ymin)/maxDimension);
		}
		if(fabs(zmax-zmin)/static_cast<Real>(sampleZ) > maxDimension) {
			sampleZno = static_cast<int>(fabs(zmax-zmin)/maxDimension);
		}
		if(twoDimension==true) {
			sampleYno=1;
		}
		sample->SetSampleDimensions(sampleXno,sampleYno,sampleZno);
		sample->ComputeNormalsOff();
		//sample->Update();
		vtkSmartPointer<vtkContourFilter> contours = vtkContourFilter::New();
    #ifdef YADE_VTK6
		  contours->SetInputData(sample->GetOutput());
    #else
		  contours->SetInput(sample->GetOutput());
		#endif
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
		if (b->isDynamic() == false) {
			color = Vector3r(157,157,157);
		}
		unsigned char c[3]; //c = {color[0],color[1],color[2]};
		c[0]=color[0];
		c[1]=color[1];
		c[2]=color[2];
		int nbCells=polydata->GetNumberOfPoints();
		for (int i=0; i<nbCells; i++) {
			pbColors->InsertNextTupleValue(c);
		}
		polydata->GetPointData()->SetScalars(pbColors);
		//polydata->Update();




		Vector3r centre (b->state->pos[0], b->state->pos[1], b->state->pos[2]);
		Quaternionr orientation= b->state->ori;
		orientation.normalize();
		AngleAxisr aa(orientation);
		//Vector3r axis = aa.axis();
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    #ifdef YADE_VTK6
		  transformFilter->SetInputData( polydata );
    #else
		  transformFilter->SetInput( polydata );
		#endif
		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

		transformFilter->SetTransform( transform );
		transform->PostMultiply();

		transform->Translate (centre[0], centre[1],centre[2]);
		//transform->RotateWXYZ(angle,xAxis, yAxis, zAxis);
		//transformFilter->Update();
		appendFilter->AddInputConnection(transformFilter-> GetOutputPort());


		// ################## velocity ####################
		if(REC_VELOCITY == true) {
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

	if(REC_VELOCITY == true) {
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
    #ifdef YADE_VTK6
		  writerA->SetInputData(pbUg);
    #else
		  writerA->SetInput(pbUg);
		#endif
		writerA->Write();
		//writerA->Delete();
		//pbUg->Delete();
	}

	//###################### bodyId ###############################
	if(REC_ID == true) {
		vtkSmartPointer<vtkUnstructuredGrid> pbUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
		pbUg->SetPoints(pbPosID);
		pbUg->SetCells(VTK_VERTEX, pbIdCells);
		pbUg->GetPointData()->AddArray(blockId);
		vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerA = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
		writerA->SetDataModeToAscii();
		string fv=fileName+"Id."+std::to_string(scene->iter)+".vtu";
		writerA->SetFileName(fv.c_str());
    #ifdef YADE_VTK6
		  writerA->SetInputData(pbUg);
    #else
		  writerA->SetInput(pbUg);
		#endif
		writerA->Write();
		//writerA->Delete();
		//pbUg->Delete();
	}


	// ################## contact point ####################
	if(REC_INTERACTION == true) {
		int count = 0;
		FOREACH(const shared_ptr<Interaction>& I, *scene->interactions) {
			if(!I->isReal()) {
				continue;
			}
			const KnKsPhys* phys = YADE_CAST<KnKsPhys*>(I->phys.get());
			const ScGeom* geom = YADE_CAST<ScGeom*>(I->geom.get());
			vtkIdType pid[1];
			Vector3r pos(geom->contactPoint);
			pid[0] = pbContactPoint->InsertNextPoint(pos[0], pos[1], pos[2]);
			pbCellsContact->InsertNextCell(1,pid);
			//intrBodyPos->InsertNextPoint(geom->contactPoint[0],geom->contactPoint[1],geom->contactPoint[2]);
			// gives _signed_ scalar of normal force, following the convention used in the respective constitutive law
			float fn[3]= {(float)phys->normalForce[0], (float)phys->normalForce[1], (float)phys->normalForce[2]};
			float fs[3]= {(float)phys->shearForce[0],  (float)phys->shearForce[1],  (float)phys->shearForce[2]};
			pbNormalForce->InsertNextTupleValue(fn);
			pbShearForce->InsertNextTupleValue(fs);
			count++;
		}
		if(count>0) {
			vtkSmartPointer<vtkUnstructuredGrid> pbUgCP = vtkSmartPointer<vtkUnstructuredGrid>::New();
			pbUgCP->SetPoints(pbContactPoint);
			pbUgCP->SetCells(VTK_VERTEX, pbCellsContact);
			pbUgCP->GetPointData()->AddArray(pbNormalForce);
			pbUgCP->GetPointData()->AddArray(pbShearForce);
			vtkSmartPointer<vtkXMLUnstructuredGridWriter> writerB = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
			writerB->SetDataModeToAscii();
			string fcontact=fileName+"contactPoint."+std::to_string(scene->iter)+".vtu";
			writerB->SetFileName(fcontact.c_str());
      #ifdef YADE_VTK6
			  writerB->SetInputData(pbUgCP);
      #else
			  writerB->SetInput(pbUgCP);
		  #endif
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

YADE_PLUGIN((Gl1_PotentialParticle)(PotentialParticleVTKRecorder));


//YADE_REQUIRE_FEATURE(OPENGL)

#endif // YADE_POTENTIAL_PARTICLES
