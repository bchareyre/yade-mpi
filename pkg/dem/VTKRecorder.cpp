#ifdef YADE_VTK

#include"VTKRecorder.hpp"

#include<vtkCellArray.h>
#include<vtkPoints.h>
#include<vtkPointData.h>
#include<vtkCellData.h>
#include<vtkSmartPointer.h>
#include<vtkDoubleArray.h>
#include<vtkUnstructuredGrid.h>
#include<vtkPolyData.h>
#include<vtkXMLUnstructuredGridWriter.h>
#include<vtkXMLPolyDataWriter.h>
#include<vtkZLibDataCompressor.h>
#include<vtkTriangle.h>
#include<vtkLine.h>
#include<vtkQuad.h>
#include<vtkHexahedron.h>
#ifdef YADE_VTK_MULTIBLOCK
  #include<vtkXMLMultiBlockDataWriter.h>
  #include<vtkMultiBlockDataSet.h>
#endif

#include<core/Scene.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/common/Facet.hpp>
#include<pkg/common/Box.hpp>
#include<pkg/dem/ConcretePM.hpp>
#include<pkg/dem/WirePM.hpp>
#include<pkg/dem/JointedCohesiveFrictionalPM.hpp>
#include<pkg/dem/Shop.hpp>
#ifdef YADE_LIQMIGRATION
	#include<pkg/dem/ViscoelasticCapillarPM.hpp>
#endif

YADE_PLUGIN((VTKRecorder));
CREATE_LOGGER(VTKRecorder);

#ifdef YADE_MASK_ARBITRARY
#define GET_MASK(b) b->groupMask.to_ulong()
#else
#define GET_MASK(b) b->groupMask
#endif

#include <boost/unordered_map.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/include/pair.hpp>

