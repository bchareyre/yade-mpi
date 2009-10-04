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
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/Facet.hpp>
#include<yade/pkg-dem/ConcretePM.hpp>


YADE_PLUGIN((VTKRecorder));
YADE_REQUIRE_FEATURE(VTK)
CREATE_LOGGER(VTKRecorder);

VTKRecorder::VTKRecorder() 
{ 
	/* we always want to save the first state as well */ 
	initRun=true; 
	compress=false;
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
		else if(rec=="facets") recActive[REC_FACETS]=true;
		else if(rec=="colors") recActive[REC_COLORS]=true;
		else if(rec=="cpmDamage") recActive[REC_CPM_DAMAGE]=true;
		else LOG_ERROR("Unknown recorder named `"<<rec<<"' (supported are: spheres, facets, colors, cpmDamage). Ignored.");
	}

	vtkSmartPointer<vtkPoints> spheresPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> spheresCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> radii = vtkSmartPointer<vtkFloatArray>::New();
	radii->SetNumberOfComponents(1);
	radii->SetName("Radii");
	vtkSmartPointer<vtkFloatArray> spheresColors = vtkSmartPointer<vtkFloatArray>::New();
	spheresColors->SetNumberOfComponents(3);
	spheresColors->SetName("Colors");
	vtkSmartPointer<vtkFloatArray> damage = vtkSmartPointer<vtkFloatArray>::New();
	damage->SetNumberOfComponents(1);
	damage->SetName("cpmDamage");

	vtkSmartPointer<vtkPoints> facetsPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> facetsCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> facetsColors = vtkSmartPointer<vtkFloatArray>::New();
	facetsColors->SetNumberOfComponents(3);
	facetsColors->SetName("Colors");


	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if (recActive[REC_SPHERES])
		{
			const Sphere* sphere = dynamic_cast<Sphere*>(b->geometricalModel.get()); 
			if (sphere) 
			{
				vtkIdType pid[1];
				const Vector3r& pos = b->physicalParameters->se3.position;
				pid[0] = spheresPos->InsertNextPoint(pos[0], pos[1], pos[2]);
				spheresCells->InsertNextCell(1,pid);
				radii->InsertNextValue(sphere->radius);
				if (recActive[REC_COLORS])
				{
					const Vector3r& color = sphere->diffuseColor;
					float c[3] = {color[0],color[1],color[2]};
					spheresColors->InsertNextTupleValue(c);
				}
				if (recActive[REC_CPM_DAMAGE]) {
					damage->InsertNextValue(YADE_PTR_CAST<CpmMat>(b->physicalParameters)->normDmg);
				}
				continue;
			}
		}
		if (recActive[REC_FACETS])
		{
			const Facet* facet = dynamic_cast<Facet*>(b->geometricalModel.get()); 
			if (facet)
			{
				const Se3r& O = b->physicalParameters->se3;
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
		if (recActive[REC_CPM_DAMAGE]) spheresUg->GetPointData()->AddArray(damage);
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

	//vtkSmartPointer<vtkMultiBlockDataSet> multiblockDataset = vtkSmartPointer<vtkMultiBlockDataSet>::New();
	//multiblockDataset->SetBlock(0, spheresUg );
	//multiblockDataset->SetBlock(1, facetsUg );
	//vtkSmartPointer<vtkXMLMultiBlockDataWriter> writer = vtkSmartPointer<vtkXMLMultiBlockDataWriter>::New();
	//string fn=fileName+lexical_cast<string>(rootBody->currentIteration)+".vtm";
	//writer->SetFileName(fn.c_str());
	//writer->SetInput(multiblockDataset);
	//writer->Write();	
}

