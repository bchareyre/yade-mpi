#include"VTKRecorder.hpp"
#include<vtkCellArray.h>
#include<vtkPoints.h>
#include<vtkPointData.h>
#include<vtkCellData.h>
#include<vtkSmartPointer.h>
#include<vtkFloatArray.h>
#include<vtkUnstructuredGrid.h>
#include<vtkXMLUnstructuredGridWriter.h>
#include<vtkZLibDataCompressor.h>
//#include<vtkXMLMultiBlockDataWriter.h>
//#include<vtkMultiBlockDataSet.h>
#include<vtkTriangle.h>
#include<vtkLine.h>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingFacet.hpp>
#include<yade/pkg-dem/ConcretePM.hpp>


YADE_PLUGIN((VTKRecorder));
YADE_REQUIRE_FEATURE(VTK)
CREATE_LOGGER(VTKRecorder);

VTKRecorder::VTKRecorder() 
{ 
	/* we always want to save the first state as well */ 
	initRun=true; 
	compress=false;
	skipFacetIntr=true;
	skipNondynamic=false;
}

VTKRecorder::~VTKRecorder()
{
	
}

void VTKRecorder::init(MetaBody* rootBody)
{
}

void VTKRecorder::action(MetaBody* rootBody)
{
	vector<bool> recActive(REC_SENTINEL,false);
	FOREACH(string& rec, recorders){
		if(rec=="spheres") recActive[REC_SPHERES]=true;
		else if(rec=="velocity") recActive[REC_VELOCITY]=true;
		else if(rec=="facets") recActive[REC_FACETS]=true;
		else if(rec=="colors") recActive[REC_COLORS]=true;
		else if(rec=="cpm") recActive[REC_CPM]=true;
		// else if(rec=="intr") recActive[REC_INTR]=true;
		else LOG_ERROR("Unknown recorder named `"<<rec<<"' (supported are: spheres, velocity, facets, colors, cpm). Ignored.");
	}
	// cpm needs interactions
	if(recActive[REC_CPM]) recActive[REC_INTR]=true;

	// spheres
	vtkSmartPointer<vtkPoints> spheresPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> spheresCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> radii = vtkSmartPointer<vtkFloatArray>::New();
	radii->SetNumberOfComponents(1);
	radii->SetName("Radii");
	vtkSmartPointer<vtkFloatArray> spheresColors = vtkSmartPointer<vtkFloatArray>::New();
	spheresColors->SetNumberOfComponents(3);
	spheresColors->SetName("Colors");
	vtkSmartPointer<vtkFloatArray> spheresVelocity = vtkSmartPointer<vtkFloatArray>::New();
	spheresVelocity->SetNumberOfComponents(3);
	spheresVelocity->SetName("velocity");

	// facets
	vtkSmartPointer<vtkPoints> facetsPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> facetsCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> facetsColors = vtkSmartPointer<vtkFloatArray>::New();
	facetsColors->SetNumberOfComponents(3);
	facetsColors->SetName("Colors");

	// interactions
	vtkSmartPointer<vtkPoints> intrBodyPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> intrCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> intrForceN = vtkSmartPointer<vtkFloatArray>::New();
	intrForceN->SetNumberOfComponents(1);
	intrForceN->SetName("forceN");
	vtkSmartPointer<vtkFloatArray> intrAbsForceT = vtkSmartPointer<vtkFloatArray>::New();
	intrAbsForceT->SetNumberOfComponents(3);
	intrAbsForceT->SetName("absForceT");

	// extras for CPM
	if(recActive[REC_CPM]) CpmStateUpdater::update(rootBody);
	vtkSmartPointer<vtkFloatArray> cpmDamage = vtkSmartPointer<vtkFloatArray>::New();
	cpmDamage->SetNumberOfComponents(1);
	cpmDamage->SetName("cpmDamage");
	vtkSmartPointer<vtkFloatArray> cpmSigma = vtkSmartPointer<vtkFloatArray>::New();
	cpmSigma->SetNumberOfComponents(3);
	cpmSigma->SetName("cpmSigma");
	vtkSmartPointer<vtkFloatArray> cpmSigmaM = vtkSmartPointer<vtkFloatArray>::New();
	cpmSigmaM->SetNumberOfComponents(1);
	cpmSigmaM->SetName("cpmSigmaM");
	vtkSmartPointer<vtkFloatArray> cpmTau = vtkSmartPointer<vtkFloatArray>::New();
	cpmTau->SetNumberOfComponents(3);
	cpmTau->SetName("cpmTau");

	if(recActive[REC_INTR]){
		// save body positions, referenced by ids by vtkLine
		FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
			const Vector3r& pos=b->state->pos;
			intrBodyPos->InsertNextPoint(pos[0],pos[1],pos[2]);
		}
		FOREACH(const shared_ptr<Interaction>& I, *rootBody->interactions){
			if(!I->isReal()) continue;
			//const NormalShearInteraction* phys = YADE_CAST<NormalShearInteraction*>(i->interactionPhysics.get());
			if(skipFacetIntr){
				if(!(dynamic_cast<InteractingSphere*>(Body::byId(I->getId1())->interactingGeometry.get()))) continue;
				if(!(dynamic_cast<InteractingSphere*>(Body::byId(I->getId2())->interactingGeometry.get()))) continue;
			}
			vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
			line->GetPointIds()->SetId(0,I->getId1());
			line->GetPointIds()->SetId(1,I->getId2());
			intrCells->InsertNextCell(line);
			if(recActive[REC_CPM]){
				const CpmPhys* phys = YADE_CAST<CpmPhys*>(I->interactionPhysics.get());
				intrForceN->InsertNextValue(phys->Fn);
				float fs[3]={abs(phys->shearForce[0]),abs(phys->shearForce[1]),abs(phys->shearForce[2])};
				intrAbsForceT->InsertNextTupleValue(fs);
			}
		}
	}

	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if (!b) continue;

		if (recActive[REC_SPHERES])
		{
			const InteractingSphere* sphere = dynamic_cast<InteractingSphere*>(b->interactingGeometry.get()); 
			if (sphere) 
			{
				if(skipNondynamic && !b->isDynamic) continue;
				vtkIdType pid[1];
				const Vector3r& pos = b->state->pos;
				pid[0] = spheresPos->InsertNextPoint(pos[0], pos[1], pos[2]);
				spheresCells->InsertNextCell(1,pid);
				radii->InsertNextValue(sphere->radius);
				if (recActive[REC_COLORS])
				{
					const Vector3r& color = sphere->diffuseColor;
					float c[3] = {color[0],color[1],color[2]};
					spheresColors->InsertNextTupleValue(c);
				}
				if(recActive[REC_VELOCITY]){
					spheresVelocity->InsertNextTupleValue((float*)(&b->state->vel));
				}
				if (recActive[REC_CPM]) {
					cpmDamage->InsertNextValue(YADE_PTR_CAST<CpmMat>(b->material)->normDmg);
					const Vector3r& ss=YADE_PTR_CAST<CpmMat>(b->material)->sigma;
					const Vector3r& tt=YADE_PTR_CAST<CpmMat>(b->material)->tau;
					float s[3]={ss[0],ss[1],ss[2]};
					float t[3]={tt[0],tt[1],tt[2]};
					cpmSigma->InsertNextTupleValue(s);
					cpmSigmaM->InsertNextValue((ss[0]+ss[1]+ss[2])/3.);
					cpmTau->InsertNextTupleValue(t);
				}
				continue;
			}
		}
		if (recActive[REC_FACETS])
		{
			const InteractingFacet* facet = dynamic_cast<InteractingFacet*>(b->interactingGeometry.get()); 
			if (facet)
			{
				const Se3r& O = b->state->se3;
				const vector<Vector3r>& localPos = facet->vertices;
				Matrix3r facetAxisT; O.orientation.ToRotationMatrix(facetAxisT);
				vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
				vtkIdType nbPoints=facetsPos->GetNumberOfPoints();
				for (int i=0;i<3;++i) {
					Vector3r globalPos = O.position + facetAxisT * localPos[i];
					facetsPos->InsertNextPoint(globalPos[0], globalPos[1], globalPos[2]);
					tri->GetPointIds()->SetId(i,nbPoints+i);
				}
				facetsCells->InsertNextCell(tri);
				if (recActive[REC_COLORS])
				{
					const Vector3r& color = facet->diffuseColor;
					float c[3] = {color[0],color[1],color[2]};
					facetsColors->InsertNextTupleValue(c);
				}
				continue;
			}
		}
	}

	vtkSmartPointer<vtkDataCompressor> compressor;
	if(compress) compressor=vtkSmartPointer<vtkZLibDataCompressor>::New();

	if (recActive[REC_SPHERES])
	{
		vtkSmartPointer<vtkUnstructuredGrid> spheresUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
		spheresUg->SetPoints(spheresPos);
		spheresUg->SetCells(VTK_VERTEX, spheresCells);
		spheresUg->GetPointData()->AddArray(radii);
		if (recActive[REC_COLORS]) spheresUg->GetPointData()->AddArray(spheresColors);
		if (recActive[REC_VELOCITY]) spheresUg->GetPointData()->AddArray(spheresVelocity);
		if (recActive[REC_CPM]) {
			spheresUg->GetPointData()->AddArray(cpmDamage);
			spheresUg->GetPointData()->AddArray(cpmSigma);
			spheresUg->GetPointData()->AddArray(cpmSigmaM);
			spheresUg->GetPointData()->AddArray(cpmTau);
		}
		vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
		if(compress) writer->SetCompressor(compressor);
		string fn=fileName+"spheres."+lexical_cast<string>(rootBody->currentIteration)+".vtu";
		writer->SetFileName(fn.c_str());
		writer->SetInput(spheresUg);
		writer->Write();	
	}
	if (recActive[REC_FACETS])
	{
		vtkSmartPointer<vtkUnstructuredGrid> facetsUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
		facetsUg->SetPoints(facetsPos);
		facetsUg->SetCells(VTK_TRIANGLE, facetsCells);
		if (recActive[REC_COLORS]) facetsUg->GetCellData()->AddArray(facetsColors);
		vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
		if(compress) writer->SetCompressor(compressor);
		string fn=fileName+"facets."+lexical_cast<string>(rootBody->currentIteration)+".vtu";
		writer->SetFileName(fn.c_str());
		writer->SetInput(facetsUg);
		writer->Write();	
	}
	if (recActive[REC_INTR])
	{
		vtkSmartPointer<vtkUnstructuredGrid> intrUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
		intrUg->SetPoints(intrBodyPos);
		intrUg->SetCells(VTK_LINE, intrCells);
		if (recActive[REC_CPM]){
			 intrUg->GetCellData()->AddArray(intrForceN);
			 intrUg->GetCellData()->AddArray(intrAbsForceT);
		}
		vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
		if(compress) writer->SetCompressor(compressor);
		string fn=fileName+"intrs."+lexical_cast<string>(rootBody->currentIteration)+".vtu";
		writer->SetFileName(fn.c_str());
		writer->SetInput(intrUg);
		writer->Write();	
	}

	//vtkSmartPointer<vtkMultiBlockDataSet> multiblockDataset = vtkSmartPointer<vtkMultiBlockDataSet>::New();
	//multiblockDataset->SetBlock(0, spheresUg );
	//multiblockDataset->SetBlock(1, facetsUg );
	//vtkSmartPointer<vtkXMLMultiBlockDataWriter> writer = vtkSmartPointer<vtkXMLMultiBlockDataWriter>::New();
	//string fn=fileName+lexical_cast<string>(rootBody->currentIteration)+".vtm";
	//writer->SetFileName(fn.c_str());
	//writer->SetInput(multiblockDataset);
	//writer->Write();	
}


YADE_REQUIRE_FEATURE(PHYSPAR);