void VTKRecorder::action(){
	vector<bool> recActive(REC_SENTINEL,false);
	FOREACH(string& rec, recorders){
		if(rec=="all"){
			recActive[REC_SPHERES]=true;
			recActive[REC_VELOCITY]=true;
			recActive[REC_FACETS]=true;
			recActive[REC_BOXES]=true;
			recActive[REC_COLORS]=true;
			recActive[REC_MASS]=true;
			recActive[REC_INTR]=true;
			recActive[REC_ID]=true;
			recActive[REC_MASK]=true;
			recActive[REC_CLUMPID]=true;
			recActive[REC_MATERIALID]=true;
			recActive[REC_STRESS]=true;
			recActive[REC_FORCE]=true;
			recActive[REC_COORDNUMBER]=true;
			if (scene->isPeriodic) { recActive[REC_PERICELL]=true; }
		}
		else if(rec=="spheres") recActive[REC_SPHERES]=true;
		else if(rec=="velocity") recActive[REC_VELOCITY]=true;
		else if(rec=="facets") recActive[REC_FACETS]=true;
		else if(rec=="boxes") recActive[REC_BOXES]=true;
		else if(rec=="mass") recActive[REC_MASS]=true;
		else if((rec=="colors") || (rec=="color"))recActive[REC_COLORS]=true;
		else if(rec=="cpm") recActive[REC_CPM]=true;
		else if(rec=="wpm") recActive[REC_WPM]=true;
		else if(rec=="intr") recActive[REC_INTR]=true;
		else if((rec=="ids") || (rec=="id")) recActive[REC_ID]=true;
		else if(rec=="mask") recActive[REC_MASK]=true;
		else if((rec=="clumpids") || (rec=="clumpId")) recActive[REC_CLUMPID]=true;
		else if(rec=="materialId") recActive[REC_MATERIALID]=true;
		else if(rec=="stress") recActive[REC_STRESS]=true;
		else if(rec=="force") recActive[REC_FORCE]=true;
		else if(rec=="jcfpm") recActive[REC_JCFPM]=true;
		else if(rec=="cracks") recActive[REC_CRACKS]=true;
		else if(rec=="moments") recActive[REC_MOMENTS]=true;
		else if(rec=="pericell" && scene->isPeriodic) recActive[REC_PERICELL]=true;
		else if(rec=="liquidcontrol") recActive[REC_LIQ]=true;
		else if(rec=="bstresses") recActive[REC_BSTRESS]=true;
		else if(rec=="coordNumber") recActive[REC_COORDNUMBER]=true;
		else LOG_ERROR("Unknown recorder named `"<<rec<<"' (supported are: all, spheres, velocity, facets, boxes, color, stress, cpm, wpm, intr, id, clumpId, materialId, jcfpm, cracks, moments pericell, liquidcontrol, bstresses). Ignored.");
	}
	// cpm needs interactions
	if(recActive[REC_CPM]) recActive[REC_INTR]=true;
	
	// jcfpm needs interactions
	if(recActive[REC_JCFPM]) recActive[REC_INTR]=true;

	// wpm needs interactions
	if(recActive[REC_WPM]) recActive[REC_INTR]=true;

	// liquid control needs interactions
	if(recActive[REC_LIQ]) recActive[REC_INTR]=true;

	// spheres
	vtkSmartPointer<vtkPoints> spheresPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> spheresCells = vtkSmartPointer<vtkCellArray>::New();
	
	vtkSmartPointer<vtkDoubleArray> radii = vtkSmartPointer<vtkDoubleArray>::New();
	radii->SetNumberOfComponents(1);
	radii->SetName("radii");
	
	vtkSmartPointer<vtkDoubleArray> spheresSigI = vtkSmartPointer<vtkDoubleArray>::New();
	spheresSigI->SetNumberOfComponents(1);
	spheresSigI->SetName("sigI");
	
	vtkSmartPointer<vtkDoubleArray> spheresSigII = vtkSmartPointer<vtkDoubleArray>::New();
	spheresSigII->SetNumberOfComponents(1);
	spheresSigII->SetName("sigII");
	
	vtkSmartPointer<vtkDoubleArray> spheresSigIII = vtkSmartPointer<vtkDoubleArray>::New();
	spheresSigIII->SetNumberOfComponents(1);
	spheresSigIII->SetName("sigIII");
	
	vtkSmartPointer<vtkDoubleArray> spheresDirI = vtkSmartPointer<vtkDoubleArray>::New();
	spheresDirI->SetNumberOfComponents(3);
	spheresDirI->SetName("dirI");
	
	vtkSmartPointer<vtkDoubleArray> spheresDirII = vtkSmartPointer<vtkDoubleArray>::New();
	spheresDirII->SetNumberOfComponents(3);
	spheresDirII->SetName("dirII");
	
	vtkSmartPointer<vtkDoubleArray> spheresDirIII = vtkSmartPointer<vtkDoubleArray>::New();
	spheresDirIII->SetNumberOfComponents(3);
	spheresDirIII->SetName("dirIII");
	
	vtkSmartPointer<vtkDoubleArray> spheresMass = vtkSmartPointer<vtkDoubleArray>::New();
	spheresMass->SetNumberOfComponents(1);
	spheresMass->SetName("mass");
	
	vtkSmartPointer<vtkDoubleArray> spheresId = vtkSmartPointer<vtkDoubleArray>::New();
	spheresId->SetNumberOfComponents(1);
	spheresId->SetName("id");

#ifdef YADE_SPH
	vtkSmartPointer<vtkDoubleArray> spheresRhoSPH = vtkSmartPointer<vtkDoubleArray>::New();
	spheresRhoSPH->SetNumberOfComponents(1);
	spheresRhoSPH->SetName("SPH_Rho");
	
	vtkSmartPointer<vtkDoubleArray> spheresPressSPH = vtkSmartPointer<vtkDoubleArray>::New();
	spheresPressSPH->SetNumberOfComponents(1);
	spheresPressSPH->SetName("SPH_Press");
	
	vtkSmartPointer<vtkDoubleArray> spheresCoordNumbSPH = vtkSmartPointer<vtkDoubleArray>::New();
	spheresCoordNumbSPH->SetNumberOfComponents(1);
	spheresCoordNumbSPH->SetName("SPH_Neigh");
#endif

#ifdef YADE_DEFORM
	vtkSmartPointer<vtkDoubleArray> spheresRealRad = vtkSmartPointer<vtkDoubleArray>::New();
	spheresRealRad->SetNumberOfComponents(1);
	spheresRealRad->SetName("RealRad");
#endif

#ifdef YADE_LIQMIGRATION
	vtkSmartPointer<vtkDoubleArray> spheresLiqVol = vtkSmartPointer<vtkDoubleArray>::New();
	spheresLiqVol->SetNumberOfComponents(1);
	spheresLiqVol->SetName("Liq_Vol");
	
	vtkSmartPointer<vtkDoubleArray> spheresLiqVolIter = vtkSmartPointer<vtkDoubleArray>::New();
	spheresLiqVolIter->SetNumberOfComponents(1);
	spheresLiqVolIter->SetName("Liq_VolIter");
	
	vtkSmartPointer<vtkDoubleArray> spheresLiqVolTotal = vtkSmartPointer<vtkDoubleArray>::New();
	spheresLiqVolTotal->SetNumberOfComponents(1);
	spheresLiqVolTotal->SetName("Liq_VolTotal");
#endif

	vtkSmartPointer<vtkDoubleArray> spheresMask = vtkSmartPointer<vtkDoubleArray>::New();
	spheresMask->SetNumberOfComponents(1);
	spheresMask->SetName("mask");

	vtkSmartPointer<vtkDoubleArray> spheresCoordNumb = vtkSmartPointer<vtkDoubleArray>::New();
	spheresCoordNumb->SetNumberOfComponents(1);
	spheresCoordNumb->SetName("coordNumber");
	
	vtkSmartPointer<vtkDoubleArray> clumpId = vtkSmartPointer<vtkDoubleArray>::New();
	clumpId->SetNumberOfComponents(1);
	clumpId->SetName("clumpId");
	
	vtkSmartPointer<vtkDoubleArray> spheresColors = vtkSmartPointer<vtkDoubleArray>::New();
	spheresColors->SetNumberOfComponents(3);
	spheresColors->SetName("color");
	
	vtkSmartPointer<vtkDoubleArray> spheresLinVelVec = vtkSmartPointer<vtkDoubleArray>::New();
	spheresLinVelVec->SetNumberOfComponents(3);
	spheresLinVelVec->SetName("linVelVec");		//Linear velocity in Vector3 form
	
	vtkSmartPointer<vtkDoubleArray> spheresLinVelLen = vtkSmartPointer<vtkDoubleArray>::New();
	spheresLinVelLen->SetNumberOfComponents(1);
	spheresLinVelLen->SetName("linVelLen");		//Length (magnitude) of linear velocity
	
	vtkSmartPointer<vtkDoubleArray> spheresAngVelVec = vtkSmartPointer<vtkDoubleArray>::New();
	spheresAngVelVec->SetNumberOfComponents(3);
	spheresAngVelVec->SetName("angVelVec");		//Angular velocity in Vector3 form
	
	vtkSmartPointer<vtkDoubleArray> spheresAngVelLen = vtkSmartPointer<vtkDoubleArray>::New();
	spheresAngVelLen->SetNumberOfComponents(1);
	spheresAngVelLen->SetName("angVelLen");		//Length (magnitude) of angular velocity
	
	vtkSmartPointer<vtkDoubleArray> spheresNormalStressVec = vtkSmartPointer<vtkDoubleArray>::New();
	spheresNormalStressVec->SetNumberOfComponents(3);
	spheresNormalStressVec->SetName("normalStress");
	
	vtkSmartPointer<vtkDoubleArray> spheresShearStressVec = vtkSmartPointer<vtkDoubleArray>::New();
	spheresShearStressVec->SetNumberOfComponents(3);
	spheresShearStressVec->SetName("shearStress");
	
	vtkSmartPointer<vtkDoubleArray> spheresNormalStressNorm = vtkSmartPointer<vtkDoubleArray>::New();
	spheresNormalStressNorm->SetNumberOfComponents(1);
	spheresNormalStressNorm->SetName("normalStressNorm");
	
	vtkSmartPointer<vtkDoubleArray> spheresMaterialId = vtkSmartPointer<vtkDoubleArray>::New();
	spheresMaterialId->SetNumberOfComponents(1);
	spheresMaterialId->SetName("materialId");

	vtkSmartPointer<vtkDoubleArray> spheresForceVec = vtkSmartPointer<vtkDoubleArray>::New();
	spheresForceVec->SetNumberOfComponents(3);
	spheresForceVec->SetName("forceVec");

	vtkSmartPointer<vtkDoubleArray> spheresForceLen = vtkSmartPointer<vtkDoubleArray>::New();
	spheresForceLen->SetNumberOfComponents(1);
	spheresForceLen->SetName("forceLen");

	vtkSmartPointer<vtkDoubleArray> spheresTorqueVec = vtkSmartPointer<vtkDoubleArray>::New();
	spheresTorqueVec->SetNumberOfComponents(3);
	spheresTorqueVec->SetName("torqueVec");

	vtkSmartPointer<vtkDoubleArray> spheresTorqueLen = vtkSmartPointer<vtkDoubleArray>::New();
	spheresTorqueLen->SetNumberOfComponents(1);
	spheresTorqueLen->SetName("torqueLen");

	// facets
	vtkSmartPointer<vtkPoints> facetsPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> facetsCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkDoubleArray> facetsColors = vtkSmartPointer<vtkDoubleArray>::New();
	facetsColors->SetNumberOfComponents(3);
	facetsColors->SetName("color");
	
	vtkSmartPointer<vtkDoubleArray> facetsStressVec = vtkSmartPointer<vtkDoubleArray>::New();
	facetsStressVec->SetNumberOfComponents(3);
	facetsStressVec->SetName("stressVec");
	
	vtkSmartPointer<vtkDoubleArray> facetsStressLen = vtkSmartPointer<vtkDoubleArray>::New();
	facetsStressLen->SetNumberOfComponents(1);
	facetsStressLen->SetName("stressLen");
	
	vtkSmartPointer<vtkDoubleArray> facetsMaterialId = vtkSmartPointer<vtkDoubleArray>::New();
	facetsMaterialId->SetNumberOfComponents(1);
	facetsMaterialId->SetName("materialId");
	
	vtkSmartPointer<vtkDoubleArray> facetsMask = vtkSmartPointer<vtkDoubleArray>::New();
	facetsMask->SetNumberOfComponents(1);
	facetsMask->SetName("mask");

	vtkSmartPointer<vtkDoubleArray> facetsForceVec = vtkSmartPointer<vtkDoubleArray>::New();
	facetsForceVec->SetNumberOfComponents(3);
	facetsForceVec->SetName("forceVec");

	vtkSmartPointer<vtkDoubleArray> facetsForceLen = vtkSmartPointer<vtkDoubleArray>::New();
	facetsForceLen->SetNumberOfComponents(1);
	facetsForceLen->SetName("forceLen");

	vtkSmartPointer<vtkDoubleArray> facetsTorqueVec = vtkSmartPointer<vtkDoubleArray>::New();
	facetsTorqueVec->SetNumberOfComponents(3);
	facetsTorqueVec->SetName("torqueVec");

	vtkSmartPointer<vtkDoubleArray> facetsTorqueLen = vtkSmartPointer<vtkDoubleArray>::New();
	facetsTorqueLen->SetNumberOfComponents(1);
	facetsTorqueLen->SetName("torqueLen");
  
	vtkSmartPointer<vtkDoubleArray> facetsCoordNumb = vtkSmartPointer<vtkDoubleArray>::New();
	facetsCoordNumb->SetNumberOfComponents(1);
	facetsCoordNumb->SetName("coordNumber");

	// boxes
	vtkSmartPointer<vtkPoints> boxesPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> boxesCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkDoubleArray> boxesColors = vtkSmartPointer<vtkDoubleArray>::New();
	boxesColors->SetNumberOfComponents(3);
	boxesColors->SetName("color");
	
	vtkSmartPointer<vtkDoubleArray> boxesStressVec = vtkSmartPointer<vtkDoubleArray>::New();
	boxesStressVec->SetNumberOfComponents(3);
	boxesStressVec->SetName("stressVec");
	
	vtkSmartPointer<vtkDoubleArray> boxesStressLen = vtkSmartPointer<vtkDoubleArray>::New();
	boxesStressLen->SetNumberOfComponents(1);
	boxesStressLen->SetName("stressLen");
	
	vtkSmartPointer<vtkDoubleArray> boxesMaterialId = vtkSmartPointer<vtkDoubleArray>::New();
	boxesMaterialId->SetNumberOfComponents(1);
	boxesMaterialId->SetName("materialId");
	
	vtkSmartPointer<vtkDoubleArray> boxesMask = vtkSmartPointer<vtkDoubleArray>::New();
	boxesMask->SetNumberOfComponents(1);
	boxesMask->SetName("mask");

	vtkSmartPointer<vtkDoubleArray> boxesForceVec = vtkSmartPointer<vtkDoubleArray>::New();
	boxesForceVec->SetNumberOfComponents(3);
	boxesForceVec->SetName("forceVec");

	vtkSmartPointer<vtkDoubleArray> boxesForceLen = vtkSmartPointer<vtkDoubleArray>::New();
	boxesForceLen->SetNumberOfComponents(1);
	boxesForceLen->SetName("forceLen");

	vtkSmartPointer<vtkDoubleArray> boxesTorqueVec = vtkSmartPointer<vtkDoubleArray>::New();
	boxesTorqueVec->SetNumberOfComponents(3);
	boxesTorqueVec->SetName("torqueVec");

	vtkSmartPointer<vtkDoubleArray> boxesTorqueLen = vtkSmartPointer<vtkDoubleArray>::New();
	boxesTorqueLen->SetNumberOfComponents(1);
	boxesTorqueLen->SetName("torqueLen");

	// interactions
	vtkSmartPointer<vtkPoints> intrBodyPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> intrCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkDoubleArray> intrForceN = vtkSmartPointer<vtkDoubleArray>::New();
	intrForceN->SetNumberOfComponents(1);
	intrForceN->SetName("forceN");
	vtkSmartPointer<vtkDoubleArray> intrAbsForceT = vtkSmartPointer<vtkDoubleArray>::New();
	intrAbsForceT->SetNumberOfComponents(3);
	intrAbsForceT->SetName("absForceT");

	// pericell
	vtkSmartPointer<vtkPoints> pericellPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> pericellHexa = vtkSmartPointer<vtkCellArray>::New();

	// extras for CPM
	if(recActive[REC_CPM]){ CpmStateUpdater csu; csu.update(scene); }
	vtkSmartPointer<vtkDoubleArray> cpmDamage = vtkSmartPointer<vtkDoubleArray>::New();
	cpmDamage->SetNumberOfComponents(1);
	cpmDamage->SetName("cpmDamage");
	vtkSmartPointer<vtkDoubleArray> cpmStress = vtkSmartPointer<vtkDoubleArray>::New();
	cpmStress->SetNumberOfComponents(9);
	cpmStress->SetName("cpmStress");

	// extras for JCFpm
	vtkSmartPointer<vtkDoubleArray> nbCracks = vtkSmartPointer<vtkDoubleArray>::New();
	nbCracks->SetNumberOfComponents(1);
	nbCracks->SetName("nbCracks");
	vtkSmartPointer<vtkDoubleArray> jcfpmDamage = vtkSmartPointer<vtkDoubleArray>::New();
	jcfpmDamage->SetNumberOfComponents(1);
	jcfpmDamage->SetName("damage");
	vtkSmartPointer<vtkDoubleArray> intrIsCohesive = vtkSmartPointer<vtkDoubleArray>::New();
	intrIsCohesive->SetNumberOfComponents(1);
	intrIsCohesive->SetName("isCohesive");
	vtkSmartPointer<vtkDoubleArray> intrIsOnJoint = vtkSmartPointer<vtkDoubleArray>::New();
	intrIsOnJoint->SetNumberOfComponents(1);
	intrIsOnJoint->SetName("isOnJoint");
	vtkSmartPointer<vtkDoubleArray> eventNumber = vtkSmartPointer<vtkDoubleArray>::New();
	eventNumber->SetNumberOfComponents(1);
	eventNumber->SetName("eventNumber");

	// extras for cracks
	vtkSmartPointer<vtkPoints> crackPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> crackCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkDoubleArray> crackIter = vtkSmartPointer<vtkDoubleArray>::New();
	crackIter->SetNumberOfComponents(1);
	crackIter->SetName("iter");
        vtkSmartPointer<vtkDoubleArray> crackTime = vtkSmartPointer<vtkDoubleArray>::New();
	crackTime->SetNumberOfComponents(1);
	crackTime->SetName("time");
	vtkSmartPointer<vtkDoubleArray> crackType = vtkSmartPointer<vtkDoubleArray>::New();
	crackType->SetNumberOfComponents(1);
	crackType->SetName("type");
	vtkSmartPointer<vtkDoubleArray> crackSize = vtkSmartPointer<vtkDoubleArray>::New();
	crackSize->SetNumberOfComponents(1);
	crackSize->SetName("size");
	vtkSmartPointer<vtkDoubleArray> crackNorm = vtkSmartPointer<vtkDoubleArray>::New();
	crackNorm->SetNumberOfComponents(3);
	crackNorm->SetName("norm");
        vtkSmartPointer<vtkDoubleArray> crackNrg = vtkSmartPointer<vtkDoubleArray>::New();
	crackNrg->SetNumberOfComponents(1);
	crackNrg->SetName("nrg");
        vtkSmartPointer<vtkDoubleArray> crackOnJnt = vtkSmartPointer<vtkDoubleArray>::New();
	crackOnJnt->SetNumberOfComponents(1);
	crackOnJnt->SetName("onJnt");

	// extras for moments
	vtkSmartPointer<vtkPoints> momentPos = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> momentCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkDoubleArray> momentiter = vtkSmartPointer<vtkDoubleArray>::New();
	momentiter->SetNumberOfComponents(1);
	momentiter->SetName("momentiter");
	vtkSmartPointer<vtkDoubleArray> momenttime = vtkSmartPointer<vtkDoubleArray>::New();
	momenttime->SetNumberOfComponents(1);
	momenttime->SetName("momenttime");
	vtkSmartPointer<vtkDoubleArray> momentSize = vtkSmartPointer<vtkDoubleArray>::New();
	momentSize->SetNumberOfComponents(1);
	momentSize->SetName("momentSize");
	vtkSmartPointer<vtkDoubleArray> momentEventNum = vtkSmartPointer<vtkDoubleArray>::New();
	momentEventNum->SetNumberOfComponents(1);
	momentEventNum->SetName("momentEventNum");
	vtkSmartPointer<vtkDoubleArray> momentNumInts = vtkSmartPointer<vtkDoubleArray>::New();
	momentNumInts->SetNumberOfComponents(1);
	momentNumInts->SetName("momentNumInts");
	
#ifdef YADE_LIQMIGRATION
	vtkSmartPointer<vtkDoubleArray> liqVol = vtkSmartPointer<vtkDoubleArray>::New();
	liqVol->SetNumberOfComponents(1);
	liqVol->SetName("liqVol");
	
	vtkSmartPointer<vtkDoubleArray> liqVolNorm = vtkSmartPointer<vtkDoubleArray>::New();
	liqVolNorm->SetNumberOfComponents(1);
	liqVolNorm->SetName("liqVolNorm");
#endif
	
	// extras for WireMatPM
	vtkSmartPointer<vtkDoubleArray> wpmNormalForce = vtkSmartPointer<vtkDoubleArray>::New();
	wpmNormalForce->SetNumberOfComponents(1);
	wpmNormalForce->SetName("wpmNormalForce");
	vtkSmartPointer<vtkDoubleArray> wpmLimitFactor = vtkSmartPointer<vtkDoubleArray>::New();
	wpmLimitFactor->SetNumberOfComponents(1);
	wpmLimitFactor->SetName("wpmLimitFactor");

	if(recActive[REC_INTR]){
		// holds information about cell distance between spatial and displayed position of each particle
		vector<Vector3i> wrapCellDist; if (scene->isPeriodic){ wrapCellDist.resize(scene->bodies->size()); }
		// save body positions, referenced by ids by vtkLine
		
		// map to keep real body ids and their number in a vector (intrBodyPos)
		boost::unordered_map<Body::id_t,Body::id_t> bIdVector;
		Body::id_t curId = 0;
		FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			if (b) {
				if(!scene->isPeriodic) {
					intrBodyPos->InsertNextPoint(b->state->pos[0],b->state->pos[1],b->state->pos[2]);
				} else {
					Vector3r pos=scene->cell->wrapShearedPt(b->state->pos,wrapCellDist[b->id]);
					intrBodyPos->InsertNextPoint(pos[0],pos[1],pos[2]);
				}
				bIdVector.insert (std::pair<Body::id_t,Body::id_t>(b->id,curId));
				curId++;
			}
		}
		FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
			if(!I->isReal()) continue;
			if(skipFacetIntr){
				if(!(Body::byId(I->getId1()))) continue;
				if(!(Body::byId(I->getId2()))) continue;
				if(!(dynamic_cast<Sphere*>(Body::byId(I->getId1())->shape.get()))) continue;
				if(!(dynamic_cast<Sphere*>(Body::byId(I->getId2())->shape.get()))) continue;
			}
			
			const auto iterId1 = bIdVector.find (I->getId1());
			const auto iterId2 = bIdVector.find (I->getId2());
			
			if (iterId2 == bIdVector.end() || iterId2 == bIdVector.end()) continue;
			
			const auto setId1Line = iterId1->second;
			const auto setId2Line = iterId2->second;
			
			/* For the periodic boundary conditions,
				find out whether the interaction crosses the boundary of the periodic cell;
				if it does, display the interaction on both sides of the cell, with one of the
				points sticking out in each case.
				Since vtkLines must connect points with an ID assigned, we will create a new additional
				point for each point outside the cell. It might create some data redundancy, but
				let us suppose that the number of interactions crossing the cell boundary is low compared
				to total numer of interactions
			*/
			// how many times to add values defined on interactions, depending on how many times the interaction is saved
			int numAddValues=1;
			// aperiodic boundary, or interaction is inside the cell
			if(!scene->isPeriodic || (scene->isPeriodic && (I->cellDist==wrapCellDist[I->getId2()]-wrapCellDist[I->getId1()]))){
				vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
				line->GetPointIds()->SetId(0,setId1Line);
				line->GetPointIds()->SetId(1,setId2Line);
				intrCells->InsertNextCell(line);
			} else {
				assert(scene->isPeriodic);
				// spatial positions of particles
				const Vector3r& p01(Body::byId(I->getId1())->state->pos); const Vector3r& p02(Body::byId(I->getId2())->state->pos);
				// create two line objects; each of them has one endpoint inside the cell and the other one sticks outside
				// A,B are the "fake" bodies outside the cell for id1 and id2 respectively, p1,p2 are the displayed points
				// distance in cell units for shifting A away from p1; negated value is shift of B away from p2
				Vector3r ptA(p01+scene->cell->hSize*(wrapCellDist[I->getId2()]-I->cellDist).cast<Real>());
				const vtkIdType idPtA=intrBodyPos->InsertNextPoint(ptA[0],ptA[1],ptA[2]); 
				
				Vector3r ptB(p02+scene->cell->hSize*(wrapCellDist[I->getId1()]-I->cellDist).cast<Real>());
				const vtkIdType idPtB=intrBodyPos->InsertNextPoint(ptB[0],ptB[1],ptB[2]);
				
				vtkSmartPointer<vtkLine> line1B(vtkSmartPointer<vtkLine>::New());
				line1B->GetPointIds()->SetId(0,setId2Line);
				line1B->GetPointIds()->SetId(1,idPtB);
				
				vtkSmartPointer<vtkLine> lineA2(vtkSmartPointer<vtkLine>::New());
				lineA2->GetPointIds()->SetId(0,idPtA);
				lineA2->GetPointIds()->SetId(1,setId2Line);
				numAddValues=2;
			}
			const NormShearPhys* phys = YADE_CAST<NormShearPhys*>(I->phys.get());
			const GenericSpheresContact* geom = YADE_CAST<GenericSpheresContact*>(I->geom.get());
			// gives _signed_ scalar of normal force, following the convention used in the respective constitutive law
			Real fn=phys->normalForce.dot(geom->normal); 
			Real fs[3]={ (Real) std::abs(phys->shearForce[0]), (Real) std::abs(phys->shearForce[1]), (Real) std::abs(phys->shearForce[2])};
			// add the value once for each interaction object that we created (might be 2 for the periodic boundary)
			for(int i=0; i<numAddValues; i++){
				intrAbsForceT->InsertNextTupleValue(fs);
				if(recActive[REC_WPM]) {
					const WirePhys* wirephys = dynamic_cast<WirePhys*>(I->phys.get());
					if (wirephys!=NULL && wirephys->isLinked) {
						wpmLimitFactor->InsertNextValue(wirephys->limitFactor);
						wpmNormalForce->InsertNextValue(fn);
						intrForceN->InsertNextValue(NaN);
					}
					else {
						intrForceN->InsertNextValue(fn);
						wpmNormalForce->InsertNextValue(NaN);
						wpmLimitFactor->InsertNextValue(NaN);
					}
				}
				else if (recActive[REC_JCFPM]){
					const JCFpmPhys* jcfpmphys = YADE_CAST<JCFpmPhys*>(I->phys.get());
					intrIsCohesive->InsertNextValue(jcfpmphys->isCohesive);
					intrIsOnJoint->InsertNextValue(jcfpmphys->isOnJoint);
					intrForceN->InsertNextValue(fn);
					eventNumber->InsertNextValue(jcfpmphys->eventNumber);
				} else {
					intrForceN->InsertNextValue(fn);
				}
#ifdef YADE_LIQMIGRATION
				if (recActive[REC_LIQ]) {
					const ViscElCapPhys* capphys = YADE_CAST<ViscElCapPhys*>(I->phys.get());
					liqVol->InsertNextValue(capphys->Vb);
					liqVolNorm->InsertNextValue(capphys->Vb/capphys->Vmax);
				}
#endif
			}
		}
	}

	//Additional Vector for storing forces
	vector<Shop::bodyState> bodyStates;
	if(recActive[REC_STRESS]) Shop::getStressForEachBody(bodyStates);
	
	
	vector<Matrix3r> bStresses;
	if (recActive[REC_BSTRESS])
	{
	  Shop::getStressLWForEachBody(bStresses);
	}
	
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if (!b) continue;
		if(mask!=0 && !b->maskCompatible(mask)) continue;
		if (recActive[REC_SPHERES]){
			const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get()); 
			if (sphere){
				if(skipNondynamic && b->state->blockedDOFs==State::DOF_ALL) continue;
				vtkIdType pid[1];
				Vector3r pos(scene->isPeriodic ? scene->cell->wrapShearedPt(b->state->pos) : b->state->pos);
				pid[0] = spheresPos->InsertNextPoint(pos[0], pos[1], pos[2]);
				spheresCells->InsertNextCell(1,pid);
				radii->InsertNextValue(sphere->radius);
				
				if (recActive[REC_BSTRESS]) {
				  const Matrix3r& bStress = bStresses[b->getId()];
				  Eigen::SelfAdjointEigenSolver<Matrix3r> solver(bStress); // bStress is probably not symmetric (= self-adjoint for real matrices), but the solver hopefully works (considering only one half of bStress). And, moreover, existence of (real) eigenvalues is not sure for not symmetric bStress..
				  Matrix3r dirAll = solver.eigenvectors();
				  Vector3r eigenVal = solver.eigenvalues(); // cf http://eigen.tuxfamily.org/dox/classEigen_1_1SelfAdjointEigenSolver.html#a30caf3c3884a7f4a46b8ec94efd23c5e to be sure that eigenVal[i] * dirAll.col(i) = bStress * dirAll.col(i)
				  int whereSigI(-1), whereSigII(-1), whereSigIII(-1); // all whereSig_i are in [0;2] : whereSigI = 2 => sigI=eigenVal[2]
				  if ( eigenVal[0] > std::max(eigenVal[1],eigenVal[2]) ) {
				    whereSigI = 0;
				    if (eigenVal[1]>eigenVal[2]) {
				      whereSigII=1;
				      whereSigIII=2; }
				    else { //eigenVal[0] > eigenVal[2] >= eigenVal[1]
				      whereSigII = 2;
				      whereSigIII=1; } }
				  else { // max(lambda1,lambda2) >= lambda0 // lambda = eigenVal in the comments
				    if (eigenVal[1]>=eigenVal[2]) {//max(lambda1,lambda2) = lambda1 : lambda 1 >= lambda2
				      whereSigI = 1;
				      if (eigenVal[2]>=eigenVal[0]) {//lambda1 >= lambda2 >= lambda0
					whereSigII=2;
					whereSigIII=0; }
				      else { //lambda1 >= lambda0 > lambda2
					whereSigII=0;
					whereSigIII=2; } }
				    else { //max(lambda1,lambda2) = lambda2 : lambda2 > lambda1
				      whereSigI = 2;
				      if (eigenVal[1] > eigenVal[0]) {
					whereSigII = 1;
					whereSigIII = 0; }
				      else {
					whereSigIII = 1;
					whereSigII = 0; } } }
					
				  spheresSigI->InsertNextValue(eigenVal[whereSigI]);
				  spheresSigII->InsertNextValue(eigenVal[whereSigII]);
				  spheresSigIII->InsertNextValue(eigenVal[whereSigIII]);
				  Real dirI[3] { (Real) dirAll(0,whereSigI), (Real) dirAll(1,whereSigI), (Real) dirAll(2,whereSigI) };
				  spheresDirI->InsertNextTupleValue(dirI);
				  
				  Real dirII[3] { (Real) dirAll(0,whereSigII), (Real) dirAll(1,whereSigII), (Real) dirAll(2,whereSigII) };
				  spheresDirII->InsertNextTupleValue(dirII);
				  
				  Real dirIII[3] { (Real) dirAll(0,whereSigIII), (Real) dirAll(1,whereSigIII), (Real) dirAll(2,whereSigIII) };
				  spheresDirIII->InsertNextTupleValue(dirIII); }
				
				if (recActive[REC_ID]) spheresId->InsertNextValue(b->getId()); 
				if (recActive[REC_MASK]) spheresMask->InsertNextValue(GET_MASK(b));
				if (recActive[REC_MASS]) spheresMass->InsertNextValue(b->state->mass);
				if (recActive[REC_CLUMPID]) clumpId->InsertNextValue(b->clumpId);
				if (recActive[REC_COLORS]){
					const Vector3r& color = sphere->color;
					Real c[3] = { (Real) color[0], (Real) color[1], (Real) color[2]};
					spheresColors->InsertNextTupleValue(c);
				}
				if(recActive[REC_VELOCITY]){
					const Vector3r& vel = b->state->vel;
					Real v[3] = { (Real) vel[0], (Real) vel[1], (Real) vel[2] };
					spheresLinVelVec->InsertNextTupleValue(v);
					spheresLinVelLen->InsertNextValue(vel.norm());
					
					const Vector3r& angVel = b->state->angVel;
					Real av[3] = { (Real) angVel[0], (Real) angVel[1], (Real) angVel[2] };
					spheresAngVelVec->InsertNextTupleValue(av);
					spheresAngVelLen->InsertNextValue(angVel.norm());
				}
				if(recActive[REC_STRESS]){
					const Vector3r& stress = bodyStates[b->getId()].normStress;
					const Vector3r& shear = bodyStates[b->getId()].shearStress;
					Real n[3] = { (Real)  stress[0], (Real) stress[1], (Real) stress[2] };
					Real s[3] = { (Real)  shear [0], (Real) shear [1], (Real) shear [2] };
					spheresNormalStressVec->InsertNextTupleValue(n);
					spheresShearStressVec->InsertNextTupleValue(s);
					spheresNormalStressNorm->InsertNextValue(stress.norm());
				}
				if(recActive[REC_FORCE]){
					scene->forces.sync();
					const Vector3r& f = scene->forces.getForce(b->getId());
					const Vector3r& t = scene->forces.getTorque(b->getId());
					Real ff[3] = { (Real)  f[0], (Real) f[1], (Real) f[2] };
					Real tt[3] = { (Real)  t[0], (Real) t[1], (Real) t[2] };
					Real fn = f.norm();
					Real tn = t.norm();
					spheresForceVec->InsertNextTupleValue(ff);
					spheresForceLen->InsertNextValue(fn);
					spheresTorqueVec->InsertNextTupleValue(tt);
					spheresTorqueLen->InsertNextValue(tn);
				}
				
				if (recActive[REC_CPM]){
					cpmDamage->InsertNextValue(YADE_PTR_CAST<CpmState>(b->state)->normDmg);
					const Matrix3r& ss=YADE_PTR_CAST<CpmState>(b->state)->stress;
					//Real s[3]={ss[0],ss[1],ss[2]};
					Real s[9]={ (Real) ss(0,0), (Real) ss(0,1), (Real) ss(0,2), (Real) ss(1,0), (Real) ss(1,1), (Real) ss(1,2), (Real) ss(2,0), (Real) ss(2,1), (Real) ss(2,2)};
					cpmStress->InsertNextTupleValue(s);
				}
				
				if (recActive[REC_JCFPM]){
					nbCracks->InsertNextValue(YADE_PTR_CAST<JCFpmState>(b->state)->nbBrokenBonds);
					jcfpmDamage->InsertNextValue(YADE_PTR_CAST<JCFpmState>(b->state)->damageIndex);
				}
				if (recActive[REC_COORDNUMBER]){
					spheresCoordNumb->InsertNextValue(b->coordNumber());
				}
#ifdef YADE_SPH
				spheresRhoSPH->InsertNextValue(b->state->rho); 
				spheresPressSPH->InsertNextValue(b->state->press); 
				spheresCoordNumbSPH->InsertNextValue(b->coordNumber()); 
#endif

#ifdef YADE_DEFORM
				const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
				spheresRealRad->InsertNextValue(b->state->dR + sphere->radius);
#endif

#ifdef YADE_LIQMIGRATION
				if (recActive[REC_LIQ]) {
					spheresLiqVol->InsertNextValue(b->state->Vf);
					const Real tmpVolIter = liqVolIterBody(b);
					spheresLiqVolIter->InsertNextValue(tmpVolIter);
					spheresLiqVolTotal->InsertNextValue(tmpVolIter + b->state->Vf);
				}
#endif
				if (recActive[REC_MATERIALID]) spheresMaterialId->InsertNextValue(b->material->id);
				continue;
			}
		}
		if (recActive[REC_FACETS]){
			const Facet* facet = dynamic_cast<Facet*>(b->shape.get()); 
			if (facet){
				Vector3r pos(scene->isPeriodic ? scene->cell->wrapShearedPt(b->state->pos) : b->state->pos);
				const vector<Vector3r>& localPos = facet->vertices;
				Matrix3r facetAxisT=b->state->ori.toRotationMatrix();
				vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
				vtkIdType nbPoints=facetsPos->GetNumberOfPoints();
				for (int i=0;i<3;++i){
					Vector3r globalPos = pos + facetAxisT * localPos[i];
					facetsPos->InsertNextPoint(globalPos[0], globalPos[1], globalPos[2]);
					tri->GetPointIds()->SetId(i,nbPoints+i);
				}
				facetsCells->InsertNextCell(tri);
				if (recActive[REC_COLORS]){
					const Vector3r& color = facet->color;
					Real c[3] = { (Real) color[0], (Real) color[1], (Real) color[2]};
					facetsColors->InsertNextTupleValue(c);
				}
				if(recActive[REC_STRESS]){
					const Vector3r& stress = bodyStates[b->getId()].normStress+bodyStates[b->getId()].shearStress;
					Real s[3] = { (Real) stress[0], (Real) stress[1], (Real) stress[2] };
					facetsStressVec->InsertNextTupleValue(s);
					facetsStressLen->InsertNextValue(stress.norm());
				}
				if(recActive[REC_FORCE]){
					scene->forces.sync();
					const Vector3r& f = scene->forces.getForce(b->getId());
					const Vector3r& t = scene->forces.getTorque(b->getId());
					Real ff[3] = { (Real)  f[0], (Real) f[1], (Real) f[2] };
					Real tt[3] = { (Real)  t[0], (Real) t[1], (Real) t[2] };
					Real fn = f.norm();
					Real tn = t.norm();
					facetsForceVec->InsertNextTupleValue(ff);
					facetsForceLen->InsertNextValue(fn);
					facetsTorqueVec->InsertNextTupleValue(tt);
					facetsTorqueLen->InsertNextValue(tn);
				}
				if (recActive[REC_MATERIALID]) facetsMaterialId->InsertNextValue(b->material->id);
				if (recActive[REC_MASK]) facetsMask->InsertNextValue(GET_MASK(b));
				if (recActive[REC_COORDNUMBER]){
					facetsCoordNumb->InsertNextValue(b->coordNumber());
				}
				continue;
			}
		}
		if (recActive[REC_BOXES]){
			const Box* box = dynamic_cast<Box*>(b->shape.get()); 
			if (box){

				Vector3r pos(scene->isPeriodic ? scene->cell->wrapShearedPt(b->state->pos) : b->state->pos);
				Quaternionr ori(b->state->ori);
				Vector3r ext(box->extents);
				vtkSmartPointer<vtkQuad> boxes = vtkSmartPointer<vtkQuad>::New();

				Vector3r A = Vector3r(-ext[0], -ext[1], -ext[2]);
				Vector3r B = Vector3r(-ext[0], +ext[1], -ext[2]);
				Vector3r C = Vector3r(+ext[0], +ext[1], -ext[2]);
				Vector3r D = Vector3r(+ext[0], -ext[1], -ext[2]);
				
				Vector3r E = Vector3r(-ext[0], -ext[1], +ext[2]);
				Vector3r F = Vector3r(-ext[0], +ext[1], +ext[2]);
				Vector3r G = Vector3r(+ext[0], +ext[1], +ext[2]);
				Vector3r H = Vector3r(+ext[0], -ext[1], +ext[2]);

				A = pos + ori*A;
				B = pos + ori*B;
				C = pos + ori*C;
				D = pos + ori*D;
				E = pos + ori*E;
				F = pos + ori*F;
				G = pos + ori*G;
				H = pos + ori*H;

				addWallVTK(boxes, boxesPos, A, B, C, D);
				boxesCells->InsertNextCell(boxes);
				
				addWallVTK(boxes, boxesPos, E, H, G, F);
				boxesCells->InsertNextCell(boxes);
				
				addWallVTK(boxes, boxesPos, A, E, F, B);
				boxesCells->InsertNextCell(boxes);
				
				addWallVTK(boxes, boxesPos, G, H, D, C);
				boxesCells->InsertNextCell(boxes);
				
				addWallVTK(boxes, boxesPos, F, G, C, B);
				boxesCells->InsertNextCell(boxes);
				
				addWallVTK(boxes, boxesPos, D, H, E, A);
				boxesCells->InsertNextCell(boxes);
				
				for(int i=0; i<6; i++){
					if (recActive[REC_COLORS]){
						const Vector3r& color = box->color;
						Real c[3] = { (Real) color[0], (Real) color[1], (Real) color[2]};
						boxesColors->InsertNextTupleValue(c);
					}
					if(recActive[REC_STRESS]){
						const Vector3r& stress = bodyStates[b->getId()].normStress+bodyStates[b->getId()].shearStress;
						Real s[3] = { (Real) stress[0], (Real) stress[1], (Real) stress[2] };
						boxesStressVec->InsertNextTupleValue(s);
						boxesStressLen->InsertNextValue(stress.norm());
					}
					if(recActive[REC_FORCE]){
						scene->forces.sync();
						const Vector3r& f = scene->forces.getForce(b->getId());
						const Vector3r& t = scene->forces.getTorque(b->getId());
						Real ff[3] = { (Real) f[0], (Real) f[1], (Real) f[2] };
						Real tt[3] = { (Real) t[0], (Real) t[1], (Real) t[2] };
						Real fn = f.norm();
						Real tn = t.norm();
						boxesForceVec->InsertNextTupleValue(ff);
						boxesForceLen->InsertNextValue(fn);
						boxesTorqueVec->InsertNextTupleValue(tt);
						boxesTorqueLen->InsertNextValue(tn);
					}
					if (recActive[REC_MATERIALID]) boxesMaterialId->InsertNextValue(b->material->id);
					if (recActive[REC_MASK]) boxesMask->InsertNextValue(GET_MASK(b));
				}
				continue;
			}
		}
	}

	if (recActive[REC_PERICELL]) {
		const Matrix3r& hSize = scene->cell->hSize;
		Vector3r v0 = hSize*Vector3r(0,0,1);
		Vector3r v1 = hSize*Vector3r(0,1,1);
		Vector3r v2 = hSize*Vector3r(1,1,1);
		Vector3r v3 = hSize*Vector3r(1,0,1);
		Vector3r v4 = hSize*Vector3r(0,0,0);
		Vector3r v5 = hSize*Vector3r(0,1,0);
		Vector3r v6 = hSize*Vector3r(1,1,0);
		Vector3r v7 = hSize*Vector3r(1,0,0);
		pericellPoints->InsertNextPoint(v0[0],v0[1],v0[2]);
		pericellPoints->InsertNextPoint(v1[0],v1[1],v1[2]);
		pericellPoints->InsertNextPoint(v2[0],v2[1],v2[2]);
		pericellPoints->InsertNextPoint(v3[0],v3[1],v3[2]);
		pericellPoints->InsertNextPoint(v4[0],v4[1],v4[2]);
		pericellPoints->InsertNextPoint(v5[0],v5[1],v5[2]);
		pericellPoints->InsertNextPoint(v6[0],v6[1],v6[2]);
		pericellPoints->InsertNextPoint(v7[0],v7[1],v7[2]);
		vtkSmartPointer<vtkHexahedron> h = vtkSmartPointer<vtkHexahedron>::New();
		vtkIdList* l = h->GetPointIds();
		for (int i=0; i<8; i++) {
			l->SetId(i,i);
		}
		pericellHexa->InsertNextCell(h);
	}

	
	vtkSmartPointer<vtkDataCompressor> compressor;
	if(compress) compressor=vtkSmartPointer<vtkZLibDataCompressor>::New();

	vtkSmartPointer<vtkUnstructuredGrid> spheresUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
	if (recActive[REC_SPHERES]){
		spheresUg->SetPoints(spheresPos);
		spheresUg->SetCells(VTK_VERTEX, spheresCells);
		spheresUg->GetPointData()->AddArray(radii);
		if (recActive[REC_ID]) spheresUg->GetPointData()->AddArray(spheresId);
		if (recActive[REC_MASK]) spheresUg->GetPointData()->AddArray(spheresMask);
		if (recActive[REC_MASS]) spheresUg->GetPointData()->AddArray(spheresMass);
		if (recActive[REC_CLUMPID]) spheresUg->GetPointData()->AddArray(clumpId);
		if (recActive[REC_COLORS]) spheresUg->GetPointData()->AddArray(spheresColors);
		if (recActive[REC_VELOCITY]){
			spheresUg->GetPointData()->AddArray(spheresLinVelVec);
			spheresUg->GetPointData()->AddArray(spheresAngVelVec);
			spheresUg->GetPointData()->AddArray(spheresLinVelLen);
			spheresUg->GetPointData()->AddArray(spheresAngVelLen);
		}
#ifdef YADE_SPH
		spheresUg->GetPointData()->AddArray(spheresRhoSPH);
		spheresUg->GetPointData()->AddArray(spheresPressSPH);
		spheresUg->GetPointData()->AddArray(spheresCoordNumbSPH);
#endif

#ifdef YADE_DEFORM
		spheresUg->GetPointData()->AddArray(spheresRealRad);
#endif

#ifdef YADE_LIQMIGRATION
		if (recActive[REC_LIQ]) {
			spheresUg->GetPointData()->AddArray(spheresLiqVol);
			spheresUg->GetPointData()->AddArray(spheresLiqVolIter);
			spheresUg->GetPointData()->AddArray(spheresLiqVolTotal);
		}
#endif
		if (recActive[REC_STRESS]){
			spheresUg->GetPointData()->AddArray(spheresNormalStressVec);
			spheresUg->GetPointData()->AddArray(spheresShearStressVec);
			spheresUg->GetPointData()->AddArray(spheresNormalStressNorm);
		}
		if (recActive[REC_FORCE]){
			spheresUg->GetPointData()->AddArray(spheresForceVec);
			spheresUg->GetPointData()->AddArray(spheresForceLen);
			spheresUg->GetPointData()->AddArray(spheresTorqueVec);
			spheresUg->GetPointData()->AddArray(spheresTorqueLen);
		}
		if (recActive[REC_CPM]){
			spheresUg->GetPointData()->AddArray(cpmDamage);
			spheresUg->GetPointData()->AddArray(cpmStress);
		}

		if (recActive[REC_JCFPM]) {
                        spheresUg->GetPointData()->AddArray(nbCracks);
			spheresUg->GetPointData()->AddArray(jcfpmDamage);
		}
		if (recActive[REC_BSTRESS]) 
		{
			spheresUg->GetPointData()->AddArray(spheresSigI);
			spheresUg->GetPointData()->AddArray(spheresSigII);
			spheresUg->GetPointData()->AddArray(spheresSigIII);
			spheresUg->GetPointData()->AddArray(spheresDirI);
			spheresUg->GetPointData()->AddArray(spheresDirII);
			spheresUg->GetPointData()->AddArray(spheresDirIII);
		}
		if (recActive[REC_MATERIALID]) spheresUg->GetPointData()->AddArray(spheresMaterialId);
		if (recActive[REC_COORDNUMBER]) spheresUg->GetCellData()->AddArray(spheresCoordNumb);

		#ifdef YADE_VTK_MULTIBLOCK
		if(!multiblock)
		#endif
			{
			vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
			if(compress) writer->SetCompressor(compressor);
			if(ascii) writer->SetDataModeToAscii();
			string fn=fileName+"spheres."+boost::lexical_cast<string>(scene->iter)+".vtu";
			writer->SetFileName(fn.c_str());
			#ifdef YADE_VTK6
				writer->SetInputData(spheresUg);
			#else
				writer->SetInput(spheresUg);
			#endif
			writer->Write();
		}
	}
	vtkSmartPointer<vtkUnstructuredGrid> facetsUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
	if (recActive[REC_FACETS]){
		facetsUg->SetPoints(facetsPos);
		facetsUg->SetCells(VTK_TRIANGLE, facetsCells);
		if (recActive[REC_COLORS]) facetsUg->GetCellData()->AddArray(facetsColors);
		if (recActive[REC_STRESS]){
			facetsUg->GetCellData()->AddArray(facetsStressVec);
			facetsUg->GetCellData()->AddArray(facetsStressLen);
		}
		if (recActive[REC_FORCE]){
			facetsUg->GetCellData()->AddArray(facetsForceVec);
			facetsUg->GetCellData()->AddArray(facetsForceLen);
			facetsUg->GetCellData()->AddArray(facetsTorqueVec);
			facetsUg->GetCellData()->AddArray(facetsTorqueLen);
		}
		if (recActive[REC_MATERIALID]) facetsUg->GetCellData()->AddArray(facetsMaterialId);
		if (recActive[REC_MASK]) facetsUg->GetCellData()->AddArray(facetsMask);
		if (recActive[REC_COORDNUMBER]) facetsUg->GetCellData()->AddArray(facetsCoordNumb);
		#ifdef YADE_VTK_MULTIBLOCK
			if(!multiblock)
		#endif
			{
			vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
			if(compress) writer->SetCompressor(compressor);
			if(ascii) writer->SetDataModeToAscii();
			string fn=fileName+"facets."+boost::lexical_cast<string>(scene->iter)+".vtu";
			writer->SetFileName(fn.c_str());
			#ifdef YADE_VTK6
				writer->SetInputData(facetsUg);
			#else
				writer->SetInput(facetsUg);
			#endif
			writer->Write();
		}
	}
	vtkSmartPointer<vtkUnstructuredGrid> boxesUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
	if (recActive[REC_BOXES]){
		boxesUg->SetPoints(boxesPos);
		boxesUg->SetCells(VTK_QUAD, boxesCells);
		if (recActive[REC_COLORS]) boxesUg->GetCellData()->AddArray(boxesColors);
		if (recActive[REC_STRESS]){
			boxesUg->GetCellData()->AddArray(boxesStressVec);
			boxesUg->GetCellData()->AddArray(boxesStressLen);
		}
		if (recActive[REC_FORCE]){
			boxesUg->GetCellData()->AddArray(boxesForceVec);
			boxesUg->GetCellData()->AddArray(boxesForceLen);
			boxesUg->GetCellData()->AddArray(boxesTorqueVec);
			boxesUg->GetCellData()->AddArray(boxesTorqueLen);
		}
		if (recActive[REC_MATERIALID]) boxesUg->GetCellData()->AddArray(boxesMaterialId);
		if (recActive[REC_MASK]) boxesUg->GetCellData()->AddArray(boxesMask);
		#ifdef YADE_VTK_MULTIBLOCK
			if(!multiblock)
		#endif
			{
			vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
			if(compress) writer->SetCompressor(compressor);
			if(ascii) writer->SetDataModeToAscii();
			string fn=fileName+"boxes."+boost::lexical_cast<string>(scene->iter)+".vtu";
			writer->SetFileName(fn.c_str());
			#ifdef YADE_VTK6
				writer->SetInputData(boxesUg);
			#else
				writer->SetInput(boxesUg);
			#endif
			writer->Write();	
		}
	}
	vtkSmartPointer<vtkPolyData> intrPd = vtkSmartPointer<vtkPolyData>::New();
	if (recActive[REC_INTR]){
		intrPd->SetPoints(intrBodyPos);
		intrPd->SetLines(intrCells);
		intrPd->GetCellData()->AddArray(intrForceN);
		intrPd->GetCellData()->AddArray(intrAbsForceT);
#ifdef YADE_LIQMIGRATION
		if (recActive[REC_LIQ]) { 
			intrPd->GetCellData()->AddArray(liqVol);
			intrPd->GetCellData()->AddArray(liqVolNorm);
		}
#endif
		if (recActive[REC_JCFPM]) { 
			intrPd->GetCellData()->AddArray(intrIsCohesive);
			intrPd->GetCellData()->AddArray(intrIsOnJoint);
			intrPd->GetCellData()->AddArray(eventNumber);
		}
		if (recActive[REC_WPM]){
			intrPd->GetCellData()->AddArray(wpmNormalForce);
			intrPd->GetCellData()->AddArray(wpmLimitFactor);
		}
		#ifdef YADE_VTK_MULTIBLOCK
			if(!multiblock)
		#endif
			{
			vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
			if(compress) writer->SetCompressor(compressor);
			if(ascii) writer->SetDataModeToAscii();
			string fn=fileName+"intrs."+boost::lexical_cast<string>(scene->iter)+".vtp";
			writer->SetFileName(fn.c_str());
			#ifdef YADE_VTK6
				writer->SetInputData(intrPd);
			#else
				writer->SetInput(intrPd);
			#endif
			writer->Write();
		}
	}
	vtkSmartPointer<vtkUnstructuredGrid> pericellUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
	if (recActive[REC_PERICELL]){
		pericellUg->SetPoints(pericellPoints);
		pericellUg->SetCells(12,pericellHexa);
		#ifdef YADE_VTK_MULTIBLOCK
			if(!multiblock)
		#endif
			{
			vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
			if(compress) writer->SetCompressor(compressor);
			if(ascii) writer->SetDataModeToAscii();
			string fn=fileName+"pericell."+boost::lexical_cast<string>(scene->iter)+".vtu";
			writer->SetFileName(fn.c_str());
			#ifdef YADE_VTK6
				writer->SetInputData(pericellUg);
			#else
				writer->SetInput(pericellUg);
			#endif
			writer->Write();
		}
	}

	if (recActive[REC_CRACKS]) {
		string fileCracks = "cracks_"+Key+".txt";
		std::ifstream file (fileCracks.c_str(),std::ios::in);
		vtkSmartPointer<vtkUnstructuredGrid> crackUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
		
		if(file){
			 while ( !file.eof() ){
				std::string line;
				Real iter,time,p0,p1,p2,type,size,n0,n1,n2,nrg,onJnt;
				while ( std::getline(file, line)) {/* writes into string "line", a line of file "file". To go along diff. lines*/
					file >> iter >> time >> p0 >> p1 >> p2 >> type >> size >> n0 >> n1 >> n2 >> nrg >> onJnt;
					vtkIdType pid[1];
					pid[0] = crackPos->InsertNextPoint(p0, p1, p2);
					crackCells->InsertNextCell(1,pid);
                                        crackIter->InsertNextValue(iter);
                                        crackTime->InsertNextValue(time);
					crackType->InsertNextValue(type);
					crackSize->InsertNextValue(size);					
					Real n[3] = { n0, n1, n2 };
					crackNorm->InsertNextTupleValue(n);
                                        crackNrg->InsertNextValue(nrg);
                                        crackOnJnt->InsertNextValue(onJnt);
				}
			}
			 file.close();
		}
// 
		crackUg->SetPoints(crackPos);
		crackUg->SetCells(VTK_VERTEX, crackCells);
		crackUg->GetPointData()->AddArray(crackIter);
                crackUg->GetPointData()->AddArray(crackTime);
		crackUg->GetPointData()->AddArray(crackType);
		crackUg->GetPointData()->AddArray(crackSize);
		crackUg->GetPointData()->AddArray(crackNorm); //see https://www.mail-archive.com/paraview@paraview.org/msg08166.html to obtain Paraview 2D glyphs conforming to this normal 
                crackUg->GetPointData()->AddArray(crackNrg);
                crackUg->GetPointData()->AddArray(crackOnJnt);

		vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
		if(compress) writer->SetCompressor(compressor);
		if(ascii) writer->SetDataModeToAscii();
		string fn=fileName+"cracks."+boost::lexical_cast<string>(scene->iter)+".vtu";
		writer->SetFileName(fn.c_str());
		#ifdef YADE_VTK6
			writer->SetInputData(crackUg);
		#else
			writer->SetInput(crackUg);
		#endif
		writer->Write(); }

// doing same thing for moments that we did for cracks:
	if (recActive[REC_MOMENTS]) {
		string fileMoments = "moments_"+Key+".txt";
		std::ifstream file (fileMoments.c_str(),std::ios::in);
		vtkSmartPointer<vtkUnstructuredGrid> momentUg = vtkSmartPointer<vtkUnstructuredGrid>::New();
		
		if(file){
			 while ( !file.eof() ){
				std::string line;
				Real i, p0, p1, p2, moment, numInts, eventNum, time;
				while ( std::getline(file, line)) {/* writes into string "line", a line of file "file". To go along diff. lines*/
					file >> i >> p0 >> p1 >> p2 >> moment >> numInts >> eventNum >> time;
					vtkIdType pid[1];
					pid[0] = momentPos->InsertNextPoint(p0, p1, p2);
					momentCells->InsertNextCell(1,pid);
					momentSize->InsertNextValue(moment);
					momentiter->InsertNextValue(i);
					momenttime->InsertNextValue(time);
					momentNumInts->InsertNextValue(numInts);
					momentEventNum->InsertNextValue(eventNum);
				}
			}
			 file.close();
		}
// 
		momentUg->SetPoints(momentPos);
		momentUg->SetCells(VTK_VERTEX, momentCells);
		momentUg->GetPointData()->AddArray(momentiter);
		momentUg->GetPointData()->AddArray(momenttime);
		momentUg->GetPointData()->AddArray(momentSize);
		momentUg->GetPointData()->AddArray(momentNumInts);
		momentUg->GetPointData()->AddArray(momentEventNum);

		vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
		if(compress) writer->SetCompressor(compressor);
		if(ascii) writer->SetDataModeToAscii();
		string fn=fileName+"moments."+boost::lexical_cast<string>(scene->iter)+".vtu";
		writer->SetFileName(fn.c_str());
		#ifdef YADE_VTK6
			writer->SetInputData(momentUg);
		#else
			writer->SetInput(momentUg);
		#endif
		writer->Write(); }

	#ifdef YADE_VTK_MULTIBLOCK
		if(multiblock){
			vtkSmartPointer<vtkMultiBlockDataSet> multiblockDataset = vtkSmartPointer<vtkMultiBlockDataSet>::New();
			int i=0;
			if(recActive[REC_SPHERES]) multiblockDataset->SetBlock(i++,spheresUg);
			if(recActive[REC_FACETS]) multiblockDataset->SetBlock(i++,facetsUg);
			if(recActive[REC_INTR]) multiblockDataset->SetBlock(i++,intrPd);
			if(recActive[REC_PERICELL]) multiblockDataset->SetBlock(i++,pericellUg);
			vtkSmartPointer<vtkXMLMultiBlockDataWriter> writer = vtkSmartPointer<vtkXMLMultiBlockDataWriter>::New();
			if(ascii) writer->SetDataModeToAscii();
			string fn=fileName+boost::lexical_cast<string>(scene->iter)+".vtm";
			writer->SetFileName(fn.c_str());
			#ifdef YADE_VTK6
				writer->SetInputData(multiblockDataset);
			#else
				writer->SetInput(multiblockDataset);
			#endif
			writer->Write();	
		}
	#endif
};

void VTKRecorder::addWallVTK (vtkSmartPointer<vtkQuad>& boxes, vtkSmartPointer<vtkPoints>& boxesPos, Vector3r& W1, Vector3r& W2, Vector3r& W3, Vector3r& W4) {
	//Function for exporting walls of boxes
	vtkIdType nbPoints=boxesPos->GetNumberOfPoints();
	
	boxesPos->InsertNextPoint(W1[0], W1[1], W1[2]);
	boxes->GetPointIds()->SetId(0,nbPoints+0);
	
	boxesPos->InsertNextPoint(W2[0], W2[1], W2[2]);
	boxes->GetPointIds()->SetId(1,nbPoints+1);
	
	boxesPos->InsertNextPoint(W3[0], W3[1], W3[2]);
	boxes->GetPointIds()->SetId(2,nbPoints+2);
	
	boxesPos->InsertNextPoint(W4[0], W4[1], W4[2]);
	boxes->GetPointIds()->SetId(3,nbPoints+3);
};

#endif /* YADE_VTK */
#undef GET_MASK
